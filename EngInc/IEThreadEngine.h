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

#ifndef __FILEFINDERTHREAD_H__
#define __FILEFINDERTHREAD_H__

// INCLUDES
#include <e32base.h> 
#include <badesca.h> 
#include <IEBgpsInfo.h>
#include <PathInfo.h>
#include <BAUTILS.H>

// FORWARD DECLARATIONS
class IEImageFinderAO;
class CImageData;
class CIEFileLoader;


// CLASS DECLARATION

class CMediator: public CBase
{
public:
    RArray<CImageData*>*    iFileNameData;
    RArray<CImageData*>*    iFaceFileNameData;
    TBool*                  iAll128x128TNsDone;
    TBool*                  iAll640x480TNsDone;
    TBool*                  iAll320x320TNsDone;
    CIEFileLoader*          iFileLoader;
    RCriticalSection*       iCritical;
    TFileName                  iFileName;
};


/**
*  CFileFinderThread application engine class.
*/
class CFileFinderThread: public CBase
	{
public: 

	static CFileFinderThread* NewL(CIEFileLoader* aFileLoader, 
                                    RArray<CImageData*>& aFileNameData, 
                                    RArray<CImageData*>& aFaceFileNameData, 
                                    RCriticalSection* aCritical, TDesC& aFileName);
	
	static CFileFinderThread* NewLC(CIEFileLoader* aFileLoader, 
                                    RArray<CImageData*>& aFileNameData, 
                                    RArray<CImageData*>& aFaceFileNameData,  
	                                RCriticalSection* aCritical, TDesC& aFileName);
	
	CFileFinderThread(CIEFileLoader* aFileLoader, 
                        RArray<CImageData*>& aFileNameData, 
                        RArray<CImageData*>& aFaceFileNameData, 
	                    RCriticalSection* aCritical, TDesC& aFileName);
	
	~CFileFinderThread(void);

	void StartL();
	void Stop();
	static TInt ExecuteThreadOne(TAny *aPtr);
	static void CreateFileFinderL(CMediator* aMediator);

private: //functions
	void CreateThreadsL();

private: // Basic two-phase Symbian OS constructors
	void ConstructL();
	CFileFinderThread();
	
private: // data members       
	
	// a handle for thread1
	RThread                iThreadOne;
	TBool                  iCreatedThreads;
	CMediator*             iSMediator;
	RArray<CImageData*>&   iFileNameData;
	RArray<CImageData*>&   iFaceFileNameData;
	CIEFileLoader*         iFileLoader;
	RCriticalSection*      iCritical;
	TBufC<KMaxFileName>    iRootPath;
    TBufC<KMaxFileName>    iSearchName;
    TFileName              iFilename;
    };

#endif // __FILEFINDERTHREAD_H__
