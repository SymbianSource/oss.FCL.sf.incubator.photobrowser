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

#ifndef DRAWFACEBROWSING_H
#define DRAWFACEBROWSING_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "ImagicContainerBrowser.h"


// CLASS DECLARATION

/**
 *  CDrawFaceBrowsing
 * 
 */
class CDrawFaceBrowsing : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CDrawFaceBrowsing();

    /**
     * Two-phased constructor.
     */
    static CDrawFaceBrowsing* NewL(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex);

    /**
     * Two-phased constructor.
     */
    static CDrawFaceBrowsing* NewLC(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex);
    
    void InitFaceBrowsing();
    void DrawFaceBrowsing(const TSize &aSize);
    void DrawFaceFrame(TInt aFace2bDrawn);
    TBool IsDrawingNeededFaceBrowsing();
    FloatCoords ConvertCoordsFromAlgo2OGl(const TInt aFaceIndex);
    void GetFaceCoordinatesL(TRect& aRect, TFileName& aFilename);
    void SetCurrentFaceNro(TInt aNro);
    TInt GetCurrentFaceNro();
    TInt GetNumberOfFaces();
    void SetFaceCoords(RArray<TRect>& aCoordinates);
    void ClearFaceArray();
    TInt GetFaceCount();
    void IncFaceNumber();
    void DecFaceNumber();
    void KeyPressed();
    void KeyReleased();
    void KeyEvent();
    void GetFBZoomAndLocation(TReal& aDrawZoom, TReal& aInPictureX, TReal& aInPictureY);
    
private:

    /**
     * Constructor for performing 1st stage construction
     */
    CDrawFaceBrowsing(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex);
    TBool ShowUtilities();
    
private:    
    CImagicContainerBrowser* iContainer;
    //TInt&                    iCurrentIndex;
    RArray<TRect>            iCoordinates;
    RArray<FloatCoords>      iFloatCoordinates;
    TInt                     iCoordIndex;
    TInt                     iFaceNro;
    float                    iFBMovingSpeed;
    TInt                     iFBRectCounter;
    float                    iDrawWidth, iDrawHeight;//OneByOne mode image size Widht and Height
    float                    iImageWidth, iImageHeight;
    //float                    iDrawFBZoom;
    float                    iDrawZoom;//Used for image scaling in OneByOne mode
    float                    iDrawFBTargetZoom;
    //Coordinates, when moving in picture zoomed in
    float                    iInPictureX, iInPictureY;
    float                    iDrawTargetX, iDrawTargetY;//target value of target X and Y
    float                    iDrawX, iDrawY;//current value of target X and Y
    float                    iFBZoomingSpeed;
    TReal                    iMenuAlpha;
    };

#endif // DRAWFACEBROWSING_H
