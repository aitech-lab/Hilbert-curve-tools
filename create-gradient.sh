#!/bin/bash
convert -size 512x512 gradient:yellow-blue \
        \( gradient:black-lime -rotate -90 \) \
        -compose CopyGreen -composite  gradient.png
