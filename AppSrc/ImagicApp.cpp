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


// INCLUDE FILES
#include    "ImagicApp.h"
#include    "ImagicDocument.h"

 

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CImagicApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CImagicApp::AppDllUid() const
    {
    return KUidImagic;
    }

   
// ---------------------------------------------------------
// CImagicApp::CreateDocumentL()
// Creates CImagicDocument object
// ---------------------------------------------------------
//
CApaDocument* CImagicApp::CreateDocumentL()
    {
 
       return (static_cast<CApaDocument*>
                       ( CImagicDocument::NewL( *this ) ) );
    }




    

// End of File  

