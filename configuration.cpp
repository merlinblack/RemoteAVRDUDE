#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <variant>
#include <filesystem>
#include <stdlib.h>

#include "configuration.h"
#include "stringmanip.h"

#ifdef DEBUG
#include <iostream>
#endif

using std::string;
using std::variant;
using std::holds_alternative;
using std::get;
using std::unordered_map;
using std::ifstream;
using std::istringstream;
using std::ostringstream;
using std::invalid_argument;
using std::getline;
using std::filesystem::path;

using OptionMap = unordered_map<string,string>;

string get_config_filename()
{
	path configDir;

	if (getenv("XDG_CONFIG_HOME"))
	{
		configDir = getenv("XDG_CONFIG_HOME");
	}
	else
	{
		configDir = getenv("HOME");
		configDir /= ".config";
	}

	return configDir / "remote_avrdude.conf";
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
		variant<string*,bool*,path*> ptr;
	};

	Option availableOptions[] = {
		{ "hostname", &config.hostname },
		{ "quiet",    &config.quiet },
		{ "clean",    &config.clean },
		{ "scp",      &config.scp },
		{ "ssh",      &config.ssh },
		{ "avrdude",  &config.avrdude },
		{ "remote_directory", &config.remoteDir },
	};

	try
	{
		for(const auto& opt : availableOptions)
		{
			if (options.count(opt.key))
			{
				if (holds_alternative<bool*>(opt.ptr))
				{
					*get<bool*>(opt.ptr) = str2bool(options[opt.key]);
				}
				else if (holds_alternative<path*>(opt.ptr))
				{
					*get<path*>(opt.ptr) = options[opt.key];
				}
				else
				{
					*get<string*>(opt.ptr) = options[opt.key];
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
