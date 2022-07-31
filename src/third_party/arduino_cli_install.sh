#!/bin/bash

# This script zips and installs third party dependencies. It is untested
# outside of the docker container, use at your own risk.


# Make sure to run from the third_party directory
cd "$(dirname "${BASH_SOURCE[0]}")"

# provide the full path to the arduino-cli executable
arduino_cli=$1

dirs=$(find . -maxdepth 1 -mindepth 1 -type d)
while read dirToZip; do
    zipfile="$dirToZip".zip
    zip -r "$zipfile" "$dirToZip"
    "$arduino_cli" lib install --zip-path "$zipfile"
done <<< "$dirs"
