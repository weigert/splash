#!/bin/bash

# Get the user dir for the user that is executing the script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
HOME=/home/$(echo $DIR | cut -d/ -f3)


# Setup script for splash
printf "Setting up splash...\n"

printf "Setting up install directory ... "
if [ -d "$HOME/.config/splash" ]; then
  if ! [ -d "$HOME/.config/splash/exec" ]; then
    mkdir "$HOME/.config/splash/exec"
  fi
else
  mkdir "$HOME/.config/splash"
  mkdir "$HOME/.config/splash/exec"
fi
printf "done!\n"
printf "Install location: $HOME/.config/splash\n"

# Check for Existence of Dependencies
printf "Setting up configuration file\n"
cp config ~/.config/splash/config

printf "Compile splash? [Y / N] "
read b
if [ $b = "Y" ] || [ $b = "y" ];then
  echo "Compiling splash..."
  make -C "splash" splash
fi

printf "Compile example programs? [Y / N] "
read b
if [ $b == "Y" ] || [ $b = "y" ]; then
  echo "Compiling programs..."
  make -C "program" all
fi

printf "splash: Success\n"
