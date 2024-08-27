#ifndef FIT_PARSER_H
#define FIT_PARSER_H

#include "parser.h"

#include "utils/logging/logger.h"

#include <fit_decode.hpp>
#include <fit_mesg_broadcaster.hpp>

#include <fstream>
#include <functional>

namespace vgraph {
namespace telemetry {

class fit_parser : public parser {
public:
    fit_parser() = default;
    ~fit_parser() = default;

private:
    utils::logging::logger log{"fit_parser"};

    std::shared_ptr<datapoint_sequence> parse_impl(const std::filesystem::path& path) override;
    std::shared_ptr<datapoint_sequence> parse_filestream(std::fstream& file);
    void handle_record(fit::RecordMesg& record, std::shared_ptr<datapoint_sequence> out_seq);
    std::chrono::time_point<std::chrono::system_clock> parse_timestamp(FIT_UINT32 timestamp);

    class listener : public fit::RecordMesgListener
    {
    public:
        listener(std::function<void(fit::RecordMesg&)> cb);
        void OnMesg(fit::RecordMesg& record) override;
    private:
        std::function<void(fit::RecordMesg&)> msg_cb;
    };
};

} // namespace telemetry
} // namespace vgraph

#endif // FIT_PARSER_H
