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

#ifndef __IETNCREATOR_H__
#define __IETNCREATOR_H__

#include <IEImageProcessing.h>
#include <IEBgpsInfo.h>

/**
 *  Internal Observer class between client and server
 */ 
class MIETNInternalObserver
{
public:
    /**
    *  When Thumbnail generation is completed this method is 
    *  called from RunL.
    * 
    * @param  aErrorCode - KErrNone in case of no error otherwise
    *                    - System wide error code
    */
	virtual void HandleEvents(TInt aErrorCode) = 0;
};
/**
 *  ThumbnailCreator class
 */ 
class  CImagicBGPSAO : public CActive
{
public:
    /**
     * Symbian 1st Phase construction.
     * Creating CImagicBGPSAO object.
     * 
     * @param aIETNObserver - Internal observer between client and server
     * @return A pointer to a new instance of the CImagicBGPSAO class.
     */
	static CImagicBGPSAO* NewL(MIETNInternalObserver& aIETNObserver);
	
	/**
	 * Destructor.
	 */
	virtual ~CImagicBGPSAO();
	
private:
	void ConstructL();
	CImagicBGPSAO(MIETNInternalObserver& aIETNObserver);
	
protected: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
public:  
    /**
     * Activate Thumbnail genration request.
     * 
     */
	void ActivateTNCreatorAO();
	
	/**
     * Cancels Thumbnail genration request.
     * 
     */
	void DeActivateTNCreatoAO();
	
private:
    /** Internal observer reference **/
    MIETNInternalObserver &iIETNObserver;
};

#endif

 
