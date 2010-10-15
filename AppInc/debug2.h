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

#ifndef __DEBUG_H__
#define __DEBUG_H__

//  - INCLUDES ------------------------
#include <e32debug.h>

//  - NAMESPACE -----------------------

//  - MACROS --------------------------

// Check that D_FLOW is defined
#ifndef D_FLOW
#warning D_FLOW not defined!
#define D_FLOW 0
#endif

// Small debug functions
#define DebugIn()				if (DEBUG && D_FLOW)	RDebug::Printf("> %s", __FUNCTION__)
#define DebugOut()				if (DEBUG && D_FLOW)	RDebug::Printf("< %s", __FUNCTION__)
#define DebugInD(fmt,args...)	if (DEBUG && D_FLOW)	RDebug::Printf("> %s: "fmt, __FUNCTION__, ##args)
#define DebugOutD(fmt,args...)	if (DEBUG && D_FLOW)	RDebug::Printf("< %s: "fmt, __FUNCTION__, ##args)
#define DebugAt(X, fmt,args...)	if (DEBUG && X)			RDebug::Printf("= %s: "fmt, __FUNCTION__, ##args)

//  - CONSTANTS -----------------------

//  - EXTERNAL DATA -------------------

//  - FORWARD DECLARATIONS ------------

//  - DATA TYPES ----------------------

//  - FUNCTION DECLARATIONS -----------

//  - Inline Functions ----------------

#endif // __DEBUG_H__

// End of File

