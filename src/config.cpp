#include "config.hpp"

#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ncurses.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string>

#define MAX_LINE 300

Config config;

std::string Config::getKeycodeName(const char *keycode)
{
    if (keycode == nullptr) return {};
    if (keycode[0] == 0) return {};
    if (memcmp(keycode, KEY, KEY_SIZE) != 0) return {};
    if (strlen(keycode) < KEY_SIZE + 2) return {};
    const auto key_val{keycode + KEY_SIZE + 1};
    const auto is_vt100{key_val[0] == 'f' && key_val[1] == '.'};
    int k{0};
    try { k = std::stoi((is_vt100) ? key_val + 2 : key_val); }
    catch (const std::exception&) { return{}; }
    if (is_vt100) return { "VT100_F" + std::to_string(k - 79) };
    return {keyname(k)};
}

const config_map Config::getConfig() const
{
    return config;
}

const config_map Config::getKeycodeNameEvents() const
{
    config_map keycodes{};

    for (const auto& it : config) {
        const auto kn{getKeycodeName(it.first.c_str())};
        if (kn.empty()) continue;
        keycodes[kn] = it.second;
    }
    return keycodes;
}

bool Config::getDefaultConfigFile()
{
    std::error_code ec{};
    std::string f{FILENAME};
    fs::path config_dir{ getenv(XDG_CONFIG) };
    if (!fs::is_directory(config_dir, ec))
    {
        config_dir = getenv("HOME");
        f.append(".");
    }

    if (fs::is_directory(config_dir, ec))
    {
        full_path = fs::absolute(config_dir / f, ec);
        if (!fs::exists(full_path, ec))
        {
            fprintf(
              stderr,
              "Default file '%s' does not exist, creating one for you ;)\n",
              full_path.c_str()
            );
            return createDefault();
        }
        return true;
    }

    fprintf(
      stderr,
      "Unable to find 'HOME' and/or '%s' folders.\n",
        XDG_CONFIG
    );
    return false;
}

bool Config::getConfigFile(std::optional<fs::path>& conf)
{
    if (!conf.has_value())
    {
        if (getDefaultConfigFile()) return true;
        return false;
    }

    std::error_code ec{};
    const auto config_file{conf.value()};
    if (!fs::exists(config_file, ec))
    {
        fprintf(stderr, "Unable to find config file '%s'.\n", config_file.c_str());
        return false;
    }

    full_path = fs::absolute(config_file, ec);
    return true;
}

void Config::init(std::optional<fs::path>& conf)
{
    if (!getConfigFile(conf)) exit(EXIT_FAILURE);
    if (!readConfig()) exit(EXIT_FAILURE);
}

bool Config::readConfig()
{
    FILE *f = fopen(full_path.c_str(), "rb");
    if (f == nullptr) {
        fprintf(stderr, "Unable to open config file '%s'.\n", full_path.c_str());
        return false;
    }

    while (feof(f) == 0) {
      char line[MAX_LINE] = {0};
      bool instring = false;
      std::string key;
      std::string val;
      std::string *current = &key;

      if (fgets(&line[0], MAX_LINE, f) == nullptr) {
        break;
      }

      char *tmp = &line[0];

      while (*tmp != '\0' && *tmp != '\n' && *tmp != '\r') {
        if (*tmp == '=') {
          current = &val;
        } else if (*tmp == '#') {
          break;
        } else if (*tmp == '"') {
          instring = !instring;
        } else if (*tmp != ' ' || instring) {
          current->append(tmp, 1);
        }

        tmp++;
      }

      if (key.length() > 0) {
        config[key] = val;
      }
    }

    fclose(f);
    return true;
}

std::string Config::getString(const char *key, const std::string &def)
{
    auto conf = config.find(key);

    if (conf == config.end()) {
        config[key] = def;
    }

    return config[key];
}

int Config::getInt(const char *key, int def)
{
    int ret = atoi(getString(key, std::to_string(def)).c_str());
    return ret;
}

bool Config::getBool(const char *key, bool def)
{
    std::string ret = getString(key, (def) ? "true" : "false");
    return (ret == "1" || ret == "yes" || ret == "true");
}

bool Config::keyExists(const char *key) const
{
    auto conf = config.find(key);
    return !(conf == config.end());
}

bool Config::keyEmpty(const char *key)
{
    if (keyExists(key)) {
        return (config[key].empty());
    }

    return true;
}

bool Config::createDefault() const
{
    // light      ░ \u2593
    // medium     ▒ \u2592
    // dark shade ▓ \u2593
    // block      █ \u2588
    // lower      ▁ \u2581
    // higher     ▔ \u2594
    // triangle   ▲ \u25b2
    // https://en.wikipedia.org/wiki/Block_Elements

    FILE *f = fopen(full_path.c_str(), "w");
    if (f == nullptr) {
        fprintf(stderr, "Unable to create default config!\n");
        return false;
    }

    fprintf(
      f,
      "\"theme\" = \"default\"\n"
      "\n"
      "# Default theme {\n"
      "   \"theme.default.static_bar\"             = false\n"
      "   \"theme.default.default_indicator\"      = \"♦ \"\n"
      "   \"theme.default.bar_style.bg\"           = \"░\"\n"
      "   \"theme.default.bar_style.fg\"           = \"█\"\n"
      "   \"theme.default.bar_style.indicator\"    = \"█\"\n"
      "   \"theme.default.bar_style.top\"          = \"▁\"\n"
      "   \"theme.default.bar_style.bottom\"       = \"▔\"\n"
      "   \"theme.default.bar_low.front\"          = 2\n"
      "   \"theme.default.bar_low.back\"           = 0\n"
      "   \"theme.default.bar_mid.front\"          = 3\n"
      "   \"theme.default.bar_mid.back\"           = 0\n"
      "   \"theme.default.bar_high.front\"         = 1\n"
      "   \"theme.default.bar_high.back\"          = 0\n"
      "   \"theme.default.volume_low\"             = 2\n"
      "   \"theme.default.volume_mid\"             = 3\n"
      "   \"theme.default.volume_high\"            = 1\n"
      "   \"theme.default.volume_peak\"            = 1\n"
      "   \"theme.default.volume_indicator\"       = -1\n"
      "   \"theme.default.selected\"               = 2\n"
      "   \"theme.default.default\"                = -1\n"
      "   \"theme.default.border\"                 = -1\n"
      "   \"theme.default.dropdown.selected_text\" = 0\n"
      "   \"theme.default.dropdown.selected\"      = 2\n"
      "   \"theme.default.dropdown.unselected\"    = -1\n"
      "# }\n"
      "# c0r73x theme {\n"
      "   \"theme.c0r73x.static_bar\"             = false\n"
      "   \"theme.c0r73x.default_indicator\"      = \"■ \"\n"
      "   \"theme.c0r73x.bar_style.bg\"           = \"■\"\n"
      "   \"theme.c0r73x.bar_style.fg\"           = \"■\"\n"
      "   \"theme.c0r73x.bar_style.indicator\"    = \"■\"\n"
      "   \"theme.c0r73x.bar_style.top\"          = \"\" \n"
      "   \"theme.c0r73x.bar_style.bottom\"       = \"\" \n"
      "   \"theme.c0r73x.bar_low.front\"          = 0\n"
      "   \"theme.c0r73x.bar_low.back\"           = -1\n"
      "   \"theme.c0r73x.bar_mid.front\"          = 0\n"
      "   \"theme.c0r73x.bar_mid.back\"           = -1\n"
      "   \"theme.c0r73x.bar_high.front\"         = 0\n"
      "   \"theme.c0r73x.bar_high.back\"          = -1\n"
      "   \"theme.c0r73x.volume_low\"             = 6\n"
      "   \"theme.c0r73x.volume_mid\"             = 6\n"
      "   \"theme.c0r73x.volume_high\"            = 6\n"
      "   \"theme.c0r73x.volume_peak\"            = 1\n"
      "   \"theme.c0r73x.volume_indicator\"       = 15\n"
      "   \"theme.c0r73x.selected\"               = 6\n"
      "   \"theme.c0r73x.default\"                = -1\n"
      "   \"theme.c0r73x.border\"                 = -1\n"
      "   \"theme.c0r73x.dropdown.selected_text\" = 0\n"
      "   \"theme.c0r73x.dropdown.selected\"      = 6\n"
      "   \"theme.c0r73x.dropdown.unselected\"    = -1\n"
      "# }\n"
      "# Keybinds {\n"
      "   \"keycode.9\"    = \"switch\"          # tab\n"
      "   \"keycode.13\"   = \"select\"          # enter\n"
      "   \"keycode.27\"   = \"quit\"            # escape\n"
      "   \"keycode.99\"   = \"dropdown\"        # c\n"
      "   \"keycode.113\"  = \"quit\"            # q\n"
      "   \"keycode.109\"  = \"mute\"            # m\n"
      "   \"keycode.100\"  = \"set_default\"     # d\n"
      "   \"keycode.108\"  = \"volume_up\"       # l\n"
      "   \"keycode.104\"  = \"volume_down\"     # h\n"
      "   \"keycode.261\"  = \"volume_up\"       # arrow right\n"
      "   \"keycode.260\"  = \"volume_down\"     # arrow left\n"
      "   \"keycode.107\"  = \"move_up\"         # k\n"
      "   \"keycode.106\"  = \"move_down\"       # j\n"
      "   \"keycode.259\"  = \"move_up\"         # arrow up\n"
      "   \"keycode.258\"  = \"move_down\"       # arrow down\n"
      "   \"keycode.338\"  = \"page_up\"         # page up\n"
      "   \"keycode.339\"  = \"page_down\"       # page down\n"
      "   \"keycode.76\"   = \"tab_next\"        # L\n"
      "   \"keycode.72\"   = \"tab_prev\"        # H\n"
      "   \"keycode.265\"  = \"tab_playback\"    # f1\n"
      "   \"keycode.266\"  = \"tab_recording\"   # f2\n"
      "   \"keycode.267\"  = \"tab_output\"      # f3\n"
      "   \"keycode.268\"  = \"tab_input\"       # f4\n"
      "   \"keycode.269\"  = \"tab_config\"      # f5\n"
      "   \"keycode.f.80\" = \"tab_playback\"    # f1 VT100\n"
      "   \"keycode.f.81\" = \"tab_recording\"   # f2 VT100\n"
      "   \"keycode.f.82\" = \"tab_output\"      # f3 VT100\n"
      "   \"keycode.f.83\" = \"tab_input\"       # f4 VT100\n"
      "   \"keycode.f.84\" = \"tab_config\"      # f5 VT100\n"
      "   \"keycode.71\"   = \"move_last\"       # G\n"
      "   \"keycode.103\"  = \"move_first\"      # g\n"
      "#   \"keycode.48\"   = \"set_volume_100\"  # 0\n"
      "   \"keycode.48\"   = \"set_volume_0\"    # 0\n"
      "   \"keycode.49\"   = \"set_volume_10\"   # 1\n"
      "   \"keycode.50\"   = \"set_volume_20\"   # 2\n"
      "   \"keycode.51\"   = \"set_volume_30\"   # 3\n"
      "   \"keycode.52\"   = \"set_volume_40\"   # 4\n"
      "   \"keycode.53\"   = \"set_volume_50\"   # 5\n"
      "   \"keycode.54\"   = \"set_volume_60\"   # 6\n"
      "   \"keycode.55\"   = \"set_volume_70\"   # 7\n"
      "   \"keycode.56\"   = \"set_volume_80\"   # 8\n"
      "   \"keycode.57\"   = \"set_volume_90\"   # 9\n"
      "   \"keycode.63\"   = \"help\"            # ?\n"
      "# }"
    );

    fclose(f);
    return true;
}
