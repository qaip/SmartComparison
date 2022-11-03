#!/bin/bash
diffrence="$(diff -qrs "$1" "$2")"
size="$(du -ch "$1" "$2" | tail -1 | cut -f 1)"
echo -e "\x1b[34mPID = $3 \x1b[33m \r$diffrence \x1b[35m \rTotal size: $size \x1b[0m \n"