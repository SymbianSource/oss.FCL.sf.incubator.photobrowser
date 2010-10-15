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

#ifndef __IEBGPCLIENT_H__
#define __IEBGPCLIENT_H__

// Include files
#include <e32base.h>
#include <IEImageProcessing.h>
#include <IEBgpsInfo.h>

/**
 * EIGBPS client 
 * 
 */
class RIEBgpClient : public RSessionBase
{
public:
	/**
	Connects to the server, creates a session.
	
	@return - it returns KErrNone if connection is successful, else error code
	*/	
	TInt Connect();
	
	/**
	Disconnects from the server.
	*/
	void Disconnect();
	
	/**
	Gets the session ID.
	
	@return - it returns the session id with the server.
	*/
	TInt SessionId();
	
	/**
	Gets the version of the server
	
	@return - it returns the version of the server
	*/
	TVersion Version() const;
	
	/**
	Closes the session with the server. Destroy kernel side object also.
	*/
	void Close();
	
	/**
     * Overloaded function generates Thumbnails. 
     *  
     * @Param aMGDir - Jpeg File with absolute path.
     * @aImageArrary aTNDir - Thumbnail file name with abolutepath
     */ 
	void GenerateThumbnails(TRequestStatus &aStatus, const TDesC& aMGDir,  const TDesC& aTNDir);
	
	/**
     * Overloaded function generates Thumbnails. 
     * 
     * @Param aMGDir - Jpeg File with absolute path.
     * @param aImageArrary aTNDir - Thumbnail file name with abolutepath
     * @Param aSize - Thumbnail size.
     */ 
	void GenerateThumbnails(TRequestStatus &aStatus, const TDesC& aMGDir, const TDesC& aTNDir, const TSize &aSize);
	void GenerateThumbnails(TRequestStatus &aStatus, const TDesC& aMGDir, const TDesC& aTNDir, const TSize &aSize, CFbsBitmap* aSrcBitmap);
	void CancelTNGeneration();
	
private: 
    /** Server Thread **/
	RThread iServerThread;	
	
	/** Jpeg file name **/
	TFileName iJpegFileName;
	
	/** Gallery file name **/
	TFileName iGalleryFileName;
	/** Thumbmail size**/
	TSize iSize;
	
	TFileName iTempFileName;
};

#endif // __IEBGPCLIENT_H__
