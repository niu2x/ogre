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
#ifndef __AxisAlignedBox_H_
#define __AxisAlignedBox_H_

#include <array>

// Precompiler options
#include "OgrePrerequisites.h"

#include "matrix4.h"

namespace Ogre {
    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup Math
    *  @{
    */

    /** A 3D box aligned with the x/y/z axes.

    This class represents a simple box which is aligned with the
    axes. Internally it only stores 2 points as the extremeties of
    the box, one which is the minima of all 3 axes, and the other
    which is the maxima of all 3 axes. This class is typically used
    for an axis-aligned bounding box (AABB) for collision and
    visibility determination.
    */
    class AxisAlignedBox
    {
    public:
        enum class Extent
        {
            NIL,
            FINITE,
            INFINITE
        };
    private:

        Vector3 minimum_;
        Vector3 maximum_;
        Extent extent_;

    public:
        /*
           1-------2
          /|      /|
         / |     / |
        5-------4  |
        |  0----|--3
        | /     | /
        |/      |/
        6-------7
        */
        enum class Corner {
            FAR_LEFT_BOTTOM = 0,
            FAR_LEFT_TOP = 1,
            FAR_RIGHT_TOP = 2,
            FAR_RIGHT_BOTTOM = 3,
            NEAR_RIGHT_BOTTOM = 7,
            NEAR_LEFT_BOTTOM = 6,
            NEAR_LEFT_TOP = 5,
            NEAR_RIGHT_TOP = 4
        };
        typedef std::array<Vector3, 8> Corners;

        AxisAlignedBox()
        {
            // Default to a null box 
            set_minimum( -0.5, -0.5, -0.5 );
            set_maximum( 0.5, 0.5, 0.5 );
            extent_ = Extent::NIL;
        }
        AxisAlignedBox(Extent e)
        {
            set_minimum( -0.5, -0.5, -0.5 );
            set_maximum( 0.5, 0.5, 0.5 );
            extent_ = e;
        }

        AxisAlignedBox( const Vector3& min, const Vector3& max )
        {
            set_extents( min, max );
        }

        AxisAlignedBox(Real mx, Real my, Real mz, Real Mx, Real My, Real Mz)
        {
            set_extents( mx, my, mz, Mx, My, Mz );
        }

        /** Gets the minimum corner of the box.
        */
        inline const Vector3& minimum(void) const
        { 
            return minimum_; 
        }

        /** Gets a modifiable version of the minimum
        corner of the box.
        */
        inline Vector3& minimum(void)
        { 
            return minimum_; 
        }

        /** Gets the maximum corner of the box.
        */
        inline const Vector3& maximum(void) const
        { 
            return maximum_;
        }

        /** Gets a modifiable version of the maximum
        corner of the box.
        */
        inline Vector3& maximum(void)
        { 
            return maximum_;
        }


        /** Sets the minimum corner of the box.
        */
        inline void set_minimum( const Vector3& vec )
        {
            extent_ = Extent::FINITE;
            minimum_ = vec;
        }

        inline void set_minimum( Real x, Real y, Real z )
        {
            extent_ = Extent::FINITE;
            minimum_.x = x;
            minimum_.y = y;
            minimum_.z = z;
        }

        /** Changes one of the components of the minimum corner of the box
        used to resize only one dimension of the box
        */
        inline void set_minimumX(Real x)
        {
            minimum_.x = x;
        }

        inline void set_minimumY(Real y)
        {
            minimum_.y = y;
        }

        inline void set_minimumZ(Real z)
        {
            minimum_.z = z;
        }

        /** Sets the maximum corner of the box.
        */
        inline void set_maximum( const Vector3& vec )
        {
            extent_ = Extent::FINITE;
            maximum_ = vec;
        }

        inline void set_maximum( Real x, Real y, Real z )
        {
            extent_ = Extent::FINITE;
            maximum_.x = x;
            maximum_.y = y;
            maximum_.z = z;
        }

        /** Changes one of the components of the maximum corner of the box
        used to resize only one dimension of the box
        */
        inline void set_maximum_x( Real x )
        {
            maximum_.x = x;
        }

        inline void set_maximum_y( Real y )
        {
            maximum_.y = y;
        }

        inline void set_maximum_z( Real z )
        {
            maximum_.z = z;
        }

        /** Sets both minimum and maximum extents at once.
        */
        inline void set_extents( const Vector3& min, const Vector3& max )
        {
            assert( (min.x <= max.x && min.y <= max.y && min.z <= max.z) &&
                "The minimum corner of the box must be less than or equal to maximum corner" );

            extent_ = Extent::FINITE;
            minimum_ = min;
            maximum_ = max;
        }

        inline void set_extents(
            Real mx, Real my, Real mz,
            Real Mx, Real My, Real Mz )
        {
            assert( (mx <= Mx && my <= My && mz <= Mz) &&
                "The minimum corner of the box must be less than or equal to maximum corner" );

            extent_ = Extent::FINITE;

            minimum_.x = mx;
            minimum_.y = my;
            minimum_.z = mz;

            maximum_.x = Mx;
            maximum_.y = My;
            maximum_.z = Mz;

        }

        /** Returns a pointer to an array of 8 corner points, useful for
        collision vs. non-aligned objects.

        If the order of these corners is important, they are as
        follows: The 4 points of the minimum Z face (note that
        because Ogre uses right-handed coordinates, the minimum Z is
        at the 'back' of the box) starting with the minimum point of
        all, then anticlockwise around this face (if you are looking
        onto the face from outside the box). Then the 4 points of the
        maximum Z face, starting with maximum point of all, then
        anticlockwise around this face (looking onto the face from
        outside the box). Like this:
        <pre>
           1-------2
          /|      /|
         / |     / |
        5-------4  |
        |  0----|--3
        | /     | /
        |/      |/
        6-------7
        </pre>
        */
        inline Corners all_corners(void) const
        {
            assert( (extent_ == Extent::FINITE) && "Can't get corners of a null or infinite AAB" );

            // The order of these items is, using right-handed coordinates:
            // Minimum Z face, starting with Min(all), then anticlockwise
            //   around face (looking onto the face)
            // Maximum Z face, starting with Max(all), then anticlockwise
            //   around face (looking onto the face)
            // Only for optimization/compatibility.
            Corners corners;

            corners[0] = corner(Corner::FAR_LEFT_BOTTOM);
            corners[1] = corner(Corner::FAR_LEFT_TOP);
            corners[2] = corner(Corner::FAR_RIGHT_TOP);
            corners[3] = corner(Corner::FAR_RIGHT_BOTTOM);

            corners[4] = corner(Corner::NEAR_RIGHT_TOP);
            corners[5] = corner(Corner::NEAR_LEFT_TOP);
            corners[6] = corner(Corner::NEAR_LEFT_BOTTOM);
            corners[7] = corner(Corner::NEAR_RIGHT_BOTTOM);

            return corners;
        }

        /** Gets the position of one of the corners
        */
        Vector3 corner(Corner cornerToGet) const
        {
            switch(cornerToGet)
            {
            case Corner::FAR_LEFT_BOTTOM:
                return minimum_;
            case Corner::FAR_LEFT_TOP:
                return Vector3(minimum_.x, maximum_.y, minimum_.z);
            case Corner::FAR_RIGHT_TOP:
                return Vector3(maximum_.x, maximum_.y, minimum_.z);
            case Corner::FAR_RIGHT_BOTTOM:
                return Vector3(maximum_.x, minimum_.y, minimum_.z);
            case Corner::NEAR_RIGHT_BOTTOM:
                return Vector3(maximum_.x, minimum_.y, maximum_.z);
            case Corner::NEAR_LEFT_BOTTOM:
                return Vector3(minimum_.x, minimum_.y, maximum_.z);
            case Corner::NEAR_LEFT_TOP:
                return Vector3(minimum_.x, maximum_.y, maximum_.z);
            case Corner::NEAR_RIGHT_TOP:
                return maximum_;
            default:
                return Vector3();
            }
        }

        friend std::ostream& operator<<( std::ostream& o, const AxisAlignedBox &aab )
        {
            switch (aab.extent_)
            {
            case Extent::NIL:
                o << "AxisAlignedBox(null)";
                return o;

            case Extent::FINITE:
                o << "AxisAlignedBox(min=" << aab.minimum_ << ", max=" << aab.maximum_ << ")";
                return o;

            case Extent::INFINITE:
                o << "AxisAlignedBox(infinite)";
                return o;

            default: // shut up compiler
                assert( false && "Never reached" );
                return o;
            }
        }

        /** Merges the passed in box into the current box. The result is the
        box which encompasses both.
        */
        void merge( const AxisAlignedBox& rhs )
        {
            // Do nothing if rhs null, or this is infinite
            if ((rhs.extent_ == Extent::NIL) || (extent_ == Extent::INFINITE))
            {
                return;
            }
            // Otherwise if rhs is infinite, make this infinite, too
            else if (rhs.extent_ == Extent::INFINITE)
            {
                extent_ = Extent::INFINITE;
            }
            // Otherwise if current null, just take rhs
            else if (extent_ == Extent::NIL)
            {
                set_extents(rhs.minimum_, rhs.maximum_);
            }
            // Otherwise merge
            else
            {
                Vector3 min = minimum_;
                Vector3 max = maximum_;
                max.make_ceil(rhs.maximum_);
                min.make_floor(rhs.minimum_);

                set_extents(min, max);
            }

        }

        /** Extends the box to encompass the specified point (if needed).
        */
        inline void merge( const Vector3& point )
        {
            switch (extent_)
            {
            case Extent::NIL: // if null, use this point
                set_extents(point, point);
                return;

            case Extent::FINITE:
                maximum_.make_ceil(point);
                minimum_.make_floor(point);
                return;

            case Extent::INFINITE: // if infinite, makes no difference
                return;
            }

            assert( false && "Never reached" );
        }

        /** Transforms the box according to the matrix supplied.

        By calling this method you get the axis-aligned box which
        surrounds the transformed version of this box. Therefore each
        corner of the box is transformed by the matrix, then the
        extents are mapped back onto the axes to produce another
        AABB. Useful when you have a local AABB for an object which
        is then transformed.
        */
        inline void transform_by( const Matrix4& matrix )
        {
            // Do nothing if current null or infinite
            if( extent_ != Extent::FINITE )
                return;

            Vector3 oldMin, oldMax, currentCorner;

            // Getting the old values so that we can use the existing merge method.
            oldMin = minimum_;
            oldMax = maximum_;

            // reset
            set_null();

            // We sequentially compute the corners in the following order :
            // 0, 6, 5, 1, 2, 4 ,7 , 3
            // This sequence allows us to only change one member at a time to get at all corners.

            // For each one, we transform it using the matrix
            // Which gives the resulting point and merge the resulting point.

            // First corner 
            // min min min
            currentCorner = oldMin;
            merge( matrix * currentCorner );

            // min,min,max
            currentCorner.z = oldMax.z;
            merge( matrix * currentCorner );

            // min max max
            currentCorner.y = oldMax.y;
            merge( matrix * currentCorner );

            // min max min
            currentCorner.z = oldMin.z;
            merge( matrix * currentCorner );

            // max max min
            currentCorner.x = oldMax.x;
            merge( matrix * currentCorner );

            // max max max
            currentCorner.z = oldMax.z;
            merge( matrix * currentCorner );

            // max min max
            currentCorner.y = oldMin.y;
            merge( matrix * currentCorner );

            // max min min
            currentCorner.z = oldMin.z;
            merge( matrix * currentCorner ); 
        }

        /** Transforms the box according to the affine matrix supplied.

        By calling this method you get the axis-aligned box which
        surrounds the transformed version of this box. Therefore each
        corner of the box is transformed by the matrix, then the
        extents are mapped back onto the axes to produce another
        AABB. Useful when you have a local AABB for an object which
        is then transformed.
        */
        void transform_by(const Affine3& m)
        {
            // Do nothing if current null or infinite
            if ( extent_ != Extent::FINITE )
                return;

            Vector3 centre = center();
            Vector3 halfSize = half_size();

            Vector3 newCentre = m * centre;
            Vector3 newHalfSize(
                Math::Abs(m[0][0]) * halfSize.x + Math::Abs(m[0][1]) * halfSize.y + Math::Abs(m[0][2]) * halfSize.z, 
                Math::Abs(m[1][0]) * halfSize.x + Math::Abs(m[1][1]) * halfSize.y + Math::Abs(m[1][2]) * halfSize.z,
                Math::Abs(m[2][0]) * halfSize.x + Math::Abs(m[2][1]) * halfSize.y + Math::Abs(m[2][2]) * halfSize.z);

            set_extents(newCentre - newHalfSize, newCentre + newHalfSize);
        }

        /** Sets the box to a 'null' value i.e. not a box.
        */
        inline void set_null()
        {
            extent_ = Extent::NIL;
        }

        /** Returns true if the box is null i.e. empty.
        */
        inline bool is_null(void) const
        {
            return (extent_ == Extent::NIL);
        }

        /** Returns true if the box is finite.
        */
        bool finite(void) const
        {
            return (extent_ == Extent::FINITE);
        }

        /** Sets the box to 'infinite'
        */
        inline void set_infinite()
        {
            extent_ = Extent::INFINITE;
        }

        /** Returns true if the box is infinite.
        */
        bool isInfinite(void) const
        {
            return (extent_ == Extent::INFINITE);
        }

        /** Returns whether or not this box intersects another. */
        inline bool intersects(const AxisAlignedBox& b2) const
        {
            // Early-fail for nulls
            if (this->is_null() || b2.is_null())
                return false;

            // Early-success for infinites
            if (this->isInfinite() || b2.isInfinite())
                return true;

            // Use up to 6 separating planes
            if (maximum_.x < b2.minimum_.x)
                return false;
            if (maximum_.y < b2.minimum_.y)
                return false;
            if (maximum_.z < b2.minimum_.z)
                return false;

            if (minimum_.x > b2.maximum_.x)
                return false;
            if (minimum_.y > b2.maximum_.y)
                return false;
            if (minimum_.z > b2.maximum_.z)
                return false;

            // otherwise, must be intersecting
            return true;

        }

        /// Calculate the area of intersection of this box and another
        inline AxisAlignedBox intersection(const AxisAlignedBox& b2) const
        {
            if (this->is_null() || b2.is_null())
            {
                return AxisAlignedBox();
            }
            else if (this->isInfinite())
            {
                return b2;
            }
            else if (b2.isInfinite())
            {
                return *this;
            }

            Vector3 intMin = minimum_;
            Vector3 intMax = maximum_;

            intMin.make_ceil(b2.minimum());
            intMax.make_floor(b2.maximum());

            // Check intersection isn't null
            if (intMin.x < intMax.x &&
                intMin.y < intMax.y &&
                intMin.z < intMax.z)
            {
                return AxisAlignedBox(intMin, intMax);
            }

            return AxisAlignedBox();
        }

        /// Calculate the volume of this box
        Real volume(void) const
        {
            switch (extent_)
            {
            case Extent::NIL:
                return 0.0f;

            case Extent::FINITE:
                {
                    Vector3 diff = maximum_ - minimum_;
                    return diff.x * diff.y * diff.z;
                }

            case Extent::INFINITE:
                return Math::POS_INFINITY;

            default: // shut up compiler
                assert( false && "Never reached" );
                return 0.0f;
            }
        }

        /** Scales the AABB by the vector given. */
        inline void scale_by(const Vector3& s)
        {
            // Do nothing if current null or infinite
            if (extent_ != Extent::FINITE)
                return;

            // NB assumes centered on origin
            Vector3 min = minimum_ * s;
            Vector3 max = maximum_ * s;
            set_extents(min, max);
        }

        /** Tests whether this box intersects a sphere. */
        bool intersects(const Sphere& s) const
        {
            return Math::intersects(s, *this); 
        }
        /** Tests whether this box intersects a plane. */
        bool intersects(const Plane& p) const
        {
            return Math::intersects(p, *this);
        }
        /** Tests whether the vector point is within this box. */
        bool intersects(const Vector3& v) const
        {
            switch (extent_)
            {
            case Extent::NIL:
                return false;

            case Extent::FINITE:
                return(v.x >= minimum_.x  &&  v.x <= maximum_.x  && 
                    v.y >= minimum_.y  &&  v.y <= maximum_.y  && 
                    v.z >= minimum_.z  &&  v.z <= maximum_.z);

            case Extent::INFINITE:
                return true;
            }

            return false;
        }
        /// Gets the centre of the box
        Vector3 center(void) const
        {
            assert( (extent_ == Extent::FINITE) && "Can't get center of a null or infinite AAB" );

            return Vector3(
                (maximum_.x + minimum_.x) * 0.5f,
                (maximum_.y + minimum_.y) * 0.5f,
                (maximum_.z + minimum_.z) * 0.5f);
        }
        /// Gets the size of the box
        Vector3 size(void) const
        {
            switch (extent_)
            {
            case Extent::NIL:
                return Vector3::zero;

            case Extent::FINITE:
                return maximum_ - minimum_;

            case Extent::INFINITE:
                return Vector3(
                    Math::POS_INFINITY,
                    Math::POS_INFINITY,
                    Math::POS_INFINITY);

            default: // shut up compiler
                assert( false && "Never reached" );
                return Vector3::zero;
            }
        }
        /// Gets the half-size of the box
        Vector3 half_size(void) const
        {
            switch (extent_)
            {
            case Extent::NIL:
                return Vector3::zero;

            case Extent::FINITE:
                return (maximum_ - minimum_) * 0.5;

            case Extent::INFINITE:
                return Vector3(
                    Math::POS_INFINITY,
                    Math::POS_INFINITY,
                    Math::POS_INFINITY);

            default: // shut up compiler
                assert( false && "Never reached" );
                return Vector3::zero;
            }
        }

        /** Tests whether the given point contained by this box.
        */
        bool contains(const Vector3& v) const
        {
            if (is_null())
                return false;
            if (isInfinite())
                return true;

            return minimum_.x <= v.x && v.x <= maximum_.x &&
                   minimum_.y <= v.y && v.y <= maximum_.y &&
                   minimum_.z <= v.z && v.z <= maximum_.z;
        }
        
        /** Returns the squared minimum distance between a given point and any part of the box.
         *  This is faster than distance since avoiding a squareroot, so use if you can. */
        Real squared_distance(const Vector3& v) const
        {

            if (this->contains(v))
                return 0;
            else
            {
                Vector3 maxDist(0,0,0);

                if (v.x < minimum_.x)
                    maxDist.x = minimum_.x - v.x;
                else if (v.x > maximum_.x)
                    maxDist.x = v.x - maximum_.x;

                if (v.y < minimum_.y)
                    maxDist.y = minimum_.y - v.y;
                else if (v.y > maximum_.y)
                    maxDist.y = v.y - maximum_.y;

                if (v.z < minimum_.z)
                    maxDist.z = minimum_.z - v.z;
                else if (v.z > maximum_.z)
                    maxDist.z = v.z - maximum_.z;

                return maxDist.squared_length();
            }
        }
        
        /** Returns the minimum distance between a given point and any part of the box. */
        Real distance (const Vector3& v) const
        {
            return Ogre::Math::Sqrt(squared_distance(v));
        }

        /** Tests whether another box contained by this box.
        */
        bool contains(const AxisAlignedBox& other) const
        {
            if (other.is_null() || this->isInfinite())
                return true;

            if (this->is_null() || other.isInfinite())
                return false;

            return this->minimum_.x <= other.minimum_.x &&
                   this->minimum_.y <= other.minimum_.y &&
                   this->minimum_.z <= other.minimum_.z &&
                   other.maximum_.x <= this->maximum_.x &&
                   other.maximum_.y <= this->maximum_.y &&
                   other.maximum_.z <= this->maximum_.z;
        }

        /** Tests 2 boxes for equality.
        */
        bool operator== (const AxisAlignedBox& rhs) const
        {
            if (this->extent_ != rhs.extent_)
                return false;

            if (!this->finite())
                return true;

            return this->minimum_ == rhs.minimum_ &&
                   this->maximum_ == rhs.maximum_;
        }

        /** Tests 2 boxes for inequality.
        */
        bool operator!= (const AxisAlignedBox& rhs) const
        {
            return !(*this == rhs);
        }

        // special values
        static const AxisAlignedBox box_null;
        static const AxisAlignedBox box_infinite;


    };

    /** @} */
    /** @} */
} // namespace Ogre

#endif
