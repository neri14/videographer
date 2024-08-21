#include "generator.h"

namespace vgraph {
namespace video {

generator::generator(const std::vector<std::string>& input, const std::string& output):
    input_(input),
    output_(output)
{}

void generator::generate()
{
    log.info("Starting video generation");

    log.info("Finished video generation");
}

} // namespace video
} // namespace vgraph
