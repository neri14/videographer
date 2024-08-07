#ifndef ENUM_FIELD_H
#define ENUM_FIELD_H

namespace vgraph {
namespace telemetry {

enum class EField
{
    Timestamp,
    Latitude,
    Longitude,
    Altitude,
    Temperature,
    Distance,
    Speed,
    Cadence,
    Power,
    LeftPedalSmoothness,
    RightPedalSmoothness,
    LeftTorqueEffectiveness,
    RightTorqueEffectiveness,
    RespirationRate,
    HeartRate,
    Grit,
    Flow,
};

} // namespace telemetry
} // namespace vgraph

#endif // ENUM_FIELD_H
