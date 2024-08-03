#include "file.h"

namespace vgraph {
namespace telemetry {

file::file(const std::string& path)
    : path_(path)
{}

std::shared_ptr<track> file::get_track()
{
    if (!track_) {
        load();
    }
    return track_;
}

} // namespace telemetry
} // namespace vgraph
