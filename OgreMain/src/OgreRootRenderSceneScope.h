#ifndef __RootRenderSceneScope_H_
#define __RootRenderSceneScope_H_

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
};

} // namespace Ogre

#endif
