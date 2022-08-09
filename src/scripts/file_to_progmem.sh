#!/bin/bash

file=$1

if [ -z $file ]; then
    echo usage: $0
    exit 1
fi

fileName=$(basename $file)
fileName="${fileName%%.*}"

appendOutFileBuffer() {
    if [ "$2" = "nonewline" ]; then
        outFileBuffer=$(printf '%s%s' "$outFileBuffer" "$1")
        return
    fi
    outFileBuffer=$(printf '%s\n%s' "$outFileBuffer" "$1")
}

outFileBuffer=""
appendOutFileBuffer "#ifndef ${fileName^^}_H"
appendOutFileBuffer "#define ${fileName^^}_H"
appendOutFileBuffer ""
appendOutFileBuffer "const char ${fileName^^}[] PROGMEM = {"

# Convert to ascii-hex. No spaces, no newlines
inHex=$(hexdump $file -ve '1/1 "%02x"')

while read -n 2 -d "\x00" byte; do
    if [ -n $byte ]; then
        appendOutFileBuffer "0x$byte," "nonewline"
    fi
done <<< "$inHex"
outFileBuffer="${outFileBuffer::-1}" # remove last ','

appendOutFileBuffer "};" "nonewline"
appendOutFileBuffer "#endif"

echo "$outFileBuffer"

