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

#ifndef __IMAGEMONITORAO_H__
#define __IMAGEMONITORAO_H__

class CIEEngineImp;

class CImageMonitorAO : public CActive
    {
    public:
        // Constructor and destructor
        static CImageMonitorAO* NewL(CIEEngineImp* aEngImp);
        static CImageMonitorAO* NewLC(CIEEngineImp* aEngImp);
        void ConstructL();
        CImageMonitorAO(CIEEngineImp* aEngImp);
        ~CImageMonitorAO();
        void ActiveRequest();
       
    private:
        // From CActive
        void RunL();
        void DoCancel();
        void RunError();
        
    private: // Data
        CIEEngineImp*          iEngImp;

        
    };

#endif //__IMAGEMONITORAO_H__
