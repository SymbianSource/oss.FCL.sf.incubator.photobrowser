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

#ifndef __IEBGPSINFO_H__
#define __IEBGPSINFO_H__

// Include files
#include <e32base.h> 

/**
 * Server name
 */
_LIT(KIEBgpServerName, "IEBgpServer_0x2002135"); 

/**
 * Server version
 */
const TUint KIEBgpServerMajorVersion = 0;
const TUint KIEBgpServerMinorVersion = 1;
const TUint KIEBgpServerBuildVersion = 1; 

/**
 * Default message slots for the server
 */
const TInt KIEDefaultMsgSlot = 4; 

/**
 * Thread's max and min heap sizes
 */
const TUint KIEHeapSizeMin = 0x10000;
const TUint KIEHeapSizeMax = 0x3000000;

enum TImageArrayMode
    {
    EImages = 1,
    EFaces
    };

/**
 * Server seesion sevices
 */
enum TIEBgpServerRequests
{
	EIESrvTest = 1,
	EIESrvCloseSession,
	EIEThumbnailGeneration,
	EIESingleTNGeneration,
	EIESingleTNGenerationWithBitmap,
	EIECancelThumbnailGeneration ,
	EIEStartProcessing
};

enum TBGPSEventCode
{
	EEventNone = 1,
	ETNGenerationcomplete,
	ETNGenerationCancelled,
	EFaceDetectionComplete,
	EFaceAddedToExif,
	EFaceRemovedFromExif,
	ESingleFaceDetectionComplete,
	EFaceDetectionCancelled
};

/**
 * starts the server thread
 * called by client
 */
IMPORT_C TInt CreateServerThread(RThread& aThread);

#endif // __IEBGPSINFO_H__
