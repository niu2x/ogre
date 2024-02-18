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
#ifndef __Matrix3_H__
#define __Matrix3_H__

#include "OgrePrerequisites.h"

#include "vector.h"

// NB All code adapted from Wild Magic 0.2 Matrix math (free source code)
// http://www.geometrictools.com/

// NOTE.  The (x,y,z) coordinate system is assumed to be right-handed.
// Coordinate axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.

namespace Ogre
{
    /** \addtogroup Core
    *  @{
    */
    /** \addtogroup Math
    *  @{
    */
    /** A 3x3 matrix which can represent rotations around axes.
        @note
            <b>All the code is adapted from the Wild Magic 0.2 Matrix
            library (http://www.geometrictools.com/).</b>
        @par
            The coordinate system is assumed to be <b>right-handed</b>.
    */
    class Matrix3
    {
    public:
        /** Default constructor.
            @note
                It does <b>NOT</b> initialize the matrix for efficiency.
        */
        Matrix3 () {}
        explicit Matrix3 (const Real arr[3][3])
        {
            memcpy(m_, arr,9*sizeof(Real));
        }

        Matrix3 (Real fEntry00, Real fEntry01, Real fEntry02,
                    Real fEntry10, Real fEntry11, Real fEntry12,
                    Real fEntry20, Real fEntry21, Real fEntry22)
        {
            m_[0][0] = fEntry00;
            m_[0][1] = fEntry01;
            m_[0][2] = fEntry02;
            m_[1][0] = fEntry10;
            m_[1][1] = fEntry11;
            m_[1][2] = fEntry12;
            m_[2][0] = fEntry20;
            m_[2][1] = fEntry21;
            m_[2][2] = fEntry22;
        }

        /// Member access, allows use of construct mat[r][c]
        const Real* operator[] (size_t iRow) const
        {
            return m_[iRow];
        }

        Real* operator[] (size_t iRow)
        {
            return m_[iRow];
        }

        Vector3 column(size_t iCol) const
        {
            assert(iCol < 3);
            return Vector3(m_[0][iCol], m_[1][iCol], m_[2][iCol]);
        }
        void set_column(size_t iCol, const Vector3& vec)
        {
            assert(iCol < 3);
            m_[0][iCol] = vec.x;
            m_[1][iCol] = vec.y;
            m_[2][iCol] = vec.z;
        }
        void from_axes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis)
        {
            set_column(0, xAxis);
            set_column(1, yAxis);
            set_column(2, zAxis);
        }

        /** Tests 2 matrices for equality.
         */
        bool operator== (const Matrix3& rkMatrix) const;

        /** Tests 2 matrices for inequality.
         */
        bool operator!= (const Matrix3& rkMatrix) const
        {
            return !operator==(rkMatrix);
        }

        // arithmetic operations
        /** Matrix addition.
         */
        Matrix3 operator+ (const Matrix3& rkMatrix) const;

        /** Matrix subtraction.
         */
        Matrix3 operator- (const Matrix3& rkMatrix) const;

        /** Matrix concatenation using '*'.
         */
        Matrix3 operator* (const Matrix3& rkMatrix) const;
        Matrix3 operator- () const;

        /// Vector * matrix [1x3 * 3x3 = 1x3]
        friend Vector3 operator* (const Vector3& rkVector,
            const Matrix3& rkMatrix);

        /// Matrix * scalar
        Matrix3 operator* (Real fScalar) const;

        /// Scalar * matrix
        friend Matrix3 operator* (Real fScalar, const Matrix3& rkMatrix);

        // utilities
        Matrix3 transpose () const;

        bool inverse (Matrix3* rkInverse, Real fTolerance = 1e-06f) const;

        Matrix3 inverse (Real fTolerance = 1e-06f) const;

        Real determinant() const
        {
            Real fCofactor00 = m_[1][1] * m_[2][2] - m_[1][2] * m_[2][1];
            Real fCofactor10 = m_[1][2] * m_[2][0] - m_[1][0] * m_[2][2];
            Real fCofactor20 = m_[1][0] * m_[2][1] - m_[1][1] * m_[2][0];

            return m_[0][0] * fCofactor00 + m_[0][1] * fCofactor10 + m_[0][2] * fCofactor20;
        }

        /** Determines if this matrix involves a negative scaling. */
        bool has_negative_scale() const { return determinant() < 0; }

        /// Singular value decomposition
        void singular_value_decomposition (Matrix3* rkL, Vector3* rkS,
            Matrix3* rkR) const;

        void singular_value_composition (const Matrix3& rkL,
            const Vector3& rkS, const Matrix3& rkR);

        /// Gram-Schmidt orthogonalisation (applied to columns of rotation matrix)
        Matrix3 orthonormalised() const
        {
            // Algorithm uses Gram-Schmidt orthogonalisation.  If 'this' matrix is
            // M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
            //
            //   q0 = m0/|m0|
            //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
            //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
            //
            // where |V| indicates length of vector V and A*B indicates dot
            // product of vectors A and B.

            Matrix3 Q;
            // compute q0
            Q.set_column(0, column(0) / column(0).length());

            // compute q1
            Real dot0 = Q.column(0).dot_product(column(1));
            Q.set_column(1, (column(1) - dot0 * Q.column(0)).normalised_copy());

            // compute q2
            Real dot1 = Q.column(1).dot_product(column(2));
            dot0 = Q.column(0).dot_product(column(2));
            Q.set_column(2, (column(2) - dot0 * Q.column(0) + dot1 * Q.column(1)).normalised_copy());

            return Q;
        }

        /// @deprecated
        OGRE_DEPRECATED void orthonormalize() { *this = orthonormalised(); }

        /// Orthogonal Q, diagonal D, upper triangular U stored as (u01,u02,u12)
        void QDU_decomposition (Matrix3* rkQ, Vector3* rkD,
            Vector3* rkU) const;

        Real spectral_norm () const;

        /// Note: Matrix must be orthonormal
        void to_angle_axis (Vector3* rkAxis, Radian* rfAngle) const;

        inline void to_angle_axis (Vector3* rkAxis, Degree* angle) const {
            Radian r;
            to_angle_axis ( rkAxis, &r );
            *angle = r;
        }
        void from_angle_axis (const Vector3& rkAxis, const Radian& fRadians);

        /**
            @name Euler angle conversions
            (De-)composes the matrix in/ from yaw, pitch and roll angles,
            where yaw is rotation about the Y vector, pitch is rotation about the
            X axis, and roll is rotation about the Z axis.

            The function suffix indicates the (de-)composition order;
            e.g. with the YXZ variants the matrix will be (de-)composed as yaw*pitch*roll

            For to_euler_angles_*, the return value denotes whether the solution is unique.
            @note The matrix to be decomposed must be orthonormal.
            @{
        */
        bool to_euler_angles_xyz(Radian* rfYAngle, Radian* rfPAngle, Radian* rfRAngle) const;
        bool to_euler_angles_xzy(Radian* rfYAngle, Radian* rfPAngle, Radian* rfRAngle) const;
        bool to_euler_angles_yxz(Radian* rfYAngle, Radian* rfPAngle, Radian* rfRAngle) const;
        bool to_euler_angles_yzx(Radian* rfYAngle, Radian* rfPAngle, Radian* rfRAngle) const;
        bool to_euler_angles_zxy(Radian* rfYAngle, Radian* rfPAngle, Radian* rfRAngle) const;
        bool to_euler_angles_zyx(Radian* rfYAngle, Radian* rfPAngle, Radian* rfRAngle) const;
        void from_euler_angles_xyz (const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
        void from_euler_angles_xzy (const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
        void from_euler_angles_yxz (const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
        void from_euler_angles_yzx (const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
        void from_euler_angles_zxy (const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
        void from_euler_angles_zyx (const Radian& fYAngle, const Radian& fPAngle, const Radian& fRAngle);
        /// @}
        /// Eigensolver, matrix must be symmetric
        void eigen_solve_symmetric (Real afEigenvalue[3],
            Vector3 akEigenvector[3]) const;

        static void tensor_product (const Vector3& rkU, const Vector3& rkV,
            Matrix3& rkProduct);

        /** Determines if this matrix involves a scaling. */
        bool has_scale() const
        {
            // check magnitude of column vectors (==local axes)
            Real t = m_[0][0] * m_[0][0] + m_[1][0] * m_[1][0] + m_[2][0] * m_[2][0];
            if (!Math::RealEqual(t, 1.0, (Real)1e-04))
                return true;
            t = m_[0][1] * m_[0][1] + m_[1][1] * m_[1][1] + m_[2][1] * m_[2][1];
            if (!Math::RealEqual(t, 1.0, (Real)1e-04))
                return true;
            t = m_[0][2] * m_[0][2] + m_[1][2] * m_[1][2] + m_[2][2] * m_[2][2];
            if (!Math::RealEqual(t, 1.0, (Real)1e-04))
                return true;

            return false;
        }

        /** Function for writing to a stream.
        */
        inline friend std::ostream& operator <<
            ( std::ostream& o, const Matrix3& mat )
        {
            o << "Matrix3(" << mat[0][0] << ", " << mat[0][1] << ", " << mat[0][2] << "; "
                            << mat[1][0] << ", " << mat[1][1] << ", " << mat[1][2] << "; "
                            << mat[2][0] << ", " << mat[2][1] << ", " << mat[2][2] << ")";
            return o;
        }

        static const Real epsilon;
        static const Matrix3 zero;
        static const Matrix3 identity;

    private:
        // support for eigensolver
        void tridiagonal (Real afDiag[3], Real afSubDiag[3]);
        bool QL_algorithm (Real afDiag[3], Real afSubDiag[3]);

        // support for singular value decomposition
        static const unsigned int ms_svd_max_iterations_;
        static void bidiagonalize (Matrix3* kA, Matrix3* kL,
            Matrix3* kR);
        static void golub_kahan_step (Matrix3* kA, Matrix3* kL,
            Matrix3* kR);

        // support for spectral norm
        static Real max_cubic_root (Real afCoeff[3]);

        Real m_[3][3];

        // for faster access
        friend class Matrix4;
    };

    /// Matrix * vector [3x3 * 3x1 = 3x1]
    inline Vector3 operator*(const Matrix3& m, const Vector3& v)
    {
        return Vector3(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z);
    }

    inline Matrix3 Math::look_rotation(const Vector3& direction, const Vector3& yaw)
    {
        Matrix3 ret;
        // cross twice to rederive, only direction is unaltered
        const Vector3& xAxis = yaw.cross_product(direction).normalised_copy();
        const Vector3& yAxis = direction.cross_product(xAxis);
        ret.from_axes(xAxis, yAxis, direction);
        return ret;
    }
    /** @} */
    /** @} */
}
#endif
