#include <iostream>
#include "config.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string>
#include <vector>

using StringVector = std::vector<std::string>;

int callExternal(std::string& path, StringVector& args)
{
	std::vector<char *> cargs;

	cargs.reserve(args.size()+1);

	cargs.push_back(path.data());

	for(std::string& sp: args)
	{
		cargs.push_back(sp.data());
	}

	cargs.push_back(nullptr);

	pid_t pid;;

	pid = fork();
	if (pid == 0) // Child process
	{
		if(-1 == execv(cargs[0], cargs.data()))
		{
			throw std::runtime_error(std::string("Could not execute: ") + path);
		}
	}

	int status;

	waitpid( pid, &status, 0);

	if (!WIFEXITED(status))
	{
		throw std::runtime_error(std::string("Problem executing: ") + path);
	}

	return WEXITSTATUS(status);
}

void scp(const std::string& from, const std::string& to)
{
	Configuration& config = getConfiguration();
	StringVector args;
	args.push_back(from);
	args.push_back(to);

	callExternal(config.scp, args);
}

int main(int argc, char *argv[])
{
	try {
		Configuration& config = getConfiguration();
		scp("../main.cpp","./main.cpp");
	}
	catch(std::exception& e)
	{
		std::cerr << "There was a problem: " << e.what() << std::endl;
	}
}