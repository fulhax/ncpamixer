#include <stdio.h>
#include <pulse/pulseaudio.h>

#include <vector>

#include "ui/ui.hpp"
#include "pa.hpp"
#include "config.hpp"

int main(void)
{
    config.init();

    Ui ui;

    pa.init();

    if(ui.init() > 0) {
        ui.run();
    }

    return 0;
}
