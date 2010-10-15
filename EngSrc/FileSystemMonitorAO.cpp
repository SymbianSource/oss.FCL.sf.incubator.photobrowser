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

#include "FileSystemMonitorAO.h"

    
CFileSystemMonitorAO* CFileSystemMonitorAO::NewL(RFs& aFileServer, CIEImageFinder* aImageFinder)
    {
    CFileSystemMonitorAO* self = new(ELeave) CFileSystemMonitorAO(aFileServer, aImageFinder);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CFileSystemMonitorAO* CFileSystemMonitorAO::NewLC(RFs& aFileServer, CIEImageFinder* aImageFinder)
    {
    CFileSystemMonitorAO* self = new(ELeave) CFileSystemMonitorAO(aFileServer, aImageFinder);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

 
CFileSystemMonitorAO::CFileSystemMonitorAO(RFs& aFileServer, CIEImageFinder* aImageFinder) :
        CActive(CActive::EPriorityLow),
        iFileServer(aFileServer),
        iImageFinder(aImageFinder)
    {
    CActiveScheduler::Add(this);
    }

void CFileSystemMonitorAO::ConstructL()
    {
    //StartMonitoring();
    }


CFileSystemMonitorAO::~CFileSystemMonitorAO()
    {
    Cancel();
    }
 
void CFileSystemMonitorAO::RunL()
    {
    iImageFinder->FileSystemChanged();
    }

 
void CFileSystemMonitorAO::DoCancel()
    {
    if(IsActive())
        Cancel();
    
    iFileServer.NotifyChangeCancel(iStatus);
    }

 
void CFileSystemMonitorAO::RunError()
    {
    // Nothing here
    }

//This name should be changed to IssueActiveRequest()
//This is more meaningful...
void CFileSystemMonitorAO::StartMonitoring()
    {//ENotifyEntry, ENotifyFile
#ifdef __WINS__
    iFileServer.NotifyChange(ENotifyEntry, iStatus, KRootImagePath);
#else
    TFileName rootPath = PathInfo::MemoryCardRootPath();
    rootPath.Append(ImagePath);
    iFileServer.NotifyChange(ENotifyFile, iStatus, rootPath);
#endif
    
    SetActive();
    }

void CFileSystemMonitorAO::StopMonitoring()
    {
    iFileServer.NotifyChangeCancel();    
    }

 
