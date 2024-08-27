#include "parser.h"
#include "utils/geo.h"
#include <deque>

namespace vgraph {
namespace telemetry {
namespace consts {
    double min_dist_gradient(15);// meters in 15s
}

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

    std::deque<std::shared_ptr<datapoint>> points_15s;
    std::deque<std::shared_ptr<datapoint>> points_10s;
    std::deque<std::shared_ptr<datapoint>> points_3s;
    std::shared_ptr<datapoint> last;

    for (std::shared_ptr<datapoint>& data : *seq) {
        points_15s.push_back(data);
        points_10s.push_back(data);
        points_3s.push_back(data);

        if (points_15s.size() > 10) {
            points_15s.pop_front();
        }
        if (points_10s.size() > 10) {
            points_10s.pop_front();
        }
        if (points_3s.size() > 3) {
            points_3s.pop_front();
        }

        if (last) {
            total_dist += utils::geo_distance(
                              last->fields.at(EField::Latitude),
                              last->fields.at(EField::Longitude),
                              data->fields.at(EField::Latitude),
                              data->fields.at(EField::Longitude)) / 1000.0;
        }

        data->fields[EField::TrackDistance] = total_dist;
        if (!data->fields.contains(EField::Distance)) {
            data->fields[EField::Distance] = total_dist;
        }
        if (!data->fields.contains(EField::Speed)) {
            double hours = std::chrono::duration_cast<std::chrono::seconds>((data->timestamp - first->timestamp)).count() / 3600.0;
            data->fields[EField::Speed] = data->fields[EField::Distance] / hours;
        }
        if (!data->fields.contains(EField::Gradient) && data->fields.contains(EField::Altitude)) {
            double dist = 1000 * (points_15s.back()->fields[EField::Distance] - points_15s.front()->fields[EField::Distance]);
            double elev = points_15s.back()->fields[EField::Altitude] - points_15s.front()->fields[EField::Altitude];
            if (dist > consts::min_dist_gradient) {
                data->fields[EField::Gradient] = utils::gradient(dist, elev);
            }
        }

            EField::Power3s;//calculate if EField::Power available
            EField::Power10s;//calculate if EField::Power available

        last = data;
    }
}

void parser::print_stats(std::shared_ptr<datapoint_sequence>& seq)
{
    double altitude = 0;
    int altitude_cnt =  0;

    double power = 0;
    int power_cnt =  0;

    double cadence = 0;
    int cadence_cnt = 0;

    double heartrate = 0;
    int heartrate_cnt = 0;

    double respiration = 0;
    int respiration_cnt = 0;

    for (auto data : *seq) {
        if (data->fields.contains(EField::Altitude))
            altitude = altitude * (altitude_cnt++)/altitude_cnt + data->fields[EField::Altitude] / altitude_cnt;
        if (data->fields.contains(EField::Power))
            power = power * (power_cnt++)/power_cnt + data->fields[EField::Power] / power_cnt;
        if (data->fields.contains(EField::Cadence))
            cadence = cadence * (cadence_cnt++)/cadence_cnt + data->fields[EField::Cadence] / cadence_cnt;
        if (data->fields.contains(EField::HeartRate))
            heartrate = heartrate * (heartrate_cnt++)/heartrate_cnt + data->fields[EField::HeartRate] / heartrate_cnt;
        if (data->fields.contains(EField::RespirationRate))
            respiration = respiration * (respiration_cnt++)/respiration_cnt + data->fields[EField::RespirationRate] / respiration_cnt;
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
    log.info("                    Time: {:%H:%M:%S}", time);
    log.info("                Distance: {:7.2f} km", distance);
    log.info("          Track Distance: {:7.2f} km", track_distance);
    log.info("           Average Speed: {:7.2f} km/h", distance/hours);
    if (altitude_cnt)
        log.info("        Average Altitude: {:7.2f} m", altitude);
    if (power_cnt)
        log.info("           Average Power: {:7.2f} W", power);
    if (cadence_cnt)
        log.info("         Average Cadence: {:7.2f} rpm", cadence);
    if (heartrate_cnt)
        log.info("      Average Heart Rate: {:7.2f} bpm", heartrate);
    if (respiration_cnt)
        log.info("Average Respiration Rate: {:7.2f} brpm", respiration);
}

} // namespace telemetry
} // namespace vgraph
