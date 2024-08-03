// std::shared_ptr<track> load() override;

#include "fit_file.h"

namespace vgraph {
namespace telemetry {

fit_file::fit_file(const std::string& path)
    : file(path)
{}

void fit_file::load()
{
    //FIXME to be implemented
}

} // namespace telemetry
} // namespace vgraph
