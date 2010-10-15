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

// Include files
#include "IEImageDecoder.h"

// ========================== MEMBER FUNCTIONS ============================= //

CIEImageDecoder* CIEImageDecoder::NewL(RFs& aFileServer, MDecodingObserver& aObserver)
{
	CIEImageDecoder* self = new (ELeave) CIEImageDecoder(aFileServer, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
}

CIEImageDecoder::~CIEImageDecoder()
{
	if(iImageDecoder)
	{
		delete iImageDecoder;
		iImageDecoder = NULL;
	}
	
	if(iExtImageDecoder)
	{
		delete iExtImageDecoder;
		iExtImageDecoder = NULL;
	}
	
	if(iVisualFrame)
	{
		delete iVisualFrame;
		iVisualFrame = NULL;
	}
}

//EPriorityIdle, EPriorityLow, EPriorityStandard, EPriorityUserInput, EPriorityHigh
CIEImageDecoder::CIEImageDecoder(RFs& aFileServer, MDecodingObserver& aObserver)
: CActive(EPriorityStandard), iFileServer(aFileServer), iObserver(aObserver), iSrcPtr(NULL, 0, 0)
{	
}

void CIEImageDecoder::ConstructL()
{
	CActiveScheduler::Add(this);
	
	iDecoderBusy = EFalse;
	iDecode2Yuv = EFalse;
	iDecode2Bitmap = EFalse;
	
	iNumOfBitmaps = 0;
}

void CIEImageDecoder::RunL()
{
/*	
    TPtr8 ptr = iVisualFrame->DataPtrL();
	TInt dataSize = ptr.Size(); 
	TUint8* bufU = (TUint8*) ptr.Ptr();
*/
	if(iDecode2Yuv)
	{
		iDecode2Yuv = EFalse;
		
		if(iExtImageDecoder)
	    {
	        delete iExtImageDecoder;
	        iExtImageDecoder = NULL;
	    }
		
		//iObserver.YuvImageReady(iStatus.Int());
	}
		
	if(iDecode2Bitmap)
	{
		iDecode2Bitmap = EFalse;
		
		if(iImageDecoder)
	    {
	        delete iImageDecoder;
	        iImageDecoder = NULL;
	    }
		
		iObserver.BitmapReady(iStatus.Int());
	}		
	
	iDecoderBusy = EFalse;	
}

void CIEImageDecoder::DoCancel()
{
	
}

void CIEImageDecoder::GetImageSizeL(const TFileName aFileName, TSize& aSize)
{
	if(iImageDecoder)
	{
		delete iImageDecoder;
		iImageDecoder = NULL;
	}
	
	iImageDecoder = CImageDecoder::FileNewL(iFileServer, aFileName);
	TFrameInfo frameInfo = iImageDecoder->FrameInfo();
	
	aSize.iWidth = frameInfo.iOverallSizeInPixels.iWidth;
	aSize.iHeight = frameInfo.iOverallSizeInPixels.iHeight;
	
	delete iImageDecoder;
	iImageDecoder = NULL;	
}

void CIEImageDecoder::ConvertJpeg2YuvL(const TDesC& aSourceFile, 
								HBufC8& aBuffer)
{
	TInt fileSize = 0;
	TInt blocks = 0;
	
	iDecoderBusy = ETrue;
	iDecode2Yuv = ETrue;
	
	if(iExtImageDecoder)
	{
		delete iExtImageDecoder;
		iExtImageDecoder = NULL;
	}
	
	iExtImageDecoder = CExtJpegDecoder::FileNewL(iFileServer, aSourceFile);
	
	TFrameInfo frameInfo = iExtImageDecoder->FrameInfo();
	
	TInt width = frameInfo.iOverallSizeInPixels.iWidth;
	TInt height = frameInfo.iOverallSizeInPixels.iHeight;

	/*if(width%2 != 0)
	    width++;
    if(height%2 != 0)
        height++;*/
	        
	TPtr8 bufPtr = aBuffer.Des();
	
	iBufU = (TUint8*)bufPtr.Ptr();
	
	if(iVisualFrame)
	{
		delete iVisualFrame;
		iVisualFrame = NULL;
	}
	
	iVisualFrame = CVisualFrame::NewL(bufPtr, 
											TSize(width, height), 
											CVisualFrame::EFormatYUV420Planar);
	
	
	iExtImageDecoder->ConvertL(&iStatus, iVisualFrame, blocks);
	
	if(!IsActive())
		SetActive();
	
}


void CIEImageDecoder::ConvertJpeg2BitmapL(CFbsBitmap& aDestBitmap, TDesC8& aSourceData)
{
	TInt frameNumber = 0;
	
	iDecoderBusy = ETrue;
	iDecode2Bitmap = ETrue;
	
	TInt dataSize = aSourceData.Size();
	
	iSrcPtr.Set((TUint8*)aSourceData.Ptr(), aSourceData.Size(), aSourceData.Size());
	
	if(iImageDecoder)
	{
		delete iImageDecoder;
		iImageDecoder = NULL;
	}
	
	iImageDecoder = CImageDecoder::DataNewL(iFileServer, iSrcPtr);
	
	iImageDecoder->Convert(&iStatus, aDestBitmap, frameNumber);
	
	iNumOfBitmaps++;
	
	if(!IsActive())
		SetActive();
}


TPtr8 CIEImageDecoder::GetVisualFrame()
{
	return iVisualFrame->DataPtrL();	 
}

void CIEImageDecoder::CancelDecoding()
{
	if(iDecoderBusy)
	{
		if(iImageDecoder)
			iImageDecoder->Cancel();
	
		if(iExtImageDecoder)
			iExtImageDecoder->Cancel();
	}
		
	if(IsActive())
		Cancel();	
}


// EOF
