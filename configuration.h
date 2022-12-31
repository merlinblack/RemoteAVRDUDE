#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Configuration {
    public:
    bool initialised;
    std::string hostname;
    bool quiet;
    bool clean;
    std::string remoteDir;
    std::string scp;
    std::string ssh;
    std::string avrdude;

    private:
    Configuration() :
        initialised(false),
        hostname(""),
        quiet(false),
        clean(true),
        remoteDir("/tmp"),
        scp("/usr/bin/scp"),
        ssh("/usr/bin/ssh"),
        avrdude("/usr/local/bin/avrdude")
        {}

    friend Configuration& get_configuration();
};

Configuration& get_configuration();

#endif // CONFIG_H