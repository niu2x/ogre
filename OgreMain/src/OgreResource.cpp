/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

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
// Ogre includes
#include "OgreStableHeaders.h"

namespace Ogre 
{
//-----------------------------------------------------------------------
Resource::Resource(
    ResourceManager* creator,
    const String& name,
    ResourceHandle handle,
    const String& group,
    bool isManual,
    ManualResourceLoader* loader)
: creator_(creator)
, name_(name)
, group_(group)
, handle_(handle)
, loading_state_(LoadingState::UNLOADED)
, is_background_loaded_(false)
, is_manual_(isManual)
, size_(0)
, loader_(loader)
, state_count_(0)
{
}
//-----------------------------------------------------------------------
Resource::~Resource() { }

Resource& Resource::operator=(const Resource& rhs)
{
    name_ = rhs.name_;
    group_ = rhs.group_;
    creator_ = rhs.creator_;
    is_manual_ = rhs.is_manual_;
    loader_ = rhs.loader_;
    handle_ = rhs.handle_;
    size_ = rhs.size_;

    loading_state_.store(rhs.loading_state_.load());
    is_background_loaded_ = rhs.is_background_loaded_;

    return *this;
}
//-----------------------------------------------------------------------
void Resource::escalate_loading()
{
    // Just call load as if this is the background thread, locking on
    // load status will prevent race conditions
    load(true);
    _fire_loading_complete();
}
//-----------------------------------------------------------------------
void Resource::prepare(bool background)
{
    // quick check that avoids any synchronisation
    LoadingState old = loading_state_.load();

    if (old != LoadingState::UNLOADED && old != LoadingState::PREPARING)
        return;

    // atomically do slower check to make absolutely sure,
    // and set the load state to PREPARING
    old = LoadingState::UNLOADED;
    if (!loading_state_.compare_exchange_strong(old, LoadingState::PREPARING)) {
        while (loading_state_.load() == LoadingState::PREPARING) { }

        LoadingState state = loading_state_.load();
        if (state != LoadingState::PREPARED && state != LoadingState::LOADING
            && state != LoadingState::LOADED) {
            OGRE_EXCEPT(
                Exception::ERR_INVALIDPARAMS,
                "Another thread failed in resource operation",
                "Resource::prepare");
        }
        return;
    }

    // Scope lock for actual loading
    try {

        if (is_manual_) {
            if (loader_) {
                loader_->prepareResource(this);
            } else {
                // Warn that this resource is not reloadable
                LogManager::getSingleton().stream(LogMsgLevel::TRIVIAL)
                    << "Note: " << creator_->getResourceType() << " instance '"
                    << name_ << "' was defined as manually "
                    << "loaded, but no manual loader was provided. This "
                       "Resource "
                    << "will be lost if it has to be reloaded.";
            }
        } else {
            if (group_
                == ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME) {
                // Derive resource group
                change_group_ownership(ResourceGroupManager::getSingleton()
                                           .findGroupContainingResource(name_));
            }
            prepare_impl();
        }
    } catch (...) {
        loading_state_.store(LoadingState::UNLOADED);

        unload_impl();

        throw;
    }

    loading_state_.store(LoadingState::PREPARED);

    // Since we don't distinguish between GPU and CPU RAM, this
    // seems pointless
    // if(creator_)
    //  creator_->_notifyResourcePrepared(this);

    // Fire events (if not background)
    if (!background)
        _fire_preparing_complete();
}
//---------------------------------------------------------------------
void Resource::load(bool background)
{
    // Early-out without lock (mitigate perf cost of ensuring loaded)
    // Don't load if:
    // 1. We're already loaded
    // 2. Another thread is loading right now
    // 3. We're marked for background loading and this is not the background
    //    loading thread we're being called by

    if (is_background_loaded_ && !background)
        return;

    // This next section is to deal with cases where 2 threads are fighting over
    // who gets to prepare / load - this will only usually happen if loading is
    // escalated
    bool keep_checking = true;
    LoadingState old = LoadingState::UNLOADED;
    while (keep_checking) {
        // quick check that avoids any synchronisation
        old = loading_state_.load();

        if (old == LoadingState::PREPARING) {
            while (loading_state_.load() == LoadingState::PREPARING) { }
            old = loading_state_.load();
        }

        if (old != LoadingState::UNLOADED && old != LoadingState::PREPARED
            && old != LoadingState::LOADING)
            return;

        // atomically do slower check to make absolutely sure,
        // and set the load state to LOADING
        if (old == LoadingState::LOADING
            || !loading_state_.compare_exchange_strong(
                old,
                LoadingState::LOADING)) {
            while (loading_state_.load() == LoadingState::LOADING) { }

            LoadingState state = loading_state_.load();
            if (state == LoadingState::PREPARED
                || state == LoadingState::PREPARING) {
                // another thread is preparing, loop around
                continue;
            } else if (state != LoadingState::LOADED) {
                OGRE_EXCEPT(
                    Exception::ERR_INVALIDPARAMS,
                    "Another thread failed in resource operation",
                    "Resource::load");
            }
            return;
        }
        keep_checking = false;
    }

    // Scope lock for actual loading
    try {

        if (is_manual_) {
            if (old == LoadingState::UNLOADED && loader_)
                loader_->prepareResource(this);

            pre_load_impl();

            // Load from manual loader
            if (loader_) {
                loader_->loadResource(this);
            } else {
                // Warn that this resource is not reloadable
                LogManager::getSingleton().stream(LogMsgLevel::TRIVIAL)
                    << "Note: " << creator_->getResourceType() << " instance '"
                    << name_ << "' was defined as manually "
                    << "loaded, but no manual loader was provided. This "
                       "Resource "
                    << "will be lost if it has to be reloaded.";
            }
            post_load_impl();
        } else {

            if (old == LoadingState::UNLOADED)
                prepare_impl();

            pre_load_impl();

            if (group_
                == ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME) {
                // Derive resource group
                change_group_ownership(ResourceGroupManager::getSingleton()
                                           .findGroupContainingResource(name_));
            }

            load_impl();

            post_load_impl();
        }

        // Calculate resource size
        size_ = calculate_size();

    } catch (...) {
        // Reset loading in-progress flag, in case failed for some reason.
        // We reset it to UNLOADED because the only other case is when
        // old == PREPARED in which case the loadImpl should wipe out
        // any prepared data since it might be invalid.
        loading_state_.store(LoadingState::UNLOADED);

        unload_impl();

        // Re-throw
        throw;
    }

    loading_state_.store(LoadingState::LOADED);
    _dirty_state();

    // Notify manager
    if (creator_)
        creator_->_notifyResourceLoaded(this);

    // Fire events, if not background
    if (!background)
        _fire_loading_complete();
}
//---------------------------------------------------------------------
size_t Resource::calculate_size(void) const
{
    size_t mem_size = 0; // sizeof(*this) should be called by deriving classes
    mem_size += name_.size() * sizeof(char);
    mem_size += group_.size() * sizeof(char);
    mem_size += origin_.size() * sizeof(char);
    mem_size += sizeof(void*) * listeners_.size();

    return mem_size;
}
//---------------------------------------------------------------------
void Resource::_dirty_state()
{
    // don't worry about threading here, count only ever increases so
    // doesn't matter if we get a lost increment (one is enough)
    ++state_count_;
}
//-----------------------------------------------------------------------
void Resource::change_group_ownership(const String& new_group)
{
    if (group_ != new_group) {
        String old_group = group_;
        group_ = new_group;
        ResourceGroupManager::getSingleton()._notifyResourceGroupChanged(
            old_group,
            this);
    }
}
//-----------------------------------------------------------------------
void Resource::unload(void)
{
    // Early-out without lock (mitigate perf cost of ensuring unloaded)
    LoadingState old = loading_state_.load();
    if (old != LoadingState::LOADED && old != LoadingState::PREPARED)
        return;

    if (!loading_state_.compare_exchange_strong(old, LoadingState::UNLOADING))
        return;

    // Scope lock for actual unload
    {

        if (old == LoadingState::PREPARED) {
            unprepare_impl();
        } else {
            pre_unload_impl();
            unload_impl();
            post_unload_impl();
        }
    }

    loading_state_.store(LoadingState::UNLOADED);

    // Notify manager
    // Note if we have gone from PREPARED to UNLOADED, then we haven't actually
    // unloaded, i.e. there is no memory freed on the GPU.
    if (old == LoadingState::LOADED && creator_)
        creator_->_notifyResourceUnloaded(this);

    _fire_unloading_complete();
}
//-----------------------------------------------------------------------
void Resource::reload(LoadingFlags flags)
{

    if (loading_state_.load() == LoadingState::LOADED) {
        unload();
        load();
    }
}
//-----------------------------------------------------------------------
void Resource::touch(void)
{
    // make sure loaded
    load();

    if (creator_)
        creator_->_notifyResourceTouched(this);
}
//-----------------------------------------------------------------------
void Resource::add_listener(Resource::Listener* lis) { listeners_.insert(lis); }
//-----------------------------------------------------------------------
void Resource::remove_listener(Resource::Listener* lis)
{
    // O(n) but not called very often

    listeners_.erase(lis);
}
//-----------------------------------------------------------------------
void Resource::_fire_loading_complete(bool unused)
{
    // Lock the listener list

    for (auto& l : listeners_) {
        l->loading_complete(this);
    }
}
//-----------------------------------------------------------------------
void Resource::_fire_preparing_complete(bool unused)
{
    // Lock the listener list

    for (auto& l : listeners_) {
        l->preparing_complete(this);
    }
}
//-----------------------------------------------------------------------
void Resource::_fire_unloading_complete(void)
{
    // Lock the listener list

    for (auto& l : listeners_) {
        l->unloading_complete(this);
    }
}
}
