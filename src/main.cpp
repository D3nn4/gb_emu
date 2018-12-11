#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include "fileio.hpp"
#include "romloader.hpp"
#include "cpu.hpp"

namespace logging = boost::log;

int main()
{
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::debug);
    logging::add_file_log ("sample_%N.log");
    logging::add_console_log(std::cout,
                             boost::log::keywords::format = ">> %Message%");
    BOOST_LOG_TRIVIAL(debug) << "GBU STARTING DEBUG";
    std::string fileName = "./cpu_instrs/individual/01-special.gb";
    FileIO fileIO;
    RomLoader romLoader(fileIO);
    Cpu cpu(romLoader);
    cpu.launchGame(fileName);
    return 0;
}
