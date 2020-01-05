# SaveNX
Currently base project to understand more about the internals of Nintendo Switch file saving. In the future I plan to transition this into a sys module or if that is not possible, application that can cloud sync save files automatically.

## Building
1. Install devkitpro
2. Install switch-dev (`sudo pacman -S switch-dev`)
3. `cd savenx`
4. `make`

## Credits
(https://github.com/WerWolv/EdiZon/)[EdiZon] - Used to quickly see which functions are critical to reading save file information because official libnx documentation is lacking.

(https://github.com/switchbrew/switch-examples)[libnx examples] - Used as a quickstart base for application.
