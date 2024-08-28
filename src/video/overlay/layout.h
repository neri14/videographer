#ifndef LAYOUT_H
#define LAYOUT_H

#include <string>
#include <memory>

#include "widget/widget.h"
#include "widget/color.h"
#include "widget/text_align.h"

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
    struct common_text_params;

    utils::logging::logger log{"layout::load"};

    bool load_widgets(pugi::xml_node node, int x_offset=0, int y_offset=0);
    
    bool create_widget(pugi::xml_node node, int x_offset, int y_offset);
    bool create_timestamp_widget(pugi::xml_node node, int x_offset, int y_offset);
    bool create_string_widget(pugi::xml_node node, int x_offset, int y_offset);
    bool create_value_widget(pugi::xml_node node, int x_offset, int y_offset);

    common_text_params text_params(pugi::xml_node node, int x_offset, int y_offset, bool& out_status);
    pugi::xml_attribute mandatory_attribute(pugi::xml_node node, const std::string& attr_name, bool& out_status);

    std::shared_ptr<layout> widgets;
};


struct layout_parser::common_text_params{
    int x;
    int y;
    ETextAlign align;
    std::string font;
    rgba color;
    rgba border_color;
    int border_width;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // LAYOUT_H
