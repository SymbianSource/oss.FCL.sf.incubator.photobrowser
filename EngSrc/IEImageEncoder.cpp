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
#include "IEImageEncoder.h"

// ========================== MEMBER FUNCTIONS ============================= //

CIEImageEncoder* CIEImageEncoder::NewL(RFs& aFileServer, MEncodingObserver& aObserver)
{
	CIEImageEncoder* self = new (ELeave) CIEImageEncoder(aFileServer, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
}

CIEImageEncoder::~CIEImageEncoder()
{
	if(iFrameImageData)
	{
		delete iFrameImageData;
		iFrameImageData = NULL;
	}
	
	if(iImageEncoder)
	{
		delete iImageEncoder;
		iImageEncoder = NULL;
	}
	
	if(iExtImageEncoder)
	{
		delete iExtImageEncoder;
		iExtImageEncoder = NULL;
	}
	
	if(iVisualFrame)
	{
		delete iVisualFrame;
		iVisualFrame = NULL;
	}
}


CIEImageEncoder::CIEImageEncoder(RFs& aFileServer, MEncodingObserver& aObserver)
: CActive(EPriorityStandard), iFileServer(aFileServer), iObserver(aObserver)
{	
}

void CIEImageEncoder::ConstructL()
{
	iEncoderBusy = EFalse;
	
	CActiveScheduler::Add(this);
}

void CIEImageEncoder::RunL()
{
	TInt error = iStatus.Int();
	
	//iObserver.JpegImageReadyL(iStatus.Int());
	iEncoderBusy = EFalse;
}

void CIEImageEncoder::DoCancel()
{
	
}

void CIEImageEncoder::ConvertYuv2JpegL(HBufC8*& aDestBuffer, 
								HBufC8& aSourceBuffer, 
								const TSize aSize, 
								const TImageForamt /*aFormat*/)
{
	TInt blocks = 0;

	iEncoderBusy = ETrue;

	if(iExtImageEncoder)
	{
		delete iExtImageEncoder;
		iExtImageEncoder = NULL;
		
	}
	
	iExtImageEncoder = CExtJpegEncoder::DataNewL(aDestBuffer, _L8("image/jpeg"));
	
	TPtr8 ptrSrc = aSourceBuffer.Des();
	TInt bufSize = aSourceBuffer.Size();
		
	if(iVisualFrame)
	{
		delete iVisualFrame;
		iVisualFrame = NULL;
	}
	
	iVisualFrame = CVisualFrame::NewL(ptrSrc, aSize, CVisualFrame::EFormatYUV420Planar);
	
	SetJpegImageDataL();
	iExtImageEncoder->ConvertL(&iStatus, iVisualFrame, blocks, iFrameImageData);
	
	if(!IsActive())
		SetActive();
}

void CIEImageEncoder::ConvertYuv2JpegL(TDesC& aFileName, 
								HBufC8& aSourceBuffer, 
								const TSize aSize, 
								const TImageForamt /*aFormat*/)
{
	TInt blocks = 0;

	iEncoderBusy = ETrue;

	if(iExtImageEncoder)
	{
		delete iExtImageEncoder;
		iExtImageEncoder = NULL;
		
	}
	
	iExtImageEncoder = CExtJpegEncoder::FileNewL(iFileServer, aFileName, _L8("image/jpeg"));
	
	TPtr8 ptrSrc = aSourceBuffer.Des();
	TInt bufSize = aSourceBuffer.Size();
		
	if(iVisualFrame)
	{
		delete iVisualFrame;
		iVisualFrame = NULL;
	}
	
	iVisualFrame = CVisualFrame::NewL(ptrSrc, aSize, CVisualFrame::EFormatYUV420Planar);
	
	SetJpegImageDataL();
	iExtImageEncoder->ConvertL(&iStatus, iVisualFrame, blocks, iFrameImageData);
	
	if(!IsActive())
		SetActive();
}

void CIEImageEncoder::SetJpegImageDataL()
{
	TJpegImageData* jpegImageData;
	jpegImageData = new (ELeave) TJpegImageData;
	jpegImageData->iSampleScheme = TJpegImageData::EColor420;
	jpegImageData->iQualityFactor = 90;	
	
	if(iFrameImageData)
	{
		delete iFrameImageData;
		iFrameImageData = NULL;
	}
	
	iFrameImageData = CFrameImageData::NewL();
	// Ownership of jpegImageData lies with CFrameImageData
	User::LeaveIfError(iFrameImageData->AppendImageData(jpegImageData));
}

void CIEImageEncoder::CancelEncoding()
{
	if(iEncoderBusy)
	{
		if(iImageEncoder)
			iImageEncoder->Cancel();
		if(iExtImageEncoder)
			iExtImageEncoder->Cancel();
	}
	
	if(IsActive())
		Cancel();
}

// EOF
