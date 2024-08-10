#include "utils/logging/logger.h"
#include "utils/logging/stream_sink.h"
#include "utils/logging/log_level.h"
#include "utils/logging/backend.h"

#include <gtest/gtest.h>

namespace vgraph {
namespace utils {
namespace logging {

namespace consts {
    const std::string logger_name("test");

    const std::string log_msg("this is a test log");
    const std::string log_msg_with_args("this is a test log {} {} {:.3f}");

    const std::string log_arg1 = "abc";
    const int log_arg2 = 123;
    const double log_arg3 = 1.234567;

    const std::string expected_debug("[DBG] test: this is a test log\n");
    const std::string expected_info("[INF] test: this is a test log\n");
    const std::string expected_warning("[WRN] test: this is a test log\n");
    const std::string expected_error("[ERR] test: this is a test log\n");

    const std::string expected_debug_args("[DBG] test: this is a test log abc 123 1.235\n");
    const std::string expected_info_args("[INF] test: this is a test log abc 123 1.235\n");
    const std::string expected_warning_args("[WRN] test: this is a test log abc 123 1.235\n");
    const std::string expected_error_args("[ERR] test: this is a test log abc 123 1.235\n");

    const std::string empty_string("");
} // namespace consts

class logger_test : public ::testing::Test {
protected:
    void SetUp() override
    {
        sink = std::make_shared<stream_sink>(sink_stream);
    }

    void TearDown() override
    {
        backend::get_instance().remove_sink(sink);
        sink = nullptr;
        sink_stream.str("");
    }

    void reset_stream()
    {
        sink_stream.str("");
    }

    std::shared_ptr<stream_sink> sink;
    std::ostringstream sink_stream;
};

/* test simple logs */
TEST_F(logger_test, debug_sink_receives_all_logs)
{
    backend::get_instance().add_sink(ELogLevel::Debug, sink);
    logger uut(consts::logger_name);

    uut.debug(consts::log_msg);
    ASSERT_EQ(consts::expected_debug, sink_stream.str());

    reset_stream();

    uut.info(consts::log_msg);
    ASSERT_EQ(consts::expected_info, sink_stream.str());

    reset_stream();

    uut.warning(consts::log_msg);
    ASSERT_EQ(consts::expected_warning, sink_stream.str());

    reset_stream();

    uut.error(consts::log_msg);
    ASSERT_EQ(consts::expected_error, sink_stream.str());
}

TEST_F(logger_test, info_sink_receives_info_warning_error_logs_only)
{
    backend::get_instance().add_sink(ELogLevel::Info, sink);
    logger uut(consts::logger_name);

    uut.debug(consts::log_msg);
    ASSERT_EQ(consts::empty_string, sink_stream.str());

    reset_stream();

    uut.info(consts::log_msg);
    ASSERT_EQ(consts::expected_info, sink_stream.str());

    reset_stream();

    uut.warning(consts::log_msg);
    ASSERT_EQ(consts::expected_warning, sink_stream.str());

    reset_stream();

    uut.error(consts::log_msg);
    ASSERT_EQ(consts::expected_error, sink_stream.str());
}

TEST_F(logger_test, warning_sink_receives_warning_error_logs_only)
{
    backend::get_instance().add_sink(ELogLevel::Warning, sink);
    logger uut(consts::logger_name);

    uut.debug(consts::log_msg);
    ASSERT_EQ(consts::empty_string, sink_stream.str());

    reset_stream();

    uut.info(consts::log_msg);
    ASSERT_EQ(consts::empty_string, sink_stream.str());

    reset_stream();

    uut.warning(consts::log_msg);
    ASSERT_EQ(consts::expected_warning, sink_stream.str());

    reset_stream();

    uut.error(consts::log_msg);
    ASSERT_EQ(consts::expected_error, sink_stream.str());
}

TEST_F(logger_test, error_sink_receives_error_logs_only)
{
    backend::get_instance().add_sink(ELogLevel::Error, sink);
    logger uut(consts::logger_name);

    uut.debug(consts::log_msg);
    ASSERT_EQ(consts::empty_string, sink_stream.str());

    reset_stream();

    uut.info(consts::log_msg);
    ASSERT_EQ(consts::empty_string, sink_stream.str());

    reset_stream();

    uut.warning(consts::log_msg);
    ASSERT_EQ(consts::empty_string, sink_stream.str());

    reset_stream();

    uut.error(consts::log_msg);
    ASSERT_EQ(consts::expected_error, sink_stream.str());
}

/* test logs with arguments */
TEST_F(logger_test, debug_sink_receives_all_logs_with_args)
{
    backend::get_instance().add_sink(ELogLevel::Debug, sink);
    logger uut(consts::logger_name);

    uut.debug(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::expected_debug_args, sink_stream.str());

    reset_stream();

    uut.info(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::expected_info_args, sink_stream.str());

    reset_stream();

    uut.warning(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::expected_warning_args, sink_stream.str());

    reset_stream();

    uut.error(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::expected_error_args, sink_stream.str());
}

TEST_F(logger_test, info_sink_receives_info_warning_error_logs_only_with_args)
{
    backend::get_instance().add_sink(ELogLevel::Info, sink);
    logger uut(consts::logger_name);

    uut.debug(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::empty_string, sink_stream.str());

    reset_stream();

    uut.info(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::expected_info_args, sink_stream.str());

    reset_stream();

    uut.warning(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::expected_warning_args, sink_stream.str());

    reset_stream();

    uut.error(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::expected_error_args, sink_stream.str());
}

TEST_F(logger_test, warning_sink_receives_warning_error_logs_only_with_args)
{
    backend::get_instance().add_sink(ELogLevel::Warning, sink);
    logger uut(consts::logger_name);

    uut.debug(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::empty_string, sink_stream.str());

    reset_stream();

    uut.info(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::empty_string, sink_stream.str());

    reset_stream();

    uut.warning(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::expected_warning_args, sink_stream.str());

    reset_stream();

    uut.error(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::expected_error_args, sink_stream.str());
}

TEST_F(logger_test, error_sink_receives_error_logs_only_with_args)
{
    backend::get_instance().add_sink(ELogLevel::Error, sink);
    logger uut(consts::logger_name);

    uut.debug(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::empty_string, sink_stream.str());

    reset_stream();

    uut.info(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::empty_string, sink_stream.str());

    reset_stream();

    uut.warning(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::empty_string, sink_stream.str());

    reset_stream();

    uut.error(consts::log_msg_with_args, consts::log_arg1, consts::log_arg2, consts::log_arg3);
    ASSERT_EQ(consts::expected_error_args, sink_stream.str());
}

} // namespace logging
} // namespace utils
} // namespace vgraph
