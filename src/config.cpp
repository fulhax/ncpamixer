#include "config.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <string.h>

#include <string>

#define MAX_LINE 300

Config config;

Config::Config()
{
    memset(filename, 0, sizeof(filename));
}

Config::~Config()
{

}

const char *Config::getHomeDir()
{
    const char *homedir = getenv("HOME");

    if (homedir != nullptr) {
        return homedir;
    }

    passwd pwd;
    passwd *result;
    char *buf;
    size_t bufsize;
    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);

    if (bufsize == (size_t)(-1)) {
        bufsize = 16384;
    }

    buf = new char[bufsize];
    getpwuid_r(getuid(), &pwd, buf, bufsize, &result);

    if (result == nullptr) {
        fprintf(stderr, "Unable to find home-directory\n");
        exit(EXIT_FAILURE);
    }

    delete [] buf;
    homedir = result->pw_dir;

    return homedir;
}

bool Config::fileExists(const char *name)
{
    struct stat buffer;
    return (stat(name, &buffer) == 0);
}

void Config::init(const char *conf)
{
    if (strlen(conf) != 0) {
        if (fileExists(conf)) {
            snprintf(filename, sizeof(filename), "%s", conf);
        } else {
            fprintf(stderr, "Unable to find config file %s.\n", conf);
            exit(EXIT_FAILURE);
        }
    }

    if (strlen(filename) == 0) {
        const char *confdir = nullptr;
        char file[255] = {"/ncpamixer.conf"};

        confdir = getenv("XDG_CONFIG_HOME");

        if (confdir == nullptr) {
            snprintf(file, sizeof(file), "/.ncpamixer.conf");
            confdir = getHomeDir();
        }

        snprintf(filename, sizeof(filename), "%s%s", confdir, file);
    }

    for (;;) {
        if (!readConfig()) {
            fprintf(
                stderr,
                "Unable to find config file %s, creating default config.\n",
                filename
            );

            createDefault();
        } else {
            break;
        }
    }
}

int Config::readConfig()
{
    FILE *f = fopen(filename, "rb");

    if (f) {
        while (!feof(f)) {
            char line[MAX_LINE] = {0};
            bool instring = false;
            std::string key;
            std::string val;
            std::string *current = &key;

            if (fgets(line, MAX_LINE, f) == nullptr) {
                break;
            }

            char *tmp = line;

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

            if (key.length() > 0 && val.length() > 0) {
                config[key] = val;
            }
        }

        fclose(f);

        return 1;
    }

    return 0;
}

std::string Config::getString(const char *key, std::string def)
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

    if (ret == "1" || ret == "yes" || ret == "true") {
        return true;
    }

    return false;
}

void Config::createDefault()
{
    FILE *f = fopen(filename, "w");

    if (f) {
        fprintf(
            f,
            "\"theme\" = \"default\"\n"
            "\n"
            "# Default theme {\n"
            "   \"theme.default.bar_low\"                = 2\n"
            "   \"theme.default.bar_mid\"                = 3\n"
            "   \"theme.default.bar_high\"               = 1\n"
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
            "# }"
        );
        fprintf(
            f,
            "# Keybinds {\n"
            "   \"keycode.9\"    = \"switch\"          # tab\n"
            "   \"keycode.13\"   = \"select\"          # enter\n"
            "   \"keycode.27\"   = \"quit\"            # escape\n"
            "   \"keycode.99\"   = \"dropdown\"        # c\n"
            "   \"keycode.113\"  = \"quit\"            # q\n"
            "   \"keycode.109\"  = \"mute\"            # m\n"
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
            "# }"
        );
        fclose(f);
    } else {
        fprintf(stderr, "Unable to create default config!\n");
        exit(EXIT_FAILURE);
    }
}
