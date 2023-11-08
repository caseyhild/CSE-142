#include <iostream>
#include <fstream>
#include "archlab.hpp"
#include <cstring>


#include <stdlib.h>
#include <sstream>      // std::stringstream
#include<time.h>
#include <stdarg.h>


#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>


namespace po = boost::program_options;


po::options_description archlab_cmd_line_options("ArchLab driver");
std::vector<BaseOptionSpec*> options;

po::variables_map archlab_parsed_options;

extern "C" {
	void archlab_parse_cmd_line(int *argc, char *argv[])
	{
		std::vector<std::string> default_stats;
		archlab_cmd_line_options.add_options()
			("help"      , "produce help message");
    
		po::parsed_options parsed = po::command_line_parser(*argc, argv).options(archlab_cmd_line_options).run();
		po::store(parsed, archlab_parsed_options);
		po::notify(archlab_parsed_options);

		for(auto i: options) {
			i->assign(archlab_parsed_options);
		}

		std::vector<std::string> to_pass_further = po::collect_unrecognized(parsed.options, po::include_positional);
    
		if (archlab_parsed_options.count("help")) {
			std::cout << archlab_cmd_line_options << std::endl;
      
			exit(0);
		}

		po::notify(archlab_parsed_options);

		*argc = to_pass_further.size() + 1;
		int c = 1;
		for(auto i: to_pass_further) {
			argv[c++] = strdup(i.c_str());
		}
		if (archlab_parsed_options.count("help")) {
			argv[c++] = strdup("--help");
			(*argc)++;
		}
    
	}

	uint64_t si_parse(const char *s)
	{
		static std::map<std::string, uint64_t> prefixes;
		if (!prefixes.size()) {
			prefixes["ki"] = 1024;
			prefixes["Mi"] = 1024*1024;
			prefixes["Gi"] = 1024*1024*1024;
			prefixes["Ti"] = 1024ull*1024*1024*1024;
			prefixes["k"] = 1000;
			prefixes["M"] = 1000*1000;
			prefixes["G"] = 1000*1000*1000;
			prefixes["T"] = 1000ull*1000*1000*1000;
		}

		char *tail;
		uint64_t b = strtoll(s, &tail, 0);

		auto p = prefixes.find(tail);
		if (*tail == '\0') {
			return b;
		} else if (p != prefixes.end()) {
			return b * p->second;
		} else {
			std::cerr << "unknown suffix: " << tail << std::endl;
			abort();
		}
    
	}

}


void archlab_add_flag(const std::string & name, bool & dest, const bool& def, const std::string & desc) {
	archlab_cmd_line_options.add_options()
		(name.c_str(), desc.c_str());
	options.push_back(new FlagOptionSpec(name, dest));
}
