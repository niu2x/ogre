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
#include "threading/thread_header.h"

namespace Ogre {
/// Dictionary of parameters
static ParamDictionaryMap dictionarys_map;

ParamDictionary::ParamDictionary() { }
ParamDictionary::~ParamDictionary() { }

ParamCommand* ParamDictionary::get_param_command(const String& name)
{
    const ParamDictionary* self = this;
    return const_cast<ParamCommand*>(self->get_param_command(name));
}

const ParamCommand* ParamDictionary::get_param_command(const String& name) const
{
    auto i = param_commands_.find(name);
    if (i != param_commands_.end()) {
        return i->second;
    } else {
        return 0;
    }
}

void ParamDictionary::add_parameter(const String& name, ParamCommand* param_cmd)
{
    param_defs_.push_back(name);
    param_commands_[name] = param_cmd;
}

bool StringInterface::create_param_dictionary(const String& class_name)
{

    ParamDictionaryMap::iterator it = dictionarys_map.find(class_name);

    if (it == dictionarys_map.end()) {
        param_dict_
            = &dictionarys_map
                   .insert(std::make_pair(class_name, ParamDictionary()))
                   .first->second;
        param_dict_name_ = class_name;
        return true;
    } else {
        param_dict_ = &it->second;
        param_dict_name_ = class_name;
        return false;
    }
}

const ParameterList& StringInterface::parameters(void) const
{
    static ParameterList empty_list;
    const ParamDictionary* dict = param_dictionary();
    if (dict)
        return dict->parameters();
    else
        return empty_list;
}

String StringInterface::get_parameter(const String& name) const
{
    // Get dictionary
    const ParamDictionary* dict = param_dictionary();

    if (dict) {
        // Look up command object
        const ParamCommand* cmd = dict->param_command(name);

        if (cmd) {
            return cmd->get(this);
        }
    }

    // Fallback
    return "";
}

bool StringInterface::set_parameter(const String& name, const String& value)
{
    // Get dictionary
    ParamDictionary* dict = param_dictionary();

    if (dict) {
        // Look up command object
        ParamCommand* cmd = dict->param_command(name);
        if (cmd) {
            cmd->set(this, value);
            return true;
        }
    }
    // Fallback
    return false;
}
//-----------------------------------------------------------------------
void StringInterface::set_parameter_list(const NameValuePairList& params)
{
    for (auto& e : params) {
        set_parameter(e.first, e.second);
    }
}

void StringInterface::copy_parameters_to(StringInterface* dest) const
{
    // Get dictionary
    if (const ParamDictionary* dict = param_dictionary()) {
        // Iterate through own parameters
        for (const auto& name : dict->parameters()) {
            dest->set_parameter(name, parameter(name));
        }
    }
}

//-----------------------------------------------------------------------
void StringInterface::clean() { dictionarys_map.clear(); }
}
