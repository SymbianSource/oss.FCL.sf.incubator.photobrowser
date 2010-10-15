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

#ifndef __IEBITMAPLOADER_H__
#define __IEBITMAPLOADER_H__

// Include files
#include <f32file.h>
#include <FBS.H>
#include <ImageConversion.h>

#include <IEEngine.h>
#include "IEFileLoader.h"
//#include "IEEngineImp.h"
#include <iclexifimageframe.h>
//#include <oommonitorsession.h>

#define DECODE_FROM_BUFFER

// Forward class declaration
class CIEFileLoader;
class CIEEngineUtils;

class MBitmapLoaderObserver
{
public:
	virtual void BitmapsLoadedL(TInt aError) = 0;
	virtual CIEFileLoader* GetFileLoader() = 0;
	virtual TInt GetGleMaxRes() = 0;
};

// Class declaration
class CIEBitmapLoader : public CActive
{
public:
	static CIEBitmapLoader* NewL(RFs& aFileServer, MBitmapLoaderObserver& aBitmapLoaderObserver, RCriticalSection* aCritical);
	~CIEBitmapLoader();
private:
	void ConstructL();
	CIEBitmapLoader(RFs& aFileServer, MBitmapLoaderObserver& aBitmapLoaderObserver, RCriticalSection* aCritical);
	
public: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
public:
	void CIEBitmapLoader::LoadBitmapsL(TInt aIndex, TThumbSize aThumbRes);
	void GetOneBitmapL(CImageData* aImageData, CFbsBitmap* aBitmap, TThumbSize aThumbRes);
	void CancelFullSizeLoading();
	void SetImageDataMode(TImageArrayMode aMode);
		
private:
	void TargetDecodingSize(TSize aTgtSize, TSize& aSrcSize);
	//TPtr8 LoadImageIntoMemoryLC(const TDesC& aFileName);
	void CropImageL(CFbsBitmap* aOutput, CFbsBitmap* aInput) const;

	
private: //Data
	RFs&                   iFileServer;
	MBitmapLoaderObserver& iBitmapLoaderObserver;
	CImageDecoder*         iImageDecoder;
	CJPEGExifDecoder*      iExifDecoder;
	RCriticalSection*      iCritical;
	TImageArrayMode        iImageArrayMode;
	CImageData*            iImageData;
	TBool                  iUseExifTn;
	HBufC8*                iExifTn;
    CFbsBitmap*            iExifBitmap;
    CFbsBitmap*            iOutputBitmap;
    TUid                   decoderUid;
    TThumbSize             iThumbRes;
#ifdef DECODE_FROM_BUFFER
    HBufC8*                iSourceData;
#endif
};


#endif // __IEBITMAPLOADER_H__
