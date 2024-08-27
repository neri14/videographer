#include "fit_parser.h"

namespace vgraph {
namespace telemetry {
namespace consts {
    const double magic_lonlat = 11930465.0;
    const double speed_convert = 3.6;
    const double distance_convert = 1000.0;
    const unsigned int garmin_epoch = 631065600; // seconds since 1970-01-01 00:00:00 to 1989-12-31 00:00:00
}

std::shared_ptr<datapoint_sequence> fit_parser::parse_impl(const std::filesystem::path& path)
{
    log.info("Decoding FIT file {}", path.string());

    if (!std::filesystem::exists(path)) {
        log.error("File does not exist: {}", path.string());
        return nullptr;
    }

    if (path.extension() != ".fit") {
        log.error("File extension is not .fit: {}", path.string());
        return nullptr;
    }

    std::fstream file;
    file.open(path.c_str(), std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        log.error("Error opening file {}", path.string());
        return nullptr;
    }

    std::shared_ptr<datapoint_sequence> ptr = parse_filestream(file);
    if (ptr) {
        log.info("Decoded FIT file {}", path.string());
    }

    return ptr;
}

std::shared_ptr<datapoint_sequence> fit_parser::parse_filestream(std::fstream& file)
{
    fit::Decode decode;
    if (!decode.CheckIntegrity(file)) {
        log.warning("FIT file integrity check failed");
    }

    using std::placeholders::_1;

    fit::MesgBroadcaster msg_broadcaster;
    std::shared_ptr<datapoint_sequence> seq = std::make_shared<datapoint_sequence>();
    listener listen(std::bind(&fit_parser::handle_record, this, _1, seq));

    msg_broadcaster.AddListener(listen);

   try
   {
      decode.Read(file, msg_broadcaster, msg_broadcaster);
   }
   catch (const fit::RuntimeException& e)
   {
      log.error("Exception decoding file: {}", e.what());
      return nullptr;
   }
   catch (...)
   {
	   log.error("Exception decoding file");
      return nullptr;
   }

    return seq;
}

void fit_parser::handle_record(fit::RecordMesg& record, std::shared_ptr<datapoint_sequence> out_seq)
{
    std::shared_ptr<vgraph::telemetry::datapoint> data = std::make_shared<vgraph::telemetry::datapoint>();

    if (record.IsTimestampValid())
        data->timestamp = parse_timestamp(record.GetTimestamp());
    if (record.IsPositionLatValid())
        data->fields[EField::Latitude] = record.GetPositionLat() / consts::magic_lonlat;
    if (record.IsPositionLongValid())
        data->fields[EField::Longitude] = record.GetPositionLong() / consts::magic_lonlat;
    if (record.IsAltitudeValid())
        data->fields[EField::Altitude] = record.GetAltitude();
    if (record.IsGradeValid())
        data->fields[EField::Gradient] = record.GetGrade();
    if (record.IsTemperatureValid())
        data->fields[EField::Temperature] = record.GetTemperature();
    if (record.IsDistanceValid())
        data->fields[EField::Distance] = record.GetDistance() / consts::distance_convert;
    if (record.IsSpeedValid())
        data->fields[EField::Speed] = record.GetSpeed() * consts::speed_convert;
    if (record.IsCadenceValid())
        data->fields[EField::Cadence] = record.GetCadence();
    if (record.IsPowerValid())
        data->fields[EField::Power] = record.GetPower();
    if (record.IsRespirationRateValid())
        data->fields[EField::RespirationRate] = record.GetRespirationRate();
    else if (record.IsEnhancedRespirationRateValid())
        data->fields[EField::RespirationRate] = record.GetEnhancedRespirationRate();
    if (record.IsHeartRateValid())
        data->fields[EField::HeartRate] = record.GetHeartRate();
    if (record.IsGritValid())
        data->fields[EField::Grit] = record.GetGrit();
    if (record.IsFlowValid())
        data->fields[EField::Flow] = record.GetFlow();

    out_seq->push_back(data);
}

// timestamp: seconds since UTC 00:00 Dec 31 1989
std::chrono::time_point<std::chrono::system_clock> fit_parser::parse_timestamp(FIT_UINT32 timestamp)
{
    std::chrono::system_clock::time_point tp{std::chrono::seconds{consts::garmin_epoch + timestamp}};
    return tp;
}

fit_parser::listener::listener(std::function<void(fit::RecordMesg&)> cb) :
    msg_cb(cb)
{}

void fit_parser::listener::OnMesg(fit::RecordMesg& record)
{
    msg_cb(record);
}

} // namespace telemetry
} // namespace vgraph
