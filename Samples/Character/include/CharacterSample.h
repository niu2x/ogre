#ifndef __Character_H__
#define __Character_H__

#include "SdkSample.h"
#include "SinbadCharacterController.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreSampleClassExport Sample_Character : public SdkSample
{
public:

    Sample_Character()
    {
        mInfo["Title"] = "Character";
        mInfo["Description"] = "A demo showing 3rd-person character control and use of TagPoints.";
        mInfo["Thumbnail"] = "thumb_char.png";
        mInfo["Category"] = "Animation";
        mInfo["Help"] = "Use the WASD keys to move Sinbad, and the space bar to jump. "
            "Use mouse to look around and mouse wheel to zoom. Press Q to take out or put back "
            "Sinbad's swords. With the swords equipped, you can left click to slice vertically or "
            "right click to slice horizontally. When the swords are not equipped, press E to "
            "start/stop a silly dance routine.";
    }

protected:

    void setupContent() override
    {
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
        // add integrated depth shadows
        auto& rtShaderGen = RTShader::ShaderGenerator::singleton();
        auto schemRenderState = rtShaderGen.getRenderState(MSN_SHADERGEN);
        schemRenderState->addTemplateSubRenderState(rtShaderGen.createSubRenderState(RTShader::SRS_SHADOW_MAPPING));

        // Make this viewport work with shader generator scheme.
        mViewport->setMaterialScheme(MSN_SHADERGEN);
        // update scheme for FFP supporting rendersystems
        MaterialManager::singleton().setActiveScheme(
            mViewport->getMaterialScheme());
#endif
        // set background and some fog
        mViewport->setBackgroundColour(ColorValue(1.0f, 1.0f, 0.8f));
        mSceneMgr->setFog(
            Ogre::FOG_LINEAR,
            ColorValue(1.0f, 1.0f, 0.8f),
            0,
            15,
            100);

        // set shadow properties
        mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
        mSceneMgr->setShadowTexturePixelFormat(PixelFormat::DEPTH16);
        mSceneMgr->setShadowColour(ColorValue(0.5, 0.5, 0.5));
        mSceneMgr->setShadowTextureSize(1024);
        mSceneMgr->setShadowTextureCount(1);
        mSceneMgr->setShadowDirLightTextureOffset(0);
        mSceneMgr->setShadowFarDistance(50);
        mSceneMgr->setShadowCameraSetup(LiSPSMShadowCameraSetup::create());

        // disable default camera control so the character can do its own
        mCameraMan->setStyle(CS_MANUAL);

        // use a small amount of ambient lighting
        mSceneMgr->setAmbientLight(ColorValue(0.3, 0.3, 0.3));

        // add a bright light above the scene
        Light* light = mSceneMgr->createLight(Light::LT_POINT);
        mSceneMgr->getRootSceneNode()
            ->createChildSceneNode(Vector3(-10, 40, 20))
            ->attachObject(light);
        light->setSpecularColour(ColorValue::White);

        // create a floor mesh resource
        MeshManager::singleton().createPlane(
            "floor",
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Plane(Vector3::unit_y, 0),
            100,
            100,
            10,
            10,
            true,
            1,
            10,
            10,
            Vector3::unit_z);

        // create a floor entity, give it a material, and place it at the origin
        Entity* floor = mSceneMgr->createEntity("Floor", "floor");
        floor->setMaterialName("Examples/Rockwall");
        floor->setCastShadows(false);
        mSceneMgr->getRootSceneNode()->attachObject(floor);

        //      LogManager::singleton().log_message("creating sinbad");
        // create our character controller
        mChara = std::make_unique<SinbadCharacterController>(mCamera);

        mInputListenerChain = TouchAgnosticInputListenerChain(
            mWindow,
            { mTrayMgr.get(), this, mChara.get() });

        //      LogManager::singleton().log_message("toggling stats");
        mTrayMgr->toggleAdvancedFrameStats();

        //      LogManager::singleton().log_message("creating panel");
        StringVector items;
        items.push_back("Help");
        ParamsPanel* help = mTrayMgr->createParamsPanel(TL_TOPLEFT, "HelpMessage", 100, items);
        help->setParamValue("Help", "H / F1");

        //      LogManager::singleton().log_message("all done");
    }

    void cleanupContent() override
    {
        MeshManager::singleton().remove(
            "floor",
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }

    std::unique_ptr<SinbadCharacterController> mChara;
};

#endif
