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

#ifndef __IETNGENERATORAO_H__
#define __IETNGENERATORAO_H__

#include <e32base.h>
#include <f32file.h>
#include <BitmapTransforms.h>
#include <ImageConversion.h>

#include "IEBgpServerSession.h"
#include <IEBgpsInfo.h>
#include <ICLExif.h> 
#include <exifmodify.h>
#include <exifread.h>
#include "debug.h"

class CImageDecoder;
class CFbsBitmap;

//#define USE_EXT_JPEG_DEC

#ifdef USE_EXT_JPEG_DEC
class CExtJpegDecoder;
#endif


#define DECODE_FROM_BUFFER

// CONSTANTS
const TInt KMimeStringLength = 256;

class  CIETNGeneratorAO : public CActive
{
public:
	static CIETNGeneratorAO* NewL(RFs& aFileServer, MIEThumbNailObserver &aObserver);
	virtual ~CIETNGeneratorAO();
	
private:
	void ConstructL();
	
	CIETNGeneratorAO(RFs& aFileServer, MIEThumbNailObserver &aObserver);


protected:  // Type declarations
    enum TIETNConvertStatus
    {
    	ENone = 0,
        EDecoding,
        EScaling,
        EEncoding,
        EReady
    };
	
public:

	void SetImageArray(	RArray<CImageData*> aImageArray);
	void CancelOutStaningRequests();
	void DeleteObjects();
	void CancelRequestsAndDeleteObjects();
	
public: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
public: // Other public functions
 
	RArray<CImageData*> iImageArray;
	void CreateThumbnailL(const TDes& aSourceFile, const TDes& aThumbnailFile, const TSize &aSize);
	void CreateThumbnailL(const TDes& aSourceFile, const TDes& aThumbnailFile, const TSize &aSize, CFbsBitmap* a512x512TnBitmap);
	
	
private:
	void SetJpegImageDataL();
	void WriteExifDataL(const TDes &aFilename, TSize aSize);
	void DecodeL();
	void EncodeL();
	void ScaleL();
	void TargetDecodingSize(const TSize aTgtSize, TSize& aSrcSize);
	TBool IsLargeThumbnail(const TSize& aResolution) const;
	TBool IsJPEG(const TSize& aResolution) const;
	
private:
  
    RFs&                  iFileServer;
#ifdef USE_EXT_JPEG_DEC
    CExtJpegDecoder*      iImageDecoder;
#else
    CImageDecoder*        iImageDecoder;
#endif
    CImageEncoder*        iImageEncoder;
    CFbsBitmap*           iBitmap;
    CBitmapScaler*        iBitmapScaler;
    TIETNConvertStatus    iConvertStatus;   /** Convert status */
    MIEThumbNailObserver& iThumbnailObserver;
    TFileName             iSourceFileName;
    TFileName             iThumbnailFileName;
    TBuf8<KMimeStringLength> iMimeString;   /** The source file Mime-string */
    TSize                 iSourceSize;
    CFrameImageData*      iFrameImageData;
    TSize                 iThumbnailSize;
    TTime                 iSourceTime;      /** Source file time */
    TInt                  iError;  
    CFbsBitmap*           i512x512TnBitmap;
    TUid                  decoderUid;

#ifdef DECODE_FROM_BUFFER
    HBufC8*               iSourceData;
#endif

};


#endif
