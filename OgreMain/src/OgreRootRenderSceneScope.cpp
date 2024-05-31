#include "OgreRootRenderSceneScope.h"
#include "OgreRoot.h"

namespace Ogre
{

RootRenderSceneScope::RootRenderSceneScope(SceneManager* sm)
{
    prev_sm_ = Root::getSingleton()._getCurrentSceneManager();
    Root::getSingleton()._setCurrentSceneManager(sm);

    // preserve the previous scheme, in case this is a RTT update with an outer _renderScene pending
    MaterialManager& matMgr = MaterialManager::getSingleton();
    prev_material_scheme_ = matMgr.getActiveScheme();
}
RootRenderSceneScope::~RootRenderSceneScope()
{
    MaterialManager& matMgr = MaterialManager::getSingleton();
    matMgr.setActiveScheme(prev_material_scheme_);

    Root::getSingleton()._setCurrentSceneManager(prev_sm_);
}

} // namespace Ogre