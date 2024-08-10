#include "fit_parser.h"

namespace vgraph {
namespace telemetry {

std::shared_ptr<datapoint_sequence> fit_parser::parse(const std::filesystem::path& path)
{
    if (!std::filesystem::exists(path)) {
        log.error(std::format("File does not exist: {}", path.string()));
        //FIXME implement logging methods that take arguments like std::format i.e.: log.error("File does not exist: {}", path.string());

        return nullptr;
    }

    if (path.extension() != ".fit") {
        log.error(std::format("File extension is not .fit: {}", path.string()));
        return nullptr;
    }

    auto seq = std::make_shared<datapoint_sequence>();
    //TODO parsing to be implemented
    return seq;
}

} // namespace telemetry
} // namespace vgraph
