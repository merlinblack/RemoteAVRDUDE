#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Configuration {
    bool initialised;
    std::string hostname;
    bool quiet;
    bool clean;
    std::string remoteDir;
    std::string scp;
    std::string ssh;

    Configuration() :
        initialised(false),
        hostname(""),
        quiet(false),
        clean(true),
        remoteDir("/tmp"),
        scp("/usr/bin/scp"),
        ssh("/usr/bin/ssh")
        {}
};

Configuration& getConfiguration();

#endif // CONFIG_H