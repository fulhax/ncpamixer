#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <pulse/pulseaudio.h>
#include <wordexp.h>
#include <errno.h>

#include <vector>

#include "ui/ui.hpp"
#include "pa.hpp"
#include "config.hpp"
#include "version.hpp"

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
    printf("-h --help                   Print this help screen\n");
    printf("-v --version                Print version info\n");
}

int main(int argc, char *argv[])
{
    static const struct option longOpts[] = {
        { "version", no_argument, 0, 'v' },
        { "help", no_argument, 0, 'h' },
        { "config", required_argument, 0, 'c' },
        { 0, 0, 0, 0 }
    };

    int c;
    int longIndex = 0;

    char conf[PATH_MAX] = {0};

    while ((c = getopt_long(argc, argv, "vhc:", longOpts, &longIndex)) != -1) {

        switch (c) {
            case 'v':
                version();
                return 0;

            case 'h':
                help();
                return 0;

            case 'c':
                if(realpath(optarg, conf) == NULL) {
                    fprintf(
                        stderr,
                        "realpath error code: %d, %s",
                        errno,
                        strerror(errno)
                    );
                }
                break;

            case '?':
                return 0;

            default:
                break;
        }
    }

    config.init(conf);

    pulse.init();

    if (ui.init() > 0) {
        ui.run();
    }

    return 0;
}
