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

#ifndef __IEBGPSTRACE_H__
#define __IEBGPSTRACE_H__

#define _DEBUG
#ifdef _DEBUG

#include <e32debug.h>

#define IEBGPSPRINT(x)	RDebug::Print x;

#else

#define IEBGPSPRINT(x)	

#endif // _DEBUG


#endif //__IEBGPSTRACE_H__
