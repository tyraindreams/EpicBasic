#!/bin/sh

find . -type f -name '*.eb' -print | while read filename; do
    echo "$filename"
    ebsci "$filename"
done > output.txt
