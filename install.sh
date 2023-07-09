#! /usr/bin/sh

REQUIRED_LIB="libx11-dev"

LIB_EXIST=$(dpkg-query -s $REQUIRED_LIB 2> /dev/null | grep "Status: install ok installed") 

if [ "" = "$LIB_EXIST" ]; 
    then echo "$REQUIRED_LIB is not installed in your system.Install? [Y/N] "
    read ANSWER
    if [ "y" = "$ANSWER" -o "Y" = "$ANSWER" ];
        then sudo apt install $REQUIRED_LIB -y
    fi
else
    echo "$REQUIRED_LIB is installed.You can proceed with compilation.Run < make all > command."
fi

 