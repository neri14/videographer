#include "geo.h"

#include <numbers>
#include <cmath>

namespace vgraph {
namespace utils {
namespace consts {
    const double radio_terrestre(6372797.5605);
    const double grados_radianes(std::numbers::pi / 180);
}

double geo_distance(double lat1, double lon1, double lat2, double lon2)
{
    lat1 = lat1 * consts::grados_radianes;
    lon1 = lon1 * consts::grados_radianes;
    lat2 = lat2 * consts::grados_radianes;
    lon2 = lon2 * consts::grados_radianes;

    double haversine = 
        std::pow(std::sin((lat2 - lat1)/2.0), 2) +
        std::cos(lat1) * std::cos(lat2) * std::pow(std::sin((lon2 - lon1)/2.0), 2);
    
    double dist = 2 * std::asin(std::min(1.0, std::sqrt(haversine))) * consts::radio_terrestre;

    return dist;
}

double gradient(double dist, double elev)
{
    return 100 * elev/dist; //converted to pct
}

} // namespace utils
} // namespace vgraph