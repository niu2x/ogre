#include "Ogre.h"
#include "OgreApplicationContext.h"
#include <iostream>

int main()
{

    OgreBites::ApplicationContext app;

    app.initApp();
    app.getRoot()->startRendering();
    app.closeApp();

    return 0;
}