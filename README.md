# Tierra
[![Build check](https://github.com/bioerrorlog/Tierra/actions/workflows/build.yml/badge.svg)](https://github.com/bioerrorlog/Tierra/actions/workflows/build.yml)


Source code of Tom Ray's artificial life model "Tierra" from http://tomray.me/tierra/source/

Tierra Simulator V6.02: Copyright (c) 1990 - 2004 Thomas S. Ray

## Versions
- Patched for Linux (Ubuntu 20.04): [main branch](https://github.com/bioerrorlog/Tierra/tree/main)
- Original v6.02: [v6.02-original branch](https://github.com/bioerrorlog/Tierra/tree/v6.02-original)

## How to run Tierra
On Linux (Ubuntu 20.04), run following commands.

WARNING: At this time, a build error occurs on Ubuntu 22.04
```sh
# Install required libraries
sudo apt update
sudo apt install -y build-essential libncurses-dev libxt-dev libxaw7-dev libtirpc-dev x11-apps

# Clone this repository
git clone https://github.com/bioerrorlog/Tierra.git

# Build Tierra
cd Tierra/tierra
./configure
make clean
make

# Generate gb (gene bank)
cd ./gb0
../arg c 0080.gen 0080aaa.tie
../arg x 0080.gen aaa
cp 0080.gen 0080gen.vir

# Execute Tierra
cd ..
HOSTNAME=localhost ./tierra si0
```

To visualize the Tierra runnning process, use Beagle Explorer.  
Open new terminal and run following commands:
```sh
# Build Beagle Explorer
cd ../Bglclnt/
make -f Makefile.Bgl clean
make -f Makefile.Bgl

# Run Beagle Explorer
HOSTNAME=localhost ./bgl-GUI_X11-Linux
```
For more detail, see [Tierra.doc](https://github.com/bioerrorlog/Tierra/blob/main/Tierra.doc).

## Running Tierra (Video)

[![Running Tierra - Artificial Life](https://user-images.githubusercontent.com/51422347/204118501-448ab941-a367-4b3d-b698-7aa20d6ba054.png)](https://youtu.be/X5QBazw4NF4)

## Reference
- http://tomray.me/tierra/source/
- [Running "Tierra": Tom Ray's Artificial Life Simulation - BioErrorLog Tech Blog](https://en.bioerrorlog.work/entry/run-tierra-artificial-life)
- https://github.com/Mortal/tierra/commit/92ff9f2a9ad76b0c08ea93cb10d2d8f539c5cbae
