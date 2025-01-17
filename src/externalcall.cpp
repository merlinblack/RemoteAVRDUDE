#include "configuration.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <filesystem>
#include <stdexcept>

using std::string;
using std::vector;
using std::runtime_error;
using std::filesystem::path;
using StringVector = vector<string>;

int call_external(const path& path, const StringVector& args)
{
	vector<char *> cargs;
	string path_str(path.string());

	cargs.reserve(args.size()+1);

	cargs.push_back(path_str.data());

	for(const string& sp: args)
	{
		cargs.push_back(const_cast<string&>(sp).data());
	}

	cargs.push_back(nullptr);

	pid_t pid;;

	pid = fork();
	if (pid == 0) // Child process
	{
		if(-1 == execv(cargs[0], cargs.data()))
		{
			throw runtime_error(string("Could not execute: ") + path_str);
		}
	}

	int status;

	waitpid( pid, &status, 0);

	if (!WIFEXITED(status))
	{
		throw runtime_error(string("Problem executing: ") + path_str);
	}

	return WEXITSTATUS(status);
}

void scp(const string& from, const string& to)
{
	Configuration& config = get_configuration();
	StringVector args;

	args.push_back("-q");
	args.push_back(from);
	args.push_back(to);

	call_external(config.scp, args);
}

int ssh_command(const string& host, const string& command)
{
	Configuration& config = get_configuration();
	StringVector args;

	args.push_back(host);
	args.push_back(command);

	return call_external(config.ssh, args);
}