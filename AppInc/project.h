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

#ifndef PROJECT_H_
#define PROJECT_H_

#include <e32std.h>
#include <GLES\egl.h>
#define GLdouble GLfloat

GLint gluProject(GLdouble objx, GLdouble objy, GLdouble objz,
       const GLdouble model[16], const GLdouble proj[16],
       const GLint viewport[4],
       GLdouble * winx, GLdouble * winy, GLdouble * winz);

GLint gluUnProject(GLdouble winx, GLdouble winy, GLdouble winz,
         const GLdouble model[16], const GLdouble proj[16],
         const GLint viewport[4],
         GLdouble * objx, GLdouble * objy, GLdouble * objz);

#endif /* PROJECT_H_ */
