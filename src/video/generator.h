#ifndef GENERATOR_H
#define GENERATOR_H

#include "utils/logging/logger.h"

#include "overlay/overlay.h"

#include <map>
#include <string>

extern "C" {
    #include <gst/gst.h>
    #include <gst/video/video.h>

    #include <cairo.h>
}

namespace vgraph {
namespace video {

struct pipeline_element {
    std::string name;
    std::string type;
    std::optional<std::string> src_pad_filter = std::nullopt;
};

bool operator==(const pipeline_element& lhs, const pipeline_element& rhs);

class generator {
public:
    generator(const std::string& input_path,
             const std::string& output_path,
             std::shared_ptr<overlay::overlay> overlay,
             bool gpu,
             std::pair<int, int> output_resolution,
             int output_bitrate,
             bool debug,
             std::optional<double> clip_length);
    ~generator();

    bool generate();

    //FIXME to check if can be reworked and privatized
    void pad_added_handler(GstElement* src, GstPad* new_pad);

private:
    bool setup();
    bool setup_input();
    bool setup_audio();
    bool setup_video_gpu();
    bool setup_video_cpu();
    bool setup_output();

    bool build();
    bool build_pipeline_bin();
    bool build_pipeline_elements(const std::vector<pipeline_element>& elems);

    bool link();
    bool link_path_elements(const std::vector<pipeline_element>& path);
    bool link_paths(const std::vector<pipeline_element>& from, const std::vector<pipeline_element>& to);
    bool link_elements(const pipeline_element& src, const pipeline_element& dest);

    bool config_elements();
    bool connect_signals();

    bool execute();

    void print_stats(const GstClockTime& pos, const GstClockTime& len);

    utils::logging::logger log{"generator"};
    bool debug_;

    std::shared_ptr<overlay::overlay> overlay_;
    std::map<std::string, GstElement*> elements_;

    bool gpu_;

    std::string input_path_;
    std::string output_path_;

    std::pair<int, int> output_resolution_;
    int output_bitrate_;

    std::vector<pipeline_element> input_path;
    std::vector<pipeline_element> audio_path;
    std::vector<pipeline_element> video_path;
    std::vector<pipeline_element> output_path;

    std::optional<double> clip_length_;

    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

} // namespace video
} // namespace vgraph

#endif // GENERATOR_H