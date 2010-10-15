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

#ifndef __IMAGIC_TRACE_H__
#define __IMAGIC_TRACE_H__

//#define _IMAGIC_DEBUG
#ifdef _IMAGIC_DEBUG

#include <e32debug.h>

#define DP0_IMAGIC(string)                            RDebug::Print(string)
#define DP1_IMAGIC(string,arg1)                       RDebug::Print(string,arg1)
#define DP2_IMAGIC(string,arg1,arg2)                  RDebug::Print(string,arg1,arg2)
#define DP3_IMAGIC(string,arg1,arg2,arg3)             RDebug::Print(string,arg1,arg2,arg3)
#define DP4_IMAGIC(string,arg1,arg2,arg3,arg4)        RDebug::Print(string,arg1,arg2,arg3,arg4)
#define DP5_IMAGIC(string,arg1,arg2,arg3,arg4,arg5)   RDebug::Print(string,arg1,arg2,arg3,arg4,arg5)

#else

#define DP0_IMAGIC(string)                            
#define DP1_IMAGIC(string,arg1)                       
#define DP2_IMAGIC(string,arg1,arg2)                  
#define DP3_IMAGIC(string,arg1,arg2,arg3)             
#define DP4_IMAGIC(string,arg1,arg2,arg3,arg4)        
#define DP5_IMAGIC(string,arg1,arg2,arg3,arg4,arg5)
#endif // _DEBUG


#endif //__IMAGIC_TRACE_H__
