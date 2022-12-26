#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Configuration {
    std::string hostname;
    bool quiet;
    bool clean;
    std::string remoteDir;

    Configuration() : hostname(""), quiet(false), clean(true), remoteDir("/tmp") {}
};

Configuration getConfiguration();

#endif // CONFIG_H