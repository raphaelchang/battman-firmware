#battman-firmware
Firmware for the "battman" battery management system.
##Setup
 Clone repostiory. 
```
git clone https://github.com/raphaelchang/battman-firmware.git
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
##Programming
You'll need to install dfu-util v0.9 or higher. 
####Mac OS
On Mac, the homebrew version is adequate. 
```
brew install dfu-util
```
####Ubuntu
On Ubuntu 14.04, dfu-util must be installed from source. See instructions here: http://dfu-util.sourceforge.net/build.html.

####Editing Makefile
Prior to uploading code to Battman, the Makefile must be edited for your particular board's serial number.

1. Plug in Battman to computer via USB.
2. Run ```dfu-util -l```.
3. Copy the serial number of Battman into the Makefile in the following location.
    ```sudo dfu-util -a 0 -D build/$(PROJECT).bin -S <serial> -s 0x08000000```

####Uploading code
Run the following to build and upload the code.
```
make upload
```
