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

// INCLUDES
#include <e32base.h> 
#include <aknnotewrappers.h>

#include "IEThreadEngine.h"
#include "IEImageFinder.h"
#include "ImagicConsts.h"


// ----------------------------------------------------------------------------
// CFileFinderThread::CFileFinderThread(CSharedIntermediator* aSMediator)
//
// Constructor.
// ----------------------------------------------------------------------------
CFileFinderThread::CFileFinderThread(
        CIEFileLoader* aFileLoader, 
        RArray<CImageData*>& aFileNameData, 
        RArray<CImageData*>& aFaceFileNameData,
        RCriticalSection* aCritical,
        TDesC& aFileName) :
    iCreatedThreads(EFalse),
    iFileLoader(aFileLoader),
    iFileNameData(aFileNameData),
    iFaceFileNameData(aFaceFileNameData),
    iCritical(aCritical)
    {
	iFilename.Copy(aFileName);
	}

// ----------------------------------------------------------------------------
// CFileFinderThread::~CFileFinderThread(void)
//
// Destructor.
// ----------------------------------------------------------------------------
CFileFinderThread::~CFileFinderThread(void)
	{
	DP0_IMAGIC(_L("CFileFinderThread::~CFileFinderThread++"));
	
	if(iSMediator)
	{
		delete iSMediator;
		iSMediator = NULL;
	}
	
	// Thread should be killed allready, if thread is already killed this does nothing
	iThreadOne.Kill(KErrNone);
	
	// Handles should be closed
	iThreadOne.Close();
	DP0_IMAGIC(_L("CFileFinderThread::~CFileFinderThread--"));
	}

CFileFinderThread* CFileFinderThread::NewL(
        CIEFileLoader* aFileLoader,  
        RArray<CImageData*>& aFileNameData, 
        RArray<CImageData*>& aFaceFileNameData,
        RCriticalSection* aCritical,
        TDesC& aFileName)
	{
	CFileFinderThread* self = CFileFinderThread::NewLC(
	        aFileLoader, 
	        aFileNameData, 
	        aFaceFileNameData,
	        aCritical,
	        aFileName);
	CleanupStack::Pop(self);
	return self;
	}

CFileFinderThread* CFileFinderThread::NewLC(
        CIEFileLoader* aFileLoader,  
        RArray<CImageData*>& aFileNameData,
        RArray<CImageData*>& aFaceFileNameData,
        RCriticalSection* aCritical,
        TDesC& aFileName)
	{
	CFileFinderThread* self = new (ELeave) CFileFinderThread(
	        aFileLoader, 
	        aFileNameData,
	        aFaceFileNameData,
	        aCritical,
	        aFileName);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// Standard Symbian OS 2nd phase constructor
void CFileFinderThread::ConstructL()
	{
	DP0_IMAGIC(_L("CFileFinderThread::ConstructL ++ --"));
	}

void CFileFinderThread::StartL()
	{
	DP0_IMAGIC(_L("CFileFinderThread::Start++"));
	// Create threads only once
	if ( iCreatedThreads == EFalse )
		{
		CreateThreadsL();
		}
	DP0_IMAGIC(_L("CFileFinderThread::Start--"));
	}


void CFileFinderThread::Stop()
	{
	//TRequestStatus aStatus;
	//iThreadOne.Logon(aStatus);
	//iThreadOne.Suspend();
	}

// ----------------------------------------------------------------------------
// CFileFinderThread::ExecuteThread(TAny *aPtr)
//
// Threadfunction of threadOne. Executed only by threadOne.
// ----------------------------------------------------------------------------
TInt CFileFinderThread::ExecuteThreadOne(TAny *aPtr)
	{
	DP0_IMAGIC(_L("CFileFinderThread::ExecuteThreadOne++"));
	
	CMediator* aSMediator = static_cast<CMediator*>( aPtr );
	
	//Create cleanupstack	
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	
	//Test cleanup stack, additional cleanup stack must be prosessed under
	//TRAP
    //We can't use low level cleanup stack handling
	TRAPD(error, CFileFinderThread::CreateFileFinderL(aSMediator));

	delete cleanupStack;
	
	DP0_IMAGIC(_L("CFileFinderThread::ExecuteThreadOne--"));
	
	return 0;
	}


// ----------------------------------------------------------------------------
// CFileFinderThread::CreateActiveScheduler(CSharedIntermediator* aSMediator)
//
// Create ActiveScheduler for thread1.
// ----------------------------------------------------------------------------
void CFileFinderThread::CreateFileFinderL(CMediator* aSMediator)
	{
	DP0_IMAGIC(_L("CFileFinderThread::CreateFileFinderL++"));
	// create a new active scheduler
	CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(activeScheduler);

	// use static function Install to install previously created scheduler
	CActiveScheduler::Install(activeScheduler);
	
	//Create and use iTNImageFinder to handle in backround AO iFileNameData array filling
	CIEImageFinder* TNImageFinder = new (ELeave) CIEImageFinder(
	        aSMediator->iFileLoader, 
            *aSMediator->iFileNameData,
            *aSMediator->iFaceFileNameData,
            aSMediator->iCritical);
    
	CleanupStack::PushL(TNImageFinder);
	TNImageFinder->ConstructL();
	TNImageFinder->StartFinderL(KFileString);

	DP0_IMAGIC(_L("CFileFinderThread::CreateFileFinderL finder ended"));
	aSMediator->iFileLoader->ImageFinderStopped();
	aSMediator->iFileLoader->AllFilesAddedToFilenameArray();
	
	// Start active scheduler
    //CActiveScheduler::Start();
    
    // Remove and delete scheduler and the rest.
	CleanupStack::PopAndDestroy(2);
	DP0_IMAGIC(_L("CFileFinderThread::CreateFileFinderL--"));
	}


// ----------------------------------------------------------------------------
// CFileFinderThread::CreateThreadsL()
//
// Create thread1 and resume it. Activate thread1 listener.
// ----------------------------------------------------------------------------
void CFileFinderThread::CreateThreadsL()
	{
	DP0_IMAGIC(_L("CFileFinderThread::CreateThreadsL++"));
	iSMediator = new (ELeave) CMediator();
	
	iSMediator->iFileLoader = iFileLoader;
    iSMediator->iFileNameData = &iFileNameData;
    iSMediator->iFaceFileNameData = &iFaceFileNameData;
    iSMediator->iCritical = iCritical;
    iSMediator->iFileName = iFilename;
    
	// Create thread which uses the same heap as main program.
	iThreadOne.Create(_L("FileFinderThread"), ExecuteThreadOne, 12040, NULL, iSMediator);
	//EPriorityNormal, EPriorityLess, EPriorityMuchLess
	iThreadOne.SetPriority(EPriorityMuchLess);
	iThreadOne.Resume();
	
	iCreatedThreads = ETrue;
	DP0_IMAGIC(_L("CFileFinderThread::CreateThreadsL--"));
	}

