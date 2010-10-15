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
#include <e32cons.h>

#include <string.h>
#include <stdlib.h>

#include "IEBgpsInfo.h"
#include "IEImageData.h"
//#include <IEBGPSTrace.h>

#include "IEBgpServerSession.h"
#include "IETNGenerator.h"

// --------------------------- MEMBER FUNCTIONS ---------------------------- //

CIEBgpServerSession* CIEBgpServerSession::NewL(RFs* aFileServer)
{
	DP0_IMAGIC((_L("CIEBgpServerSession::NewL ++")));
	CIEBgpServerSession* self = new (ELeave) CIEBgpServerSession();
	CleanupStack::PushL(self);
	self->ConstructL(aFileServer);
	CleanupStack::Pop();
	DP0_IMAGIC((_L("<CIEBgpServerSession::NewL --")));
	return self;
}

CIEBgpServerSession::~CIEBgpServerSession()
{
	DP0_IMAGIC((_L("CIEBgpServerSession::~CIEBgpServerSession ++")));
 
 	if(iIETNGeneratorAO)
 	{
 		delete iIETNGeneratorAO;
 		iIETNGeneratorAO = NULL;
 	}
 	
 	if(iFaceBrowser)
 	{
 	    delete iFaceBrowser;
 	    iFaceBrowser = NULL;
 	}
	DP0_IMAGIC((_L("<CIEBgpServerSession::~CIEBgpServerSession --")));
}

CIEBgpServerSession::CIEBgpServerSession()
{	
	DP0_IMAGIC((_L("CIEBgpServerSession::CIEBgpServerSession ++")));
	DP0_IMAGIC((_L("CIEBgpServerSession::CIEBgpServerSession --")));
}

void CIEBgpServerSession::ConstructL(RFs* aFileServer)
    {
	DP0_IMAGIC((_L("CIEBgpServerSession::ConstructL ++")));
/*	
	TInt initError = KErrNone;
	if(initError != KErrNone)
	{
	DP0_IMAGIC((_L("CIEBgpServerSession::ConstructL- IDL Engine Creation Failed")));
	User::Leave(initError);
	}
	
*/	
	iResolutionSize.iHeight = 512;
	iResolutionSize.iWidth = 512;
	
	iIETNGeneratorAO = CIETNGeneratorAO::NewL(*aFileServer, *this);
	
	iFaceBrowser = CFaceBrowser::NewLC(*aFileServer, *this);
	
	DP0_IMAGIC((_L("CIEBgpServerSession::ConstructL --")));
    }

void CIEBgpServerSession::ServiceL(const RMessage2& aMessage)
    {
	DP0_IMAGIC((_L("CIEBgpServerSession::ServiceL++")));

	TInt error = KErrNone;
	
	switch(aMessage.Function())
    	{
    		case EIESrvTest:
        		{
                TInt value = aMessage.Int0();
                aMessage.Complete(error);
                break;
        		}
    		case EIESrvCloseSession:
        		{
        		DP0_IMAGIC((_L("CIEBgpServerSession::ServiceL - EIESrvCloseSession")));
                break;
        		}
    	
    		case EIEStartProcessing:
        		{     
        		DP0_IMAGIC((_L("CIEBgpServerSession::ServiceL - EIEStartProcessing")));
                break;
        		}
    		
    		case EIEThumbnailGeneration:
        		{
        		DP0_IMAGIC((_L("CIEBgpServerSession::ServiceL - EIEThumbnailGeneration")));
        		iMessage = aMessage;
        		iImageArray = *(RArray<CImageData*>*)aMessage.Ptr0();
        		TInt count = iImageArray.Count();
        		iIETNGeneratorAO->SetImageArray(iImageArray);
        		
        		iCount = 0;
         		TFileName jpegFileName;
         		TFileName imagicThumbFileName;
                iImageArray[0]->GetFileName(jpegFileName, EFullSize);         		
         		iImageArray[0]->GetFileName(imagicThumbFileName, ESize512x512);
        	
        		iIETNGeneratorAO->CreateThumbnailL(jpegFileName, imagicThumbFileName, iResolutionSize);
        		break;	
        		}
    	
    		case EIESingleTNGeneration:
        		{
        		DP0_IMAGIC((_L("CIEBgpServerSession::ServiceL - EIESingleTNGeneration")));
        		iSingleTNGeneration = ETrue;
        		iMessage = aMessage;
        	
        		TFileName jpegFileName;
        	 	TFileName imagicThumbFileName;
        	
        		error  =  aMessage.Read(0, iJpegFileName, 0);
        		error  =  aMessage.Read(1, iImagicThumbFileName, 0);
        		
        	 	iResolutionSize = *(TSize*)aMessage.Ptr2();
        		if(error  == KErrNone)
        			{
        			iIETNGeneratorAO->CreateThumbnailL(iJpegFileName,iImagicThumbFileName,iResolutionSize);	
        			}
        		else
        			{
        			iSingleTNGeneration	= EFalse;
        			iMessage.Complete(error);	
        			}
                break;
        		}
        		
    		case EIESingleTNGenerationWithBitmap:
                {
                DP0_IMAGIC((_L("CIEBgpServerSession::ServiceL - EIESingleTNGenerationWithBitmap")));
                iSingleTNGeneration = ETrue;
                iMessage = aMessage;
            
                TFileName jpegFileName;
                TFileName imagicThumbFileName;
            
                error  =  aMessage.Read(0, iJpegFileName, 0);
                error  =  aMessage.Read(1, iImagicThumbFileName, 0);
                
                iResolutionSize = *(TSize*)aMessage.Ptr2();
                iSrcBitmap = (CFbsBitmap*)aMessage.Ptr3();
                
                if(error  == KErrNone)
                    {
                    iIETNGeneratorAO->CreateThumbnailL(iJpegFileName,iImagicThumbFileName,iResolutionSize); 
                    }
                else
                    {
                    iSingleTNGeneration = EFalse;
                    iMessage.Complete(error);   
                    }
                break;
                }
        		
    		case EIECancelThumbnailGeneration:
        		{
        		DP0_IMAGIC((_L("CIEBgpServerSession::ServiceL - EIECancelThumbnailGeneration")));
        		iIETNGeneratorAO->CancelRequestsAndDeleteObjects();
        		//Complete current "Cancel" message
                aMessage.Complete(KErrNone);
        		break;
        		}   		
    	
    		default:
    			break;
    	}
	
	DP0_IMAGIC((_L("CIEBgpServerSession::ServiceL --")));	
    }

void CIEBgpServerSession::ThumbNailGenerationCompleted(TInt aError)
    {
    DP0_IMAGIC((_L("CIEBgpServerSession::ThumbNailGenerationCompleted++")));
    
	if(aError != KErrNone)
	    {
	    if(aError == KErrCancel)
	        iMessage.Complete(ETNGenerationCancelled);
	    else
	        iMessage.Complete(aError);   
	    }
	if(aError == KErrNone)
	    {
    	if(iSingleTNGeneration)
    		{
    		iSingleTNGeneration	= EFalse;
    		iMessage.Complete(ETNGenerationcomplete);	
    		}
    	else
    		{
    		iCount++;
    
    		if(iCount > iImageArray.Count()-1 )
    		    {
    			iMessage.Complete(ETNGenerationcomplete);	
    		    }
    		else
    		    {
    	        TFileName jpegFileName; 
    	        TFileName imagicThumbFileName;
    			iImageArray[0]->GetFileName(jpegFileName, EFullSize);
                iImageArray[0]->GetFileName(imagicThumbFileName, ESize512x512);    			
    			iIETNGeneratorAO->CreateThumbnailL(jpegFileName, imagicThumbFileName, iResolutionSize);		
    		    }
    		}
	    }
	DP0_IMAGIC((_L("CIEBgpServerSession::ThumbNailGenerationCompleted --")));
    }

void CIEBgpServerSession::FaceBrowsingComplete()
    {
    DP0_IMAGIC((_L("CIEBgpServerSession::FaceBrowsingComplete")));
    iMessage.Complete(EFaceDetectionComplete);
    }

void CIEBgpServerSession::FaceBrowsingError(TInt aError)
    {
    DP0_IMAGIC((_L("CIEBgpServerSession::FaceBrowsingError")));
    iMessage.Complete(aError);
    }

void CIEBgpServerSession::FaceCroppingError(TInt aError)
    {
    DP0_IMAGIC((_L("CIEBgpServerSession::FaceCroppingError")));
    iMessage.Complete(aError);
    }

void CIEBgpServerSession::FaceSingleFaceBrowsingComplete()
    {
    DP0_IMAGIC((_L("CIEBgpServerSession::FaceSingleFaceBrowsingComplete")));
    iMessage.Complete(ESingleFaceDetectionComplete);
    }

// EOF
