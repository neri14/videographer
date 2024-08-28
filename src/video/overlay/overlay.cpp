#include "overlay.h"

#include "widget/string_widget.h"

#include <format>
#include <chrono>

namespace vgraph {
namespace video {
namespace overlay {

overlay::overlay(std::shared_ptr<layout> lay, std::shared_ptr<telemetry::telemetry> tele, std::pair<int, int> resolution, bool timecode):
    tele_(tele),
    layout_(*lay),
    width(resolution.first),
    height(resolution.second)
{
    if (lay) {
        for (auto wdgt : *lay) {
            add_widget(wdgt);
        }
    }

    static_cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    dynamic_cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

    if (timecode) {
        tc_widget_ = std::make_shared<timecode_widget>(width/2);
    }
}

overlay::~overlay()
{
    double total_s = total_drawing_time/1000000.0;
    log.info("Total drawing time: {:.3f} s ({:.6f} s/frame, {} frames)",
             total_s, total_s/total_drawn_frames, total_drawn_frames);
    log.info("Cache hit ratio {:.1f}%", (100.0*cache_hit)/(cache_hit+cache_miss));

    if (static_cache) {
        cairo_surface_destroy(static_cache);
        static_cache = nullptr;
    }
    if (dynamic_cache) {
        cairo_surface_destroy(dynamic_cache);
        dynamic_cache = nullptr;
    }
}

void overlay::precache()
{
    auto t1 = std::chrono::high_resolution_clock::now();

    for (auto w : layout_) {
        w->prepare(tele_->get_all());
    }

    cairo_t* cr = cairo_create(static_cache);
    for (auto w : static_widgets_) {
        w->draw_static(cr);
    }
    cairo_destroy(cr);

    auto t2 = std::chrono::high_resolution_clock::now();
    total_drawing_time += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
}


//TODO future improvement: multithreaded predrawing of frames - then draw() will only take from cache
//     for now, because draw is always sequential, cache keeps only last generated overlay until invalidated
void overlay::draw(cairo_t* cr, double timestamp)
{
    auto t1 = std::chrono::high_resolution_clock::now();

    std::shared_ptr<telemetry::datapoint> data = tele_->get(timestamp);

    if (last_data != data) {
        update_dynamic_cache(data);
        cache_miss++;
        last_data = data;
    } else {
        cache_hit++;
    }

    cairo_set_source_surface(cr, dynamic_cache, 0, 0);
    cairo_paint(cr);

    if (tc_widget_) {
        tc_widget_->draw(cr, timestamp);
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    total_drawing_time += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    total_drawn_frames++;
}

void overlay::update_dynamic_cache(std::shared_ptr<telemetry::datapoint> data)
{
    cairo_t* cr = cairo_create(dynamic_cache);

    // clear the surface
    cairo_save(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_restore(cr);

    if (data) { //paint overlays only if there is data
        // paint the static over 
        cairo_set_source_surface(cr, static_cache, 0, 0);
        cairo_paint(cr);

        // draw dynamic widgets over
        for (auto w : dynamic_widgets_) {
            w->draw_dynamic(cr, data);
        }
    }
    
    // cleanup
    cairo_destroy(cr);
}

void overlay::add_widget(std::shared_ptr<widget> ptr)
{
    if (ptr->is_static()) {
        static_widgets_.push_back(ptr);
    }
    if (ptr->is_dynamic()) {
        dynamic_widgets_.push_back(ptr);
    }
}

} // namespace overlay
} // namespace video
} // namespace vgraph



