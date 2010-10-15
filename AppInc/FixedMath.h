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

//------------------------------
#ifndef FIXED_MATH_H_
#define FIXED_MATH_H_
//------------------------------
 
// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <e32math.h>
#include <GLES/gl.h>   
 
// FUNCTIONS
 
inline GLfixed IntToFixed (GLint aValue)
{ return aValue << 16; }
 
 
inline GLfixed FloatToFixed (GLfloat aValue)
{ return (GLfixed) (aValue * 65536.0f); }
 
 
inline GLint FixedToInt (GLfixed aValue)
{ return aValue >> 16; }
 
 
inline GLfloat FixedToFloat (GLfixed aValue)
{ return (GLfloat) (aValue * (1 / 65536.0f)); }
 
 
inline GLfixed MultiplyFixed (GLfixed op1, GLfixed op2) 
{
  TInt64 r = (TInt64)op1 * (TInt64)op2;
  return (GLfixed) (r >> 16);
}
 
 
//------------------------------
#endif
