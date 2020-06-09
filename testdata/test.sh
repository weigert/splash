#!/usr/bin/env bash
#Splash Example Commands

#Display Raw Gif in Background
echo "smb.gif" | splash gif -p 660 340 600 400 --bg --ni --a --ns

#Test the Fidget Spinner (doesn't take data)
splash fidget -p 710 290 500 500 --ns --fg

#Test displaying an image!
echo "canyon.png" | splash img -p 360 140 1200 800

#Test displaying a clock!
date +%R | splash text -p 100 100 800 400 --ns --ni
