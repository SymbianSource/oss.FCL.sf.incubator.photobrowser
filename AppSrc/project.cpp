/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors: Juha Kauppinen, Mika Hokkanen
* 
* Description: Photo Browser
*
*/

#include "project.h"
#define fabs(x) ((x) < 0 ? -(x) : (x))
#define MEMCPY(x,y,z) Mem::Copy((x),(y),(z))

/*
 * Transform a point (column vector) by a 4x4 matrix.  I.e.  out = m * in
 * Input:  aMatrix - the 4x4 matrix
 *         aIn - the 4x1 vector
 * Output: aOut - the resulting 4x1 vector.
 */
static void TransformPoint(GLdouble aOut[4], const GLdouble aMatrix[16], const GLdouble aIn[4])
    {
#define M(row,col)  aMatrix[col*4+row]
    aOut[0] = M(0, 0) * aIn[0] + M(0, 1) * aIn[1] + M(0, 2) * aIn[2] + M(0, 3) * aIn[3];
    aOut[1] = M(1, 0) * aIn[0] + M(1, 1) * aIn[1] + M(1, 2) * aIn[2] + M(1, 3) * aIn[3];
    aOut[2] = M(2, 0) * aIn[0] + M(2, 1) * aIn[1] + M(2, 2) * aIn[2] + M(2, 3) * aIn[3];
    aOut[3] = M(3, 0) * aIn[0] + M(3, 1) * aIn[1] + M(3, 2) * aIn[2] + M(3, 3) * aIn[3];
#undef M
    }

/*
 * Perform a 4x4 matrix multiplication  (product = a x b).
 * Input:  a, b - matrices to multiply
 * Output: aProduct - product of a and b
 */
static void MultiplyMatrix(GLdouble* aProduct, const GLdouble* a, const GLdouble* b)
    {
    GLdouble temp[16];
    GLint i;

#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define T(row,col)  temp[(col<<2)+row]

    for (i = 0; i < 4; i++)
        {
        T(i, 0) = A(i, 0) * B(0, 0) + A(i, 1) * B(1, 0) + A(i, 2) * B(2, 0) + A(i, 3) * B(3, 0);
        T(i, 1) = A(i, 0) * B(0, 1) + A(i, 1) * B(1, 1) + A(i, 2) * B(2, 1) + A(i, 3) * B(3, 1);
        T(i, 2) = A(i, 0) * B(0, 2) + A(i, 1) * B(1, 2) + A(i, 2) * B(2, 2) + A(i, 3) * B(3, 2);
        T(i, 3) = A(i, 0) * B(0, 3) + A(i, 1) * B(1, 3) + A(i, 2) * B(2, 3) + A(i, 3) * B(3, 3);
        }

#undef A
#undef B
#undef T
   MEMCPY(aProduct, temp, 16 * sizeof(GLdouble));
    }

/*
 * Compute inverse of 4x4 transformation matrix.
 * Return GL_TRUE for success, GL_FALSE for failure (singular matrix)
 */
static GLboolean InvertMatrix(const GLdouble * aMatrix, GLdouble * aOut)
    {
    // OpenGL Matrices are COLUMN major
#define SWAP_ROWS(a, b) { GLdouble *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

    GLdouble wtmp[4][8];
    GLdouble m0, m1, m2, m3, s;
    GLdouble *r0, *r1, *r2, *r3;

    r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

    r0[0] = MAT(aMatrix, 0, 0), r0[1] = MAT(aMatrix, 0, 1),
    r0[2] = MAT(aMatrix, 0, 2), r0[3] = MAT(aMatrix, 0, 3),
    r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
    r1[0] = MAT(aMatrix, 1, 0), r1[1] = MAT(aMatrix, 1, 1),
    r1[2] = MAT(aMatrix, 1, 2), r1[3] = MAT(aMatrix, 1, 3),
    r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
    r2[0] = MAT(aMatrix, 2, 0), r2[1] = MAT(aMatrix, 2, 1),
    r2[2] = MAT(aMatrix, 2, 2), r2[3] = MAT(aMatrix, 2, 3),
    r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
    r3[0] = MAT(aMatrix, 3, 0), r3[1] = MAT(aMatrix, 3, 1),
    r3[2] = MAT(aMatrix, 3, 2), r3[3] = MAT(aMatrix, 3, 3),
    r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

    // choose pivot - or die
    if (fabs(r3[0]) > fabs(r2[0]))
        SWAP_ROWS(r3, r2);
    if (fabs(r2[0]) > fabs(r1[0]))
        SWAP_ROWS(r2, r1);
    if (fabs(r1[0]) > fabs(r0[0]))
        SWAP_ROWS(r1, r0);
    if (0.0 == r0[0])
        return GL_FALSE;

    // eliminate first variable
    m1 = r1[0] / r0[0];
    m2 = r2[0] / r0[0];
    m3 = r3[0] / r0[0];
    s = r0[1];
    r1[1] -= m1 * s;
    r2[1] -= m2 * s;
    r3[1] -= m3 * s;
    s = r0[2];
    r1[2] -= m1 * s;
    r2[2] -= m2 * s;
    r3[2] -= m3 * s;
    s = r0[3];
    r1[3] -= m1 * s;
    r2[3] -= m2 * s;
    r3[3] -= m3 * s;
    s = r0[4];

    if (s != 0.0)
        {
        r1[4] -= m1 * s;
        r2[4] -= m2 * s;
        r3[4] -= m3 * s;
        }
    s = r0[5];

    if (s != 0.0)
        {
        r1[5] -= m1 * s;
        r2[5] -= m2 * s;
        r3[5] -= m3 * s;
        }
    s = r0[6];

    if (s != 0.0)
        {
        r1[6] -= m1 * s;
        r2[6] -= m2 * s;
        r3[6] -= m3 * s;
        }
    s = r0[7];

    if (s != 0.0)
        {
        r1[7] -= m1 * s;
        r2[7] -= m2 * s;
        r3[7] -= m3 * s;
        }

    // choose pivot - or die
    if (fabs(r3[1]) > fabs(r2[1]))
        SWAP_ROWS(r3, r2);
    if (fabs(r2[1]) > fabs(r1[1]))
        SWAP_ROWS(r2, r1);
    if (0.0 == r1[1])
        return GL_FALSE;

    // eliminate second variable
    m2 = r2[1] / r1[1];
    m3 = r3[1] / r1[1];
    r2[2] -= m2 * r1[2];
    r3[2] -= m3 * r1[2];
    r2[3] -= m2 * r1[3];
    r3[3] -= m3 * r1[3];
    s = r1[4];

    if (0.0 != s)
        {
        r2[4] -= m2 * s;
        r3[4] -= m3 * s;
        }
    s = r1[5];

    if (0.0 != s)
        {
        r2[5] -= m2 * s;
        r3[5] -= m3 * s;
        }
    s = r1[6];

    if (0.0 != s)
        {
        r2[6] -= m2 * s;
        r3[6] -= m3 * s;
        }
    s = r1[7];

    if (0.0 != s)
        {
        r2[7] -= m2 * s;
        r3[7] -= m3 * s;
        }

    // choose pivot - or die
    if (fabs(r3[2]) > fabs(r2[2]))
        SWAP_ROWS(r3, r2);
    if (0.0 == r2[2])
        return GL_FALSE;

    // eliminate third variable
    m3 = r3[2] / r2[2];
    r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
    r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6], r3[7] -= m3 * r2[7];

    // last check
    if (0.0 == r3[3])
        return GL_FALSE;

    s = 1.0 / r3[3];     /* now back substitute row 3 */
    r3[4] *= s;
    r3[5] *= s;
    r3[6] *= s;
    r3[7] *= s;

    m2 = r2[3];          /* now back substitute row 2 */
    s = 1.0 / r2[2];
    r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
    r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
    m1 = r1[3];
    r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
    r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
    m0 = r0[3];
    r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
    r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

    m1 = r1[2];          /* now back substitute row 1 */
    s = 1.0 / r1[1];
    r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
    r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
    m0 = r0[2];
    r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
    r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

    m0 = r0[1];          /* now back substitute row 0 */
    s = 1.0 / r0[0];
    r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
    r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

    MAT(aOut, 0, 0) = r0[4];
    MAT(aOut, 0, 1) = r0[5], MAT(aOut, 0, 2) = r0[6];
    MAT(aOut, 0, 3) = r0[7], MAT(aOut, 1, 0) = r1[4];
    MAT(aOut, 1, 1) = r1[5], MAT(aOut, 1, 2) = r1[6];
    MAT(aOut, 1, 3) = r1[7], MAT(aOut, 2, 0) = r2[4];
    MAT(aOut, 2, 1) = r2[5], MAT(aOut, 2, 2) = r2[6];
    MAT(aOut, 2, 3) = r2[7], MAT(aOut, 3, 0) = r3[4];
    MAT(aOut, 3, 1) = r3[5], MAT(aOut, 3, 2) = r3[6];
    MAT(aOut, 3, 3) = r3[7];
    
    return GL_TRUE;

#undef MAT
#undef SWAP_ROWS
    }

GLint gluProject(GLdouble objx, GLdouble objy, GLdouble objz,
       const GLdouble model[16], const GLdouble proj[16],
       const GLint viewport[4],
       GLdouble* winx, GLdouble* winy, GLdouble* winz)
    {
    GLdouble in[4], out[4];

    in[0] = objx;
    in[1] = objy;
    in[2] = objz;
    in[3] = 1.0;
    TransformPoint(out, model, in);
    TransformPoint(in, proj, out);

    if (in[3] == 0.0)
        return GL_FALSE;

    in[0] /= in[3];
    in[1] /= in[3];
    in[2] /= in[3];

    *winx = viewport[0] + (1 + in[0]) * viewport[2] / 2;
    *winy = viewport[1] + (1 + in[1]) * viewport[3] / 2;
    *winz = (1 + in[2]) / 2;
    return GL_TRUE;
}

GLint gluUnProject(GLdouble winx, GLdouble winy, GLdouble winz,
         const GLdouble model[16], const GLdouble proj[16],
         const GLint viewport[4],
         GLdouble * objx, GLdouble * objy, GLdouble * objz)
    {
    GLdouble m[16], A[16];
    GLdouble in[4], out[4];

    in[0] = (winx - viewport[0]) * 2 / viewport[2] - 1.0;
    in[1] = (winy - viewport[1]) * 2 / viewport[3] - 1.0;
    in[2] = 2 * winz - 1.0;
    in[3] = 1.0;

    MultiplyMatrix(A, proj, model);
    InvertMatrix(A, m);

    TransformPoint(out, m, in);
    if (out[3] == 0.0)
        return GL_FALSE;
    *objx = out[0] / out[3];
    *objy = out[1] / out[3];
    *objz = out[2] / out[3];
    return GL_TRUE;
    }
