#include "parser.h"
#include "utils/geo.h"

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

    update_calculated_fields(seq);
    print_stats(seq);

    return seq;
}

void parser::update_calculated_fields(std::shared_ptr<datapoint_sequence>& seq)
{
    log.info("Calculating additional data fields");

    double total_dist = 0;

    std::shared_ptr<datapoint> first = seq->front();

    std::deque<std::shared_ptr<datapoint>> last10s;
    std::deque<std::shared_ptr<datapoint>> last3s;
    std::shared_ptr<datapoint> last;

    for (std::shared_ptr<datapoint>& data : *seq) {
        // handling of moving time windows
        last10s.push_back(data);
        last3s.push_back(data);

        if (last10s.size() > 10) {
            last10s.pop_front();
        }
        if (last3s.size() > 3) {
            last3s.pop_front();
        }

        // handling track distance
        if (last) {
            total_dist += utils::geo_distance(
                              last->fields.at(EField::Latitude),
                              last->fields.at(EField::Longitude),
                              data->fields.at(EField::Latitude),
                              data->fields.at(EField::Longitude)) / 1000.0;
        }

        // handling distance if not parsed from file
        data->fields[EField::TrackDistance] = total_dist;
        if (!data->fields.contains(EField::Distance)) {
            data->fields[EField::Distance] = total_dist;
        }

        // handling average power and smoothed altitude
        std::optional<double> power3s = field_avg(last3s, EField::Power);
        if (power3s) {
            data->fields[EField::Power3s] = *power3s;
        }
        std::optional<double> power10s = field_avg(last10s, EField::Power);
        if (power10s) {
            data->fields[EField::Power10s] = *power10s;
        }
        std::optional<double> s_alt = field_avg(last10s, EField::Altitude);
        if (s_alt) {
            data->fields[EField::SmoothAltitude] = *s_alt;
        }

        // handling speed if not parsed from file
        if (!data->fields.contains(EField::Speed)) {
            double hours = std::chrono::duration_cast<std::chrono::seconds>((data->timestamp - first->timestamp)).count() / 3600.0;
            data->fields[EField::Speed] = data->fields[EField::Distance] / hours;
        }

        bool calculate_gradient = !data->fields.contains(EField::Gradient) &&
                                  data->fields.contains(EField::SmoothAltitude) &&
                                  data->fields.contains(EField::TrackDistance);
        if (calculate_gradient) {
            double dist = 1000 * (last10s.back()->fields[EField::TrackDistance] - last10s.front()->fields[EField::TrackDistance]);
            if (dist > consts::min_dist_gradient) {
                double elev = last10s.back()->fields[EField::SmoothAltitude] - last10s.front()->fields[EField::SmoothAltitude];
                data->fields[EField::Gradient] = utils::gradient(dist, elev);
            }
        }

        last = data;
    }
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

std::optional<double> parser::field_avg(std::deque<std::shared_ptr<datapoint>> points, EField field)
{
    bool ok = false;
    double sum = 0.0;

    for (std::shared_ptr<datapoint>& data : points) {
        if (data->fields.contains(field)) {
            sum += data->fields.at(field);
            ok = true;
        }
    }

    if (ok) {
        return sum / points.size();
    }
    return std::nullopt;
}

} // namespace telemetry
} // namespace vgraph
