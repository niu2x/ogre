#include "OgreRootRenderSceneScope.h"
#include "OgreRoot.h"

namespace Ogre
{

RootRenderSceneScope::RootRenderSceneScope(SceneManager* sm)
{
    prev_sm_ = Root::getSingleton()._getCurrentSceneManager();
    Root::getSingleton()._setCurrentSceneManager(sm);
}
RootRenderSceneScope::~RootRenderSceneScope() { Root::getSingleton()._setCurrentSceneManager(prev_sm_); }

} // namespace Ogre