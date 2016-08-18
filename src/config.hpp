#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <map>
#include <string>

class Config
{
public:
    Config();
    virtual ~Config();

    void init(const char* conf);

    std::string getString(const char *key, std::string def);
    int getInt(const char *key, int def);
    bool getBool(const char *key, bool def);
private:
    std::map<std::string, std::string> config;
    char filename[255];

    static const char *getHomeDir();
    static bool fileExists(const char *name);
    void createDefault();
    int readConfig();
};

extern Config config;

#endif // CONFIG_HPP_
