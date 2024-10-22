#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <filesystem>

class Configuration {
    public:
    bool initialised;
    std::string hostname;
    bool quiet;
    bool clean;
    std::filesystem::path remoteDir;
    std::filesystem::path scp;
    std::filesystem::path ssh;
    std::filesystem::path avrdude;

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