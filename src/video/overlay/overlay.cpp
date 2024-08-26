#include "overlay.h"

#include "widget/string_widget.h"

#include <format>
#include <chrono>

namespace vgraph {
namespace video {
namespace overlay {

overlay::overlay(std::pair<int, int> resolution, bool timecode):
    width(resolution.first),
    height(resolution.second)
{
    //TODO here we'll normally create widgets based on template file
    add_widget(std::make_shared<string_widget>(1000,  10, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Left));
    add_widget(std::make_shared<string_widget>(1000,  80, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Center));
    add_widget(std::make_shared<string_widget>(1000, 150, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Right));
    add_widget(std::make_shared<string_widget>(1000, 220, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Center));
    add_widget(std::make_shared<string_widget>(1000, 290, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Left));
    add_widget(std::make_shared<string_widget>(1000, 360, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Center));
    add_widget(std::make_shared<string_widget>(1000, 430, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Right));
    add_widget(std::make_shared<string_widget>(1000, 500, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Center));
    add_widget(std::make_shared<string_widget>(1000, 570, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Left));
    add_widget(std::make_shared<string_widget>(1000, 640, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Center));
    add_widget(std::make_shared<string_widget>(1000, 710, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Right));
    add_widget(std::make_shared<string_widget>(1000, 780, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Center));
    add_widget(std::make_shared<string_widget>(1000, 850, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Left));
    add_widget(std::make_shared<string_widget>(1000, 920, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Center));
    add_widget(std::make_shared<string_widget>(1000, 990, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Right));

    static_cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    dynamic_cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

    if (timecode) {
        tc_widget_ = std::make_shared<timecode_widget>(width/2);
    }
}

overlay::~overlay()
{
    double total_s = total_drawing_time/1000000.0;
    log.info("Total drawing time: {:.6f} s", total_s);
    log.info("Average drawing time: {:.6f} s/frame", total_s/total_drawn_frames);
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

    //FIXME target impl:
    // data = all_data[timestamp]
    // if data has changed:
    //     draw static_cache to dynamic_cache
    //     draw all widgets to dynamic_cache
    // fi
    //
    // draw dynamic_cache to cr

    if (true) { //FIXME for now - always assuming data has changed so invalidate cache
        update_dynamic_cache(timestamp);
        cache_miss++;
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

void overlay::update_dynamic_cache(double timestamp)
{
    cairo_t* cr = cairo_create(dynamic_cache);

    // clear the surface
    cairo_save(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_restore(cr);

    // paint the static over 
    cairo_set_source_surface(cr, static_cache, 0, 0);
    cairo_paint(cr);

    // draw dynamic widgets over
    for (auto w : dynamic_widgets_) {
        w->draw_static(cr);
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



