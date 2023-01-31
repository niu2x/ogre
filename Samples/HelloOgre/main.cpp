/*-------------------------------------------------------------------------
This source file is a part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/


Copyright (c) 2000-2013 Torus Knot Software Ltd
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:


The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE
-------------------------------------------------------------------------*/

//! [fullsource]

#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreTrays.h"
#include "OgreRTShaderSystem.h"
#include <iostream>


using namespace Ogre;
using namespace OgreBites;

class HelloOgre : public ApplicationContext, public InputListener
{
public:
    HelloOgre();
    virtual ~HelloOgre() {}

    virtual void setup() override;
    virtual void shutdown() override;
    virtual 
bool frameRenderingQueued(const Ogre::FrameEvent& evt)  override;    
    bool keyPressed(const KeyboardEvent& evt);

private:
    OgreBites::TrayManager* mTrayMgr;
};

HelloOgre::HelloOgre() : ApplicationContext("OgreTutorialApp"), mTrayMgr(nullptr) {}

void HelloOgre::shutdown()
{

    if (mTrayMgr)
    {
        delete mTrayMgr;
        mTrayMgr = nullptr;
    }
    ApplicationContext::shutdown();
}

void HelloOgre::setup()
{
    // do not forget to call the base first
    ApplicationContext::setup();

    // get a pointer to the already created root
    Root* root = getRoot();
    SceneManager* scnMgr = root->createSceneManager();

    // register our scene with the RTSS
    RTShader::ShaderGenerator* shadergen = RTShader::ShaderGenerator::getSingletonPtr();
    shadergen->addSceneManager(scnMgr);

    // -- tutorial section start --
    //! [turnlights]
    scnMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
    //! [turnlights]

    //! [newlight]
    Light* light = scnMgr->createLight("MainLight");
    SceneNode* lightNode = scnMgr->getRootSceneNode()->createChildSceneNode();
    lightNode->attachObject(light);
    //! [newlight]

    //! [lightpos]
    lightNode->setPosition(20, 80, 50);
    //! [lightpos]

    //! [camera]
    SceneNode* camNode = scnMgr->getRootSceneNode()->createChildSceneNode();

    // create the camera
    Camera* cam = scnMgr->createCamera("myCam");
    cam->setNearClipDistance(5); // specific to this sample
    cam->setAutoAspectRatio(true);
    camNode->attachObject(cam);
    camNode->setPosition(0, 0, 140);

    // and tell it to render into the main window
    getRenderWindow()->addViewport(cam);

    scnMgr->addRenderQueueListener(getOverlaySystem());

    mTrayMgr = new OgreBites::TrayManager("InterfaceName", getRenderWindow());
    addInputListener(mTrayMgr);
    Button* b = mTrayMgr->createButton(TL_CENTER, "MyButton", "Click Me!");
    mTrayMgr->showAll();

    mTrayMgr->showLogo(TL_CENTER);
}


bool HelloOgre::frameRenderingQueued(const Ogre::FrameEvent& evt) 
{
    mTrayMgr->frameRendered(evt);
    return ApplicationContext::frameRenderingQueued(evt);
}

bool HelloOgre::keyPressed(const KeyboardEvent& evt)
{
    if (evt.keysym.sym == SDLK_ESCAPE)
    {
        getRoot()->queueEndRendering();
    }
    return true;
}

int main(int argc, char** argv)
{
    try
    {
        HelloOgre app;
        app.initApp();
        app.getRoot()->startRendering();
        app.closeApp();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error occurred during execution: " << e.what() << '\n';
        return 1;
    }

    return 0;
}

//! [fullsource]
