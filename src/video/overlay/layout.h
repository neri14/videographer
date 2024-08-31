#ifndef LAYOUT_H
#define LAYOUT_H

#include "widget/widget.h"

#include <memory>
#include <vector>

namespace vgraph {
namespace video {
namespace overlay {

using layout = std::vector<std::shared_ptr<widget>>;

std::shared_ptr<layout> generate_alignment_layout();

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // LAYOUT_H
