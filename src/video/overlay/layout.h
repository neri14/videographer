#ifndef LAYOUT_H
#define LAYOUT_H

#include <string>
#include <memory>

#include "widget/widget.h"

#include <pugixml.hpp>

namespace vgraph {
namespace video {
namespace overlay {

using layout = std::vector<std::shared_ptr<widget>>;

class layout_parser {
public:
    layout_parser();
    ~layout_parser() = default;

    std::shared_ptr<layout> parse(const std::string& path);

private:
    utils::logging::logger log{"layout::load"};

    bool load_widgets(pugi::xml_node node, int x_offset=0, int y_offset=0);
    
    bool create_widget(pugi::xml_node node, int x_offset, int y_offset);
    bool create_datetime_widget(pugi::xml_node node, int x_offset, int y_offset);
    bool create_string_widget(pugi::xml_node node, int x_offset, int y_offset);
    bool create_value_widget(pugi::xml_node node, int x_offset, int y_offset);

    pugi::xml_attribute mandatory_attribute(pugi::xml_node node, const std::string& attr_name, bool& out_status);

    std::shared_ptr<layout> widgets;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // LAYOUT_H
