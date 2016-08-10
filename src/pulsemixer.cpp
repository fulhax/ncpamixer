#include <stdio.h>
#include <pulse/pulseaudio.h>

#include <vector>

#include "ui/ui.hpp"
#include "pa.hpp"
#include "config.hpp"

int main(void)
{
    Ui ui;

    config.init();

    // Config test
    const char* ev = config.getInput("a");
    printf("Get %s from config\n", ev);
    ev = config.getInput("b");
    printf("Get %s from config\n", ev);
    ev = config.getInput("c");
    printf("Get %s from config\n", ev);
    // End Config test

    if(ui.init() > 0) {
        ui.run();
    }

    return 0;
}
