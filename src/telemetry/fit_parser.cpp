#include "fit_parser.h"

namespace vgraph {
namespace telemetry {

std::shared_ptr<datapoint_sequence> fit_parser::parse(const std::filesystem::path& path)
{
    if (!std::filesystem::exists(path)) {
        //FIXME add error log
        return nullptr;
    }

    if (path.extension() != ".fit") {
        //FIXME add error log
        return nullptr;
    }

    auto seq = std::make_shared<datapoint_sequence>();
    //TODO parsing to be implemented
    return seq;
}

} // namespace telemetry
} // namespace vgraph
