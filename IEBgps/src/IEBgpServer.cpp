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

#include <IEBgpsInfo.h>
//#include <IEBGPSTrace.h>

#include "IEBgpServer.h"
#include "IEBgpServerSession.h"


// --------------------------- MEMBER FUNCTIONS ---------------------------- //

// ----------------------------------------------------------------------------
//
//
// ----------------------------------------------------------------------------
CIEBgpServer* CIEBgpServer::NewL()
{
	DP0_IMAGIC((_L("CIEBgpServer::NewL ++")));
	CIEBgpServer* self = new (ELeave) CIEBgpServer();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	DP0_IMAGIC((_L("CIEBgpServer::NewL --")));
	return self;
}

CIEBgpServer::~CIEBgpServer()
{
DP0_IMAGIC((_L("CIEBgpServer::~CIEBgpServer")));

iFileServer.Close();
RFbsSession::Disconnect();

DP0_IMAGIC((_L("CIEBgpServer::~CIEBgpServer")));	
}

//EPriorityIdle, EPriorityLow, EPriorityStandard, EPriorityUserInput, EPriorityHigh
CIEBgpServer::CIEBgpServer()
:CServer2(EPriorityIdle)
{	
}

void CIEBgpServer::ConstructL()
{
	DP0_IMAGIC((_L("CIEBgpServer::ConstructL ++")));
	StartL(KIEBgpServerName);
	
	User::LeaveIfError(iFileServer.Connect());
    User::LeaveIfError( FbsStartup() );
    User::LeaveIfError(RFbsSession::Connect());
	
	DP0_IMAGIC((_L("CIEBgpServer::ConstructL --")));
}

CSession2* CIEBgpServer::NewSessionL(const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/) const
{
	DP0_IMAGIC((_L("CIEBgpServer::NewSessionL ++")));
	CIEBgpServerSession* session = CIEBgpServerSession::NewL(const_cast<RFs*>(&iFileServer));
	DP0_IMAGIC((_L("CIEBgpServer::NewSessionL --")));
	return session;
}

// ----------------------------------------------------------------------------
// Thread function to start the server
//
// ----------------------------------------------------------------------------
GLDEF_C TInt CIEBgpServer::ThreadFunction(TAny* /*aParam*/)
{
	DP0_IMAGIC((_L("CIEBgpServer::ThreadFunction ++")));
	// First get the cleanup stack
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	
	TRAPD(error, StartServerL());
	
	delete cleanupStack;
	
	DP0_IMAGIC((_L("CIEBgpServer::ThreadFunction --")));
	
	return error;
}

// ----------------------------------------------------------------------------
// Starts the server
//
// ----------------------------------------------------------------------------
void CIEBgpServer::StartServerL()
{
	DP0_IMAGIC((_L("CIEBgpServer::StartServerL ++")));
	// Create an active scheduler before server is created
	CActiveScheduler* as = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(as);
	CActiveScheduler::Install(as);
	
	// Create server
	CIEBgpServer* server = CIEBgpServer::NewL();
	CleanupStack::PushL(server);
	
	RThread::Rendezvous(KErrNone);
	
	// Start active scheduler
	CActiveScheduler::Start();
	
	// Clean up
	CleanupStack::PopAndDestroy(2, as);
	
	DP0_IMAGIC((_L("CIEBgpServer::StartServerL --")));
}

// ----------------------------------------------------------------------------
// Creates the server thread
//
// ----------------------------------------------------------------------------
EXPORT_C TInt CreateServerThread(RThread& aThread)
{
	DP0_IMAGIC((_L("CIEBgpServer::CreateServerThread ++")));
	TInt error = KErrNone;
	
	// Check if the server already exists
	TFindServer findServer(KIEBgpServerName);
	TFileName matchingServer;
	
	if(findServer.Next(matchingServer) != KErrNone)
	{
		error = aThread.Create(KIEBgpServerName, // Server name
							CIEBgpServer::ThreadFunction, // thread function to call when thread is created and resumed
							KDefaultStackSize, // stack size
							KIEHeapSizeMin, // min heap size
							KIEHeapSizeMax, // max heap size
							NULL); // data ptr needed (if)

		if(error == KErrNone)
		{
			// Thread created successfully, resume it
			TRequestStatus rendezvousStatus;
			
			//Keep priority low to enable smooth UI drawing
			//aThread.SetPriority(EPriorityNormal);
			aThread.SetPriority(EPriorityLess);//EPriorityNormal, EPriorityLess, EPriorityMuchLess, EPriorityNull
			aThread.Rendezvous(rendezvousStatus);
			aThread.Resume();
			User::WaitForRequest(rendezvousStatus);
		}
		else
		{
			// error in thread creation
			aThread.Close();
		}
	}
	else
	{
		error = KErrAlreadyExists;
	}
	DP0_IMAGIC((_L("CIEBgpServer::CreateServerThread --")));
	return error;
}

// EOF
