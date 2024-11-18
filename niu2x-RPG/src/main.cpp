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
    // app.createDummyScene();

    app.getRoot()->createSceneManager("DefaultSceneManager", "DummyScene");

    auto sm = app.getRoot()->getSceneManager("DummyScene");

    app.get_shader_generator()->addSceneManager(sm);

    auto sceneRoot = sm->getRootSceneNode();

    auto modelNode = sm->createSceneNode("modelModel");
    auto fish = sm->createEntity("model", "WoodPallet.mesh");

    modelNode->attachObject(fish);
    sceneRoot->addChild(modelNode);
    modelNode->setPosition(0, 0, 0);
    // modelNode->setScale(20, 20, 20);

    Ogre::Camera* cam = sm->createCamera("DummyCamera");

    // cam->setFarClipDistance(10000);
    cam->setNearClipDistance(1);

    printf("getNearClipDistance %f\n", cam->getNearClipDistance());
    printf("getFarClipDistance %f\n", cam->getFarClipDistance());

    auto cameraNode = sm->createSceneNode("cameraNode");
    sceneRoot->addChild(cameraNode);
    cameraNode->attachObject(cam);
    cameraNode->setPosition(30, 30, 30);
    cameraNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_WORLD);

    sm->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0, 1.0));

    auto light = sm->createLight("light", Ogre::Light::LT_POINT);
    light->setDiffuseColour(1.0, 1.0, 1.0);
    sceneRoot->attachObject(light);

    auto viewport = app.getRenderWindow()->addViewport(cam);
    viewport->setBackgroundColour(Ogre::ColourValue{1.0, 0, 0, 1.0});

    app.getRoot()->startRendering();
    app.closeApp();

    return 0;
}