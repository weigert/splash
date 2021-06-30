#!/usr/bin/env bash
#Splash Example Commands

#Display Raw Gif in Background
echo "smb.gif" | splash gif -p 660 340 600 400 --bg --ni --a --ns

#Test the Fidget Spinner (doesn't take data)
splash fidget -p 710 290 500 500 --ns --fg
splash hairy -p 710 290 500 500 --ns --fg
splash spiky -p 710 290 500 500 --ns --fg

#Test displaying an image!
echo "canyon.png" | splash img -p 360 140 1200 800 --fg

#Test displaying some text
echo "dude i just literally wrote on your desktop background" | splash text --a --ns --ni --fg -fs 32 -ff "Roboto-Bold.ttf" -fc 0xFF0000

#Piped Clock (Run both of these!)
splash text --a --ns --ni -fs 300 --fg -ff "Roboto-Thin.ttf" -fc 0xff0000
watch -n1 'date +'%H:%M:%S' > ~/.config/splash/pipe/pipe0'
