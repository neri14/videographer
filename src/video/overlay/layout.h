#ifndef LAYOUT_H
#define LAYOUT_H

#include <string>
#include <memory>

namespace vgraph {
namespace video {
namespace overlay {

class layout {
public:
    layout() = default;
    ~layout() = default;

    static std::shared_ptr<layout> load(const std::string& path);
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // LAYOUT_H
