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
#ifndef __Vector_H__
#define __Vector_H__


#include "OgrePrerequisites.h"
#include "OgreMath.h"
#include "OgreQuaternion.h"

namespace Ogre
{

    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup Math
    *  @{
    */
    /// helper class to implement legacy API. Notably x, y, z access
    template <int dims, typename T> struct VectorBase
    {
        VectorBase() {}
        VectorBase(T _x, T _y)
        {
            static_assert(dims > 1, "must have at least 2 dimensions");
            data[0] = _x; data[1] = _y;
        }
        VectorBase(T _x, T _y, T _z)
        {
            static_assert(dims > 2, "must have at least 3 dimensions");
            data[0] = _x; data[1] = _y; data[2] = _z;
        }
        VectorBase(T _x, T _y, T _z, T _w)
        {
            static_assert(dims > 3, "must have at least 4 dimensions");
            data[0] = _x; data[1] = _y; data[2] = _z; data[3] = _w;
        }
        T data[dims];
        T* ptr() { return data; }
        const T* ptr() const { return data; }
    };
    template <> struct VectorBase<2, Real>
    {
        VectorBase() {}
        VectorBase(Real _x, Real _y) : x(_x), y(_y) {}
        Real x, y;
        Real* ptr() { return &x; }
        const Real* ptr() const { return &x; }

        /** Returns a vector at a point half way between this and the passed
            in vector.
        */
        Vector2 mid_point( const Vector2& vec ) const;

        /** Calculates the 2 dimensional cross-product of 2 vectors, which results
            in a single floating point value which is 2 times the area of the triangle.
        */
        Real cross_product( const VectorBase& other ) const
        {
            return x * other.y - y * other.x;
        }

        /** Generates a vector perpendicular to this vector (eg an 'up' vector).

                This method will return a vector which is perpendicular to this
                vector. There are an infinite number of possibilities but this
                method will guarantee to generate one of them. If you need more
                control you should use the Quaternion class.
        */
        Vector2 perpendicular(void) const;

        /** Generates a new random vector which deviates from this vector by a
            given angle in a random direction.

                This method assumes that the random number generator has already
                been seeded appropriately.
            @param angle
                The angle at which to deviate in radians
            @return
                A random vector which deviates from this vector by angle. This
                vector will not be normalised, normalise it if you wish
                afterwards.
        */
        Vector2 random_deviant(Radian angle) const;

        /**  Gets the oriented angle between 2 vectors.

            Vectors do not have to be unit-length but must represent directions.
            The angle is comprised between 0 and 2 PI.
        */
        Radian angle_to(const Vector2& other) const;

        // special points
        static const Vector2 zero;
        static const Vector2 unit_x;
        static const Vector2 unit_y;
        static const Vector2 negative_unit_x;
        static const Vector2 negative_unit_y;
        static const Vector2 unit_scale;
    };

    template <> struct VectorBase<3, Real>
    {
        VectorBase() {}
        VectorBase(Real _x, Real _y, Real _z) : x(_x), y(_y), z(_z) {}
        Real x, y, z;
        Real* ptr() { return &x; }
        const Real* ptr() const { return &x; }

        /** Calculates the cross-product of 2 vectors, i.e. the vector that
            lies perpendicular to them both.

                The cross-product is normally used to calculate the normal
                vector of a plane, by calculating the cross-product of 2
                non-equivalent vectors which lie on the plane (e.g. 2 edges
                of a triangle).
            @param rkVector
                Vector which, together with this one, will be used to
                calculate the cross-product.
            @return
                A vector which is the result of the cross-product. This
                vector will <b>NOT</b> be normalised, to maximise efficiency
                - call Vector3::normalise on the result if you wish this to
                be done. As for which side the resultant vector will be on, the
                returned vector will be on the side from which the arc from 'this'
                to rkVector is anticlockwise, e.g. unit_y.cross_product(unit_z)
                = unit_x, whilst unit_z.cross_product(unit_y) = -unit_x.
                This is because OGRE uses a right-handed coordinate system.
            @par
                For a clearer explanation, look a the left and the bottom edges
                of your monitor's screen. Assume that the first vector is the
                left edge and the second vector is the bottom edge, both of
                them starting from the lower-left corner of the screen. The
                resulting vector is going to be perpendicular to both of them
                and will go <i>inside</i> the screen, towards the cathode tube
                (assuming you're using a CRT monitor, of course).
        */
        Vector3 cross_product( const Vector3& rkVector ) const;

        /** Generates a vector perpendicular to this vector (eg an 'up' vector).

                This method will return a vector which is perpendicular to this
                vector. There are an infinite number of possibilities but this
                method will guarantee to generate one of them. If you need more
                control you should use the Quaternion class.
        */
        Vector3 perpendicular(void) const;

        /** Calculates the absolute dot (scalar) product of this vector with another.

                This function work similar dotProduct, except it use absolute value
                of each component of the vector to computing.
            @param
                vec Vector with which to calculate the absolute dot product (together
                with this one).
            @return
                A Real representing the absolute dot product value.
        */
        Real abs_dot_product(const VectorBase& vec) const
        {
            return Math::Abs(x * vec.x) + Math::Abs(y * vec.y) + Math::Abs(z * vec.z);
        }

        /** Returns a vector at a point half way between this and the passed
            in vector.
        */
        Vector3 mid_point( const Vector3& vec ) const;

        /** Generates a new random vector which deviates from this vector by a
            given angle in a random direction.

                This method assumes that the random number generator has already
                been seeded appropriately.
            @param
                angle The angle at which to deviate
            @param
                up Any vector perpendicular to this one (which could generated
                by cross-product of this vector and any other non-colinear
                vector). If you choose not to provide this the function will
                derive one on it's own, however if you provide one yourself the
                function will be faster (this allows you to reuse up vectors if
                you call this method more than once)
            @return
                A random vector which deviates from this vector by angle. This
                vector will not be normalised, normalise it if you wish
                afterwards.
        */
        Vector3 random_deviant(const Radian& angle, const Vector3& up = zero) const;

        /** Gets the shortest arc quaternion to rotate this vector to the destination
            vector.

            If you call this with a dest vector that is close to the inverse
            of this vector, we will rotate 180 degrees around the 'fallbackAxis'
            (if specified, or a generated axis if not) since in this case
            ANY axis of rotation is valid.
        */
        Quaternion rotation_to(const Vector3& dest, const Vector3& fallbackAxis = zero) const;

        /** Returns whether this vector is within a positional tolerance
            of another vector, also take scale of the vectors into account.
        @param rhs The vector to compare with
        @param tolerance The amount (related to the scale of vectors) that distance
            of the vector may vary by and still be considered close
        */
        bool position_closes(const Vector3& rhs, Real tolerance = 1e-03f) const;

        /** Returns whether this vector is within a directional tolerance
            of another vector.
        @param rhs The vector to compare with
        @param tolerance The maximum angle by which the vectors may vary and
            still be considered equal
        @note Both vectors should be normalised.
        */
        bool direction_equals(const Vector3& rhs, const Radian& tolerance) const;

        /// Extract the primary (dominant) axis from this direction vector
        const Vector3& primary_axis() const;

        // special points
        static const Vector3 zero;
        static const Vector3 unit_x;
        static const Vector3 unit_y;
        static const Vector3 unit_z;
        static const Vector3 negative_unit_x;
        static const Vector3 negative_unit_y;
        static const Vector3 negative_unit_z;
        static const Vector3 unit_scale;
    };

    template <> struct VectorBase<4, Real>
    {
        VectorBase() {}
        VectorBase(Real _x, Real _y, Real _z, Real _w) : x(_x), y(_y), z(_z), w(_w) {}
        Real x, y, z, w;
        Real* ptr() { return &x; }
        const Real* ptr() const { return &x; }

        // special points
        static const Vector4 zero;
    };

    /** Standard N-dimensional vector.

        A direction in N-D space represented as distances along the
        orthogonal axes. Note that positions, directions and
        scaling factors can be represented by a vector, depending on how
        you interpret the values.
    */
    template<int dims, typename T>
    class Vector : public VectorBase<dims, T>
    {
    public:
        using VectorBase<dims, T>::ptr;

        /** Default constructor.
            @note It does <b>NOT</b> initialize the vector for efficiency.
        */
        Vector() {}
        Vector(T _x, T _y) : VectorBase<dims, T>(_x, _y) {}
        Vector(T _x, T _y, T _z) : VectorBase<dims, T>(_x, _y, _z) {}
        Vector(T _x, T _y, T _z, T _w) : VectorBase<dims, T>(_x, _y, _z, _w) {}

        // use enable_if as function parameter for VC < 2017 compatibility
        template <int N = dims>
        explicit Vector(const typename std::enable_if<N == 4, Vector<3, T>>::type& rhs, T fW = 1.0f) : VectorBase<dims, T>(rhs[0], rhs[1], rhs[2], fW) {}

        template<typename U>
        explicit Vector(const U* _ptr) {
            for (int i = 0; i < dims; i++)
                ptr()[i] = T(_ptr[i]);
        }

        template<typename U>
        explicit Vector(const Vector<dims, U>& o) : Vector(o.ptr()) {}


        explicit Vector(T s)
        {
            for (int i = 0; i < dims; i++)
                ptr()[i] = s;
        }

        /** Swizzle-like narrowing operations
        */
        Vector<3, T> xyz() const
        {
            static_assert(dims > 3, "just use assignment");
            return Vector<3, T>(ptr());
        }
        Vector<2, T> xy() const
        {
            static_assert(dims > 2, "just use assignment");
            return Vector<2, T>(ptr());
        }

        const T& operator[](size_t i) const
        {
            assert(i < dims);
            return ptr()[i];
        }

        T& operator[](size_t i)
        {
            assert(i < dims);
            return ptr()[i];
        }

        bool operator==(const Vector& v) const
        {
            for (int i = 0; i < dims; i++)
                if (ptr()[i] != v[i])
                    return false;
            return true;
        }

        /** Returns whether this vector is within a positional tolerance
            of another vector.
        @param rhs The vector to compare with
        @param tolerance The amount that each element of the vector may vary by
            and still be considered equal
        */
        bool position_equals(const Vector& rhs, Real tolerance = 1e-03f) const
        {
            for (int i = 0; i < dims; i++)
                if (!Math::RealEqual(ptr()[i], rhs[i], tolerance))
                    return false;
            return true;
        }

        bool operator!=(const Vector& v) const { return !(*this == v); }

        /** Returns true if the vector's scalar components are all greater
            that the ones of the vector it is compared against.
        */
        bool operator<(const Vector& rhs) const
        {
            for (int i = 0; i < dims; i++)
                if (!(ptr()[i] < rhs[i]))
                    return false;
            return true;
        }

        /** Returns true if the vector's scalar components are all smaller
            that the ones of the vector it is compared against.
        */
        bool operator>(const Vector& rhs) const
        {
            for (int i = 0; i < dims; i++)
                if (!(ptr()[i] > rhs[i]))
                    return false;
            return true;
        }

        /** Sets this vector's components to the minimum of its own and the
            ones of the passed in vector.

                'Minimum' in this case means the combination of the lowest
                value of x, y and z from both vectors. Lowest is taken just
                numerically, not magnitude, so -1 < 0.
        */
        void make_floor(const Vector& cmp)
        {
            for (int i = 0; i < dims; i++)
                if (cmp[i] < ptr()[i])
                    ptr()[i] = cmp[i];
        }

        /** Sets this vector's components to the maximum of its own and the
            ones of the passed in vector.

                'Maximum' in this case means the combination of the highest
                value of x, y and z from both vectors. Highest is taken just
                numerically, not magnitude, so 1 > -3.
        */
        void make_ceil(const Vector& cmp)
        {
            for (int i = 0; i < dims; i++)
                if (cmp[i] > ptr()[i])
                    ptr()[i] = cmp[i];
        }

        /** Calculates the dot (scalar) product of this vector with another.

                The dot product can be used to calculate the angle between 2
                vectors. If both are unit vectors, the dot product is the
                cosine of the angle; otherwise the dot product must be
                divided by the product of the lengths of both vectors to get
                the cosine of the angle. This result can further be used to
                calculate the distance of a point from a plane.
            @param
                vec Vector with which to calculate the dot product (together
                with this one).
            @return
                A float representing the dot product value.
        */
        T dot_product(const VectorBase<dims, T>& vec) const
        {
            T ret = 0;
            for (int i = 0; i < dims; i++)
                ret += ptr()[i] * vec.ptr()[i];
            return ret;
        }

        /** Returns the square of the length(magnitude) of the vector.

                This  method is for efficiency - calculating the actual
                length of a vector requires a square root, which is expensive
                in terms of the operations required. This method returns the
                square of the length of the vector, i.e. the same as the
                length but before the square root is taken. Use this if you
                want to find the longest / shortest vector without incurring
                the square root.
        */
        T squared_length() const { return dot_product(*this); }

        /** Returns true if this vector is zero length. */
        bool is_zero() const
        {
            return squared_length() < 1e-06 * 1e-06;
        }

        /** Returns the length (magnitude) of the vector.
            @warning
                This operation requires a square root and is expensive in
                terms of CPU operations. If you don't need to know the exact
                length (e.g. for just comparing lengths) use squared_length()
                instead.
        */
        Real length() const { return Math::Sqrt(squared_length()); }

        /** Returns the distance to another vector.
            @warning
                This operation requires a square root and is expensive in
                terms of CPU operations. If you don't need to know the exact
                distance (e.g. for just comparing distances) use squared_distance()
                instead.
        */
        Real distance(const Vector& rhs) const
        {
            return (*this - rhs).length();
        }

        /** Returns the square of the distance to another vector.

                This method is for efficiency - calculating the actual
                distance to another vector requires a square root, which is
                expensive in terms of the operations required. This method
                returns the square of the distance to another vector, i.e.
                the same as the distance but before the square root is taken.
                Use this if you want to find the longest / shortest distance
                without incurring the square root.
        */
        T squared_distance(const Vector& rhs) const
        {
            return (*this - rhs).squared_length();
        }

        /** Normalises the vector.

                This method normalises the vector such that it's
                length / magnitude is 1. The result is called a unit vector.
            @note
                This function will not crash for zero-sized vectors, but there
                will be no changes made to their components.
            @return The previous length of the vector.
        */
        Real normalise()
        {
            Real fLength = length();

            // Will also work for zero-sized vectors, but will change nothing
            // We're not using epsilons because we don't need to.
            // Read http://www.ogre3d.org/forums/viewtopic.php?f=4&t=61259
            if (fLength > Real(0.0f))
            {
                Real fInvLength = 1.0f / fLength;
                for (int i = 0; i < dims; i++)
                    ptr()[i] *= fInvLength;
            }

            return fLength;
        }

        /** As normalise, except that this vector is unaffected and the
            normalised vector is returned as a copy. */
        Vector normalised_copy() const
        {
            Vector ret = *this;
            ret.normalise();
            return ret;
        }

#ifndef OGRE_FAST_MATH
        /// Check whether this vector contains valid values
        bool is_nan() const
        {
            for (int i = 0; i < dims; i++)
                if (Math::isNaN(ptr()[i]))
                    return true;
            return false;
        }
#endif

        /** Gets the angle between 2 vectors.

            Vectors do not have to be unit-length but must represent directions.
        */
        Radian angle_between(const Vector& dest) const
        {
            Real lenProduct = length() * dest.length();

            // Divide by zero check
            if(lenProduct < 1e-6f)
                lenProduct = 1e-6f;

            Real f = dot_product(dest) / lenProduct;

            f = Math::Clamp(f, (Real)-1.0, (Real)1.0);
            return Math::ACos(f);

        }

        /** Calculates a reflection vector to the plane with the given normal .
        @remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
        */
        Vector reflect(const Vector& normal) const { return *this - (2 * dot_product(normal) * normal); }

        // Vector: arithmetic updates
        Vector& operator*=(Real s)
        {
            for (int i = 0; i < dims; i++)
                ptr()[i] *= s;
            return *this;
        }

        Vector& operator/=(Real s)
        {
            assert( s != 0.0 ); // legacy assert
            Real fInv = 1.0f/s;
            for (int i = 0; i < dims; i++)
                ptr()[i] *= fInv;
            return *this;
        }

        Vector& operator+=(Real s)
        {
            for (int i = 0; i < dims; i++)
                ptr()[i] += s;
            return *this;
        }

        Vector& operator-=(Real s)
        {
            for (int i = 0; i < dims; i++)
                ptr()[i] -= s;
            return *this;
        }

        Vector& operator+=(const Vector& b)
        {
            for (int i = 0; i < dims; i++)
                ptr()[i] += b[i];
            return *this;
        }

        Vector& operator-=(const Vector& b)
        {
            for (int i = 0; i < dims; i++)
                ptr()[i] -= b[i];
            return *this;
        }

        Vector& operator*=(const Vector& b)
        {
            for (int i = 0; i < dims; i++)
                ptr()[i] *= b[i];
            return *this;
        }

        Vector& operator/=(const Vector& b)
        {
            for (int i = 0; i < dims; i++)
                ptr()[i] /= b[i];
            return *this;
        }

        // Scalar * Vector
        friend Vector operator*(Real s, Vector v)
        {
            v *= s;
            return v;
        }

        friend Vector operator+(Real s, Vector v)
        {
            v += s;
            return v;
        }

        friend Vector operator-(Real s, const Vector& v)
        {
            Vector ret;
            for (int i = 0; i < dims; i++)
                ret[i] = s - v[i];
            return ret;
        }

        friend Vector operator/(Real s, const Vector& v)
        {
            Vector ret;
            for (int i = 0; i < dims; i++)
                ret[i] = s / v[i];
            return ret;
        }

        // Vector * Scalar
        Vector operator-() const
        {
            return -1 * *this;
        }

        const Vector& operator+() const
        {
            return *this;
        }

        Vector operator*(Real s) const
        {
            return s * *this;
        }

        Vector operator/(Real s) const
        {
            assert( s != 0.0 ); // legacy assert
            Real fInv = 1.0f / s;
            return fInv * *this;
        }

        Vector operator-(Real s) const
        {
            return -s + *this;
        }

        Vector operator+(Real s) const
        {
            return s + *this;
        }

        // Vector * Vector
        Vector operator+(const Vector& b) const
        {
            Vector ret = *this;
            ret += b;
            return ret;
        }

        Vector operator-(const Vector& b) const
        {
            Vector ret = *this;
            ret -= b;
            return ret;
        }

        Vector operator*(const Vector& b) const
        {
            Vector ret = *this;
            ret *= b;
            return ret;
        }

        Vector operator/(const Vector& b) const
        {
            Vector ret = *this;
            ret /= b;
            return ret;
        }

        friend std::ostream& operator<<(std::ostream& o, const Vector& v)
        {
            o << "Vector" << dims << "(";
            for (int i = 0; i < dims; i++) {
                o << v[i];
                if(i != dims - 1) o << ", ";
            }
            o <<  ")";
            return o;
        }
    };

    inline Vector2 VectorBase<2, Real>::mid_point( const Vector2& vec ) const
    {
        return Vector2(
            ( x + vec.x ) * 0.5f,
            ( y + vec.y ) * 0.5f );
    }

    inline Vector2 VectorBase<2, Real>::random_deviant(Radian angle) const
    {
        angle *= Math::RangeRandom(-1, 1);
        Real cosa = Math::Cos(angle);
        Real sina = Math::Sin(angle);
        return Vector2(cosa * x - sina * y,
                       sina * x + cosa * y);
    }

    inline Radian VectorBase<2, Real>::angle_to(const Vector2& other) const
    {
        Radian angle = ((const Vector2*)this)->angle_between(other);

        if (cross_product(other)<0)
            angle = Radian(Math::TWO_PI) - angle;

        return angle;
    }

    inline Vector2 VectorBase<2, Real>::perpendicular(void) const
    {
        return Vector2(-y, x);
    }

    inline Vector3 VectorBase<3, Real>::perpendicular() const
    {
        // From Sam Hocevar's article "On picking an orthogonal
        // vector (and combing coconuts)"
        Vector3 perp = Math::Abs(x) > Math::Abs(z)
                     ? Vector3(-y, x, 0.0) : Vector3(0.0, -z, y);
        return perp.normalised_copy();
    }

    inline Vector3 VectorBase<3, Real>::cross_product( const Vector3& rkVector ) const
    {
        return Vector3(
            y * rkVector.z - z * rkVector.y,
            z * rkVector.x - x * rkVector.z,
            x * rkVector.y - y * rkVector.x);
    }

    inline Vector3 VectorBase<3, Real>::mid_point( const Vector3& vec ) const
    {
        return Vector3(
            ( x + vec.x ) * 0.5f,
            ( y + vec.y ) * 0.5f,
            ( z + vec.z ) * 0.5f );
    }

    inline Vector3 VectorBase<3, Real>::random_deviant(const Radian& angle, const Vector3& up) const
    {
        Vector3 newUp;

        if (up == zero)
        {
            // Generate an up vector
            newUp = ((const Vector3*)this)->perpendicular();
        }
        else
        {
            newUp = up;
        }

        // Rotate up vector by random amount around this
        Quaternion q;
        q.from_angle_axis( Radian(Math::UnitRandom() * Math::TWO_PI), (const Vector3&)*this );
        newUp = q * newUp;

        // Finally rotate this by given angle around randomised up
        q.from_angle_axis( angle, newUp );
        return q * (const Vector3&)(*this);
    }

    inline Quaternion VectorBase<3, Real>::rotation_to(const Vector3& dest, const Vector3& fallbackAxis) const
    {
        // From Sam Hocevar's article "Quaternion from two vectors:
        // the final version"
        Real a = Math::Sqrt(((const Vector3*)this)->squared_length() * dest.squared_length());
        Real b = a + dest.dot_product(*this);

        if (Math::RealEqual(b, 2 * a) || a == 0)
            return Quaternion::IDENTITY;

        Vector3 axis;

        if (b < (Real)1e-06 * a)
        {
            b = (Real)0.0;
            axis = fallbackAxis != Vector3::zero ? fallbackAxis
                 : Math::Abs(x) > Math::Abs(z) ? Vector3(-y, x, (Real)0.0)
                 : Vector3((Real)0.0, -z, y);
        }
        else
        {
            axis = this->cross_product(dest);
        }

        Quaternion q(b, axis.x, axis.y, axis.z);
        q.normalise();
        return q;
    }

    inline bool VectorBase<3, Real>::position_closes(const Vector3& rhs, Real tolerance) const
    {
        return ((const Vector3*)this)->squared_distance(rhs) <=
            (((const Vector3*)this)->squared_length() + rhs.squared_length()) * tolerance;
    }

    inline bool VectorBase<3, Real>::direction_equals(const Vector3& rhs, const Radian& tolerance) const
    {
        Real dot = rhs.dot_product(*this);
        Radian angle = Math::ACos(dot);

        return Math::Abs(angle.valueRadians()) <= tolerance.valueRadians();
    }

    inline const Vector3& VectorBase<3, Real>::primary_axis() const
    {
        Real absx = Math::Abs(x);
        Real absy = Math::Abs(y);
        Real absz = Math::Abs(z);
        if (absx > absy)
            if (absx > absz)
                return x > 0 ? unit_x : negative_unit_x;
            else
                return z > 0 ? unit_z : negative_unit_z;
        else // absx <= absy
            if (absy > absz)
                return y > 0 ? unit_y : negative_unit_y;
            else
                return z > 0 ? unit_z : negative_unit_z;
    }

    // Math functions
    inline Vector3 Math::calculateBasicFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3)
    {
        Vector3 normal = (v2 - v1).cross_product(v3 - v1);
        normal.normalise();
        return normal;
    }
    inline Vector4 Math::calculateFaceNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3)
    {
        Vector3 normal = calculateBasicFaceNormal(v1, v2, v3);
        // Now set up the w (distance of tri from origin
        return Vector4(normal.x, normal.y, normal.z, -(normal.dot_product(v1)));
    }
    inline Vector3 Math::calculateBasicFaceNormalWithoutNormalize(
        const Vector3& v1, const Vector3& v2, const Vector3& v3)
    {
        return (v2 - v1).cross_product(v3 - v1);
    }

    inline Vector4 Math::calculateFaceNormalWithoutNormalize(const Vector3& v1,
                                                             const Vector3& v2,
                                                             const Vector3& v3)
    {
        Vector3 normal = calculateBasicFaceNormalWithoutNormalize(v1, v2, v3);
        // Now set up the w (distance of tri from origin)
        return Vector4(normal.x, normal.y, normal.z, -(normal.dot_product(v1)));
    }
    /** @} */
    /** @} */

}
#endif
