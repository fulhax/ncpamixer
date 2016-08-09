#include <stdio.h>
#include <pulse/pulseaudio.h>

#include <vector>

#include "ui.hpp"
#include "pa.hpp"

int main(void)
{
    Ui ui;

    if(ui.init() > 0) {
        ui.run();
    }

    return 0;
}
