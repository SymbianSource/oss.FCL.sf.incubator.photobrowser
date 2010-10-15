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

#ifndef CDRAWGRID_H
#define CDRAWGRID_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "ImagicContainerBrowser.h"

// CLASS DECLARATION


struct TGridXY
    {
    float iX;
    float iY;
    };
    
    
/**
 *  CDrawGrid
 * 
 */
class CDrawGrid : public CBase
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CDrawGrid();

    /**
     * Two-phased constructor.
     */
    static CDrawGrid* NewL(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex);

    /**
     * Two-phased constructor.
     */
    static CDrawGrid* NewLC(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex);

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CDrawGrid(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex);
    
    void MovingDirection();
    void DisplayDate();
    void SetPrespective(const TSize &aSize);
    void DrawFrame(TInt aIndex);
    void BubbleEffect(TInt& x, TInt& y, float& z);
    void HandleKeys();
    
public:
    void InitDrawGrid();
    void DrawGridL(const TSize &aSize);
    TBool IsDrawingNeededGrid();
    TReal GetCurrentGridTilt();
    TReal GetGridZoom();
    TGridXY GetGridTargetXY();
    void SetGridTargetXY(TGridXY aValue);
    TGridXY GetGridXY();
    void SetGridXY(TGridXY aValue);
    void UpdateImageCoordinates(const TInt aFirstIndex);    
    void KeyPressed();
    void KeyReleased();
    void KeyEvent();
    
    CImagicContainerBrowser* iContainer;
    
    float               iPerspectiveCurrent;//current value of perspective when moving on grid by tilting while grid
    //float               iPerspectiveTarget;//target value of perspective
    float               iGridMovingSpeed;
    float               iGridZoomSpeed;//zoom value in Grid
    float               iGridZoomStep;//zoom value in Grid
    float               iDrawGridZoom;//Holds zooming value for Grid
    float               iDrawGridTargetZoom;//Holds target zooming value for Grid
    float               iTargetPerspective;
    TBool               iIntheEndOfGrid;
    TBool               iJumpOver;
    TUint               iLastGridTexture;
    TBool               iMovingRight;
    TBool               iMovingLeft;
    TDateTime           iPrevDateTime;
    TGridXY             iDrawGridXY;
    TGridXY             iDrawGridTargetXY;
    
    
    
    //variables used in Buble mode
    TBool               iBubbleEffect;//Flag for "Bubble" effect to set it on/off
    TInt                iDistanceX;
    TInt                iDistanceY;
    TInt                iDiff;
    TInt                iDistX;
    TInt                iDistY;
    TInt                iBindDiff;
    
    
    TInt                iKeyTimer;
    TInt                iKeyTimer2;
    
    TReal               iMenuAlpha;
    
    float               iScaleTarget;
    };

#endif // CDRAWGRID_H
