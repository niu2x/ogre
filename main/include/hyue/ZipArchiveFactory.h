#pragma once

#include <hyue/ArchiveFactory.h>

namespace hyue {

class HYUE_API ZipArchiveFactory : public ArchiveFactory {
public:
    const String& get_type(void) const override;

    using ArchiveFactory::create_instance;

    Archive* create_instance(const String& name, bool read_only) override;
};

class HYUE_API EmbeddedZipArchiveFactory : public ZipArchiveFactory {
public:
    EmbeddedZipArchiveFactory();
    virtual ~EmbeddedZipArchiveFactory();

    const String& get_type(void) const override;

    //! @cond Doxygen_Suppress
    using ArchiveFactory::create_instance;
    //! @endcond

    Archive* create_instance(const String& name, bool readOnly) override;
    void     destroy_instance(Archive* ptr) override;

    /// Add an embedded file to the embedded file list
    static void add_embbedded_file(const String& name, const uint8_t* file_data, size_t file_size);

    /// Remove an embedded file to the embedded file list
    static void remove_embbedded_file(const String& name);
};

} // namespace hyue
