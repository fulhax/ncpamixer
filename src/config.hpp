#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <libconfig.h++>

class Config
{
public:
    Config();
    virtual ~Config();

    void init();
    const char *getInput(const char *key);
private:
    const char *getHomeDir();
    void createDefault();
    char filename[255];

    libconfig::Config cfg;
};

extern Config config;

#endif // CONFIG_HPP_
