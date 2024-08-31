#include "layout_parser.h"
#include "widget/string_widget.h"
#include "widget/value_widget.h"
#include "widget/timestamp_widget.h"
#include "widget/chart_widget.h"
#include "widget/moving_chart_widget.h"
#include "widget/map_widget.h"
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

    const std::string type(node.attribute("type").as_string());

    if (type == "datetime") {
        return create_timestamp_widget(node, x_offset, y_offset);
    } else if (type == "string") {
        return create_string_widget(node, x_offset, y_offset);
    } else if (type == "value") {
        return create_value_widget(node, x_offset, y_offset);
    } else if (type == "chart") {
        return create_chart_widget(node, x_offset, y_offset);
    } else if (type == "map") {
        return create_map_widget(node, x_offset, y_offset);
    } else if (type == "moving-chart") {
        return create_moving_chart_widget(node, x_offset, y_offset);
    }

    log.error("Unknown widget type \"{}\"", type);
    return false;
}

layout_parser::common_text_params layout_parser::text_params(pugi::xml_node node, int x_offset, int y_offset, bool& out_status)
{
    return {
        mandatory_attribute(node, "x", out_status).as_int() + x_offset,
        mandatory_attribute(node, "y", out_status).as_int() + y_offset,
        text_align_from_string(mandatory_attribute(node, "align", out_status).as_string()),
        mandatory_attribute(node, "font", out_status).as_string(),
        color_from_string(mandatory_attribute(node, "color", out_status).as_string()),
        color_from_string(mandatory_attribute(node, "border-color", out_status).as_string()),
        mandatory_attribute(node, "border-width", out_status).as_int()
    };
}

layout_parser::common_chart_params layout_parser::chart_params(pugi::xml_node node, int x_offset, int y_offset, bool& out_status)
{
    return {
        mandatory_attribute(node, "x", out_status).as_int() + x_offset,
        mandatory_attribute(node, "y", out_status).as_int() + y_offset,
        mandatory_attribute(node, "width", out_status).as_int(),
        mandatory_attribute(node, "height", out_status).as_int(),
        color_from_string(mandatory_attribute(node, "line-color", out_status).as_string()),
        mandatory_attribute(node, "line-width", out_status).as_int(),
        color_from_string(mandatory_attribute(node, "point-color", out_status).as_string()),
        mandatory_attribute(node, "point-size", out_status).as_int()
    };
}

bool layout_parser::create_timestamp_widget(pugi::xml_node node, int x_offset, int y_offset)
{
    bool status = true;

    auto txt = text_params(node, x_offset, y_offset, status);
    std::string format = mandatory_attribute(node, "format", status).as_string();
    int utcoffset = mandatory_attribute(node, "utcoffset", status).as_int();

    widgets->push_back(std::make_shared<timestamp_widget>(txt.x, txt.y, txt.align, txt.font, txt.color, txt.border_color, txt.border_width, format, utcoffset));
    return status;
}

bool layout_parser::create_string_widget(pugi::xml_node node, int x_offset, int y_offset)
{
    bool status = true;

    auto txt = text_params(node, x_offset, y_offset, status);
    std::string text = mandatory_attribute(node, "text", status).as_string();

    widgets->push_back(std::make_shared<string_widget>(txt.x, txt.y, txt.align, txt.font, txt.color, txt.border_color, txt.border_width, text));
    return status;
}

bool layout_parser::create_value_widget(pugi::xml_node node, int x_offset, int y_offset)
{
    bool status = true;

    auto txt = text_params(node, x_offset, y_offset, status);
    std::string key = mandatory_attribute(node, "key", status).as_string();
    int precision = mandatory_attribute(node, "precision", status).as_int();

    widgets->push_back(std::make_shared<value_widget>(txt.x, txt.y, txt.align, txt.font, txt.color, txt.border_color, txt.border_width, key, precision));
    return status;
}

bool layout_parser::create_chart_widget(pugi::xml_node node, int x_offset, int y_offset)
{
    bool status = true;

    auto chrt = chart_params(node, x_offset, y_offset, status);
    std::string x_key = mandatory_attribute(node, "x-key", status).as_string();
    std::string y_key = mandatory_attribute(node, "y-key", status).as_string();

    widgets->push_back(std::make_shared<chart_widget>(chrt.x, chrt.y, chrt.width, chrt.height, chrt.line_color, chrt.line_width, chrt.point_color, chrt.point_size, x_key, y_key));
    return status;
}

bool layout_parser::create_moving_chart_widget(pugi::xml_node node, int x_offset, int y_offset)
{
    bool status = true;

    auto chrt = chart_params(node, x_offset, y_offset, status);
    std::string x_key = mandatory_attribute(node, "x-key", status).as_string();
    std::string y_key = mandatory_attribute(node, "y-key", status).as_string();

    widgets->push_back(std::make_shared<moving_chart_widget>(chrt.x, chrt.y, chrt.width, chrt.height, chrt.line_color, chrt.line_width, chrt.point_color, chrt.point_size, x_key, y_key));
    return status;
}

bool layout_parser::create_map_widget(pugi::xml_node node, int x_offset, int y_offset)
{
    bool status = true;

    auto chrt = chart_params(node, x_offset, y_offset, status);

    widgets->push_back(std::make_shared<map_widget>(chrt.x, chrt.y, chrt.width, chrt.height, chrt.line_color, chrt.line_width, chrt.point_color, chrt.point_size));
    return status;
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
