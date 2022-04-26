# RubberNugget: USB Attack Platform
A versatile USB attack platform that lets you hack computers in seconds using a [WiFi Nugget](https://wifinugget.com)!  
More documentation coming soon.

## Creating Payloads
To upload a payload, you can save a `.txt` file to the RubberNugget flash drive.  Payloads must be saved under an operating system, then category type. (OS->Category->Payload.txt)

## Accessing the Web Interface
To access the web interface, connect to the `RubberNugget` AP with the password `password`.  In a web browser, navigate to `192.168.4.1` to access the payload deployment system.  At the moment, the web interface can only edit / run payloads.

## Current Features - v1.0 (beta)

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