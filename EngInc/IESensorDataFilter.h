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

#ifndef IESENSORDATAFILTER_H
#define IESENSORDATAFILTER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
 *  CIESensorDataFilter
 * 
 */
const TInt KDataBufferSize = 16; 

class CIESensorDataFilter : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CIESensorDataFilter();

    /**
     * Two-phased constructor.
     */
    static CIESensorDataFilter* NewL();

    /**
     * Two-phased constructor.
     */
    static CIESensorDataFilter* NewLC();

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CIESensorDataFilter();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();
 
public:

    TInt FilterSensorData(TInt aNewValue);

private:
    
    TInt* iRingBuffer;
    TInt* iRingBufferPointer;
    
    };

#endif // IESENSORDATAFILTER_H
