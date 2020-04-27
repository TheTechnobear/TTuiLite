# Intro
this gives a quick overview on how to compile this project.
It assumes you have some working knowledge of development tools and linux

# git 
after cloning you will need 
```
git submodule update --init --recursive
```

# Dependancies
CMake, i2cdev


# Building on Terminal Tedium

    sudo apt install git
    sudo apt install cmake
    sudo apt-get install -y i2c-tools

    mkdir build
    cd build
    cmake .. 
    make


# Cross-compiling

building on rPI can be slow and also I find a little bit convoluted due to not having your 'normal' tools available.
for this reason I created a 'setup' which enables me to quickly and easily cross-compile on my mac (or another x86 linux box)

you can use this too, see  https://github.com/TheTechnobear/xcRpi for instruction on how to use.

**before you do the install** 
you need to ensure that the your TT has the correct libraries for building already installed. 
(as we are going to copy these to our local machine!)

```
    sudo apt-get install -y i2c-tools
```

in practice, I would suggest building ONCE on the TT before you attempt to do cross-compiling, this will ensure you tt has all libraries needed! 


also, make sure you use a separate directory for TT, so you get the correct library versions (as discussed in the install for xcRpi)


the following assume you have done this :) 

```
. ~/xcTT/xcSetEnv
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=~/xcTT/cmake/tt-toolchain.cmake  -DTT=on ..
cmake --build .
```


this builds what we need too `./build/release/bin` and `./build/release/lib`


from here we can use xcExec to execute thing on TT, without logging into it... and xcCopy to copy things across as we need

we can then do something like 
```
xcExec mkdir -p ./tmp; xcCopy "./release/bin/*" ./tmp ; xcCopy "./release/lib/*" ./tmp
```
this copies stuff to tt

we can now try running the 'test', we need a image file for this test, so 
```
xcCopy ../ttuilite/tests/orac.pbm ./tmp
```

then to run test
```
xcExec 'cd ./tmp; sudo ./ttuitest'
```

we can see this changes to tmp directory, needed since test looks for orac.pbm in current directory, 
and we need sudo since access to the display required root privs

