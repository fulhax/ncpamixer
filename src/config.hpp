#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <map>
#include <string>
#include <optional>

using config_map = std::map<std::string, std::string>;

class Config
{
public:
    Config();
    virtual ~Config() = default;

    void init(const char *conf);

    std::string getString(const char *key, const std::string &def);
    int getInt(const char *key, int def);
    bool getBool(const char *key, bool def);
    bool keyExists(const char *key);
    bool keyEmpty(const char *key);
    const config_map getConfig() const;
    const config_map getKeycodeNameEvents() const;
    static std::string getKeycodeName(const char *keycode);
private:
    static constexpr char KEY[] = {"keycode"};
    static constexpr auto KEY_SIZE{sizeof (KEY) - 1};
    config_map config;
    char filename[255];

    static const char *getHomeDir();
    static bool fileExists(const char *name);
    void createDefault();
    int readConfig();
};

extern Config config;

#endif // CONFIG_HPP_
