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
#include "OgreStableHeaders.h"
#include "OgreParticleEmitterCommands.h"
#include "OgreParticleEmitter.h"

namespace Ogre {

    namespace EmitterCommands {

        //-----------------------------------------------------------------------
        String CmdAngle::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getAngle() );
        }
        void CmdAngle::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setAngle(StringConverter::parse_angle(val));
        }
        //-----------------------------------------------------------------------
        String CmdColour::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getColour() );
        }
        void CmdColour::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setColour(StringConverter::parse_colour_value(val));
        }
        //-----------------------------------------------------------------------
        String CmdColourRangeStart::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getColourRangeStart() );
        }
        void CmdColourRangeStart::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setColourRangeStart(StringConverter::parse_colour_value(val));
        }
        //-----------------------------------------------------------------------
        String CmdColourRangeEnd::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getColourRangeEnd() );
        }
        void CmdColourRangeEnd::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setColourRangeEnd(StringConverter::parse_colour_value(val));
        }
        //-----------------------------------------------------------------------
        String CmdDirection::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getDirection() );
        }
        void CmdDirection::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setDirection(StringConverter::parse_vector3(val));
        }
        //-----------------------------------------------------------------------
        String CmdUp::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getUp() );
        }
        void CmdUp::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setUp(StringConverter::parse_vector3(val));
        }
        //-----------------------------------------------------------------------
        String CmdDirPositionRef::get(const void* target) const
        {
            Vector4 val( static_cast<const ParticleEmitter*>(target)->getDirPositionReference() );
            val.w = static_cast<const ParticleEmitter*>(target)->getDirPositionReferenceEnabled();
            return StringConverter::to_string( val );
        }
        void CmdDirPositionRef::set(void* target, const String& val)
        {
            const Vector4 parsed = StringConverter::parse_vector4(val);
            const Vector3 vPos( parsed.x, parsed.y, parsed.z );
            static_cast<ParticleEmitter*>(target)->setDirPositionReference( vPos, parsed.w != 0 );
        }
        //-----------------------------------------------------------------------
        String CmdEmissionRate::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getEmissionRate() );
        }
        void CmdEmissionRate::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setEmissionRate(StringConverter::parse_real(val));
        }
        //-----------------------------------------------------------------------
        String CmdMaxTTL::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getMaxTimeToLive() );
        }
        void CmdMaxTTL::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setMaxTimeToLive(StringConverter::parse_real(val));
        }
        //-----------------------------------------------------------------------
        String CmdMinTTL::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getMinTimeToLive() );
        }
        void CmdMinTTL::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setMinTimeToLive(StringConverter::parse_real(val));
        }
        //-----------------------------------------------------------------------
        String CmdMaxVelocity::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getMaxParticleVelocity() );
        }
        void CmdMaxVelocity::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setMaxParticleVelocity(StringConverter::parse_real(val));
        }
        //-----------------------------------------------------------------------
        String CmdMinVelocity::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getMinParticleVelocity() );
        }
        void CmdMinVelocity::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setMinParticleVelocity(StringConverter::parse_real(val));
        }
        //-----------------------------------------------------------------------
        String CmdPosition::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getPosition() );
        }
        void CmdPosition::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setPosition(StringConverter::parse_vector3(val));
        }
        //-----------------------------------------------------------------------
        String CmdTTL::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getTimeToLive() );
        }
        void CmdTTL::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setTimeToLive(StringConverter::parse_real(val));
        }
        //-----------------------------------------------------------------------
        String CmdVelocity::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getParticleVelocity() );
        }
        void CmdVelocity::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setParticleVelocity(StringConverter::parse_real(val));
        }
        //-----------------------------------------------------------------------
        String CmdDuration::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getDuration() );
        }
        void CmdDuration::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setDuration(StringConverter::parse_real(val));
        }
        //-----------------------------------------------------------------------
        String CmdMinDuration::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getMinDuration() );
        }
        void CmdMinDuration::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setMinDuration(StringConverter::parse_real(val));
        }
        //-----------------------------------------------------------------------
        String CmdMaxDuration::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getMaxDuration() );
        }
        void CmdMaxDuration::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setMaxDuration(StringConverter::parse_real(val));
        }
        //-----------------------------------------------------------------------
        String CmdRepeatDelay::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getRepeatDelay() );
        }
        void CmdRepeatDelay::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setRepeatDelay(StringConverter::parse_real(val));
        }
        //-----------------------------------------------------------------------
        String CmdMinRepeatDelay::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getMinRepeatDelay() );
        }
        void CmdMinRepeatDelay::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setMinRepeatDelay(StringConverter::parse_real(val));
        }
        //-----------------------------------------------------------------------
        String CmdMaxRepeatDelay::get(const void* target) const
        {
            return StringConverter::to_string(
                static_cast<const ParticleEmitter*>(target)->getMaxRepeatDelay() );
        }
        void CmdMaxRepeatDelay::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setMaxRepeatDelay(StringConverter::parse_real(val));
        }
        //-----------------------------------------------------------------------
        String CmdName::get(const void* target) const
        {
            return static_cast<const ParticleEmitter*>(target)->name();
        }
        void CmdName::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setName(val);
        }
        //-----------------------------------------------------------------------
        String CmdEmittedEmitter::get(const void* target) const
        {
            return 
                static_cast<const ParticleEmitter*>(target)->getEmittedEmitter();
        }
        void CmdEmittedEmitter::set(void* target, const String& val)
        {
            static_cast<ParticleEmitter*>(target)->setEmittedEmitter(val);
        }
 

    
    }
}

