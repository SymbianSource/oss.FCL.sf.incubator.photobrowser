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

#include "IEImageProcessingImp.h"

#include "IEImagicBGPSAO.h"
#include "debug.h"

EXPORT_C CIEImageProcessing* CIEImageProcessing::NewL(MIETNObserver& aObserver)
    {
	DP0_IMAGIC((_L("CIEImageProcessing::NewL ++")));
	return CIEImageProcessingImp::NewL(aObserver);
    }

CIEImageProcessingImp* CIEImageProcessingImp::NewL(MIETNObserver& aObserver)
    {
	CIEImageProcessingImp* self = new (ELeave) CIEImageProcessingImp(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
    }

CIEImageProcessingImp::~CIEImageProcessingImp()
    {
	DP0_IMAGIC((_L("CIEImageProcessingImp::~CIEImageProcessingImp ++")));

	if(iImagicBGPSAO)
	{
	delete iImagicBGPSAO;	
	iImagicBGPSAO = NULL;
	}
	
	iIEBgpClient.Close();
	
	DP0_IMAGIC((_L("CIEImageProcessingImp::~CIEImageProcessingImp --")));
    }

CIEImageProcessingImp::CIEImageProcessingImp(MIETNObserver& aObserver)
    : iObserver(aObserver)
    {
    DP0_IMAGIC((_L("CIEImageProcessingImp::CIEImageProcessingImp ++")));
    DP0_IMAGIC((_L("CIEImageProcessingImp::CIEImageProcessingImp --")));	
    }

void CIEImageProcessingImp::ConstructL()
    {
	DP0_IMAGIC((_L("CIEImageProcessingImp::ConstructL ++")));
	User::LeaveIfError(iIEBgpClient.Connect());
	iImagicBGPSAO = CImagicBGPSAO::NewL(*this);
    }

void CIEImageProcessingImp::HandleEvents(TInt aErrorCode)
    {
    DP0_IMAGIC((_L("CIEImageProcessingImp::HandleEvents++")));
    
    switch(aErrorCode)
        {
        case ETNGenerationCancelled:
            DP0_IMAGIC((_L("CIEImageProcessingImp::HandleEvents - ETNGenerationCancelled")));
            iObserver.ThumbnailGenerationCancelled(KErrNone);
            break;
        case ETNGenerationcomplete:
            DP0_IMAGIC((_L("CIEImageProcessingImp::HandleEvents - ETNGenerationcomplete")));
            iObserver.ThumbnailGenerationCompleted(KErrNone);
            break;
        /*case EFaceDetectionComplete:
            DP0_IMAGIC((_L("CIEImageProcessingImp::HandleEvents - EFaceDetectionComplete")));
            iObserver.FaceDetectionComplete(KErrNone);
            break;
        case EFaceCroppingComplete:
            DP0_IMAGIC((_L("CIEImageProcessingImp::HandleEvents - EFaceCroppingComplete")));
            iObserver.FaceCroppingComplete(KErrNone);
            break;
        case EFaceAddedToExif:
            DP0_IMAGIC((_L("CIEImageProcessingImp::HandleEvents - EFaceAddedToExif")));
            iObserver.FaceCoordinatesAdded(aErrorCode);
            break;
        case EFaceRemovedFromExif:
            DP0_IMAGIC((_L("CIEImageProcessingImp::HandleEvents - EFaceRemovedFromExif")));
            iObserver.FaceCoordinatesRemoved(aErrorCode);
            break;
        case ESingleFaceDetectionComplete:
            DP0_IMAGIC((_L("CIEImageProcessingImp::HandleEvents - ESingleFaceDetectionComplete")));
            iObserver.SingleFaceDetectionComplete(KErrNone);
            break;
        case EFaceDetectionCancelled:
            DP0_IMAGIC((_L("CIEImageProcessingImp::HandleEvents - EFaceDetectionCancelled")));
            iObserver.StartSingleFaceDetection();*/
        default:
            DP0_IMAGIC((_L("CIEImageProcessingImp::HandleEvents - Error in BGPS processing")));
            iObserver.HandleError(aErrorCode);
            break;
        }	
    DP0_IMAGIC((_L("CIEImageProcessingImp::HandleEvents--")));
    } 


void CIEImageProcessingImp::GenerateTN(const TDesC& aMGDir, const TDesC& aTNDir)
    {
	DP0_IMAGIC((_L("CIEImageProcessingImp::GenerateTN ++")));

	iIEBgpClient.GenerateThumbnails(iImagicBGPSAO->iStatus,aMGDir,aTNDir);
	iImagicBGPSAO->ActivateTNCreatorAO();

	DP0_IMAGIC((_L("CIEImageProcessingImp::GenerateTN --")));
    }

void CIEImageProcessingImp::CancelTNGeneration()
    {
    DP0_IMAGIC((_L("CIEImageProcessingImp::CancelTNGeneration ++")));

    iIEBgpClient.CancelTNGeneration();

    DP0_IMAGIC((_L("CIEImageProcessingImp::CancelTNGeneration --")));
    }


void CIEImageProcessingImp::GenerateTN(const TDesC& aMGDir, const TDesC& aTNDir, const TSize &aSize)
    {
	DP0_IMAGIC((_L("CIEImageProcessingImp::GenerateTN ++")));

	iIEBgpClient.GenerateThumbnails(iImagicBGPSAO->iStatus,aMGDir,aTNDir,aSize);
	iImagicBGPSAO->ActivateTNCreatorAO();

	DP0_IMAGIC((_L("CIEImageProcessingImp::GenerateTN --")));
    }

void CIEImageProcessingImp::GenerateTN(const TDesC& aMGDir, const TDesC& aTNDir, const TSize &aSize, CFbsBitmap* aSrcBitmap)
    {
    DP0_IMAGIC((_L("CIEImageProcessingImp::GenerateTN ++")));

    iIEBgpClient.GenerateThumbnails(iImagicBGPSAO->iStatus,aMGDir,aTNDir,aSize,aSrcBitmap);
    iImagicBGPSAO->ActivateTNCreatorAO();

    DP0_IMAGIC((_L("CIEImageProcessingImp::GenerateTN --")));
    }


// EOF
