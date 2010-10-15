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

#include <IEBgpsInfo.h>
#include "debug.h"
#include "IEBgpClient.h"


//const TInt KTimeDelay = 2000000;
//const TInt KTimeDelay = 100000;
const TInt KTimeDelay = 10000;

/**
 *  Create server thread, session.
 *  returns KErrNone in case of no error otherwise
 *  system wide error code.
 */
TInt RIEBgpClient::Connect()
{
	DP0_IMAGIC((_L("RIEBgpClient::Connect ++")));
	TInt error = KErrNone;
	
	// First create the server thread
	DP0_IMAGIC((_L("RIEBgpClient::Connect - Start creating ServerThread")));
	error = CreateServerThread(iServerThread);
	DP0_IMAGIC((_L("RIEBgpClient::Connect - ServerThread created")));
	
	if(error != KErrNone)
		return error;	
	else
	    {
	    //User::After( KTimeDelay );
		error = CreateSession(KIEBgpServerName, Version(), KIEDefaultMsgSlot);
		DP0_IMAGIC((_L("RIEBgpClient::Connect - Create session finished")));
	}		
	DP0_IMAGIC((_L("RIEBgpClient::Connect --")));
	return error;
}

/** 
 * Close the client 
 */
void RIEBgpClient::Disconnect()
{
	DP0_IMAGIC((_L("RIEBgpClient::Disconnect ++")));
	Close();
	DP0_IMAGIC((_L("RIEBgpClient::Disconnect --")));
}

TInt RIEBgpClient::SessionId()
{
	return KErrNone;
}

/** client version*/
TVersion RIEBgpClient::Version() const
{
	DP0_IMAGIC((_L("RIEBgpClient::Version ++")));
	return (TVersion(KIEBgpServerMajorVersion, 
				KIEBgpServerMinorVersion, 
				KIEBgpServerBuildVersion));
	
}
/**
 *  Closes server thread and session.
 */
void RIEBgpClient::Close()
{
	
	DP0_IMAGIC((_L("RIEBgpClient::Close ++")));
	DP0_IMAGIC((_L("RIEBgpClient::Close 1")));
	TInt err = SendReceive(EIECancelThumbnailGeneration);
    DP0_IMAGIC((_L("RIEBgpClient::Close 2")));		
	iServerThread.Close();
	
	RSessionBase::Close();
	
	DP0_IMAGIC((_L("RIEBgpClient::Close --")));
}

void RIEBgpClient::GenerateThumbnails(TRequestStatus &aStatus, const TDesC& aMGDir, const TDesC& aTNDir)
{
	DP0_IMAGIC((_L("RIEBgpClient::GenerateThumbnails ++")));
	
	iJpegFileName.Copy(aMGDir);
	iGalleryFileName.Copy(aTNDir);
	
	TIpcArgs args(&iJpegFileName, &iGalleryFileName);
	
	SendReceive(EIESingleTNGeneration,args,aStatus);
	
	DP0_IMAGIC((_L("RIEBgpClient::GenerateThumbnails --")));
}

void RIEBgpClient::CancelTNGeneration()
{
    DP0_IMAGIC((_L("RIEBgpClient::CancelTNGeneration ++")));
    
    SendReceive(EIECancelThumbnailGeneration);
    
    DP0_IMAGIC((_L("RIEBgpClient::CancelTNGeneration --")));
}

void RIEBgpClient::GenerateThumbnails(TRequestStatus &aStatus, const TDesC& aMGDir, const TDesC& aTNDir, const TSize &aSize)
{
	DP0_IMAGIC((_L("RIEBgpClient::GenerateThumbnails ++")));
	
	iJpegFileName.Copy(aMGDir);
	iGalleryFileName.Copy(aTNDir);
	iSize = aSize;
	
	TIpcArgs args(&iJpegFileName, &iGalleryFileName, &iSize);
	
	SendReceive(EIESingleTNGeneration,args,aStatus);
	
	DP0_IMAGIC((_L("RIEBgpClient::GenerateThumbnails --")));
}

void RIEBgpClient::GenerateThumbnails(TRequestStatus &aStatus, const TDesC& aMGDir, const TDesC& aTNDir, 
                                      const TSize &aSize, CFbsBitmap* aSrcBitmap)
{
    DP0_IMAGIC((_L("RIEBgpClient::GenerateThumbnails ++")));
    
    iJpegFileName.Copy(aMGDir);
    iGalleryFileName.Copy(aTNDir);
    iSize = aSize;
    
    TIpcArgs args(&iJpegFileName, &iGalleryFileName, &iSize, &aSrcBitmap);
    
    SendReceive(EIESingleTNGenerationWithBitmap,args,aStatus);
    
    DP0_IMAGIC((_L("RIEBgpClient::GenerateThumbnails --")));
}

// EOF
