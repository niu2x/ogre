#pragma once

namespace hyue {

template <typename T>
class FactoryObj {
public:
    virtual ~FactoryObj()
    {
    }

    /** Returns the factory type.
    @return
        The factory type.
    */
    virtual const String& get_type() const = 0;

    /** Creates a new object.
    @param name Name of the object to create
    @return
        An object created by the factory. The type of the object depends on
        the factory.
    */
    virtual T* create_instance(const String& name) = 0;
    /** Destroys an object which was created by this factory.
    @param ptr Pointer to the object to destroy
    */
    virtual void destroy_instance(T* ptr)
    {
        delete ptr;
    }
};

} // namespace hyue
