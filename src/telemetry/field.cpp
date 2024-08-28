#include "field.h"

#include <map>

namespace vgraph {
namespace telemetry {
namespace consts {
    const std::map<std::string, EField> key_field_map = {
        {"latitude",         EField::Latitude},
        {"longitude",        EField::Longitude},
        {"altitude",         EField::Altitude},
        {"smooth_altitude",  EField::SmoothAltitude},
        {"gradient",         EField::Gradient},
        {"temperature",      EField::Temperature},
        {"distance",         EField::Distance},
        {"track_distance",   EField::TrackDistance},
        {"speed",            EField::Speed},
        {"cadence",          EField::Cadence},
        {"power",            EField::Power},
        {"power3s",          EField::Power3s},
        {"power10s",         EField::Power10s},
        {"respiration_rate", EField::RespirationRate},
        {"heart_rate",       EField::HeartRate},
        {"grit",             EField::Grit},
        {"flow",             EField::Flow}
    };
}

EField map_key_to_field(const std::string& key)
{
    if (consts::key_field_map.contains(key)) {
        return consts::key_field_map.at(key);
    }
    return EField::UNDEFINED;
}

} // namespace telemetry
} // namespace vgraph
