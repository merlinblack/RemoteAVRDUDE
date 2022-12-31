#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <stdlib.h>

#include "configuration.h"
#include "stringmanip.h"

#ifdef DEBUG
#include <iostream>
#endif

using std::string;
using std::unordered_map;
using std::ifstream;
using std::istringstream;
using std::ostringstream;
using std::invalid_argument;
using std::getline;

using OptionMap = unordered_map<string,string>;

string get_config_filename()
{
	string homeDir(getenv("HOME"));

	return homeDir + string("/.local/remote_avrdude.conf");
}

string parse_file(ifstream& file, OptionMap& options)
{
	string key, value, problem;
	char buffer[255];
	int lineCount = 0;

	for (string line; getline(file, line);)
	{
		lineCount++;
		if (line[0] == '#' || line.size() == 0)
		{
			continue; // Commented out or blank line
		}

		istringstream iss(line);

		iss.getline(buffer, 255, '=');

		if (iss.fail())
		{
			problem = "Problem with a key on line: " + lineCount;
		}
		else
		{
			key = buffer;
			iss.getline(buffer, 255);
			if (iss.fail())
			{
				ostringstream ss;
				ss << "Problem with a value for key: " << key << "on line: " << lineCount;
				problem = ss.str();
				break;
			}
			else
			{
				value = buffer;

				str_trim(key);
				str_trim(value);

				options[key] = value;
			}
		}
	}

	return problem;
}

string map_options_to_config(OptionMap& options, Configuration& config)
{
	string problem;

	class Option {
		public:
		string key;
		string *strptr;
		bool *boolptr;
	};

	Option availableOptions[] = {
		{ "hostname", &config.hostname },
		{ "quiet",    nullptr, &config.quiet },
		{ "clean",    nullptr, &config.clean },
		{ "scp",      &config.scp },
		{ "ssh",      &config.ssh },
		{ "avrdude",  &config.avrdude },
		{ "remote_directory", &config.remoteDir },
	};

	try
	{
		for(auto& opt : availableOptions)
		{
			if (options.count(opt.key))
			{
				if (opt.boolptr)
				{
					*opt.boolptr = str2bool(options[opt.key]);
				}
				else
				{
					*opt.strptr = options[opt.key];
				}                
			}
		}
	}
	catch (invalid_argument &e)
	{
		problem = e.what();
	}

	return problem;
}

string check_required_config(Configuration& config)
{
	string problem;
	
	if (config.hostname.size() == 0)
	{
		problem = "Missing hostname from configuration.";
	}

	// Make sure remote dir has a '/' on the end.
	if (*(config.remoteDir.rbegin()) != '/')
	{
		config.remoteDir.append("/");
	}

	return problem;
}

Configuration& get_configuration()
{
	static Configuration config;

	if (config.initialised)
	{
		return config;
	}

	ifstream configFile(get_config_filename());

	OptionMap options;
	string problem;

	if (configFile.good())
	{
		problem = parse_file(configFile, options);
	}
	else
	{
		problem = "Could not open file";
	}

	if (problem.empty())
	{
		problem = map_options_to_config(options, config);
	}

	if (problem.empty())
	{
		problem = check_required_config(config);
	}
			   
	if (!problem.empty()) 
	{
		std::ostringstream ss;
		ss << "There was an error processing the config file " << get_config_filename() << ", " << problem;
		throw invalid_argument(ss.str());
	}

	config.initialised = true;

	return config;
}