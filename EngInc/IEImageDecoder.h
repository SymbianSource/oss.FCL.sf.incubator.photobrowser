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

#ifndef __IEIMAGEDECODER_H__
#define __IEIMAGEDECODER_H__

// Include files
#include <e32base.h>
#include <f32file.h>
#include <fbs.h>
#include <ImageConversion.h>
#include <IclExtJpegApi.h>

#include "IEImage.h"

class MDecodingObserver
{
public:
	//virtual void YuvImageReadyL(TInt aError) = 0;
	virtual void BitmapReadyL(TInt aError) = 0;
};

// Forward class declarations

// Class declaration
class CIEImageDecoder : CActive
{
public:
	static CIEImageDecoder* NewL(RFs& aFileServer, MDecodingObserver& aObserver);
	~CIEImageDecoder();

private:
	void ConstructL();
	CIEImageDecoder(RFs& aFileServer, MDecodingObserver& aObserver);
	
public: // From CAtive
	void RunL()	;
	void DoCancel();
	
public:
	void GetImageSizeL(const TFileName aFileName, TSize& aSize);
	void ConvertJpeg2YuvL(const TDesC& aSourceFile, 
					HBufC8& aBuffer, 
					const TImageForamt aImageFormat);
	
	void ConvertJpeg2BitmapL(CFbsBitmap& aDestBitmap, TDesC8& aSourceData);
	
	TPtr8 GetVisualFrame();
	
	void CancelDecoding();
	
private: // Data
	RFs& iFileServer;
	MDecodingObserver& iObserver;
	CImageDecoder* iImageDecoder;
	CExtJpegDecoder* iExtImageDecoder;
	CVisualFrame* iVisualFrame;
	TBool iDecoderBusy;
	TBool iDecode2Yuv;
	TBool iDecode2Bitmap;
	TPtr8 iSrcPtr;
	
	TUint8* iBufU;
	TInt iNumOfBitmaps;
};

#endif // __IEIMAGEDECODER_H__
