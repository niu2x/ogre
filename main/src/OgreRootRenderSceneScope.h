#ifndef __RootRenderSceneScope_H_
#define __RootRenderSceneScope_H_

#include "OgreString.h"

namespace Ogre
{

// class Root;
class SceneManager;

class RootRenderSceneScope
{
public:
    RootRenderSceneScope(SceneManager* sm);
    ~RootRenderSceneScope();

private:
    SceneManager* prev_sm_;
    String prev_material_scheme_;
};

} // namespace Ogre

#endif
