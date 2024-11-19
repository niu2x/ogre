#include "Ogre.h"
#include "OgreApplicationContext.h"
#include "OgreImGuiOverlay.h"
#include "OgreOverlayManager.h"
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

        auto modelNode = sm->createSceneNode("modelModel");
        auto fish = sm->createEntity("model", "WoodPallet.mesh");

        modelNode->attachObject(fish);
        sceneRoot->addChild(modelNode);
        modelNode->setPosition(0, 0, 0);

        Ogre::Camera* cam = sm->createCamera("DummyCamera");

        cam->setNearClipDistance(1);

        auto cameraNode = sm->createSceneNode("cameraNode");
        sceneRoot->addChild(cameraNode);
        cameraNode->attachObject(cam);
        cameraNode->setPosition(30, 30, 30);
        cameraNode->lookAt(Ogre::Vector3(0, 0, 0), Ogre::Node::TS_WORLD);

        sm->setAmbientLight(Ogre::ColourValue(1.0, 1.0, 1.0, 1.0));

        auto light = sm->createLight("light", Ogre::Light::LT_POINT);
        light->setDiffuseColour(1.0, 1.0, 1.0);
        sceneRoot->attachObject(light);

        getRenderWindow()->addListener(this);

        auto viewport = getRenderWindow()->addViewport(cam);
        viewport->setBackgroundColour(Ogre::ColourValue{1.0, 0, 0, 1.0});

        float vpScale = getDisplayDPI() / 96;
        Ogre::OverlayManager::getSingleton().setPixelRatio(vpScale);

        printf("#### vpScale %f\n", vpScale);
        auto overlay = initialiseImGui();

        // ImGui::GetIO().FontGlobalScale = std::round(vpScale); // default font does not work with fractional scaling

        overlay->addFont("SdkTrays/Caption", "Essential");
        overlay->setZOrder(300);
        overlay->show();

        sm->addRenderQueueListener(getOverlaySystem());

        addInputListener(getImGuiInputListener());

        getRoot()->startRendering();
        closeApp();
    }

    void preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt) override
    {
        Ogre::ImGuiOverlay::NewFrame();
        // ImGui::SetWindowFontScale(8);
        auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;

        auto center = ImGui::GetMainViewport()->GetCenter();
        // ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::Begin("Configuration", NULL, flags);

        if (ImGui::Button("00000000000000000000"))
        {
            x_ = true;
        }
        if (x_)
        {
            if (ImGui::Button("11111"))
            {
                x_ = false;
            }
        }
        ImGui::End();
        ImGui::EndFrame();
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