#ifndef ENUM_FIELD_H
#define ENUM_FIELD_H

namespace vgraph {
namespace telemetry {

enum class EField
{
    Latitude,
    Longitude,
    Altitude,
    Gradient,
    Temperature,
    Distance,
    TrackDistance,
    Speed,
    Cadence,
    Power,
    Power3s,
    Power10s,
    RespirationRate,
    HeartRate,
    Grit,
    Flow,
};

} // namespace telemetry
} // namespace vgraph

#endif // ENUM_FIELD_H
