# RubberNugget: USB Attack Platform
A versatile USB attack platform that lets you hack computers in seconds using a [WiFi Nugget](https://wifinugget.com)!  

<img src="images/RubberNugget-Features.png"  />

## To install the new RubberNugget:
1.	Download the .BIN file here: [(direct link)](https://github.com/HakCat-Tech/RubberNugget/releases/) or from the release page: https://github.com/HakCat-Tech/RubberNugget/releases/
2.	Take your S2 Nugget out of its case
3.	Press and hold the "0" button on the back of your Nugget, plug it in to your computer, then release the "0" button. Your Nugget is now in flashing mode.
4.	Open the following URL in Google Chrome (other browsers not currently supported: https://nabucasa.github.io/esp-web-flasher/
5.	Click on "Connect" and then select the "ESP32-S2" board. Click "Erase" and "OK" to continue.
6.	Once you see "Finished", click "Choose a file" and select the .BIN file you downloaded in step 1. Click "Program" to flash your S2 Nugget!
7.	When its done, unplug your S2 Nugget and plug it in again to see the new features. 

## Creating Payloads
To upload a payload, you can save a `.txt` file to the RubberNugget flash drive.  Payloads must be saved under an operating system, then category type. (OS->Category->Payload.txt)

## Accessing the Web Interface
To access the web interface, connect to the `RubberNugget` AP with the password `nugget123`.  In a web browser, navigate to `192.168.4.1` to access the payload deployment system.  At the moment, the web interface can only edit / run payloads.

## Current Features - v1.0.x (beta)

- DuckyScript Compatible
    - Basic syntax: `STRING`, `DELAY`, keypress combos
    - Basic colors: `R` `G` `B` `C` `Y` `M` `W`
- Web Interface 
    - Deploy Payloads
    - Edit Payloads
- USB Flash Drive
    - FAT filesystem
    - Create default OS folders
    - Follows Hak5 naming convention (OS->Category->Payload)
- Nugget Interface
    - Custom graphics
    - Live command preview
    - Deploy payloads in 3 clicks
    - Reactive RGB NeoPixel

## Building / flashing from source
Dependencies: docker, make.
Docker is used to produce a replicable build environment and ensure library/tool dependensies are met.
```
# Clone the repo
git clone https://github.com/HakCat-Tech/RubberNugget.git

# Navigate to src
cd RubberNugget/src

# Build
sudo make build

# Build and flash the esp8266-s2 (may need sudo)
sudo make flash
#optionally specify port: sudo make flash PORT=<port>
#optionally install default scripts: sudo make flash RESET_SCRIPTS_DURING_FLASH=true

# Produce .img for release
sudo make generate_img
```

