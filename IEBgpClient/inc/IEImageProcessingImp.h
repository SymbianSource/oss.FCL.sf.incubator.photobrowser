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

#ifndef __IEIMAGEPROCESSINGIMP_H__
#define __IEIMAGEPROCESSINGIMP_H__

#include <IEBgpsInfo.h>

#include <IEImageProcessing.h>

#include "IEImagicBGPSAO.h"

// Include files

#include "IEBgpClient.h"

class CImagicBGPSAO;
// Forward class declarations

/**
 * Thumbnail generation implementation class.
 */ 
class CIEImageProcessingImp : public CIEImageProcessing,public MIETNInternalObserver
{
public:
    /**
     * Symbian 1st Phase construction.
     * Creation implementatation class.
     * 
     * @param aObserver - Thumbnail Observer
     */
	static CIEImageProcessingImp* NewL(MIETNObserver& aObserver);
	/**
     * Destructor.
     *
     */
	~CIEImageProcessingImp();
private:
	void ConstructL();
	CIEImageProcessingImp(MIETNObserver& aObserver);

private: // from MIETNInternalObserver
    /**
     *  When Thumbnail generation is completed this method is 
     *  called.
     * 
     * @param  aErrorCode - KErrNone in case of no error otherwise
     *                    - System wide error code
     */
	void HandleEvents(TInt aErrorCode);
	
private: // From CIEImageProcessing	 	
	void GenerateTN(const TDesC& aMGDir, const TDesC& aTNDir);
	void GenerateTN(const TDesC& aMGDir, const TDesC& aTNDir, const TSize &aSize);
	void GenerateTN(const TDesC& aMGDir, const TDesC& aTNDir, const TSize &aSize, CFbsBitmap* aSrcBitmap);
	void CancelTNGeneration();

private: // Data members
    /** Observer reference **/
	MIETNObserver& iObserver;
	/** Client **/
	RIEBgpClient iIEBgpClient;	
	/** TNCreation Active object pointer **/
	CImagicBGPSAO *iImagicBGPSAO;
};

#endif // __IEIMAGEPROCESSINGIMP_H__
