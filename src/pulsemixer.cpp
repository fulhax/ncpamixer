#include <stdio.h>
#include <getopt.h>
#include <pulse/pulseaudio.h>

#include <vector>

#include "ui/ui.hpp"
#include "pa.hpp"
#include "config.hpp"
#include "version.hpp"

int main(int argc, char *argv[])
{
    static const struct option longOpts[] = {
        { "version", no_argument, 0, 'v' }
    };
    
    int c;
    int longIndex = 0;
    
    while((c = getopt_long(argc, argv, "v", longOpts, &longIndex)) != -1) {
    
        switch(c) {
            case 'v':
                printf("ncpamixer v%s\n", GIT_VERSION);
                printf("Build type: %s\n", BUILD_TYPE);
                printf("Build date: %s\n", BUILD_DATE);
                printf("\n");

                return 0;

            default:
                break;
        }
    }

    config.init();

    pa.init();

    if(ui.init() > 0) {
        ui.run();
    }

    return 0;
}
