#include "config.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

Config config;

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

    if (bufsize == -1) {
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

Config::Config()
{

}

Config::~Config()
{

}

void Config::init()
{
    const char *confdir = nullptr;
    char file[255] = {"/ncpamixer.conf"};

    confdir = getenv("XDG_CONFIG_HOME");

    if (confdir == nullptr) {
        snprintf(file, sizeof(file), "/.ncpamixer.conf");
        confdir = getHomeDir();
    }

    snprintf(filename, sizeof(filename), "%s%s", confdir, file);

    for (;;) {
        try {
            cfg.readFile(filename);
            break;
        } catch (const libconfig::FileIOException &fioex) {
            fprintf(
                stderr,
                "Unable to find config file %s, creating default config.\n",
                filename
            );
            createDefault();
            continue;
        } catch (const libconfig::ParseException &pex) {
            fprintf(
                stderr,
                "Parse error at %s:%d - %s\n",
                pex.getFile(),
                pex.getLine(),
                pex.getError()
            );
            exit(EXIT_FAILURE);
        }
    }
}

void Config::createDefault()
{
    FILE *f = fopen(filename, "w");

    if (f) {
        fprintf(
            f,
            "input = {\n"
            "   a = \"test1\";\n"
            "   b = \"test2\";\n"
            "};\n"
        );
        fclose(f);
    } else {
        fprintf(stderr, "Unable to create default config!\n");
        exit(EXIT_FAILURE);
    }
}

const char *Config::getInput(const char *key)
{
    const libconfig::Setting &root = cfg.getRoot();

    try {
        const libconfig::Setting &input = root["input"];
        return input.lookup(key).c_str();
    } catch (const libconfig::SettingNotFoundException &nfex) {
        fprintf(stderr, "Key '%s' not found in input\n", key);
    }

    return nullptr;
}
