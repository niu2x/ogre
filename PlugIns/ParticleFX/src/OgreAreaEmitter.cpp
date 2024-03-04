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
// Original author: Tels <http://bloodgate.com>, released as public domain
#include "OgreAreaEmitter.h"
#include "OgreParticle.h"
#include "OgreQuaternion.h"
#include "exception.h"
#include "string_interface.h"



namespace Ogre {

    // Instatiate statics
    AreaEmitter::CmdWidth AreaEmitter::msWidthCmd;
    AreaEmitter::CmdHeight AreaEmitter::msHeightCmd;
    AreaEmitter::CmdDepth AreaEmitter::msDepthCmd;

    //-----------------------------------------------------------------------
    bool AreaEmitter::initDefaults(const String& t)
    {
        // called by the constructor as initDefaults("Type")

        // Defaults
        mDirection = Vector3::unit_z;
        mUp = Vector3::unit_y;
        setSize(100,100,100);
        mType = t;

        // Set up parameters
        if (create_param_dictionary(mType + "Emitter"))
        {

            addBaseParameters();
            ParamDictionary* dict = param_dictionary();

            // Custom params
            dict->add_parameter(ParameterDef("width", 
                "Width of the shape in world coordinates.",
                ParameterType::REAL),&msWidthCmd);
            dict->add_parameter(ParameterDef("height", 
                "Height of the shape in world coordinates.",
                ParameterType::REAL),&msHeightCmd);
            dict->add_parameter(ParameterDef("depth", 
                "Depth of the shape in world coordinates.",
                ParameterType::REAL),&msDepthCmd);
            return true;

        }
        return false;
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::setDirection( const Vector3& inDirection )
    {
        ParticleEmitter::setDirection( inDirection );

        // Update the ranges
        genAreaAxes();
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::setSize(const Vector3& size)
    {
        mSize = size;
        genAreaAxes();
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::setSize(Real x, Real y, Real z)
    {
        mSize.x = x;
        mSize.y = y;
        mSize.z = z;
        genAreaAxes();
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::setWidth(Real width)
    {
        mSize.x = width;
        genAreaAxes();
    }
    //-----------------------------------------------------------------------
    Real AreaEmitter::getWidth(void) const
    {
        return mSize.x;
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::setHeight(Real height)
    {
        mSize.y = height;
        genAreaAxes();
    }
    //-----------------------------------------------------------------------
    Real AreaEmitter::getHeight(void) const
    {
        return mSize.y;
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::setDepth(Real depth)
    {
        mSize.z = depth;
        genAreaAxes();
    }
    //-----------------------------------------------------------------------
    Real AreaEmitter::getDepth(void) const
    {
        return mSize.z;
    }
    //-----------------------------------------------------------------------
    void AreaEmitter::genAreaAxes(void)
    {
        Vector3 mLeft = mUp.cross_product(mDirection);

        mXRange = mLeft * (mSize.x * 0.5f);
        mYRange = mUp * (mSize.y * 0.5f);
        mZRange = mDirection * (mSize.z * 0.5f);
    }

    //-----------------------------------------------------------------------
    // Command objects
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    String AreaEmitter::CmdWidth::get(const void* target) const
    {
        return StringConverter::to_string(
            static_cast<const AreaEmitter*>(target)->getWidth() );
    }
    void AreaEmitter::CmdWidth::set(void* target, const String& val)
    {
        static_cast<AreaEmitter*>(target)->setWidth(StringConverter::parse_real(val));
    }
    //-----------------------------------------------------------------------
    String AreaEmitter::CmdHeight::get(const void* target) const
    {
        return StringConverter::to_string(
            static_cast<const AreaEmitter*>(target)->getHeight() );
    }
    void AreaEmitter::CmdHeight::set(void* target, const String& val)
    {
        static_cast<AreaEmitter*>(target)->setHeight(StringConverter::parse_real(val));
    }
    //-----------------------------------------------------------------------
    String AreaEmitter::CmdDepth::get(const void* target) const
    {
        return StringConverter::to_string(
            static_cast<const AreaEmitter*>(target)->getDepth() );
    }
    void AreaEmitter::CmdDepth::set(void* target, const String& val)
    {
        static_cast<AreaEmitter*>(target)->setDepth(StringConverter::parse_real(val));
    }



}


