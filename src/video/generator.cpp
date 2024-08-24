#include "generator.h"

namespace vgraph {
namespace video {

generator::generator(const std::string& input, const std::string& output):
    input_path_(input),
    output_path_(output)
{}

void generator::generate()
{
    log.info("Starting video generation");

    log.info("Finished video generation");
}

} // namespace video
} // namespace vgraph
