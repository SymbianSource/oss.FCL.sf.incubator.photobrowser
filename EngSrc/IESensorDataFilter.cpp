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

#include "IESensorDataFilter.h"

CIESensorDataFilter::CIESensorDataFilter()
    {
    // No implementation required
    }

CIESensorDataFilter::~CIESensorDataFilter()
    {
    delete iRingBuffer;
    }

CIESensorDataFilter* CIESensorDataFilter::NewLC()
    {
    CIESensorDataFilter* self = new (ELeave) CIESensorDataFilter();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CIESensorDataFilter* CIESensorDataFilter::NewL()
    {
    CIESensorDataFilter* self = CIESensorDataFilter::NewLC();
    CleanupStack::Pop(); // self;
    return self;
    }

void CIESensorDataFilter::ConstructL()
    {
    iRingBuffer = new (ELeave) TInt[KDataBufferSize];
    
    memset(iRingBuffer, '\0', KDataBufferSize * sizeof (TInt));
    
    iRingBufferPointer = iRingBuffer;
    
    }

TInt CIESensorDataFilter::FilterSensorData(TInt aNewValue)
    {
    // Returns the average of the measures inside the circular buffer avoiding the noise  
    
    *iRingBufferPointer = aNewValue;
    
    iRingBufferPointer++;
    
    if(iRingBufferPointer >= (iRingBuffer + (KDataBufferSize - 1 )))
        {
        iRingBufferPointer = iRingBuffer;
        }
    
    TInt sum = 0;
    for (TInt i = 0; i < KDataBufferSize; i++ )
        {
        sum = sum + iRingBuffer[i];
        }
        
    return (sum / KDataBufferSize);
    }
