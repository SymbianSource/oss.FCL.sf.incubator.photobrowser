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

#ifndef __IEIMAGEPROCESSING_H__
#define __IEIMAGEPROCESSING_H__

// Include files
#include <e32base.h>
#include "IEBgpsInfo.h"
#include "IEImageData.h"
#include <FBS.H> //CFbsBitmap

// Forward class declarations

/**
 *  Observer class. Provides the interface functions
 *   
 */ 
class MIETNObserver
{
public:
    /**
     *  When Thumbnail Genration is completed IEBGPS client
     *  call this function.
     *   
     * @param aErrorCode - Systemwide error code in case of error 
     *                   - KErrNone in case of no errors.
     */ 
	virtual void ThumbnailGenerationCompleted(TInt aErrorCode) = 0;
	virtual void ThumbnailGenerationCancelled(TInt aErrorCode) = 0;
	
	/**
     *  When Thumbnail Genration is completed IEBGPS client
     *  calls this function.
     *   
     * @param aErrorCode - Systemwide error code in case of error 
     *                   - KErrNone in case of no errors.
     */ 
	virtual void HandleError(TInt aError) = 0;
		
};

/**
 *  Thumbnail generation class.
 */ 
class CIEImageProcessing : public CBase
{
public:
    /**
     *  Symbian First phase constructor to create Imageprocessing Object
     * 
     * @param aObserver - Thumbnail observer
     */ 
	IMPORT_C static CIEImageProcessing* NewL(MIETNObserver& aObserver);
	
	/**
     * Overloaded function generates Thumbnails. 
     *  
     * @Param  aMGDir - Jpeg File with absolute path.
     * @parama aTNDir - Thumbnail file name with abolutepath
     */
	IMPORT_C virtual void GenerateTN(const TDesC& aMGDir, const TDesC& aTNDir) = 0;
	/**
     * Overloaded function generates Thumbnails. 
     * 
     * @Param aMGDir - Jpeg File with absolute path.
     * @param aImageArrary aTNDir - Thumbnail file name with abolutepath
     * @Param aSize - Thumbnail size.
     */ 
	IMPORT_C virtual void GenerateTN(const TDesC& aMGDir, const TDesC& aTNDir, const TSize &aSize) = 0;
	
	IMPORT_C virtual void GenerateTN(const TDesC& aMGDir, const TDesC& aTNDir, const TSize &aSize, CFbsBitmap* aSrcBitmap) = 0;
	
	IMPORT_C virtual void CancelTNGeneration() = 0;
		
};

#endif // __IEIMAGEPROCESSING_H__
