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
#include "OgreStableHeaders.h"
#include "OgreResourceBackgroundQueue.h"

namespace Ogre {

    // Note, no locks are required here anymore because all of the parallelisation
    // is now contained in WorkQueue - this class is entirely single-threaded
    //------------------------------------------------------------------------
    //-----------------------------------------------------------------------
    template<> ResourceBackgroundQueue* Singleton<ResourceBackgroundQueue>::msSingleton = 0;
    ResourceBackgroundQueue* ResourceBackgroundQueue::getSingletonPtr(void)
    {
        return msSingleton;
    }
    ResourceBackgroundQueue& ResourceBackgroundQueue::getSingleton(void)
    {  
        assert( msSingleton );  return ( *msSingleton );  
    }
    //------------------------------------------------------------------------
    ResourceBackgroundQueue::ResourceBackgroundQueue() {}
    ResourceBackgroundQueue::~ResourceBackgroundQueue() {}
    //------------------------------------------------------------------------
    std::future<void> ResourceBackgroundQueue::initialiseResourceGroup(const String& name)
    {
        auto task = std::make_shared<std::packaged_task<void()>>(
            [name]() { ResourceGroupManager::getSingleton().initialiseResourceGroup(name); });
        Root::getSingleton().getWorkQueue()->add_task([task]() { (*task)(); });
        return task->get_future();
    }
    //------------------------------------------------------------------------
    std::future<void> ResourceBackgroundQueue::initialiseAllResourceGroups()
    {
        auto task = std::make_shared<std::packaged_task<void()>>(
            []() { ResourceGroupManager::getSingleton().initialiseAllResourceGroups(); });
        Root::getSingleton().getWorkQueue()->add_task([task]() { (*task)(); });
        return task->get_future();
    }
    //------------------------------------------------------------------------
    std::future<void> ResourceBackgroundQueue::prepareResourceGroup(const String& name)
    {
        auto task = std::make_shared<std::packaged_task<void()>>(
            [name]() { ResourceGroupManager::getSingleton().prepareResourceGroup(name); });
        Root::getSingleton().getWorkQueue()->add_task([task]() { (*task)(); });
        return task->get_future();
    }
    //------------------------------------------------------------------------
    std::future<void> ResourceBackgroundQueue::loadResourceGroup(const String& name)
    {
        auto task = std::make_shared<std::packaged_task<void()>>([name]()
        {
#   if 0
            ResourceGroupManager::getSingleton().loadResourceGroup(name);
#   else
            ResourceGroupManager::getSingleton().prepareResourceGroup(name);
            Root::getSingleton().getWorkQueue()->add_main_thread_task([name]() {
                ResourceGroupManager::getSingleton().loadResourceGroup(name);
            });
#   endif
        });
        Root::getSingleton().getWorkQueue()->add_task([task]() { (*task)(); });
        return task->get_future();
    }
    //------------------------------------------------------------------------
    std::future<void> ResourceBackgroundQueue::prepare(const ResourcePtr& res)
    {
        auto task = std::make_shared<std::packaged_task<void()>>(
            [res]()
            {
                res->prepare(true);
                Root::getSingleton().getWorkQueue()->add_main_thread_task(
                    [res]() { res->_firepreparing_complete(); });
            });
        Root::getSingleton().getWorkQueue()->add_task([task]() { (*task)(); });
        return task->get_future();
    }
    //------------------------------------------------------------------------
    std::future<void> ResourceBackgroundQueue::load(const ResourcePtr& res)
    {
        auto task = std::make_shared<std::packaged_task<void()>>(
            [res]()
            {
#if 0
                res->load(true);
                Root::getSingleton().getWorkQueue()->add_main_thread_task([res]() { res->_fireloading_complete(); });
#else
                res->prepare(true);
                Root::getSingleton().getWorkQueue()->add_main_thread_task(
                    [res]() {
                        res->load(true);
                        res->_fireloading_complete();
                    });
#endif
            });
        Root::getSingleton().getWorkQueue()->add_task([task]() { (*task)(); });
        return task->get_future();
    }
    //---------------------------------------------------------------------
    std::future<void> ResourceBackgroundQueue::unload(const ResourcePtr& res)
    {
        auto task = std::make_shared<std::packaged_task<void()>>([res]() { res->unload(); });
        Root::getSingleton().getWorkQueue()->add_task([task]() { (*task)(); });
        return task->get_future();
    }
    //---------------------------------------------------------------------
    std::future<void> ResourceBackgroundQueue::unloadResourceGroup(const String& name)
    {
        auto task = std::make_shared<std::packaged_task<void()>>(
            [name]() { ResourceGroupManager::getSingleton().unloadResourceGroup(name); });
        Root::getSingleton().getWorkQueue()->add_task([task]() { (*task)(); });
        return task->get_future();
    }

}



