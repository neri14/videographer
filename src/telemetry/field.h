#ifndef FIELD_H
#define FIELD_H

#include <string>

namespace vgraph {
namespace telemetry {

enum class EField
{
    Latitude,
    Longitude,
    Altitude,
    SmoothAltitude,
    Gradient,
    Temperature,
    Distance,
    TrackDistance,
    Speed,
    Cadence,
    Power,
    Power3s,
    Power10s,
    Power30s,
    RespirationRate,
    HeartRate,
    Grit,
    Flow,
    UNDEFINED
};

EField map_key_to_field(const std::string& key);

} // namespace telemetry
} // namespace vgraph

#endif // FIELD_H
