/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

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
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "OgreColourFaderAffector.h"
#include "OgreParticleSystem.h"
#include "string_interface.h"
#include "OgreParticle.h"


namespace Ogre {
    
    // init statics
    ColourFaderAffector::CmdRedAdjust ColourFaderAffector::msRedCmd;
    ColourFaderAffector::CmdGreenAdjust ColourFaderAffector::msGreenCmd;
    ColourFaderAffector::CmdBlueAdjust ColourFaderAffector::msBlueCmd;
    ColourFaderAffector::CmdAlphaAdjust ColourFaderAffector::msAlphaCmd;

    //-----------------------------------------------------------------------
    ColourFaderAffector::ColourFaderAffector(ParticleSystem* psys) : ParticleAffector(psys)
    {
        mRedAdj = mGreenAdj = mBlueAdj = mAlphaAdj = 0;
        mType = "ColourFader";

        // Init parameters
        if (create_param_dictionary("ColourFaderAffector"))
        {
            ParamDictionary* dict = param_dictionary();

            dict->add_parameter(ParameterDef("red", 
                "The amount by which to adjust the red component of particles per second.",
                ParameterType::REAL), &msRedCmd);
            dict->add_parameter(ParameterDef("green", 
                "The amount by which to adjust the green component of particles per second.",
                ParameterType::REAL), &msGreenCmd);
            dict->add_parameter(ParameterDef("blue", 
                "The amount by which to adjust the blue component of particles per second.",
                ParameterType::REAL), &msBlueCmd);
            dict->add_parameter(ParameterDef("alpha", 
                "The amount by which to adjust the alpha component of particles per second.",
                ParameterType::REAL), &msAlphaCmd);


        }
    }
    //-----------------------------------------------------------------------
    void ColourFaderAffector::_affectParticles(ParticleSystem* pSystem, Real timeElapsed)
    {
        // Scale adjustments by time
        auto dc
            = ColorValue(mRedAdj, mGreenAdj, mBlueAdj, mAlphaAdj) * timeElapsed;

        for (auto p : pSystem->_getActiveParticles())
        {
            p->mColour = (ColorValue((uchar*)&p->mColour) + dc)
                             .saturate_copy()
                             .as_BYTE();
        }
    }
    //-----------------------------------------------------------------------
    void ColourFaderAffector::setAdjust(float red, float green, float blue, float alpha)
    {
        mRedAdj = red;
        mGreenAdj = green;
        mBlueAdj = blue;
        mAlphaAdj = alpha;
    }
    //-----------------------------------------------------------------------
    void ColourFaderAffector::setRedAdjust(float red)
    {
        mRedAdj = red;
    }
    //-----------------------------------------------------------------------
    float ColourFaderAffector::getRedAdjust(void) const
    {
        return mRedAdj;
    }
    //-----------------------------------------------------------------------
    void ColourFaderAffector::setGreenAdjust(float green)
    {
        mGreenAdj = green;
    }
    //-----------------------------------------------------------------------
    float ColourFaderAffector::getGreenAdjust(void) const
    {
        return mGreenAdj;
    }
    //-----------------------------------------------------------------------
    void ColourFaderAffector::setBlueAdjust(float blue)
    {
        mBlueAdj = blue;
    }
    //-----------------------------------------------------------------------
    float ColourFaderAffector::getBlueAdjust(void) const
    {
        return mBlueAdj;
    }
    //-----------------------------------------------------------------------
    void ColourFaderAffector::setAlphaAdjust(float alpha)
    {
        mAlphaAdj = alpha;
    }
    //-----------------------------------------------------------------------
    float ColourFaderAffector::getAlphaAdjust(void) const
    {
        return mAlphaAdj;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String ColourFaderAffector::CmdRedAdjust::get(const void* target) const
    {
        return StringConverter::to_string(
            static_cast<const ColourFaderAffector*>(target)->getRedAdjust() );
    }
    void ColourFaderAffector::CmdRedAdjust::set(void* target, const String& val)
    {
        static_cast<ColourFaderAffector*>(target)->setRedAdjust(
            StringConverter::parse_real(val));
    }
    //-----------------------------------------------------------------------
    String ColourFaderAffector::CmdGreenAdjust::get(const void* target) const
    {
        return StringConverter::to_string(
            static_cast<const ColourFaderAffector*>(target)->getGreenAdjust() );
    }
    void ColourFaderAffector::CmdGreenAdjust::set(void* target, const String& val)
    {
        static_cast<ColourFaderAffector*>(target)->setGreenAdjust(
            StringConverter::parse_real(val));
    }
    //-----------------------------------------------------------------------
    String ColourFaderAffector::CmdBlueAdjust::get(const void* target) const
    {
        return StringConverter::to_string(
            static_cast<const ColourFaderAffector*>(target)->getBlueAdjust() );
    }
    void ColourFaderAffector::CmdBlueAdjust::set(void* target, const String& val)
    {
        static_cast<ColourFaderAffector*>(target)->setBlueAdjust(
            StringConverter::parse_real(val));
    }
    //-----------------------------------------------------------------------
    String ColourFaderAffector::CmdAlphaAdjust::get(const void* target) const
    {
        return StringConverter::to_string(
            static_cast<const ColourFaderAffector*>(target)->getAlphaAdjust() );
    }
    void ColourFaderAffector::CmdAlphaAdjust::set(void* target, const String& val)
    {
        static_cast<ColourFaderAffector*>(target)->setAlphaAdjust(
            StringConverter::parse_real(val));
    }

}



