#include "parser.h"

namespace vgraph {
namespace telemetry {

std::shared_ptr<datapoint_sequence> parser::parse(const std::filesystem::path& path)
{
    std::shared_ptr<datapoint_sequence> seq = parse_impl(path);

    update_calculated_fields(seq);
    print_stats(seq);

    return seq;
}

void parser::update_calculated_fields(std::shared_ptr<datapoint_sequence>& seq)
{}

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

    auto time = last->timestamp - first->timestamp;
    double hours = std::chrono::duration_cast<std::chrono::seconds>((time)).count() / 3600.0;

    log.info("Telemetry Statistics:");
    log.info("                    Time: {:%H:%M:%S}", time);
    log.info("                Distance: {:7.2f} km", distance);
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
