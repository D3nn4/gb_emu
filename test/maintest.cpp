#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
namespace logging = boost::log;
void init_logging()
{
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::warning);
    logging::add_file_log ("sample_%N.log");
    // logging::add_console_log(std::cout,
    //                          boost::log::keywords::format = ">> %Message%");

}
int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    init_logging();
    int returnValue;

    //Do whatever setup here you will need for your tests here
    //
    //

    returnValue =  RUN_ALL_TESTS();

    //Do Your teardown here if required
    //
    //

    return returnValue;
}
