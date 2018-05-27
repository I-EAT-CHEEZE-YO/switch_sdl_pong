# Pong for the Nintendo Switch Written in c++ and using SDL2

A basic implementation of pong for the switch (only tested by me and confirmed working on 5.0.2)

## Getting Started

Clone or Download this git

### Building

you will need to install devkitpro and copy this source to a folder E.g. Desktop/switch_sdl_pong-master

From Msys2 Terminal :

```
cd [PATH TO SOURCE FOLDER] E.g. cd ~/Desktop/switch_sdl_pong-master
```
```
mkdir romfs
```
```
make
```

Once Compiled you will have a file in your switch_sdl_pong-master folder called switch_sdl_pong-master.nro
rename this file switch_sdl_pong.nro

```
Insert your Switch SD Card and make a new folder inside of the switch folder called switch_sdl_pong
```
```
copy the assets folder from the source folder to the sd card folder it should look like this SD:/switch/switch_sdl_pong/assets
```
```
copy the switch_sdl_pong.nro to the same directory it shoul look like this SD:/switch/switch_sdl_pong/switch_sdl_pong.nro
```

You can now reinset your sd card into the switch and run it


### Installing Pre-Built Release



Grab the latest release .zip file and extract it to the root of the switch sd card

## Authors

* **Todd Forester** - *Initial work* - [I-EAT-CHEEZE-YO](https://github.com/I-EAT-CHEEZE-YO)

## Thanks to

* Anyone Making Progress on switch hombrew
* Anyone who has played this if even for 2 seconds
* Anyone who is reading this


