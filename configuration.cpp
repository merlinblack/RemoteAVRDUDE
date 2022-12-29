#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <stdlib.h>

#include "configuration.h"
#include "stringmanip.h"

#ifdef DEBUG
#include <iostream>
#endif

using std::string;
using std::map;
using std::ifstream;
using std::istringstream;
using std::ostringstream;
using std::invalid_argument;
using std::getline;

using OptionMap = map<string,string>;

string getConfigFileName()
{
	string homeDir(getenv("HOME"));

	return homeDir + string("/.local/remote_avrdude.conf");
}

string parseFile(ifstream& file, OptionMap& options)
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

string mapOptionsToConfig(OptionMap& options, Configuration& config)
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

string checkRequiredConfig(Configuration& config)
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

Configuration& getConfiguration()
{
	static Configuration config;

	if (config.initialised)
	{
		return config;
	}

	ifstream configFile(getConfigFileName());

	OptionMap options;
	string problem;

	if (configFile.good())
	{
		problem = parseFile(configFile, options);
	}
	else
	{
		problem = "Could not open file";
	}

	if (problem.empty())
	{
		problem = mapOptionsToConfig(options, config);
	}

	if (problem.empty())
	{
		problem = checkRequiredConfig(config);
	}
			   
	if (!problem.empty()) 
	{
		std::ostringstream ss;
		ss << "There was an error processing the config file " << getConfigFileName() << ", " << problem;
		throw invalid_argument(ss.str());
	}

	config.initialised = true;

	return config;
}