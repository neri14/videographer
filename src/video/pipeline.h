#ifndef PIPELINE_H
#define PIPELINE_H

#include "utils/logging/logger.h"

#include "overlay_drawer.h"

#include <map>
#include <string>

extern "C" {
    #include <gst/gst.h>
    #include <gst/video/video.h>

    #include <cairo.h>
}

namespace vgraph {
namespace video {

class pipeline {
public:
    pipeline(const std::string& input_path, const std::string& output_path, const overlay_drawer& overlay);
    ~pipeline();

    bool build();
    bool link();
    bool init();
    bool run();

    //FIXME to check if can be reworked and privatized
    void pad_added_handler(GstElement* src, GstPad* new_pad);
    void draw_overlay(GstElement* overlay, cairo_t* cr, guint64 timestamp, guint64 duration);

private:
    bool link_elements(const std::string& src, const std::string& dest, GstCaps* filter = nullptr);

    utils::logging::logger log{"pipeline"};

    const overlay_drawer& overlay_;
    std::map<std::string, GstElement*> elements_;
    std::string input_path_;
    std::string output_path_;
};

} // namespace video
} // namespace vgraph

#endif // PIPELINE_H