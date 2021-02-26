[RoboComp](http://robocomp.org)
===============================


by [RoboLab (Universidad de Extremadura)](http://robolab.unex.es), [Aston University](https://www2.aston.ac.uk/eas), [ISIS (Universidad de Málaga)](http://www.grupoisis.uma.es/index.php?option=com_jresearch&view=staff&Itemid=3&lang=es) and many other collaborators from the Google Summer of Code program.

RoboComp is an open-source Robotics framework providing the tools to create and modify software components that communicate through public interfaces. Components may *require*, *subscribe*, *implement* or *publish*
interfaces in a seamless way. Building new components is done using two domain specific languages, IDSL and CDSL. With IDSL you define an interface and with CDSL you specify how the component will communicate with the world. With this information, a code generator creates C++ and/or Python sources, based on CMake, that compile and execute flawlessly. When some of these features have to be changed, the component can be easily regenerated and all the user specific code is preserved thanks to a simple inheritance mechanism.

:warning: This RoboComp version is based on https://github.com/robocomp/robocomp with some minor changes for the SA3IR project. If you want to contribute with something new to Robocomp or use the latests features available in RoboComp, please visit the original repository.  

:question: If you have a question please look for it in the [FAQ](doc/FAQ.md). 
-

# Installation from source

Tested in Ubuntu 18.04.  
**Note:** RoboComp is not compatible with Ubuntu 16.04. RoboComp needs to be compiled using C++11. Ice libraries with C++11 support are only available for zeroc-ice 3.7 and the packages for this version are only available since Ubuntu 18.04.
{: .note}
<!--If you are not an Ubuntu user, need to modify the core of RoboComp, or just feel like installing from sources, you can follow these instructions (they have been tested in Ubuntu 14.04, 14.10, 15.04, 16.04). If you're not in any of these scenarios, please use the packaged version.
-->

## Requirements
Make sure you have installed the following packages from the Ubuntu repository:

    sudo apt-get update
    sudo apt-get install git git-annex cmake g++ libgsl-dev libopenscenegraph-dev cmake-qt-gui zeroc-ice-all-dev freeglut3-dev libboost-system-dev libboost-thread-dev qt4-dev-tools python-pip python-pyparsing=2.2.0+dfsg1-2 python-numpy python-pyside pyside-tools libxt-dev libboost-test-dev libboost-filesystem-dev python-libxml2 python-xmltodict libccd-dev python-zeroc-ice zeroc-ice-all-runtime libxml2-dev libboost-python-dev uuid-dev
    sudo pip install networkx
    sudo pip install thriftpy

It is recommendable to install the following packages::

    sudo apt-get install yakuake qt4-designer

Note: RoboComp uses python2 for now, so make sure that you set python2 as your deafult choide if you also have python3 installed on your system. You can do so by adding this line into your ~/.bashrc file and then save it:

    alias python=python2
    
Note: One of the main tools of Robocomp, robocompdsl is using pyparsing and the current code doesn't work with 2.4 version of this library. With the previous commands we are installing the 2.2 version (python-pyparsing=2.2.0+dfsg1-2). If you a have more recent version of pyparsing installed with apt or pip we recomend you to uninstall it and install the 2.2 version. You can check your curren version of pyparsing with this command:

    python -c "import pyparsing; print(pyparsing.__version__)"
    

## Installation itself

*cd* to your home directory (you are probably in it already) and type:

    git clone https://github.com/robocomp/robocomp.git

Now we will create a symbolic link so RoboComp can find everything. You will have to enter your password:

    sudo ln -s ~ /home/robocomp
    
(the ~ is in Alt-4)
    
Edit your ~/.bashrc file 

    gedit ~/.bashrc

Add these lines at the end:

    export ROBOCOMP=~/robocomp
    export PATH=$PATH:/opt/robocomp/bin
   
make bash process the modified file by typing: 

    source ~/.bashrc

Done! Now let's compile and install the whole thing:

    sudo [ -d /opt/robocomp ] && rm -r /opt/robocomp
    cd robocomp
    mkdir build
    cd build
    cmake ..
    make
    sudo make install

Create the .rcremote file in the home folder. It should look like the following:

localhost#blabla

If you want to compile Robocomp with support for FCL, follow the instructions in the [Robocomp with FCL](doc/Compiling-RoboComp-with-collision-detection.md) tutorial.

The RoboComp's core libraries and simulator should now be compiled and installed in `/opt/robocomp`.

Let's now tell Linux where to find RoboComp's libraries:

    sudo nano /etc/ld.so.conf

and add the following line:

    /opt/robocomp/lib/
   
save the file and type:

    sudo ldconfig

Done!

    
    
    



