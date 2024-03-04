// This file is part of the OGRE project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at https://www.ogre3d.org/licensing.
// SPDX-License-Identifier: MIT
#include "OgreTextureAnimatorAffector.h"
#include "OgreParticleSystem.h"
#include "string_interface.h"
#include "OgreParticle.h"


namespace Ogre {

class CmdStart : public ParamCommand
{
public:
    String get(const void* target) const override
    {
        return StringConverter::to_string(
            static_cast<const TextureAnimatorAffector*>(target)->getTexcoordStart());
    }
    void set(void* target, const String& val) override
    {
        static_cast<TextureAnimatorAffector*>(target)->setTexcoordStart(StringConverter::parse_int32(val));
    }
};
class CmdCount : public ParamCommand
{
public:
    String get(const void* target) const override
    {
        return StringConverter::to_string(
            static_cast<const TextureAnimatorAffector*>(target)->getTexcoordCount());
    }
    void set(void* target, const String& val) override
    {
        static_cast<TextureAnimatorAffector*>(target)->setTexcoordCount(StringConverter::parse_int32(val));
    }
};
class CmdDuration : public ParamCommand
{
public:
    String get(const void* target) const override
    {
        return StringConverter::to_string(
            static_cast<const TextureAnimatorAffector*>(target)->getDuration());
    }
    void set(void* target, const String& val) override
    {
        static_cast<TextureAnimatorAffector*>(target)->setDuration(StringConverter::parse_real(val));
    }
};
class CmdOffset : public ParamCommand
{
public:
    String get(const void* target) const override
    {
        return StringConverter::to_string(
            static_cast<const TextureAnimatorAffector*>(target)->isRandomStartOffset());
    }
    void set(void* target, const String& val) override
    {
        static_cast<TextureAnimatorAffector*>(target)->useRandomStartOffset(StringConverter::parse_bool(val));
    }
};

    // init statics
    static CmdStart msStartCmd;
    static CmdCount msCountCmd;
    static CmdDuration msDurationCmd;
    static CmdOffset msOffset;

    //-----------------------------------------------------------------------
    TextureAnimatorAffector::TextureAnimatorAffector(ParticleSystem* psys) : ParticleAffector(psys)
    {
        mTexcoordStart = mTexcoordCount = mDuration = 0;
        mRandomStartOffset = false;
        mType = "TextureAnimator";

        // Init parameters
        if (create_param_dictionary("TextureAnimatorAffector"))
        {
            ParamDictionary* dict = param_dictionary();

            dict->add_parameter(ParameterDef("texcoord_start", "", ParameterType::INT), &msStartCmd);
            dict->add_parameter(ParameterDef("texcoord_count", "", ParameterType::INT), &msCountCmd);
            dict->add_parameter(ParameterDef("duration", "", ParameterType::REAL), &msDurationCmd);
            dict->add_parameter(ParameterDef("random_offset", "", ParameterType::BOOL), &msOffset);
        }
    }
    //-----------------------------------------------------------------------
    void TextureAnimatorAffector::_initParticle(Particle* pParticle)
    {
        pParticle->mTexcoordIndex = 0;

        if (!mRandomStartOffset)
            return;

        pParticle->mRandomTexcoordOffset = Math::UnitRandom() * mTexcoordCount;
        pParticle->mTexcoordIndex = pParticle->mRandomTexcoordOffset;
    }

    void TextureAnimatorAffector::_affectParticles(ParticleSystem* pSystem, Real timeElapsed)
    {
        // special case: randomly pick one cell in sprite-sheet
        if(mDuration < 0)
            return;

        for (auto p : pSystem->_getActiveParticles())
        {
            float particle_time = 1.0f - (p->mTimeToLive / p->mTotalTimeToLive);

            float speed = mDuration ? (p->mTotalTimeToLive / mDuration) : 1.0f;
            uint8 idx = uint8(particle_time * speed * mTexcoordCount + p->mRandomTexcoordOffset) % mTexcoordCount;

            p->mTexcoordIndex = idx + mTexcoordStart;
        }
    }
}
