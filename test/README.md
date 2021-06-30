# splash testdata

these are a number of files for testing default behavior.

to run all test, simply execute

    ./test.sh
    
from this directory `/splash/test`, or copy out the command you want to run.

## examples

### piped text display / clock

launch a text splash with the appropriate flags

    splash text --a --ns --ni -fs 300 --fg -ff "Roboto-Thin.ttf" -fc 0xff0000
    
this wlil then return the location of the pipe (`~/.config/splash/pipe/pipe0`, counting up). anything piped into the pipe will be rendered as text. to display a clock using splash, you could run e.g.

    watch -n1 'date +'%H:%M:%S' > ~/.config/splash/pipe/pipe0'
    
make sure the pipe name is correct and that the font exists in `~/.fonts`
