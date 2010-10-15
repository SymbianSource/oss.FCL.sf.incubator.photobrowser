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

#ifndef DRAWMAGGLASS_H
#define DRAWMAGGLASS_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "ImagicContainerBrowser.h"
#include "CDrawOneByOne.h"

#define VERTICES_PER_LINE 32

// CLASS DECLARATION
class CDrawOneByOne;


/**
 *  CDrawMagGlass
 * 
 */
class CDrawMagGlass : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CDrawMagGlass();

    /**
     * Two-phased constructor.
     */
    static CDrawMagGlass* NewL(CImagicContainerBrowser* aContainer, CDrawOneByOne* aDrawOneByOne);

    /**
     * Two-phased constructor.
     */
    static CDrawMagGlass* NewLC(CImagicContainerBrowser* aContainer, CDrawOneByOne* aDrawOneByOne);

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CDrawMagGlass();

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL(CImagicContainerBrowser* aContainer, CDrawOneByOne* aDrawOneByOne);
    void Interpolate(float &aValue, const float aTarget, const float aStep);
    
public:
    void InitDrawMagGlass();
    void DrawMagGlass(const TSize &aScreenPhysicalSize, TReal aImageAspectRatio);
    TReal GetMagGlassZoomFactor();
    
private:
    CImagicContainerBrowser* iContainer;
    CDrawOneByOne*           iDrawOneByOne;
    
    float                    iMagGlassZoomFactor;
    
    
    static const GLfixed iGlobalTexCoords[4*2];
    static GLfixed       iMagGlassVertices[VERTICES_PER_LINE*VERTICES_PER_LINE*3];
    static GLfixed       iMagGlassTex[VERTICES_PER_LINE*VERTICES_PER_LINE*2];
           
    static const TInt    iMagGlassTriCount;
    static GLushort      iMagGlassIndices[];
    };

#endif // DRAWMAGGLASS_H
