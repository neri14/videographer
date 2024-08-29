#include "parser.h"
#include "utils/geo.h"

#include <ranges>

namespace vgraph {
namespace telemetry {
namespace consts {
    double min_dist_gradient(10);// meters in 10s

    // gradient windows in meters
    double gradient_window_behind(0.015);
    double gradient_window_ahead(0.005);
    double gradient_cap_window_behind(gradient_window_behind*4);
    double gradient_cap_window_ahead(gradient_window_ahead*4);
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
        set_if_ok(*it, EField::Power3s,        field_avg(std::make_reverse_iterator(it+1), seq.rend(), EField::Power,     3));
        set_if_ok(*it, EField::Power10s,       field_avg(std::make_reverse_iterator(it+1), seq.rend(), EField::Power,    10));
        set_if_ok(*it, EField::Power30s,       field_avg(std::make_reverse_iterator(it+1), seq.rend(), EField::Power,    30));
        set_if_ok(*it, EField::SmoothAltitude, field_avg(std::make_reverse_iterator(it+1), seq.rend(), EField::Altitude, 10));

        // calculate gradient
        if (!data->fields.contains(EField::Gradient) && data->fields.contains(EField::SmoothAltitude)) {
            double grad = gradient_between(seq, track_dist - consts::gradient_window_behind, track_dist + consts::gradient_window_ahead);
            double cap = gradient_between(seq, track_dist - consts::gradient_cap_window_behind, track_dist + consts::gradient_cap_window_ahead);

            if (std::abs(grad) > std::abs(cap)) {
                grad = grad*cap > 0 ? cap : -cap; 
                // convert cap to gradient sign if needed
            }

            data->fields[EField::Gradient] = grad*100;//convert to pct
        }

        // store last point
        last = data;
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

double parser::gradient_between(const datapoint_seq& seq, double dist_a, double dist_b)
{
    if (dist_a <= dist_b) {
        return 0;
    }

    double alt_a = get_by_distance(seq, dist_a, EField::Altitude).back();
    double alt_b = get_by_distance(seq, dist_b, EField::Altitude).front();

    return (alt_b - alt_a)/((dist_b - dist_a)*1000);
}

std::vector<double> parser::get_by_distance(const datapoint_seq& seq, double dist, EField field)
{
    std::vector<double> res;
    //TODO
    return std::move(res);
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
