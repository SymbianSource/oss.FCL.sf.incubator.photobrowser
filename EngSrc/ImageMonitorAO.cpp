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

#include "IEEngineImp.h"
#include "ImageMonitorAO.h"

// Here Engine pointer is not required 
// we will define one internal observer class between Engine and ImageMonitor thread
// 
    
CImageMonitorAO* CImageMonitorAO::NewL(CIEEngineImp* aEngImp)
{
CImageMonitorAO* self = new(ELeave) CImageMonitorAO(aEngImp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

CImageMonitorAO* CImageMonitorAO::NewLC(CIEEngineImp* aEngImp)
    {
    CImageMonitorAO* self = new(ELeave) CImageMonitorAO(aEngImp);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

 
CImageMonitorAO::CImageMonitorAO(CIEEngineImp* aEngImp) :
        CActive(CActive::EPriorityLow),
        iEngImp(aEngImp)
        
    {
    CActiveScheduler::Add(this);
    }

void CImageMonitorAO::ConstructL()
    {
        
    }


CImageMonitorAO::~CImageMonitorAO()
    {
    Cancel();
    }
 
void CImageMonitorAO::RunL()
    {
    iEngImp->AllFilesAddedToFilenameArrayL();
    }

 
void CImageMonitorAO::DoCancel()
    {
    }

 
void CImageMonitorAO::RunError()
    {
    // Nothing here
    }

//This name should be changed to IssueActiveRequest()
//This is more meaningful...
void CImageMonitorAO::ActiveRequest()
    {
    if(!IsActive())
        SetActive();
    }

 
