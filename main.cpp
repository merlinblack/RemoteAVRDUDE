#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unistd.h>
#include "configuration.h"
#include "externalcall.h"

using std::string;
using std::vector;
using std::ostringstream;

using std::exception;
using std::invalid_argument;
using std::cerr;
using std::endl;

using FileCopyList = std::unordered_map<string, string>;
using ArgumentList = vector<string>;

string base_name(string const& path)
{
	return path.substr(path.find_last_of('/') + 1);
}

void handleUpdate(string argument, FileCopyList& filesBefore, FileCopyList& filesAfter, ArgumentList& args)
{
	Configuration& config = getConfiguration();

	// Split argument into components
	// <memtype>:r|w|v:<filename>[:format]
	size_t start = 0;
	size_t pos = argument.find_first_of(':');

	string memtype = argument.substr(start, pos - start);

	start = pos + 1;
	pos = argument.find_first_of(':', start);
	string operation = argument.substr(start, pos - start);

	start = pos + 1;
	pos = argument.find_first_of(':', start);
	string filename = argument.substr(start, pos - start);

	string format;
	if (pos != string::npos)
	{
		start = pos + 1;
		pos = argument.find_first_of(':', start);
		format = ":" + argument.substr(start, pos - start);
	}

	// Depding on operation, add to either before or after copy list and geneerate remote filename
	string remote = config.remoteDir + base_name(filename);

	switch (operation[0])
	{
		case 'w' :
		case 'v':
			filesBefore[filename] = config.hostname + ":" + remote;
			break;
		case 'r':
			filesAfter[config.hostname + ":" + remote] = filename;
			break;
		default:
			throw invalid_argument("Operation not supported: " + operation);
	}

	ostringstream oss;

	oss << "-U" << memtype << ":" << operation << ":" << remote << format;

	args.push_back(oss.str());
}

void handleLogFile(string local, FileCopyList& filesAfter, ArgumentList& args)
{
	Configuration &config = getConfiguration();

	// Add to list of files to copy back and generate remote filename

	string remote = config.remoteDir + base_name(local);

	filesAfter[config.hostname + ":" + remote] = local;

	ostringstream oss;
	oss << "-l" << remote;
	args.push_back(oss.str());
}

int main(int argc, char *argv[])
{
	try {
		Configuration& config = getConfiguration();

		ArgumentList args;
		FileCopyList filesBefore;
		FileCopyList filesAfter;

		while (true)
		{
			int opt = getopt(argc,argv,"?Ab:B:c:C:DeE:Fi:l:np:OP:qstU:uvVx:yY:");
			if (opt == -1)
			{
				break;	// Finished processing arguments
			}

			switch (opt)
			{
			case 'C':
				// Drop it
				break;

			case 'U':
				handleUpdate(optarg, filesBefore, filesAfter, args);
				break;

			case 'l':
				handleLogFile(optarg, filesAfter, args);
				break;

			default:
				{
					// Just put it in the list with any options.
					ostringstream oss;
					oss << "-" << (char)opt;
					if (optarg)
					{
						oss << optarg;
					}
					args.push_back(oss.str());
				}
				break;
			}

		}

		for (auto const& [ from, to ] : filesBefore)
		{
			if (!config.quiet)
			{
				std::cout << from << " -> " << to << endl;
			}
			scp( from, to );
		}

		ostringstream oss;
		oss << config.avrdude;
		for (auto const& arg : args)
		{
			oss << " " << arg;
		}

		if (!config.quiet)
		{
			std::cout << oss.str() << endl;
		}

		ssh_command(config.hostname, oss.str());

		for (auto const& [ from, to ] : filesAfter)
		{
			if (!config.quiet)
			{
				std::cout << from << " -> " << to << endl;
			}
			scp( from, to );
		}
	}
	catch(exception& e)
	{
		std::cerr << "There was a problem: " << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}