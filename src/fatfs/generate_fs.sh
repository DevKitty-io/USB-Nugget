#!/bin/bash

# cd to dir where this script is held so relative paths work
cd "$(dirname "$(readlink -f "$0")")"

# The directory to pack into the generated image
dir=${DIR:-default}
output_file=${OUTPUT_FILE:-fatfs.img}

cp fs_template.img "$output_file"


# Asterisk globbing doesn't work here because we need to include dotfiles
for entry in $(find "$dir" -mindepth 1 -maxdepth 1); do
    # mcopy has an option to print what it's copying but it only prints the
    # filename, not the whole path
    echo "copying (recursive): $entry"
    mcopy -i "$output_file" -s "$entry" ::
done
