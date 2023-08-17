#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <pulse/pulseaudio.h>
#include <wordexp.h>
#include <errno.h>
#include <locale.h>
#include <filesystem>
#include <optional>

#include <vector>

#include "ui/ui.hpp"
#include "pa.hpp"
#include "config.hpp"
#include "version.hpp"

namespace fs = std::filesystem;

void version()
{
    if (strlen(GIT_VERSION) > 0) {
        printf("ncpamixer git v%s\n", GIT_VERSION);
    } else {
        printf("ncpamixer v%s\n", FALLBACK_VERSION);
    }

    printf("Build type: %s\n", BUILD_TYPE);
    printf("Build date: %s\n", BUILD_DATE);
    printf("\n");

}

void help()
{
    version();

    printf("-c --config=CONFIG_FILE     Set custom location for config\n");
    printf("-t --tab=TAB                Open on given tab. Choices: "
           "(p)layback (default), (r)ecording, (o)utput, (i)nput, (c)onfiguration\n");
    printf("-h --help                   Print this help screen\n");
    printf("-v --version                Print version info\n");
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");

    static const struct option longOpts[] = {
        { "version", no_argument, 0, 'v' },
        { "help", no_argument, 0, 'h' },
        { "config", required_argument, 0, 'c' },
        { "tab", required_argument, 0, 't' },
        { 0, 0, 0, 0 }
    };

    int c;
    int longIndex = 0;
    int startingTab = TAB_PLAYBACK;

    std::optional<fs::path> config_path{std::nullopt};

    while ((c = getopt_long(argc, argv, "vhc:t:", longOpts, &longIndex)) != -1) {

        switch (c) {
            case 'v':
                version();
                return 0;

            case 'h':
                help();
                return 0;

            case 'c':
                config_path = optarg;
                break;

            case 't':
                switch (optarg[0]) {
                    case 'p':
                        break;
                    case 'r':
                        startingTab = TAB_RECORDING;
                        break;
                    case 'o':
                        startingTab = TAB_OUTPUT;
                        break;
                    case 'i':
                        startingTab = TAB_INPUT;
                        break;
                    case 'c':
                        startingTab = TAB_CONFIGURATION;
                        break;
                    default:
                        fprintf(stderr, "invalid tab: %s\n", optarg);
                        return 1;

                }
                break;
            case '?':
                return 0;

            default:
                break;
        }
    }

    config.init(config_path);

    pulse.init();

    if (ui.init(startingTab) > 0) {
        ui.run();
    }

    return 0;
}
