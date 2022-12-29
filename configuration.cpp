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

	try
	{
		if (options.count("hostname") > 0)
		{
			config.hostname = options["hostname"];
		}

		if (options.count("quiet") > 0)
	    {
	        config.quiet = str2bool(options["quiet"]);
	    }

	    if (options.count("clean") > 0)
	    {
	        config.clean = str2bool(options["clean"]);
	    }

		if (options.count("remote_directory") > 0)
		{
			config.remoteDir = options["remote_directory"];
		}

		if (options.count("scp") > 0)
		{
			config.scp = options["scp"];
		}

		if (options.count("ssh") > 0)
		{
			config.ssh = options["ssh"];
		}

		if (options.count("avrdude") > 0)
		{
			config.avrdude = options["avrdude"];
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