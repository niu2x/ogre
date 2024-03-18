// This file is part of the OGRE project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at https://www.ogre3d.org/licensing.
// SPDX-License-Identifier: MIT
#include "OgreTinyHardwarePixelBuffer.h"

namespace Ogre {

    TinyHardwarePixelBuffer::TinyHardwarePixelBuffer(const PixelBox& data, Usage usage)
        : HardwarePixelBuffer(data.width(), data.height(), data.depth(), data.format, usage, false), mBuffer(data)
    {
    }

    void TinyHardwarePixelBuffer::blitFromMemory(const PixelBox &src, const Box &dstBox)
    {
        if (!mBuffer.contains(dstBox))
        {
            OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Destination box out of range");
        }

        PixelBox scaled;
        if (src.getSize() != dstBox.getSize())
        {
            // Scale to destination size.
            // This also does pixel format conversion if needed.
            scaled = mBuffer.get_sub_volume(dstBox);
            Image::scale(src, scaled, Image::FILTER_BILINEAR);
        }
        else
        {
            scaled = mBuffer.get_sub_volume(dstBox);
            PixelUtil::bulk_pixel_conversion(src, scaled);
        }
    }

    void TinyHardwarePixelBuffer::blitToMemory(const Box &srcBox, const PixelBox &dst)
    {
        if (!mBuffer.contains(srcBox))
        {
            OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "source box out of range");
        }

        if(srcBox.getSize() != dst.getSize())
        {
            // We need scaling
            Image::scale(
                mBuffer.get_sub_volume(srcBox),
                dst,
                Image::FILTER_BILINEAR);
        }
        else
        {
            // Just copy the bit that we need
            PixelUtil::bulk_pixel_conversion(
                mBuffer.get_sub_volume(srcBox),
                dst);
        }
    }
}
