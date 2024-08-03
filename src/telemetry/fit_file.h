#ifndef FIT_FILE_H
#define FIT_FILE_H

#include "file.h"

namespace vgraph {
namespace telemetry {

class fit_file : public file {
public:
    fit_file(std::string path);
    ~fit_file() = default;

    void load() override;
};

} // namespace telemetry
} // namespace vgraph

#endif // FIT_FILE_H
