#battman-firmware
Firmware for the "battman" battery management system.
##Setup
 Clone repostiory. 
```
git clone https://github.com/raphaelchang/battman-firmware.git
```
 Fetch the ChibiOS submodule.
```
git submodule init
git submodule update
```
##Compiling
You'll need to install the following gcc arm cross compiler.
```
sudo apt-get install gcc-arm-none-eabi
```
To build the files. Navigate to the root of the repository and run the following:
```
make
```
##Uploading
You'll need to install dfu-util v0.9 or higher. On Mac, the homebrew version is adequate. On Ubuntu 14.04, dfu-util must be installed from source. See instructions here: http://dfu-util.sourceforge.net/build.html.

Run the following to build and upload the code.
```
make upload
```
