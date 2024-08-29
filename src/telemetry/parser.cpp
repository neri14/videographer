#include "parser.h"
#include "utils/geo.h"

#include <ranges>
#include <map>
#include <cmath>

namespace vgraph {
namespace telemetry {
namespace consts {
    double min_dist_gradient(10);// meters in 10s

    // gradient windows in meters
    int gradient_window_behind(15);
    int gradient_window_ahead(15);
    int gradient_cap_window_behind(gradient_window_behind*4);
    int gradient_cap_window_ahead(gradient_window_ahead*4);
}
namespace helper {
    int km_to_m(double km)
    {
        return static_cast<int>(std::round(km*1000));
    }

}

struct stats {
    double min = 0;
    double max = 0;
    double avg = 0;
    int count = 0;

    void add(const datapoint_ptr& data, EField field) {
        if (data->fields.contains(field)) {
            double val = data->fields.at(field);

            min = count ? std::min(min, val) : val;
            max = count ? std::max(max, val) : val;
            avg = avg * (count++)/count + val / count;
        }
    }
};

datapoint_seq parser::parse(const std::filesystem::path& path)
{
    datapoint_seq seq = parse_impl(path);

    if (!seq.empty()) {
        update_calculated_fields(seq);
        print_stats(seq);
    }

    return std::move(seq);
}

void parser::update_calculated_fields(datapoint_seq& seq)
{
    log.info("Calculating additional data fields");

    datapoint_ptr first = seq.front();
    datapoint_ptr last = first;

    double track_dist = 0;
    std::multimap<int, datapoint_ptr> seq_by_dist;

    for (auto it = seq.begin(); it != seq.end(); it++) {
        datapoint_ptr& data = *it;

        // track distance calculation
        track_dist += utils::geo_distance(
            last->fields.at(EField::Latitude), last->fields.at(EField::Longitude),
            data->fields.at(EField::Latitude), data->fields.at(EField::Longitude)) / 1000.0;

        // set track distance field
        data->fields[EField::TrackDistance] = track_dist;

        // set distance field from track distance if not parsed from file
        if (!data->fields.contains(EField::Distance)) {
            data->fields[EField::Distance] = track_dist;
        }

        if (!data->fields.contains(EField::Speed)) {
            double h = std::chrono::duration_cast<std::chrono::seconds>((data->timestamp - last->timestamp)).count() / 3600;
            double km = data->fields.at(EField::Distance) - last->fields.at(EField::Distance);

            data->fields[EField::Speed] = km / h;
        }

        // setting time averaged fields
        set_if_ok(data, EField::Power3s,        field_avg(std::make_reverse_iterator(it+1), seq.rend(), EField::Power,     3));
        set_if_ok(data, EField::Power10s,       field_avg(std::make_reverse_iterator(it+1), seq.rend(), EField::Power,    10));
        set_if_ok(data, EField::Power30s,       field_avg(std::make_reverse_iterator(it+1), seq.rend(), EField::Power,    30));
        set_if_ok(data, EField::SmoothAltitude, field_avg(std::make_reverse_iterator(it+1), seq.rend(), EField::Altitude, 10));

        // store point into sequence by distance
        seq_by_dist.insert(std::make_pair(helper::km_to_m(data->fields.at(EField::Distance)), data));

        // store last point
        last = data;
    }

    log.info("Calculating gradients");
    for (datapoint_ptr& data : seq) {
        if (!data->fields.contains(EField::Gradient) && data->fields.contains(EField::SmoothAltitude)) {
            int dst_m = helper::km_to_m(data->fields.at(EField::Distance));
            double grad = gradient_between(seq_by_dist, dst_m - consts::gradient_window_behind,     dst_m + consts::gradient_window_ahead);
            double cap  = gradient_between(seq_by_dist, dst_m - consts::gradient_cap_window_behind, dst_m + consts::gradient_cap_window_ahead);

            if (std::abs(grad) > std::abs(cap)) {
                grad = grad*cap > 0 ? cap : -cap; 
                // convert cap to gradient sign if needed
            }

            data->fields[EField::Gradient] = grad*100;//convert to pct
        }
    }
}

void parser::print_stats(datapoint_seq& seq)
{
    stats altitude;
    stats gradient;
    stats speed;
    stats power;
    stats cadence;
    stats respiration;

    for (auto data : seq) {
        altitude.add(data, EField::Altitude);
        gradient.add(data, EField::Gradient);
        speed.add(data, EField::Speed);
        power.add(data, EField::Power);
        cadence.add(data, EField::Cadence);
        respiration.add(data, EField::RespirationRate);
    }

    const datapoint_ptr& first = seq.front();
    const datapoint_ptr& last = seq.back();

    double distance = 0;
    if (last->fields.contains(EField::Distance))
        distance = last->fields.at(EField::Distance);

    double track_distance = 0;
    if (last->fields.contains(EField::TrackDistance))
        track_distance = last->fields.at(EField::TrackDistance);

    auto time = last->timestamp - first->timestamp;
    double hours = std::chrono::duration_cast<std::chrono::seconds>((time)).count() / 3600.0;

    log.info("Telemetry Statistics:");
    log.info("                        Time: {:%H:%M:%S}", time);
    log.info("                    Distance: {:7.2f} km", distance);
    log.info("              Track Distance: {:7.2f} km", track_distance);
    log.info("           Speed Min/Avg/Max: {:7.2f} / {:7.2f} / {:7.2f} km/h", speed.min,       distance/hours,  speed.max);
    log.info("        Altitude Min/Avg/Max: {:7.2f} / {:7.2f} / {:7.2f} m",    altitude.min,    altitude.avg,    altitude.max);
    log.info("        Gradient Min/Avg/Max: {:7.2f} / {:7.2f} / {:7.2f} %",    gradient.min,    gradient.avg,    gradient.max);
    log.info("           Power Min/Avg/Max: {:7.2f} / {:7.2f} / {:7.2f} W",    power.min,       power.avg,       power.max);
    log.info("         Cadence Min/Avg/Max: {:7.2f} / {:7.2f} / {:7.2f} rpm",  cadence.min,     cadence.avg,     cadence.max);
    log.info("Respiration Rate Min/Avg/Max: {:7.2f} / {:7.2f} / {:7.2f} brpm", respiration.min, respiration.avg, respiration.max);
}

double parser::gradient_between(const std::multimap<int, datapoint_ptr>& dist_points, int dist_a, int dist_b)
{
    if (dist_a >= dist_b) {
        return 0;
    }

    double alt_a = get_by_distance(dist_points, dist_a, EField::SmoothAltitude);
    double alt_b = get_by_distance(dist_points, dist_b, EField::SmoothAltitude);

    double grad = (alt_b - alt_a)/(dist_b - dist_a);
    return grad;
}

double parser::get_by_distance(const std::multimap<int, datapoint_ptr>& dist_points, int dist, EField field, bool last)
{
    std::vector<double> res;

    double ret = 0;

    if (dist_points.contains(dist)) {
        auto [it1, it2] = dist_points.equal_range(dist);

        if (last) {
            ret = (--it2)->second->fields.at(field);
        } else {
            ret = it1->second->fields.at(field);
        }
    } else {
        auto it = dist_points.upper_bound(dist);

        if (it == dist_points.end()) {
            ret = (--it)->second->fields.at(field);
        } else {
            ret = it->second->fields.at(field);
        }
    }

    return ret;
}

void parser::set_if_ok(datapoint_ptr& data, EField field, std::optional<double> value)
{
    if (value) {
        data->fields[field] = *value;
    }
}

std::optional<double> parser::field_avg(datapoint_seq::const_reverse_iterator it,
                                        datapoint_seq::const_reverse_iterator rend,
                                        EField field, int count)
{
    int i = 0;
    double total = 0.0;
    for (i=0; it != rend && i<count; it++, i++) {
        if ((*it)->fields.contains(field)) {
            total += (*it)->fields.at(field);
        }
    }

    if (i) {
        return total/i;
    }
    return std::nullopt;
}

} // namespace telemetry
} // namespace vgraph
