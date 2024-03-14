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
#include "resource_manager"

namespace Ogre {

//-----------------------------------------------------------------------
ResourceManager::ResourceManager(const String& type)
: next_handle_(1)
, memory_usage_(0)
, verbose_(true)
, load_order_(0)
, resource_type_(type)
{
    // Init memory limit & usage
    memory_budget_ = std::numeric_limits<unsigned long>::max();
}
//-----------------------------------------------------------------------
ResourceManager::~ResourceManager()
{
    destroy_all_resource_pools();
    remove_all();
}
void ResourceManager::parse_script(
    DataStreamPtr& stream,
    const String& groupName)
{
    ScriptCompilerManager::getSingleton().parse_script(stream, groupName);
}
//-----------------------------------------------------------------------
ResourcePtr ResourceManager::create_resource(
    const String& name,
    const String& group,
    bool is_manual,
    ManualResourceLoader* loader,
    const NameValuePairList* params)
{
    OgreAssert(!name.empty(), "resource name must not be empty");

    // Call creation implementation
    ResourcePtr ret = ResourcePtr(create_impl(
        name,
        generate_next_handle(),
        group,
        is_manual,
        loader,
        params));
    if (params)
        ret->set_parameter_list(*params);

    add_impl(ret);
    // Tell resource group manager
    if (ret)
        ResourceGroupManager::getSingleton()._notifyResourceCreated(ret);
    return ret;
}
//-----------------------------------------------------------------------
ResourceManager::Resourcecreate_or_retrieveResult
ResourceManager::create_or_retrieve(
    const String& name,
    const String& group,
    bool isManual,
    ManualResourceLoader* loader,
    const NameValuePairList* params)
{
    // Lock for the whole get / insert

    ResourcePtr res = get_resource_by_name(name, group);
    bool created = false;
    if (!res) {
        created = true;
        res = create_resource(name, group, isManual, loader, params);
    }

    return Resourcecreate_or_retrieveResult(res, created);
}
//-----------------------------------------------------------------------
ResourcePtr ResourceManager::prepare(
    const String& name,
    const String& group,
    bool isManual,
    ManualResourceLoader* loader,
    const NameValuePairList* loadParams,
    bool backgroundThread)
{
    ResourcePtr r
        = create_or_retrieve(name, group, isManual, loader, loadParams).first;
    // ensure prepared
    r->prepare(backgroundThread);
    return r;
}
//-----------------------------------------------------------------------
ResourcePtr ResourceManager::load(
    const String& name,
    const String& group,
    bool isManual,
    ManualResourceLoader* loader,
    const NameValuePairList* loadParams,
    bool backgroundThread)
{
    ResourcePtr r
        = create_or_retrieve(name, group, isManual, loader, loadParams).first;
    // ensure loaded
    r->load(backgroundThread);

    return r;
}
//-----------------------------------------------------------------------
void ResourceManager::add_impl(ResourcePtr& res)
{

    std::pair<ResourceMap::iterator, bool> result;
    if (ResourceGroupManager::getSingleton().isResourceGroupInGlobalPool(
            res->group())) {
        result = resources_.emplace(res->name(), res);
    } else {
        // we will create the group if it doesn't exists in our list
        auto resgroup
            = resources_with_group_.emplace(res->group(), ResourceMap()).first;
        result = resgroup->second.emplace(res->name(), res);
    }

    // Attempt to resolve the collision
    ResourceLoadingListener* listener
        = ResourceGroupManager::getSingleton().getLoadingListener();
    if (!result.second && listener) {
        if (listener->resourceCollision(res.get(), this) == false) {
            // explicitly use previous instance and destroy current
            res.reset();
            return;
        }

        // Try to do the addition again, no seconds attempts to resolve
        // collisions are allowed
        if (ResourceGroupManager::getSingleton().isResourceGroupInGlobalPool(
                res->group())) {
            result = resources_.emplace(res->name(), res);
        } else {
            auto resgroup
                = resources_with_group_.emplace(res->group(), ResourceMap())
                      .first;
            result = resgroup->second.emplace(res->name(), res);
        }
    }

    if (!result.second) {
        OGRE_EXCEPT(
            Exception::ERR_DUPLICATE_ITEM,
            resource_type() + " with the name " + res->name()
                + " already exists.",
            "ResourceManager::add");
    }

    // Insert the handle
    std::pair<ResourceHandleMap::iterator, bool> resultHandle
        = resources_by_handle_.emplace(res->handle(), res);
    if (!resultHandle.second) {
        OGRE_EXCEPT(
            Exception::ERR_DUPLICATE_ITEM,
            resource_type() + " with the handle "
                + StringConverter::to_string((long)(res->handle()))
                + " already exists.",
            "ResourceManager::add");
    }
}
//-----------------------------------------------------------------------
void ResourceManager::remove_impl(const ResourcePtr& res)
{
    OgreAssert(res, "attempting to remove nullptr");

#if OGRE_RESOURCEMANAGER_STRICT
    if (res->creator() != this)
        OGRE_EXCEPT(
            Exception::ERR_INVALIDPARAMS,
            "Resource '" + res->name() + "' was not created by the '"
                + resource_type() + "' ResourceManager");
#endif

    if (ResourceGroupManager::getSingleton().isResourceGroupInGlobalPool(
            res->group())) {
        ResourceMap::iterator nameIt = resources_.find(res->name());
        if (nameIt != resources_.end()) {
            resources_.erase(nameIt);
        }
    } else {
        ResourceWithGroupMap::iterator groupIt
            = resources_with_group_.find(res->group());
        if (groupIt != resources_with_group_.end()) {
            ResourceMap::iterator nameIt = groupIt->second.find(res->name());
            if (nameIt != groupIt->second.end()) {
                groupIt->second.erase(nameIt);
            }

            if (groupIt->second.empty()) {
                resources_with_group_.erase(groupIt);
            }
        }
    }

    ResourceHandleMap::iterator handleIt
        = resources_by_handle_.find(res->handle());
    if (handleIt != resources_by_handle_.end()) {
        resources_by_handle_.erase(handleIt);
    }
    // Tell resource group manager
    ResourceGroupManager::getSingleton()._notifyResourceRemoved(res);
}
//-----------------------------------------------------------------------
void ResourceManager::set_memory_budget(size_t bytes)
{
    // Update limit & check usage
    memory_budget_ = bytes;
    check_usage();
}
//-----------------------------------------------------------------------
size_t ResourceManager::memory_budget(void) const { return memory_budget_; }
//-----------------------------------------------------------------------
void ResourceManager::unload(const String& name, const String& group)
{
    ResourcePtr res = get_resource_by_name(name, group);

#if OGRE_RESOURCEMANAGER_STRICT
    if (!res)
        OGRE_EXCEPT(
            Exception::ERR_INVALIDPARAMS,
            "attempting to unload unknown resource: " + name + " in group "
                + group);
#endif

    if (res) {
        res->unload();
    }
}
//-----------------------------------------------------------------------
void ResourceManager::unload(ResourceHandle handle)
{
    ResourcePtr res = getByHandle(handle);

#if OGRE_RESOURCEMANAGER_STRICT
    OgreAssert(res, "attempting to unload unknown resource");
#endif

    if (res) {
        res->unload();
    }
}
//-----------------------------------------------------------------------
void ResourceManager::unload_all(Resource::LoadingFlags flags)
{

    bool reloadable_only = (flags & Resource::LF_INCLUDE_NON_RELOADABLE) == 0;
    bool unreferenced_only = (flags & Resource::LF_ONLY_UNREFERENCED) != 0;
    for (auto& r : resources_) {
        // A use count of 3 means that only RGM and RM have references
        // RGM has one (this one) and RM has 2 (by name and by handle)
        if (!unreferenced_only
            || r.second.use_count()
                == ResourceGroupManager::RESOURCE_SYSTEM_NUM_REFERENCE_COUNTS) {
            Resource* res = r.second.get();
            if (!reloadable_only || res->reloadable()) {
                res->unload();
            }
        }
    }
}
//-----------------------------------------------------------------------
void ResourceManager::reload_all(Resource::LoadingFlags flags)
{

    bool reloadable_only = (flags & Resource::LF_INCLUDE_NON_RELOADABLE) == 0;
    bool unreferenced_only = (flags & Resource::LF_ONLY_UNREFERENCED) != 0;
    for (auto& r : resources_) {
        // A use count of 3 means that only RGM and RM have references
        // RGM has one (this one) and RM has 2 (by name and by handle)
        if (!unreferenced_only
            || r.second.use_count()
                == ResourceGroupManager::RESOURCE_SYSTEM_NUM_REFERENCE_COUNTS) {
            Resource* res = r.second.get();
            if (!reloadable_only || res->reloadable()) {
                res->reload(flags);
            }
        }
    }
}
//-----------------------------------------------------------------------
void ResourceManager::remove(const ResourcePtr& res) { remove_impl(res); }
//-----------------------------------------------------------------------
void ResourceManager::remove(const String& name, const String& group)
{
    ResourcePtr res = get_resource_by_name(name, group);

#if OGRE_RESOURCEMANAGER_STRICT
    if (!res)
        OGRE_EXCEPT(
            Exception::ERR_INVALIDPARAMS,
            "attempting to remove unknown resource: " + name + " in group "
                + group);
#endif

    if (res) {
        remove_impl(res);
    }
}
//-----------------------------------------------------------------------
void ResourceManager::remove(ResourceHandle handle)
{
    ResourcePtr res = getByHandle(handle);

#if OGRE_RESOURCEMANAGER_STRICT
    OgreAssert(res, "attempting to remove unknown resource");
#endif

    if (res) {
        remove_impl(res);
    }
}
//-----------------------------------------------------------------------
void ResourceManager::remove_all(void)
{

    resources_.clear();
    resources_with_group_.clear();
    resources_by_handle_.clear();
    // Notify resource group manager
    ResourceGroupManager::getSingleton()._notifyAllResourcesRemoved(this);
}
//-----------------------------------------------------------------------
void ResourceManager::removeUnreferencedResources(bool reloadable_only)
{

    ResourceMap::iterator i, iend;
    iend = resources_.end();
    for (i = resources_.begin(); i != iend;) {
        // A use count of 3 means that only RGM and RM have references
        // RGM has one (this one) and RM has 2 (by name and by handle)
        if (i->second.use_count()
            == ResourceGroupManager::RESOURCE_SYSTEM_NUM_REFERENCE_COUNTS) {
            Resource* res = (i++)->second.get();
            if (!reloadable_only || res->reloadable()) {
                remove(res->handle());
            }
        } else {
            ++i;
        }
    }
}
//-----------------------------------------------------------------------
ResourcePtr ResourceManager::get_resource_by_name(
    const String& name,
    const String& groupName) const
{

    // resource should be in global pool
    bool isGlobal
        = ResourceGroupManager::getSingleton().isResourceGroupInGlobalPool(
            groupName);

    if (isGlobal) {
        auto it = resources_.find(name);
        if (it != resources_.end()) {
            return it->second;
        }
    }

    // look in all grouped pools
    if (groupName == ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME) {
        for (auto& r : resources_with_group_) {
            auto resMapIt = r.second.find(name);

            if (resMapIt != r.second.end()) {
                return resMapIt->second;
            }
        }
    } else if (!isGlobal) {
        // look in the grouped pool
        auto itGroup = resources_with_group_.find(groupName);
        if (itGroup != resources_with_group_.end()) {
            auto it = itGroup->second.find(name);

            if (it != itGroup->second.end()) {
                return it->second;
            }
        }

#if !OGRE_RESOURCEMANAGER_STRICT
        // fall back to global
        auto it = resources_.find(name);
        if (it != resources_.end()) {
            return it->second;
        }
#endif
    }

    return ResourcePtr();
}
//-----------------------------------------------------------------------
ResourcePtr ResourceManager::getByHandle(ResourceHandle handle) const
{

    auto it = resources_by_handle_.find(handle);
    return it == resources_by_handle_.end() ? ResourcePtr() : it->second;
}
//-----------------------------------------------------------------------
ResourceHandle ResourceManager::generate_next_handle(void)
{
    return next_handle_++;
}
//-----------------------------------------------------------------------
void ResourceManager::check_usage(void)
{
    if (memory_usage() > memory_budget_) {

        // unload unreferenced resources until we are within our budget again
        ResourceMap::iterator i, iend;
        iend = resources_.end();
        for (i = resources_.begin();
             i != iend && memory_usage() > memory_budget_;
             ++i) {
            // A use count of 3 means that only RGM and RM have references
            // RGM has one (this one) and RM has 2 (by name and by handle)
            if (i->second.use_count()
                == ResourceGroupManager::RESOURCE_SYSTEM_NUM_REFERENCE_COUNTS) {
                Resource* res = i->second.get();
                if (res->reloadable()) {
                    res->unload();
                }
            }
        }
    }
}
//-----------------------------------------------------------------------
void ResourceManager::_notify_resource_touched(Resource* res)
{
    // TODO
}
//-----------------------------------------------------------------------
void ResourceManager::_notify_resource_loaded(Resource* res)
{
    memory_usage_ += res->size();
    check_usage();
}
//-----------------------------------------------------------------------
void ResourceManager::_notify_resource_unloaded(Resource* res)
{
    memory_usage_ -= res->size();
}
//---------------------------------------------------------------------
ResourceManager::ResourcePool*
ResourceManager::get_resource_pool(const String& name)
{

    ResourcePoolMap::iterator i = resource_pool_map_.find(name);
    if (i == resource_pool_map_.end()) {
        i = resource_pool_map_
                .insert(ResourcePoolMap::value_type(
                    name,
                    OGRE_NEW ResourcePool(name)))
                .first;
    }
    return i->second;
}
//---------------------------------------------------------------------
void ResourceManager::destroy_resource_pool(ResourcePool* pool)
{
    OgreAssert(pool, "Cannot destroy a null ResourcePool");

    ResourcePoolMap::iterator i = resource_pool_map_.find(pool->name());
    if (i != resource_pool_map_.end())
        resource_pool_map_.erase(i);

    OGRE_DELETE pool;
}
//---------------------------------------------------------------------
void ResourceManager::destroy_resource_pool(const String& name)
{

    ResourcePoolMap::iterator i = resource_pool_map_.find(name);
    if (i != resource_pool_map_.end()) {
        OGRE_DELETE i->second;
        resource_pool_map_.erase(i);
    }
}
//---------------------------------------------------------------------
void ResourceManager::destroy_all_resource_pools()
{

    for (auto& i : resource_pool_map_)
        OGRE_DELETE i.second;

    resource_pool_map_.clear();
}
//-----------------------------------------------------------------------
//---------------------------------------------------------------------
ResourceManager::ResourcePool::ResourcePool(const String& name)
: name_(name)
{
}
//---------------------------------------------------------------------
ResourceManager::ResourcePool::~ResourcePool() { clear(); }
//---------------------------------------------------------------------
const String& ResourceManager::ResourcePool::name() const { return name_; }
//---------------------------------------------------------------------
void ResourceManager::ResourcePool::clear()
{
    for (auto& i : *items()) {
        i->creator()->remove(i->handle());
    }

    Pool<ResourcePtr>::clear();
}
} // namespace Ogre
