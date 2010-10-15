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

#ifndef DRAWONEBYONE_H
#define DRAWONEBYONE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "ImagicContainerBrowser.h"
#include "CDrawMagGlass.h"



class CDrawMagGlass;

struct TDrawOneByOneXY
    {
    float iX;
    float iY;
    };

// CLASS DECLARATION

/**
 *  CDrawOneByOne
 * 
 */
class CDrawOneByOne : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CDrawOneByOne();

    /**
     * Two-phased constructor.
     */
    static CDrawOneByOne* NewL(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex);

    /**
     * Two-phased constructor.
     */
    static CDrawOneByOne* NewLC(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex);

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CDrawOneByOne(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex);
    void CheckImageLocation(/*float& aImageWidth, float& aImageHeight*/);
    void HandleMovingKeysOnebyOne();
    
public:
    void InitDrawOnebyOne(TReal aDrawZoom, TReal aInPictureX, TReal aInPictureY);
    void DrawOnebyOneL(const TSize &aSize);
    TBool IsDrawingNeededOneByOne();
    float GetMovingStep();
    float GetImgeFlowLocation();
    void SetImgeFlowLocation(float aValue);
    TDrawOneByOneXY GetDrawOneByOneXY();
    TDrawOneByOneXY GetDrawOneByOneTargetXY();
    void SetDrawOneByOneTargetXY(TDrawOneByOneXY aValue);
    void ChangeDrawOneByOneTargetX(float aValue);
    void ChangeDrawOneByOneTargetY(float aValue);
    float GetDrawOneByOneZoom();
    void SetDrawOneByOneZoom(float aValue);
    float GetDrawOneByOneTargetZoom();
    void SetDrawOneByOneTargetZoom(float aValue);
    float GetDrawOneByOneWidth();
    void SetDrawOneByOneWidth(float aValue);
    float GetDrawOneByOneHeight();
    void SetDrawOneByOneHeight(float aValue);
    
    TBool IsMagGlassOn();
    void SetMagGlassStatus(TBool aValue);
    TBool IsMagGlassPrevStateOn();
    void SetMagGlassPrevStatus(TBool aValue);
    CDrawMagGlass* GetMagGlass();
    void KeyPressed();
    void KeyReleased();
    void KeyEvent();

    
    
private:    
    CImagicContainerBrowser* iContainer;
    CDrawMagGlass*           iMagGlass;
    
    float                    iMovingSpeed;
    float                    iMovingStep;
    
    float                    iZoomingStep;
    float                    iZoomingSpeed;
    float                    iScalingSpeed;
    float                    iOneByOneFlow;
    
    TDrawOneByOneXY          iDrawOneByOneXY;
    TDrawOneByOneXY          iDrawOneByOneTargetXY;
    float                    iDrawOneByOneZoom;//Used for image scaling in OneByOne mode
    float                    iDrawOneByOneTargetZoom;//Used for image zooming in OneByOne mode
    float                    iDrawOnebyOneW, iDrawOnebyOneH;//OneByOne mode image size Widht and Height
    float                    iInPictureX, iInPictureY;
    //when moving in picture zoomed in, this defines value how we can go over the image boundary
    float                    iBorder;
    //Size of the screen, when moving in picture zoomed in
    float                    iScreenW;
    float                    iScreenH;
    float                    iImageWidth;
    float                    iImageHeight;
    float                    iFadeColor;
    //MagGlass
    TBool                    iMagGlassOn;
    TBool                    iMagGlassOnPrev;
    float                    iFlowThresHold;
    TReal                    iMenuAlpha;
    
public:
    static const float KMaxMagGlassZoomFactor;
    
    };

#endif // DRAWONEBYONE_H
