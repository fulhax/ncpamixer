master: [![Build Status](http://fredrik.fulhax.nu:8090/job/ncpamixer/badge/icon)](http://fredrik.fulhax.nu:8090/job/ncpamixer)
dev:    [![Build Status](http://fredrik.fulhax.nu:8090/job/ncpamixer-dev/badge/icon)](http://fredrik.fulhax.nu:8090/job/ncpamixer-dev)

# Ncurses PulseAudio Mixer

An ncurses mixer for PulseAudio inspired by pavucontrol.

![demo](https://cloud.githubusercontent.com/assets/1078548/17714097/90dff48c-63fe-11e6-8d37-1d20c44981ef.gif)

### Custom bindings? Sure!
Your configuration gets created on first run. If  `$XDG_CONFIG_HOME` is defined then it will be created at `$XDG_CONFIG_HOME/ncpamixer.conf` otherwise `$HOME/.ncpamixer.conf`

### Default bindings
| Event | Default | |
| --- | --- |---|
| switch | tab | Cycle trought sinks, outputs, profile and ports |
| select | enter | Select option in dropdowns |
| quit | escape | Quit |
| quit | q | Quit |
| dropdown | c | Open dropdown for selecting sinks, outputs, profile and ports |
| mute | m | Mute selected item |
| volume_up | l | Increase volume on selected item |
| volume_down | h | Decrease volume on selected item |
| volume_up | arrow right |  Increase volume on selected item |
| volume_down | arrow left |  Decrease volume on selected item |
| move_up | k | Move up |
| move_down | j | Move down |
| move_up | arrow up | Move up |
| move_down | arrow down | Move down |
| page_up | page up | Previous page in dropdown |
| page_down | page down | Next page in dropdown |
| tab_next | L | Next tab | 
| tab_prev | H | Previous tab | 
| tab_playback | F1 | Jump to playback tab|
| tab_recording | F2 | Jump to recording tab |
| tab_output | F3 | Jump to output tab |
| tab_input | F4 | Jump to input tab |
| tab_config | F5 | Jump to configuration tab |
| move_last | G | Move to last item |
| move_first | g | Move to first item |

### Dependencies
* PulseAudio :alien:
* ncurses

### Build dependencies
* CMake 
* C++14 compatible compiler

### Tested on
* Gentoo kernel 4.6.2, PulseAudio 9.0 and ncurses 6.0-r1
* Arch Linux 4.6.4-1 PulseAudio 9.0 and ncurses 6.0-4

### License

MIT
