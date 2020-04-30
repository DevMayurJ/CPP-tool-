#include <iostream>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;

void init_logging()
{
	logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");

	logging::add_file_log
	(
		keywords::file_name = "sample.log",
		keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%"
	);

	logging::core::get()->set_filter
	(
		logging::trivial::severity >= logging::trivial::trace
	);

	logging::add_common_attributes();
}

int main(int, char*[])
{
	init_logging();

	int i = 10;
	BOOST_LOG_TRIVIAL(trace) << "TESTTTTTTTTT This is a trace severity message" << i;
	BOOST_LOG_TRIVIAL(debug) << "This is a debug severity message";
	BOOST_LOG_TRIVIAL(info) << "This is an informational severity message";
	BOOST_LOG_TRIVIAL(warning) << "This is a warning severity message";
	BOOST_LOG_TRIVIAL(error) << "This is an error severity message";
	BOOST_LOG_TRIVIAL(fatal) << "and this is a fatal severity message";

	std::cout << "Enter any key to exit: ";
	std::cin.get();
	return 0;
}
