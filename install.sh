#!/bin/bash

cd /home/$USER/Downloads
git clone https://github.com/raspberrypi/pico-sdk.git
sudo git submodule update --init
export PICO_SDK_PATH=/home/$USER/Downloads/pico-sdk
export PICO_BOARD=pico_w

git clone https://github.com/Daltro-Oliveira-Vinuto/IoT-secure-home.git
cd IoT-secure-home
mkdir project
cd project
cmake ..
make 
cp code.uf2 /media/$USER/RPI-RP21








