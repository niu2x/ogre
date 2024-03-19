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

#ifndef __StringInterface_H__
#define __StringInterface_H__

#include "OgrePrerequisites.h"
#include "OgreCommon.h"
#include "OgreHeaderPrefix.h"
#include "string_converter.h"

namespace Ogre {

/** \addtogroup Core
 *  @{
 */
/** \addtogroup General
 *  @{
 */

/// @deprecated do not use
enum class ParameterType {
    BOOL,
    REAL,
    INT,
    UNSIGNED_INT,
    SHORT,
    UNSIGNED_SHORT,
    LONG,
    UNSIGNED_LONG,
    STRING,
    VECTOR3,
    MATRIX3,
    MATRIX4,
    QUATERNION,
    ColorValue
};

/// @deprecated directly pass parameter name
struct ParameterDef {
    String name;
    ParameterDef(
        const String& p_name,
        const String& = "",
        ParameterType = ParameterType::INT)
    : name(p_name)
    {
    }
};

using ParameterList = std::vector<String>;

/** Abstract class which is command object which gets/sets parameters.*/
class ParamCommand {
public:
    virtual String get(const void* target) const = 0;
    virtual void set(void* target, const String& val) = 0;

    virtual ~ParamCommand() { }
};
using ParamCommandMap = std::map<String, ParamCommand*>;

#ifndef SWIG
/** Generic ParamCommand implementation
 stores pointers to the class getter and setter functions */
template <
    typename _Class,
    typename Param,
    Param (_Class::*getter)() const,
    void (_Class::*setter)(Param)>
class SimpleParamCommand : public ParamCommand {
public:
    String get(const void* target) const override
    {
        auto obj = static_cast<const _Class*>(target);
        return StringConverter::to_string((obj->*getter)());
    }

    void set(void* target, const String& val) override
    {
        typename std::decay<Param>::type tmp;
        StringConverter::parse(val, tmp);
        auto obj = static_cast<_Class*>(target);
        (obj->*setter)(tmp);
    }
};

/// specialization for strings
template <
    typename _Class,
    const String& (_Class::*getter)() const,
    void (_Class::*setter)(const String&)>
class SimpleParamCommand<_Class, const String&, getter, setter>
: public ParamCommand {
public:
    String get(const void* target) const override
    {
        return (static_cast<const _Class*>(target)->*getter)();
    }

    void set(void* target, const String& val) override
    {
        (static_cast<_Class*>(target)->*setter)(val);
    }
};
#endif

/** Class to hold a dictionary of parameters for a single class. */
class ParamDictionary {

public:
    ParamDictionary();
    ~ParamDictionary();
    /** Method for adding a parameter definition for this class.
    @param name The name of the parameter
    @param paramCmd Pointer to a ParamCommand subclass to handle the getting /
    setting of this parameter. NB this class will not destroy this on shutdown,
    please ensure you do

    */
    void add_parameter(const String& name, ParamCommand* paramCmd);

    /// @deprecated do not use
    void add_parameter(const ParameterDef& def, ParamCommand* paramCmd)
    {
        add_parameter(def.name, paramCmd);
    }
    /** Retrieves a list of parameters valid for this object.
    @return
        A reference to a static list of ParameterDef objects.

    */
    const ParameterList& parameters(void) const { return param_defs_; }


    /** Retrieves the parameter command object for a named parameter. */
    ParamCommand* get_param_command(const String& name);
    const ParamCommand* get_param_command(const String& name) const;

private:
    ParameterList param_defs_;

    /// Command objects to get/set
    ParamCommandMap param_commands_;


};
using ParamDictionaryMap = std::map<String, ParamDictionary>;

/** Class defining the common interface which classes can use to
    present a reflection-style, self-defining parameter set to callers.

    This class also holds a static map of class name to parameter dictionaries
    for each subclass to use. See ParamDictionary for details.

    In order to use this class, each subclass must call create_param_dictionary
   in their constructors which will create a parameter dictionary for the class
   if it does not exist yet.
*/
class StringInterface {

public:
    StringInterface()
    : param_dict_(NULL)
    {
    }

    /** Virtual destructor, see Effective C++ */
    virtual ~StringInterface() { }

    /** Retrieves the parameter dictionary for this class.

        Only valid to call this after create_param_dictionary.
    @return
        Pointer to ParamDictionary shared by all instances of this class
        which you can add parameters to, retrieve parameters etc.
    */
    ParamDictionary* param_dictionary() { return param_dict_; }

    const ParamDictionary* param_dictionary() const { return param_dict_; }

    /** Retrieves a list of parameters valid for this object.
    @return
        A reference to a static list of ParameterDef objects.

    */
    const ParameterList& parameters() const;

    /** Generic parameter setting method.

        Call this method with the name of a parameter and a string version of
    the value to set. The implementor will convert the string to a native type
    internally. If in doubt, check the parameter definition in the list returned
    from StringInterface::parameters.
    @param
        name The name of the parameter to set
    @param
        value String value. Must be in the right format for the type specified
    in the parameter definition. See the StringConverter class for more
    information.
    @return
        true if set was successful, false otherwise (NB no exceptions thrown -
    tolerant method)
    */
    bool set_parameter(const String& name, const String& value);
    /** Generic multiple parameter setting method.

        Call this method with a list of name / value pairs
        to set. The implementor will convert the string to a native type
    internally. If in doubt, check the parameter definition in the list returned
    from StringInterface::parameters.
    @param
        paramList Name/value pair list
    */
    void set_parameter_list(const NameValuePairList& paramList);
    /** Generic parameter retrieval method.

        Call this method with the name of a parameter to retrieve a
    string-format value of the parameter in question. If in doubt, check the
    parameter definition in the list returned from parameters for the type of
    this parameter. If you like you can use StringConverter to convert this
    string back into a native type.
    @param
        name The name of the parameter to get
    @return
        String value of parameter, blank if not found
    */
    String get_parameter(const String& name) const;
    /** Method for copying this object's parameters to another object.

        This method takes the values of all the object's parameters and tries to
    set the same values on the destination object. This provides a completely
    type independent way to copy parameters to other objects. Note that because
    of the String manipulation involved, this should not be regarded as an
    efficient process and should be saved for times outside of the rendering
    loop.
    @par
        Any unrecognised parameters will be ignored as with set_parameter
    method.
    @param dest Pointer to object to have it's parameters set the same as this
    object.

    */
    void copy_parameters_to(StringInterface* dest) const;
    /** Cleans up the static 'msDictionary' required to reset Ogre,
    otherwise the containers are left with invalid pointers, which will lead to
    a crash as soon as one of the ResourceManager implementers (e.g.
    MaterialManager) initializes.*/
    static void clean();



    /** Internal method for creating a parameter dictionary for the class, if it
    does not already exist.

        This method will check to see if a parameter dictionary exist for this
    class yet, and if not will create one. NB you must supply the name of the
    class (RTTI is not used or performance).
    @param
        className the name of the class using the dictionary
    @return
        true if a new dictionary was created, false if it was already there
    */
    bool create_param_dictionary(const String& className);

private:
    /// Class name for this instance to be used as a lookup (must be initialised
    /// by subclasses)
    String param_dict_name_;
    ParamDictionary* param_dict_;


};

/** @} */
/** @} */

} // namespace Ogre

#include "OgreHeaderSuffix.h"

#endif
