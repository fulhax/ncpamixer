#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <map>
#include <string>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

using config_map = std::map<std::string, std::string>;

class Config
{
public:
    Config() = default;
    ~Config() = default;

    void init(std::optional<fs::path>& conf);

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
    static constexpr char XDG_CONFIG[] = {"XDG_CONFIG_HOME"};
    static constexpr char FILENAME[] = {"ncpamixer.conf"};

    config_map config;
    fs::path full_path{};

    bool createDefault();
    bool readConfig();
    bool getDefaultConfigFile();
    bool getConfigFile(std::optional<fs::path>& conf);
};

extern Config config;

#endif // CONFIG_HPP_
