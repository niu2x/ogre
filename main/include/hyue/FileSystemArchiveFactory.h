#pragma once

#include <hyue/type.h>

class HYUE_API FileSystemArchiveFactory : public ArchiveFactory {
public:
    /// @copydoc FactoryObj::getType
    const String& getType(void) const override;

    //! @cond Doxygen_Suppress
    using ArchiveFactory::createInstance;
    //! @endcond

    Archive* createInstance(const String& name, bool readOnly) override;

    /// Set whether filesystem enumeration will include hidden files or not.
    /// This should be called prior to declaring and/or initializing filesystem
    /// resource locations. The default is true (ignore hidden files).
    static void setIgnoreHidden(bool ignore);

    /// Get whether hidden files are ignored during filesystem enumeration.
    static bool getIgnoreHidden();
};
