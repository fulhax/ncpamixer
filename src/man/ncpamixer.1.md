% NCPAMIXER(1) Version 1.3.5 | ncurses PulseAudio Mixer

NAME
====

**ncpamixer** — An ncurses mixer for PulseAudio inspired by pavucontrol.

SYNOPSIS
========

`ncpamixer` [-v] [-h] [-c CONFIG_FILE] [-t TAB]


DESCRIPTION
===========

An ncurses mixer for PulseAudio inspired by pavucontrol.

Options
-------

`-v, --version`
:   Print version info.

`-h, --help`
:   Print this help screen.

`-c, --config=CONFIG_FILE`
:   Set custom location for config.

`-t, --tab=TAB`
:   Open on given tab. Choices: (p)layback (default), (r)ecording, (o)utput, (i)nput, (c)onfiguration.

CONFIG
=====

Your configuration gets created on first run. If `$XDG_CONFIG_HOME` is defined then it will be created at `$XDG_CONFIG_HOME/ncpamixer.conf` otherwise `$HOME/.ncpamixer.conf`

Default bindings
----------------

| Event | Default | Description |
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
| help | ? | Pop up help screen with keycodes |
| set_volume_100 | Unbound | Set volume to 100% |
| toggle_static | Unbound | Toggle barmode static/none static |

BUGS
====

See GitHub Issues: <https://github.com/fulhax/ncpamixer/issues>

AUTHOR
======

<https://github.com/c0r73x> and contributors

