#include <hyue/Archive.h>

#include <hyue/panic.h>

namespace hyue {

Archive::Archive(const String& name, const String& arch_type)
: name_(name),
  type_(arch_type),
  read_only_(true)
{
}

/** Default destructor.
 */
Archive::~Archive()
{
}

DataStreamPtr Archive::create(const String&)
{
    panic("This archive does not support creation of files. Archive::create");
    return nullptr;
}

//---------------------------------------------------------------------
void Archive::remove(const String&)
{
    panic("This archive does not support removal of files. Archive::remove");
}

} // namespace hyue