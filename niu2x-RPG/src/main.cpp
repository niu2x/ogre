#include "Ogre.h"
#include "OgreApplicationContext.h"
#include <iostream>

// class MyFrameListener : public Ogre::FrameListener
// {
// public:
//     MyFrameListener():counter(0x160) {}
//     ~MyFrameListener() {}

//     bool frameStarted(const Ogre::FrameEvent& evt) override
//     {
//         return --counter > 0;
//     }

//     // bool frameRenderingQueued(const FrameEvent& evt) override
//     // {
//     //     (void)evt;
//     //     return true;
//     // }

//     // bool frameEnded(const FrameEvent& evt) override
//     // {
//     //     (void)evt;
//     //     return true;
//     // }
// private:
//     size_t counter;
// };

int main()
{
    OgreBites::ApplicationContext app;
    // MyFrameListener l;

    app.initApp();
    // app.getRoot()->addFrameListener(&l);
    app.getRoot()->startRendering();
    app.closeApp();

    return 0;
}