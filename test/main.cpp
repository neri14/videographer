#include <gtest/gtest.h>

#include "utils/logging/backend.h"
#include "utils/logging/stream_sink.h"

int main(int argc, char **argv)
{
    vgraph::utils::logging::backend::get_instance().add_sink(
        std::make_shared<vgraph::utils::logging::stream_sink>(std::cout));

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
