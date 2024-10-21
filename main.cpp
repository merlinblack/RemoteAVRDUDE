#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <unistd.h>
#include "configuration.h"
#include "externalcall.h"

using std::string;
using std::vector;
using std::ostringstream;

using std::exception;
using std::invalid_argument;
using std::filesystem::path;
using std::cout;
using std::cerr;
using std::endl;

using FileCopyList = std::unordered_map<string, string>;
using ArgumentList = vector<string>;

void handle_update(string argument, FileCopyList& filesBefore, FileCopyList& filesAfter, ArgumentList& args)
{
	Configuration& config = get_configuration();

	// Split argument into components
	// <memtype>:r|w|v:<filename>[:format]

	string memtype, operation, filePath, format;

	size_t start = 0;
	size_t end;

	for (auto component : { &memtype, &operation, &filePath })
	{
		end = argument.find_first_of(':', start);
		*component = argument.substr(start, end - start);
		start = end + 1;
	}

	if (end != string::npos)
	{
		format = ":" + argument.substr(start);
	}

	// Depding on operation, add to either before or after copy list and generate remote filename
	string remote = config.remoteDir / path(filePath).filename();

	ostringstream newarg;

	newarg << "-U" << memtype << ":" << operation << ":" << remote << format;

	args.push_back(newarg.str());

	remote = config.hostname + ":" + remote;

	switch (operation[0])
	{
		case 'w' :
		case 'v':
			filesBefore[filePath] = remote;
			break;
		case 'r':
			filesAfter[remote] = filePath;
			break;
		default:
			throw invalid_argument("Operation not supported: " + operation);
	}
}

void handle_log_file(const path& local, FileCopyList& filesAfter, ArgumentList& args)
{
	Configuration &config = get_configuration();

	// Add to list of files to copy back and generate remote filename

	string remote = config.remoteDir / local.filename();

	ostringstream oss;
	oss << "-l" << remote;
	args.push_back(oss.str());

	remote = config.hostname + ":" + remote;
	filesAfter[remote] = local;
}

int main(int argc, char *argv[])
{
	int ret = EXIT_FAILURE;

	try {
		Configuration& config = get_configuration();

		ArgumentList args;
		FileCopyList filesBefore;
		FileCopyList filesAfter;

		while (true)
		{
			// getopt from avrdude source code.
			int opt = getopt(argc,argv,"?Ab:B:c:C:DeE:Fi:l:np:OP:qstU:uvVx:yY:");
			if (opt == -1)
			{
				break;	// Finished processing arguments
			}

			switch (opt)
			{
			case 'C':
				// Drop it - use the standard one on the remote
				break;

			case 'U':
				handle_update(optarg, filesBefore, filesAfter, args);
				break;

			case 'l':
				handle_log_file(optarg, filesAfter, args);
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
				cout << from << " -> " << to << endl;
			}
			scp( from, to );
		}

		ostringstream command;
		command << config.avrdude;
		for (auto const& arg : args)
		{
			command << " " << arg;
		}

		if (!config.quiet)
		{
			cout << config.hostname << ": " << command.str() << endl;
		}

		ret = ssh_command(config.hostname, command.str());

		for (auto const& [ from, to ] : filesAfter)
		{
			if (!config.quiet)
			{
				cout << to << " <- " << from << endl;
			}
			scp( from, to );
		}
	}
	catch(exception& e)
	{
		cerr << "There was a problem: " << e.what() << endl;
	}

	return ret;
}
