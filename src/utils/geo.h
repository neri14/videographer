#ifndef GEO_H
#define GEO_H

namespace vgraph {
namespace utils {

double geo_distance(double lat1, double lon1, double lat2, double lon2);
double gradient(double dist, double elev);

} // namespace utils
} // namespace vgraph

#endif // GEO_H
