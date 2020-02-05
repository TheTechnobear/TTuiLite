# git 
git submodule update --init --recursive

# Dependancies
CMake

# Building linux (or mac)

    sudo apt install git
    sudo apt install cmake

    mkdir build
    cd build
    cmake .. 


# Building Mac (with xcode)

    brew install git
    brew install cmake
 

    mkdir build
    cd build
    cmake .. -GXcode 

    xcodebuild -project ttuilite.xcodeproj -target ALL_BUILD -configuration MinSizeRel

or

    cmake --build .


# Building within Sublime Text

you can also build within sublime

    mkdir build
    cd build
    cmake -G "Sublime Text 2 - Unix Makefiles" .. 


# Building on Bela
the bela web ui does not support a subdirectories, so the way install is
  
    mkdir -p ~/projects
    cd ~/projects 
    // git clone ttuilite.git here
    mkdir build
    cd build
    cmake .. 



