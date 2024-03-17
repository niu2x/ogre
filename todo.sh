#!/bin/bash

main=$({ find OgreMain -name "Ogre*.h"; find OgreMain -name "Ogre*.cpp"; }| wc -l)
echo "OgreMain: ${main}" 

total=$({ find . -name "Ogre*.h"; find . -name "Ogre*.cpp"; }| wc -l)
echo "Total: ${total}" 
