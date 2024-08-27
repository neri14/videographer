#include "layout.h"
#include "widget/string_widget.h"
#include "utils/logging/logger.h"

#include <string>

namespace vgraph {
namespace video {
namespace overlay {


layout_parser::layout_parser():
    widgets(std::make_shared<layout>())
{}

std::shared_ptr<layout> layout_parser::parse(const std::string& path)
{
    pugi::xml_document doc;
    auto result = doc.load_file(path.c_str());

    if (!result) {
        log.error("Error loading xml file: {}", result.description());
        return nullptr;
    }

    if (doc.children().empty()) {
        log.error("Empty layout file");
        return nullptr;
    }

    int count = 0;
    for (auto& node : doc.children()) {
        count++;
    }
    if (count > 1) {
        log.error("More than one root node in layout file");
        return nullptr;
    }

    pugi::xml_node root = doc.child("layout");
    bool ok = load_widgets(root);

    if (!ok) {
        return nullptr;
    }
    return widgets;
}

bool layout_parser::load_widgets(pugi::xml_node node, int x_offset, int y_offset)
{
    std::string name(node.name());

    bool ok = true;

    if (std::string(node.name()) == "layout") {
        //go fruther down
        for (pugi::xml_node& child : node.children()) {
            ok = ok && load_widgets(child, x_offset, y_offset);
        }
    } else if (std::string(node.name()) == "container") {
        //container node, pass x and y down the line
        if (node.attribute("x")) {
            x_offset += node.attribute("x").as_int();
        }
        if (node.attribute("y")) {
            y_offset += node.attribute("y").as_int();
        }
        for (pugi::xml_node& child : node.children()) {
            ok = ok && load_widgets(child, x_offset, y_offset);
        }
    } else if (std::string(node.name()) == "widget") {
        ok = ok && create_widget(node, x_offset, y_offset);
    } else {
        log.error("Unexpected node \"{}\"", name);
        ok = false;
    }

    return ok;
}

bool layout_parser::create_widget(pugi::xml_node node, int x_offset, int y_offset)
{
    if (!node.attribute("type")) {
        log.error("Missing widget type");
        return false;
    }

    std::string type(node.attribute("type").as_string());

    if (type == "datetime") {
        return create_datetime_widget(node, x_offset, y_offset);
    } else if (type == "string") {
        return create_string_widget(node, x_offset, y_offset);
    } else if (type == "value") {
        return create_value_widget(node, x_offset, y_offset);
    }

    log.error("Unknown widget type \"{}\"", type);
    return false;
}

bool layout_parser::create_datetime_widget(pugi::xml_node node, int x_offset, int y_offset)
{
    //TODO layout_parser::create_datetime_widget(pugi::xml_node node)
    log.warning("Not yet implemented: layout_parser::create_datetime_widget");
    return true;
}

bool layout_parser::create_string_widget(pugi::xml_node node, int x_offset, int y_offset)
{
// int x, int y, const std::string& text, const std::string& font, const rgba& color, const rgba& border_color, int border_width, ETextAlign align
    bool status = true;

    int x = mandatory_attribute(node, "x", status).as_int() + x_offset;
    int y = mandatory_attribute(node, "y", status).as_int() + y_offset;
    std::string text = mandatory_attribute(node, "text", status).as_string();
    std::string font = mandatory_attribute(node, "font", status).as_string();
    rgba color = color_from_string(mandatory_attribute(node, "color", status).as_string());
    rgba border_color = color_from_string(mandatory_attribute(node, "border-color", status).as_string());
    int border_width = mandatory_attribute(node, "border-width", status).as_int();
    ETextAlign align = text_align_from_string(mandatory_attribute(node, "align", status).as_string());

    widgets->push_back(std::make_shared<string_widget>(x, y, text, font, color, border_color, border_width, align));

    return status;
}

bool layout_parser::create_value_widget(pugi::xml_node node, int x_offset, int y_offset)
{
    //TODO layout_parser::create_value_widget(pugi::xml_node node)
    log.warning("Not yet implemented: layout_parser::create_value_widget");
    return true;
}

pugi::xml_attribute layout_parser::mandatory_attribute(pugi::xml_node node, const std::string& attr_name, bool& out_status)
{
    pugi::xml_attribute attr = node.attribute(attr_name.c_str());

    if (!attr) {
        log.error("Missing mandatory attribute \"{}\" in \"{}\" widget", attr_name, node.attribute("type").as_string());
        out_status = false;
    }

    return attr;
}

} // namespace overlay
} // namespace video
} // namespace vgraph
