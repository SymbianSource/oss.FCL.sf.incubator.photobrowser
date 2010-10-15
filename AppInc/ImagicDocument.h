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

#ifndef IMAGICDOCUMENT_H
#define IMAGICDOCUMENT_H

// INCLUDES
#include <akndoc.h>
   
// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CImagicDocument application class.
*/
class CImagicDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CImagicDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        virtual ~CImagicDocument();

    private:

        /**
        * EPOC default constructor.
        */
        CImagicDocument(CEikApplication& aApp);
        void ConstructL();

    private:

        /**
        * From CEikDocument, create CImagicAppUi "App UI" object.
        */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

