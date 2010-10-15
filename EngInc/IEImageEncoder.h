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

#ifndef __IEIMAGEENCODER_H__
#define __IEIMAGEENCODER_H__

// Include files
#include <e32base.h>
#include <f32file.h>
#include <fbs.h>
#include <ImageConversion.h>
#include <IclExtJpegApi.h>

#include "IEImage.h"

// Forward class declarations
class MEncodingObserver
{
public:
	virtual void JpegImageReadyL(TInt aError) = 0;
};


// Class declaration
class CIEImageEncoder : CActive
{
public:
	static CIEImageEncoder* NewL(RFs& aFileServer, MEncodingObserver& aObserver);
	~CIEImageEncoder();

private:
	void ConstructL();
	CIEImageEncoder(RFs& aFileServer, MEncodingObserver& aObserver);
	
public:
	void ConvertYuv2JpegL(HBufC8*& aDestBuffer, 
					HBufC8& aSourceBuffer, 
					const TSize aSize, 
					const TImageForamt aFormat);
	
	void ConvertYuv2JpegL(TDesC& aFileName, 
					HBufC8& aSourceBuffer, 
					const TSize aSize, 
					const TImageForamt aFormat);

	void CancelEncoding();
	
private:	
	void SetJpegImageDataL();
	
public: // From CAtive
	void RunL()	;
	void DoCancel();
	
private: // Data
	RFs& iFileServer;
	MEncodingObserver& iObserver;
	
	CImageEncoder* iImageEncoder;
	CExtJpegEncoder* iExtImageEncoder;
	
	CVisualFrame* iVisualFrame;
	
	CFrameImageData* iFrameImageData;
		
	TBool iEncoderBusy;
};

#endif // __IEIMAGEENCODER_H__
