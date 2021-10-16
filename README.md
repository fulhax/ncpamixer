master: [![Build Status](http://fredrik.fulhax.nu:8090/job/ncpamixer/badge/icon)](http://fredrik.fulhax.nu:8090/job/ncpamixer)
dev:    [![Build Status](http://fredrik.fulhax.nu:8090/job/ncpamixer-dev/badge/icon)](http://fredrik.fulhax.nu:8090/job/ncpamixer-dev)

# ncurses PulseAudio Mixer

An ncurses mixer for PulseAudio inspired by pavucontrol.

![demo](https://cloud.githubusercontent.com/assets/1078548/17714097/90dff48c-63fe-11e6-8d37-1d20c44981ef.gif)

### Config
Your configuration gets created on first run. If  `$XDG_CONFIG_HOME` is defined then it will be created at `$XDG_CONFIG_HOME/ncpamixer.conf` otherwise `$HOME/.ncpamixer.conf`



### Custom colors? Why not!
ncpamixer supports 256 colors. You can change them in ncpamixer.conf

### Custom bindings? Sure!
You find them in ncpamixer.conf 

#### Default bindings
| Event | Default | |
| --- | --- |---|
| switch | tab | Cycle trought sinks, outputs, profile and ports |
| select | enter | Select option in dropdowns |
| quit | escape | Quit |
| quit | q | Quit |
| dropdown | c | Open dropdown for selecting sinks, outputs, profile and ports |
| mute | m | Mute selected item |
| set_default | d | Set default sink/source |
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
| set_volume_0 | 0 | Set volume to 0% |
| set_volume_10 | 1 | Set volume to 10% |
| set_volume_20 | 2 | Set volume to 20% |
| set_volume_30 | 3 | Set volume to 30% |
| set_volume_40 | 4 | Set volume to 40% |
| set_volume_50 | 5 | Set volume to 50% |
| set_volume_60 | 6 | Set volume to 60% |
| set_volume_70 | 7 | Set volume to 70% |
| set_volume_80 | 8 | Set volume to 80% |
| set_volume_90 | 9 | Set volume to 90% |
| set_volume_100 | Unbound | Set volume to 100% |
| toggle_static | Unbound | Toggle barmode static/none static |

### Dependencies
* PulseAudio :alien:
* ncurses

### Build dependencies
* CMake
* C++14 compatible compiler

On Debian(-based) systems, you'd need `libncurses-dev` and `libpulse-dev`.

### Building
* In the main directory, run `make`
* On some systems (tested on Debian-stable), you may need to instead run `make USE_WIDE=True`
* The above command is needed to get UTF-8 support; it will cause CMake to look for the ncursesw library, and it'll link to it with -lncursesw
* (This also means you might have to first `rm build/CMakeCache.txt` if you ran `make` without using the USE_WIDE setting, so it'll re-generate the Makefile)

### Install

##### Arch Linux
`packer -S ncpamixer-git`
https://aur.archlinux.org/packages/ncpamixer-git/

#### Gentoo ebuild
https://github.com/fulhax/fulhax-overlay/tree/master/media-sound/ncpamixer

### Tested on
* Gentoo kernel 4.12.10, PulseAudio 11.0 and ncurses 6.0-r1
* Gentoo kernel 4.6.2, PulseAudio 9.0 and ncurses 6.0-r1
* Arch Linux 4.6.4-1 PulseAudio 9.0 and ncurses 6.0-4
* Fedora kernel 4.18.10-200, PulseAudio 12.2 and ncurses 6.1-5

### License

MIT
