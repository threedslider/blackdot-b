#!/bin/bash

#Install Build Tools
brew install bison
brew install pyenv
brew install freeimage
#brew install create-dmg

eval "$(pyenv init -)"

pyenv install 3.9.2
pyenv global 3.9.2
pyenv shell 3.9.2

sudo pip3 install -U numpy==1.19.5
sudo pip3 install pillow
sudo pip3 install pyside2
