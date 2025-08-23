#include <hyue/Archive.h>

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

} // namespace hyue