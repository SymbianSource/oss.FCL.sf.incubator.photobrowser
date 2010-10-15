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

#ifndef __IEBGPSERVER_H__
#define __IEBGPSERVER_H__

// Include files
#include <e32base.h>
#include <f32file.h>
#include "debug.h"

enum TIDLServerPanic
{
	EBadRequest = 1,
	EBadDescriptor,
	EMainSchedulerError,
	ESrvCreateError,
	ESvrStartError,
	ETrapCleanupError,
	EGeneralError
};

class CIEBgpServer : public CServer2
{
public:
	static 	CIEBgpServer* NewL();
	~CIEBgpServer();

private:
	void ConstructL();
	CIEBgpServer();

public: // From CServer2
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	
public: // New functions
	static TInt ThreadFunction(TAny* aParam);
	static void PanicServer(TIDLServerPanic aPanic);
	
private: // other private functions
	static void StartServerL();	
	
private: // Data members
    RFs    iFileServer;
		
};

#endif // __IEBGPSERVER_H__
