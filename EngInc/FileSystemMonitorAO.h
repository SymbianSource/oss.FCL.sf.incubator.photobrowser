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

#ifndef __FILESYSTEMMONITORAO_H__
#define __FILESYSTEMMONITORAO_H__

#include <e32base.h>
#include <f32file.h>
//#include "IEFileLoader.h"
#include "ImagicConsts.h"
#include "IEImageFinder.h"
//#include "IEEngineImp.h"

class CIEImageFinder;

class CFileSystemMonitorAO : public CActive
    {
    public:
        // Constructor and destructor
        static CFileSystemMonitorAO* NewL(RFs& aFileServer, CIEImageFinder* aImageFinder);
        static CFileSystemMonitorAO* NewLC(RFs& aFileServer, CIEImageFinder* aImageFinder);
        void ConstructL();
        CFileSystemMonitorAO(RFs& aFileServer, CIEImageFinder* aImageFinder);
        ~CFileSystemMonitorAO();
        void StartMonitoring();
        void StopMonitoring();
       
    private:
        // From CActive
        void RunL();
        void DoCancel();
        void RunError();
        
    private: // Data
        
        CIEImageFinder*        iImageFinder;
        RFs&                   iFileServer;

        
    };

#endif //__FILESYSTEMMONITORAO_H__
