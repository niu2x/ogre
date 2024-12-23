#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreImGuiOverlay.h"
#include "OgreOverlaySystem.h"
#include <iostream>

class MyApp : public OgreBites::ApplicationContext, public Ogre::RenderTargetListener
{
public:
    MyApp() {}
    ~MyApp() {}

    void run()
    {
        initApp();

        getRoot()->createSceneManager("DefaultSceneManager", "DummyScene");

        auto sm = getRoot()->getSceneManager("DummyScene");

        get_shader_generator()->addSceneManager(sm);

        auto sceneRoot = sm->getRootSceneNode();

        auto modelNode = sceneRoot->createChildSceneNode("modelModel");
        auto fish = sm->createEntity("model", "fish.mesh");
        modelNode->attachObject(fish);
        modelNode->setPosition(0, 0, 0);



        Ogre::Camera* cam = sm->createCamera("DummyCamera");
        cam->setAutoAspectRatio(true);
        cam->setNearClipDistance(1);

        auto cameraNode = sceneRoot->createChildSceneNode("cameraNode");
        cameraNode->attachObject(cam);
        cameraNode->setPosition(15, 15, 15);
        cameraNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_WORLD);

        sm->setAmbientLight(Ogre::ColourValue(0.2, 0.2, 0.2, 0.2));





        auto light = sm->createLight("light", Ogre::Light::LT_POINT);
        auto lightNode = sceneRoot->createChildSceneNode("lightNode");
        light->setDiffuseColour(1.0, 1.0, 1.0);
        lightNode->attachObject(light);
        lightNode->setPosition(14, 14, 14);

        

        getRenderWindow()->addListener(this);

        auto viewport = getRenderWindow()->addViewport(cam);
        viewport->setBackgroundColour(Ogre::ColourValue{1.0, 0, 0, 1.0});

        // auto overlay = initialiseImGui();
        // overlay->addFont("SdkTrays/Caption", "Essential");
        // overlay->setZOrder(300);
        // overlay->show();

        // sm->addRenderQueueListener(getOverlaySystem());

        // ImGui::GetStyle().ScaleAllSizes(1);
        // ImGui::GetIO().FontGlobalScale = 1;

        // addInputListener(getImGuiInputListener());

        getRoot()->startRendering();
        closeApp();
    }

    void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) override
    {
        // Ogre::ImGuiOverlay::NewFrame();
        // // ImGui::SetWindowFontScale(8);
        // auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse |
        //              ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

        // auto center = ImGui::GetMainViewport()->GetCenter();
        // ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        // ImGui::Begin("Configuration", NULL, flags);

        // if (ImGui::Button("00000"))
        // {
        //     x_ = true;
        // }
        // if (x_)
        // {
        //     if (ImGui::Button("11111"))
        //     {
        //         x_ = false;
        //     }
        // }
        // ImGui::End();
        // ImGui::EndFrame();
    }

private:
    bool x_;
};

int main()
{
    MyApp app;
    app.run();
    return 0;
}