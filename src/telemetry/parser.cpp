#include "parser.h"
#include "utils/geo.h"

#include <ranges>

namespace vgraph {
namespace telemetry {
namespace consts {
    double min_dist_gradient(10);// meters in 10s
}

struct stats {
    double min = 0;
    double max = 0;
    double avg = 0;
    int count = 0;

    void add(std::shared_ptr<datapoint>& data, EField field) {
        if (data->fields.contains(field)) {
            double val = data->fields.at(field);

            min = count ? std::min(min, val) : val;
            max = count ? std::max(max, val) : val;
            avg = avg * (count++)/count + val / count;
        }
    }
};

std::shared_ptr<datapoint_sequence> parser::parse(const std::filesystem::path& path)
{
    std::shared_ptr<datapoint_sequence> seq = parse_impl(path);

    if (seq) {
        update_calculated_fields(seq);
        print_stats(seq);
    }

    return seq;
}

void parser::update_calculated_fields(std::shared_ptr<datapoint_sequence>& seq)
{
    log.info("Calculating additional data fields");

    std::shared_ptr<datapoint> first = seq->front();
    std::shared_ptr<datapoint> last = first;

    double track_dist = 0;

    for (auto it = seq->begin(); it != seq->end(); it++) {
        std::shared_ptr<datapoint>& data = *it;

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
        set_if_ok(data, EField::Power3s, field_avg(std::make_reverse_iterator(it+1), seq->rend(), EField::Power, 3));
        set_if_ok(data, EField::Power10s, field_avg(std::make_reverse_iterator(it+1), seq->rend(), EField::Power, 10));
        set_if_ok(data, EField::Power30s, field_avg(std::make_reverse_iterator(it+1), seq->rend(), EField::Power, 30));
        set_if_ok(data, EField::SmoothAltitude, field_avg(std::make_reverse_iterator(it+1), seq->rend(), EField::Altitude, 10));

        // store last point
        last = data;
    }

    // for (std::shared_ptr<datapoint>& data : *seq) {
        // handling gradient
        // bool calculate_gradient = !data->fields.contains(EField::Gradient) &&
        //                           data->fields.contains(EField::SmoothAltitude) &&
        //                           data->fields.contains(EField::TrackDistance);
        // if (calculate_gradient) {
        //     double dist = 1000 * (last10s.back()->fields[EField::TrackDistance] - last10s.front()->fields[EField::TrackDistance]);
        //     if (dist > consts::min_dist_gradient) {
        //         double elev = last10s.back()->fields[EField::SmoothAltitude] - last10s.front()->fields[EField::SmoothAltitude];
        //         data->fields[EField::Gradient] = utils::gradient(dist, elev);
        //     }
        // }
    // }
}

void parser::print_stats(std::shared_ptr<datapoint_sequence>& seq)
{
    stats altitude;
    stats gradient;
    stats speed;
    stats power;
    stats cadence;
    stats heartrate;
    stats respiration;

    for (auto data : *seq) {
        altitude.add(data, EField::Altitude);
        gradient.add(data, EField::Gradient);
        speed.add(data, EField::Speed);
        power.add(data, EField::Power);
        cadence.add(data, EField::Cadence);
        heartrate.add(data, EField::HeartRate);
        respiration.add(data, EField::RespirationRate);
    }

    auto first = seq->front();
    auto last = seq->back();

    double distance = 0;
    if (last->fields.contains(EField::Distance))
        distance = last->fields[EField::Distance];

    double track_distance = 0;
    if (last->fields.contains(EField::TrackDistance))
        track_distance = last->fields[EField::TrackDistance];

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
    log.info("      Heart Rate Min/Avg/Max: {:7.2f} / {:7.2f} / {:7.2f} bpm",  heartrate.min,   heartrate.avg,   heartrate.max);
    log.info("Respiration Rate Min/Avg/Max: {:7.2f} / {:7.2f} / {:7.2f} brpm", respiration.min, respiration.avg, respiration.max);
}

void parser::set_if_ok(std::shared_ptr<datapoint>& data, EField field, std::optional<double> value)
{
    if (value) {
        data->fields[field] = *value;
    }
}

std::optional<double> parser::field_avg(datapoint_sequence::reverse_iterator it, datapoint_sequence::reverse_iterator rend, EField field, int count)
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
