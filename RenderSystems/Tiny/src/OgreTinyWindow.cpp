// This file is part of the OGRE project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at https://www.ogre3d.org/licensing.
// SPDX-License-Identifier: MIT

#include "OgreTinyWindow.h"
#include "exception.h"
#include "string_interface.h"

#include "OgreComponents.h"
#if OGRE_BITES_HAVE_SDL
#include <SDL.h>
#endif

namespace Ogre
{
TinyWindow::TinyWindow() : mParentWindow(NULL)
{
    mIsFullScreen = false;
    mActive = true;
}

void TinyWindow::create(const String& name, uint width, uint height,
                            bool fullScreen, const NameValuePairList *miscParams)
{
    mName = name;

    if(miscParams)
    {
        auto it = miscParams->find("sdlwin");
        if(it != miscParams->end())
            mParentWindow = (SDL_Window*)StringConverter::parse_size_t(it->second);
    }

    resize(width, height);
}

void TinyWindow::resize(uint width, uint height)
{
    mWidth = width;
    mHeight = height;
    mBuffer.create(PixelFormat::BYTE_RGB, width, height);
}

void TinyWindow::swapBuffers()
{
#if OGRE_BITES_HAVE_SDL
    if(!mParentWindow)
        return;

    SDL_Surface* surface = SDL_GetWindowSurface(mParentWindow);

    SDL_LockSurface(surface);

    auto format = surface->format->BytesPerPixel == 4 ? PixelFormat::BYTE_BGRA
                                                      : PixelFormat::BYTE_BGR;

    PixelBox dst(surface->w, surface->h, 1, format, surface->pixels);
    dst.row_pitch = surface->pitch / surface->format->BytesPerPixel;
    dst.data = (uchar*)surface->pixels;

    copyContentsToMemory(dst, dst, FB_BACK);

    SDL_UnlockSurface(surface);

    SDL_UpdateWindowSurface(mParentWindow);
#endif
}

void TinyWindow::copyContentsToMemory(const Box& src, const PixelBox& dst, FrameBuffer buffer)
{
    if (mClosed)
        return;

    if (src.right > mWidth || src.bottom > mHeight || src.front != 0 || src.back != 1 ||
        dst.width() != src.width() || dst.height() != src.height() || dst.depth() != 1)
    {
        OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid box");
    }

    PixelUtil::bulk_pixel_conversion(
        mBuffer.getPixelBox().get_sub_volume(src),
        dst);
}
} // namespace Ogre
