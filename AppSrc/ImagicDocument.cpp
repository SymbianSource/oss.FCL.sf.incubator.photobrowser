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
#include "ImagicDocument.h"
#include "ImagicAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CImagicDocument::CImagicDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    
    }

// destructor
CImagicDocument::~CImagicDocument()
    {
    
    }

// EPOC default constructor can leave.
void CImagicDocument::ConstructL()
    {
    
    }

// Two-phased constructor.
CImagicDocument* CImagicDocument::NewL(CEikApplication& aApp)     // CImagicApp reference
    {
 
    CImagicDocument* self = new (ELeave) CImagicDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ----------------------------------------------------
// CImagicDocument::CreateAppUiL()
// constructs CImagicAppUi
// ----------------------------------------------------
//
CEikAppUi* CImagicDocument::CreateAppUiL()
    {
   
  
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return ( static_cast <CEikAppUi*> ( new ( ELeave ) CImagicAppUi ) );
    
    }

// End of File  
