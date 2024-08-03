#ifndef FILE_H
#define FILE_H

#include "track.h"

#include <memory>
#include <string>

namespace vgraph {
namespace telemetry {

class file {
public:
    file(const std::string& path);
    ~file() = default;

    std::shared_ptr<track> get_track();

protected:
    virtual void load() = 0;

    std::string path_;
    std::shared_ptr<track> track_ = nullptr;
};

} // namespace telemetry
} // namespace vgraph

#endif // FILE_H
