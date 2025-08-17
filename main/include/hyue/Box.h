#pragma once

#include <cstdint>

#include <hyue/export.h>
#include <hyue/math.h>

namespace hyue {

struct HYUE_API Box
{
    uint32_t left, top, right, bottom, front, back;
    /// Parameterless constructor for setting the members manually
    Box()
        : left(0), top(0), right(1), bottom(1), front(0), back(1)
    {
    }
    /** Define a box from left, top, right and bottom coordinates
        This box will have depth one (front=0 and back=1).
        @param  l   x value of left edge
        @param  t   y value of top edge
        @param  r   x value of right edge
        @param  b   y value of bottom edge
        @note @copydetails Ogre::Box
    */
    Box(uint32_t l, uint32_t t, uint32_t r, uint32_t b):
        left(l),
        top(t),   
        right(r),
        bottom(b),
        front(0),
        back(1)
    {
        assert(right >= left && bottom >= top && back >= front);
    }

    /// @overload
    // template <typename T> 
    // explicit Box(const TRect<T>& r) : Box(r.left, r.top, r.right, r.bottom) {}

    /** Define a box from left, top, front, right, bottom and back
        coordinates.
        @param  l   x value of left edge
        @param  t   y value of top edge
        @param  f  z value of front edge
        @param  r   x value of right edge
        @param  b   y value of bottom edge
        @param  b  z value of back edge
        @note @copydetails Ogre::Box
    */
    Box( uint32_t l, uint32_t t, uint32_t f, uint32_t r, uint32_t b, uint32_t b ):
        left(l),
        top(t),   
        right(r),
        bottom(b),
        front(f),
        back(b)
    {
        // assert(right >= left && bottom >= top && back >= front);
    }

    /// @overload
    explicit Box(const ivec3& size)
        : left(0), top(0), right(size[0]), bottom(size[1]), front(0), back(size[2])
    {
    }

    /// Return true if the other box is a part of this one
    bool contains(const Box &def) const
    {
        return (def.left >= left && def.top >= top && def.front >= front &&
            def.right <= right && def.bottom <= bottom && def.back <= back);
    }
    
    /// Get the width of this box
    uint32_t get_width() const { return right-left; }
    /// Get the height of this box
    uint32_t get_height() const { return bottom-top; }
    /// Get the depth of this box
    uint32_t get_depth() const { return back-front; }

    /// origin (top, left, front) of the box
    ivec3 get_origin() const { return ivec3(left, top, front); }
    /// size (width, height, depth) of the box
    ivec3 get_size() const { return ivec3(get_width(), get_height(), get_depth()); }
};

}
