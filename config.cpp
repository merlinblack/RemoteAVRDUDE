
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <stdlib.h>

#include "config.h"

#ifdef DEBUG
#include <iostream>
#endif

std::string getConfigFileName()
{
    std::string homeDir(getenv("HOME"));

    return homeDir + std::string("/.local/remote_avrdude.conf");
}

void trim(std::string& str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch){return !std::isspace(ch);}));
    str.erase(std::find_if(str.rbegin(), str.rend(),[](unsigned char ch){return !std::isspace(ch);}).base(),str.end());
}

bool str2bool(std::string str)
{
    auto result = false;    // failure to assert is false

    std::istringstream is(str);
    // first try simple integer conversion
    is >> result;

    if (is.fail())
    {
        // simple integer failed; try boolean
        is.clear();
        is >> std::boolalpha >> result;
    }

    if (is.fail())
    {
        throw std::invalid_argument(str.append(" is not convertable to bool"));
    }

    return result;
}

Configuration getConfiguration() 
{
    std::ifstream configFile(getConfigFileName());
    std::map<std::string, std::string> options;
	bool error = false;
	std::string problem;

    if (configFile.good()) {
        std::string key, value;
        char buffer[255];

        for (std::string line; std::getline(configFile, line);)
        {
            if (line[0] == '#' || line.size() == 0)
            {
                continue; // Commented out or blank line
            }

            std::istringstream iss(line);

            iss.getline(buffer, 255, '=');

            if(iss.fail())
            {
                error = true;
                problem = "Problem with a key.";
            }
            else
            {
                key = buffer;
                iss.getline(buffer,255);
                if(iss.fail())
                {
                    error = true;
                    problem = "Problem with a value for key: " + key;
                }
                else
                {
                    value = buffer;
                }
            }

			trim(key);
			trim(value);

            options[key] = value;
        }
    }

#ifdef DEBUG
        for(auto const& [key, value] : options)
        {
            std::cout << key << " = " << value << std::endl;
        }
#endif
	
    Configuration config;

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
	}
	catch (std::invalid_argument &e)
	{
		error = true;
		problem = e.what();
	}

    // Check for required option
    if (config.hostname.size() == 0)
    {
		error = true;
        problem = "Missing hostname from configuration.";
    }
			   
    if (error) 
    {
		std::ostringstream ss;
        ss << "There was an error processing the config file " << getConfigFileName() << std::endl << problem;
        throw std::invalid_argument(ss.str());
    }

    return config;
}