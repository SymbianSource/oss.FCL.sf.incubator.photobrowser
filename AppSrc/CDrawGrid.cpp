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

#include "CDrawGrid.h"
#include "TextureLoader.h"
#include "DrawUtility.h"
#include "ImagicConsts.h"


const TInt  KDrawLimit = 65;
const float KInitialZoomGrid = -4.1;//zoom value for grid when application starts drawing grid

const TReal KMaxAngleLandscape = 45;//max tilt angle when moving in grid
const TReal KMaxAnglePortrait = 10;//max tilt angle when moving in grid
//const TReal KAngle2Start128Loading = 1;
const float KTargetZCoord = -1.5;
const float KZoomInMaxGrid = -3.5;//max possible zoom value 
const float KZoomOutMaxGrid = -6;//min possible zoom value
#ifdef DOUBLETAP_ZOOMGRID
const TInt  KDoubleTapZoomGrid1 = KInitialZoomGrid;
const TInt  KDoubleTapZoomGrid2 = KZoomOutMaxGrid * 7 / 10;  // 70% of max zoom out
#endif

CDrawGrid::CDrawGrid(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex):
    iContainer(aContainer)//,
    //iCurrentIndex(aCurrentIndex)
    {
    // No implementation required
    }

CDrawGrid::~CDrawGrid()
    {
    }

CDrawGrid* CDrawGrid::NewLC(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex)
    {
    CDrawGrid* self = new (ELeave) CDrawGrid(aContainer,aCurrentIndex);
    CleanupStack::PushL(self);
    self->ConstructL(aContainer,aCurrentIndex);
    return self;
    }

CDrawGrid* CDrawGrid::NewL(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex)
    {
    CDrawGrid* self = CDrawGrid::NewLC(aContainer,aCurrentIndex);
    CleanupStack::Pop(); // self;
    return self;
    }

void CDrawGrid::ConstructL(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex)
    {
    //iContainer = aContainer;
    }


TReal CDrawGrid::GetCurrentGridTilt()
    {
    return iPerspectiveCurrent;
    }

TReal CDrawGrid::GetGridZoom()
    {
    return iDrawGridZoom;
    }

TGridXY CDrawGrid::GetGridTargetXY()
    {
    return iDrawGridTargetXY;
    }

void CDrawGrid::SetGridTargetXY(TGridXY aValue)
    {
    iDrawGridTargetXY = aValue;
    }

TGridXY CDrawGrid::GetGridXY()
    {
    return iDrawGridXY;
    }

void CDrawGrid::SetGridXY(TGridXY aValue)
    {
    iDrawGridXY = aValue;
    }

void CDrawGrid::KeyPressed()
    {
    iMenuAlpha = 1;
    iKeyTimer = 0;
    iKeyTimer2 = 10;
    }

void CDrawGrid::KeyReleased()
    {
    iMenuAlpha = 0.99;
    iKeyTimer = 0;
    iKeyTimer2 = 10;
    }

void CDrawGrid::KeyEvent()
    {
    //iMenuAlpha = 1;
    }

/*----------------------------------------------------------------------*/
// Set perspective projection
//
void CDrawGrid::SetPrespective(const TSize &aSize)
    {
    DP0_IMAGIC(_L("CDrawGrid::SetPrespective"));
    // Calculate aspect ratio
    GLfloat aspectRatio = (GLfloat)(aSize.iWidth) / (GLfloat)(aSize.iHeight);

    // Calculate prespective values
    const float near = 0.001;
    const float far = 100.0;
    const float top = 0.414*near;
    const float bottom = -top;
    const float left = aspectRatio * bottom;
    const float right = aspectRatio * top;
    
    // Set perspective
    glLoadIdentity();
    glFrustumf(left,right, bottom,top, near,far);
    
    float tmp = iContainer->GetDisplayRotAngle();
    iContainer->Interpolate(tmp, iContainer->GetDisplayRotTargetAngle(), 0.2);
    iContainer->SetDisplayRotAngle(tmp);
    glRotatef(iContainer->GetDisplayRotAngle(), 0,0,1);
    }

void CDrawGrid::UpdateImageCoordinates(const TInt aFirstIndex)
    {
    const TReal KMinY = -(CImagicContainerBrowser::KGridSizeY - 1) * CImagicContainerBrowser::KSpacingY;
    TReal y2 = KMinY;
    TReal x2 = -CImagicContainerBrowser::KSpacingX;
    
    CImageData* prevImageData = iContainer->iIEngine->GetImageData(aFirstIndex - 1);
    if (prevImageData)
        {
        x2 = prevImageData->iGridData.iX;
        y2 = prevImageData->iGridData.iY;
        }

    for(TInt i = aFirstIndex; i<iContainer->iIEngine->GetTotalNumOfImages(); i++)
        {
        if ((y2 -= iContainer->KSpacingY) < KMinY)
            {
            y2 = 0;
            x2 += CImagicContainerBrowser::KSpacingX;
            }

        CImageData* imageData = iContainer->iIEngine->GetImageData(i);        
#ifdef GAP_BETWEEN_FOLDERS
        TGridMode gridMode = iContainer->iIEngine->GetImageList().GetGridMode();
        if (gridMode)   
            {
            // Make small gap between folders
            CImageData* prevImageData = iContainer->iIEngine->GetImageData(i - 1);            
            if (prevImageData != NULL)
                {
                TBool gap = EFalse; 
                if (gridMode == EGridModeFolder)
                    {
                    TFileName path, prevPath;
                    imageData->GetPath(path);
                    prevImageData->GetPath(prevPath);
                    gap = (path != prevPath);
                    }
                else
                    {
                    gap = (imageData->iPersonId != prevImageData->iPersonId);
                    }
                    
                if (gap)
                    {
                    x2 += CImagicContainerBrowser::KSpacingX / 2;
                    if (y2 < 0)
                        {
                        x2 += CImagicContainerBrowser::KSpacingX;
                        y2 = 0;
                        }
                    }
                }
            }
#endif // GAP_BETWEEN_FOLDERS            
        imageData->iGridData.iX = x2;
        imageData->iGridData.iY = y2;
        }
    }

void CDrawGrid::HandleKeys()
    {
    iKeyTimer2--;
    if(iKeyTimer2 < 0)
        iKeyTimer2 = 0;
    
    if(iKeyTimer == 0 || iKeyTimer2 == 0)
        {
        iKeyTimer = 3;
        
        CKeyData keyData = iContainer->GetKeyData();
        
        // Calculate new index from movement keys
        iContainer->SetCurrentIndex(keyData.iY + iContainer->GetCurrentIndex());

        // Get next image in same row
        for (TInt i = 0;i < Abs(keyData.iX);i++)
            {
            TInt index = iContainer->GetCurrentIndex();
            CImageData* currentImageData = iContainer->iIEngine->GetImageData(index);
            while (currentImageData)
                {
                index += keyData.iX > 0 ? 1 : -1;
                CImageData* imageData = iContainer->iIEngine->GetImageData(index);
                if (imageData == NULL)
                    {
                    currentImageData = NULL;
                    break;
                    }
                
                // Next image is found
                if (Abs(imageData->iGridData.iY - currentImageData->iGridData.iY) <
                        CImagicContainerBrowser::KSpacingY / 2)
                    {
                    iContainer->SetCurrentIndex(index);
                    currentImageData = NULL;
                    break;
                    }
                }
            }
        
        //iContainer->SetCurrentIndex(keyData.iX * CImagicContainerBrowser::KGridSizeY + iContainer->GetCurrentIndex());
		
        //We have to zero key data after reading it
        keyData.iX = 0;
        keyData.iY = 0;
        }
    else
        {
        iKeyTimer--;
        }
    }
    
/*----------------------------------------------------------------------*/
// Initializes Grid view
//
void CDrawGrid::InitDrawGrid()
    {
    DP0_IMAGIC(_L("CDrawGrid::InitDrawGrid"));
    
    iKeyTimer = 0;
    
    iPerspectiveCurrent = 0;//current value of perspective when moving on grid by tilting while grid
    //iPerspectiveTarget = 0;//target value of perspective
    iDrawGridTargetZoom = KInitialZoomGrid;//Set target zooming value when draving Grid
    
    iGridMovingSpeed = 0.05;
    iGridZoomSpeed = 0.1;
    iGridZoomStep = 0.3;
    iDrawGridZoom = KInitialZoomGrid;//Set target zooming value when draving Grid
    
    CImageData* imageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    if(imageData)
        imageData->iGridData.iScale = 6.5;
    
    // set scale a little bigger than initial target value to show animation in opening grid
//    CImageData* imageData = iIEngine->GetImageData(iCurrentIndex, iImagicAppUi->GetUIDrawMode());
//    if (imageData) imageData->iGridData.iScale += 0.5;

    // set zooming factor a little bigger than initial target value to show animation in opening grid
//    iDrawGridZoom = iDrawGridZoom + 0.5;
    }

/*----------------------------------------------------------------------*/
// Draws grid
//
void CDrawGrid::DrawGridL(const TSize &aSize)
    {
    DP0_IMAGIC(_L("CDrawGrid::DrawGrid"));
    
    SetPrespective(aSize);// Setup perspective
    
    // If user hasn't press anything, stay in beginning of the grid
    if(!iContainer->IsUserInputGiven())
        {
        TInt index = -1;  
        TReal maxX = CImagicContainerBrowser::KSpacingX; 
        if(iContainer->GetScreenSize().iHeight > 240)
            maxX = CImagicContainerBrowser::KSpacingX * 2;
        
        // Select image which closest to ideal position
        for(TInt i = 0;i < iContainer->iIEngine->GetTotalNumOfImages();i++)
            {
            CImageData* imageData = iContainer->iIEngine->GetImageData(i);
            if (imageData->iGridData.iX > maxX) 
                break;
            if (imageData->iGridData.iY > -CImagicContainerBrowser::KSpacingY * 2)
                index = i; 
            }
        
        if (index >= 0)
            iContainer->SetCurrentIndex(index);
        }
    
    
#ifdef MOMENTUM_MOVE
    // Move grid x and y automatically when flick
    // FindImageInScreen() has to be called here, before SetPerspective() to get 
    // OpenGL matrix before initialised in it.
    if(iContainer->iMomentumMove)
        {
        // so move automatically. Target is set in HandleGestureEnd(), when 
        // user releases touch.

        // Slowing down when it gets closer than the size of 10% of speed
        // TODO: use definition. not 0.5
        float gapX = (iContainer->iMomentumSpeedX)? (iDrawGridTargetXY.iX - iDrawGridXY.iX) / (0.5 * iContainer->iMomentumSpeedX): 0.0f;
        float gapY = (iContainer->iMomentumSpeedY)? (iDrawGridTargetXY.iY - iDrawGridXY.iY) / (0.5 * iContainer->iMomentumSpeedY): 0.0f;
        iContainer->CheckLimits(gapX, -1.0, 1.0); // max speed doesn't go beyond user's move
        iContainer->CheckLimits(gapY, -1.0, 1.0);

        float spdX = gapX * iContainer->iMomentumSpeedX; // speed/1sec. faster with bigger gap
        float spdY = gapY * iContainer->iMomentumSpeedY;
        iDrawGridXY.iX += spdX * iContainer->iTimeDiff; // movement = (open gl pixels)/sec * (elapsed time from last draw) 
        iDrawGridXY.iY += spdY * iContainer->iTimeDiff;

        DP4_IMAGIC(_L("spdX=%6.4f, gapX=%6.4f, iMomentumSpeedX=%6.4f, iTimeDiff=%6.4f"), spdX, gapX, iContainer->iMomentumSpeedX, iContainer->iTimeDiff);
        DP3_IMAGIC(_L("spdY=%6.4f, gapY=%6.4f, iMomentumSpeedY=%6.4f"), spdY, gapY, iContainer->iMomentumSpeedY);
        DP2_IMAGIC(_L("iDrawGridTargetX=%6.4f <=== iDrawGridX=%6.4f"), iDrawGridTargetXY.iX, iDrawGridXY.iX);
        DP2_IMAGIC(_L("iDrawGridTargetY=%6.4f <=== iDrawGridY=%6.4f"), iDrawGridTargetXY.iY, iDrawGridXY.iY);
        }

    // Pick up the picture in the center of the screen as current selected one.
    // Also does on dragging if not in the mode of no key event simulation on drag 
#ifdef HOLD_SELECTION_ONDRAG
    if(iContainer->iMomentumMove || iContainer->iHoldSelection)
#else
    if(iMomentumMove)
#endif
        {
        TInt id;
        FloatCoords coord;
        coord.iX = iDrawGridXY.iX;
        coord.iY = iDrawGridXY.iY;
#ifdef FLICK_ONLY_IN_X_IN_GRID
        if (iContainer->FindNearestImageInOGl(coord, id))
            {
            if (iContainer->iMomentumMove)
                {
                // Change iCurrentIndex only when x coord is changed
                TInt cX = iContainer->GetCurrentIndex() / CImagicContainerBrowser::KGridSizeY;
                TInt tX = id / CImagicContainerBrowser::KGridSizeY;
                if (cX != tX) 
                    //iCurrentIndex = id;
                    iContainer->SetCurrentIndex(id);
                }
            else
                //iCurrentIndex = id;
                iContainer->SetCurrentIndex(id);
            }
#else
        if (FindNearestImageInOGl(coord, id)) 
            //iCurrentIndex = id;
            iContainer->SetCurrentIndex(id);
#endif
        }

    if(iContainer->iMomentumMove)
        {
        // Stop momentum move when it gets close to target or exceeds 
        if ((!iContainer->iMomentumSpeedX || Abs(iDrawGridTargetXY.iX - iDrawGridXY.iX) < 0.15) && // 0.15 is just a guess.
            (!iContainer->iMomentumSpeedY || Abs(iDrawGridTargetXY.iY - iDrawGridXY.iY) < 0.15))
            {
            iContainer->iMomentumMove = EFalse;
            }
        }
#endif
    
    
    
#ifdef ZOOM_WHILE_ROTATING
    if(Abs(iDrawGridZoom-KZoomOutMaxGrid) < 1)
        iDrawGridTargetZoom = KInitialZoomGrid;
#endif
#ifdef ENABLE_GRID_ZOOM
    Interpolate(iDrawGridZoom, iDrawGridTargetZoom, iGridZoomSpeed);
#endif
    
#ifdef SCREEN_ROTATION_ON
    Interpolate(iDisplayRotation, iDisplayRotationTarget, 0.005);
#endif
    
    //Handle moving keys-------------------------------->
    HandleKeys();
    //Handle Rotation keys, this is for single currently selected picture
    iContainer->HandleRotationKeys();
    
    //Check that current index is in grid area
    /*TInt index = iContainer->GetCurrentIndex();
    CheckIndexLimits(index);
    iContainer->SetCurrentIndex(index);*/

    // Update AppUI class about selected picture index
    if(iContainer->GetPrevIndex() != iContainer->GetCurrentIndex())
        {
        iContainer->iImagicAppUi->SetImageIndex(iContainer->GetCurrentIndex());
        iContainer->iDynamicLoadingOn = ETrue;

//#define DISPLAY_DATE
#ifdef DISPLAY_DATE
        DisplayDate();
#endif
        //Update previous Grid index..
        iContainer->SetPrevIndex(iContainer->GetCurrentIndex());        
        }
    
#ifdef MOMENTUM_MOVE
    if(!iContainer->iMomentumMove) 
        {
#endif
        //Calculate new target X and Y positions on Grid

    CImageData* imageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    if (imageData)
        {
        iDrawGridTargetXY.iX = imageData->iGridData.iX;
        iDrawGridTargetXY.iY = -imageData->iGridData.iY;
        }

#ifdef HOLD_SELECTION_ONDRAG
        if (iContainer->iHoldSelection == EFalse) 
            {
#endif
            //and interpolate between current and target Y and X
            iContainer->Interpolate(iDrawGridXY.iX, iDrawGridTargetXY.iX, iGridMovingSpeed);
            iContainer->Interpolate(iDrawGridXY.iY, iDrawGridTargetXY.iY, iGridMovingSpeed);
    
#ifdef HOLD_SELECTION_ONDRAG
            }
#endif
#ifdef MOMENTUM_MOVE
        }
#endif
    
    // Zooming
    //Calculate new target zooming value for Grid
//unno temp iGridZoomSpeed = 20 * iTimeDiff;
#ifdef ENABLE_GRID_ZOOM
    iDrawGridTargetZoom += keyData.iZoomIn * iGridZoomStep;
    iDrawGridTargetZoom -= keyData.iZoomOut * iGridZoomStep;
    CheckLimits(iDrawGridTargetZoom, KZoomOutMaxGrid, KZoomInMaxGrid);

    keyData.iZoom = 0;//we have to reset key data after using it
    keyData.iZoomIn = keyData.iZoomOut = EFalse;
#endif
    
    //Handle Rotation keys, this is for single currently selected picture
    //iContainer->HandleRotationKeys();
    
    
    // Draw images ---------------------------------->
    
    //Tilt Grid(camera) into moving direction
    float perspectiveDiff = iDrawGridTargetXY.iX-iDrawGridXY.iX;
    perspectiveDiff *= 10;
    
    //we stop moving of the grid littele bit earlier than we are in real zero
#ifdef HOLD_SELECTION_ONDRAG
    if(iContainer->iHoldSelection == EFalse) {
#endif
    iTargetPerspective = perspectiveDiff;
#ifdef HOLD_SELECTION_ONDRAG
    }
#endif

    
    //Speed up tilting when getting closer to target
    if(iPerspectiveCurrent-iTargetPerspective != 0 && 
       (Abs(iTargetPerspective) < Abs(iPerspectiveCurrent)) && 
       Abs(iPerspectiveCurrent) < 20)
        {
        iContainer->Interpolate(iPerspectiveCurrent,iTargetPerspective, Abs((KMaxAngleLandscape/iPerspectiveCurrent)/40));
        }
    else
        {
        iContainer->Interpolate(iPerspectiveCurrent,iTargetPerspective, 0.04);
        }
        
    
#ifdef _ACCELEROMETER_SUPPORTED_
//#ifndef _S60_5x_ACCELEROMETER_
    if(iContainer->iDeviceOrientation == EOrientationDisplayLeftUp)//Landscape
/*#else if _S60_5x_ACCELEROMETER_
    if(iContainer->iDeviceOrientation == (TImagicDeviceOrientation)TSensrvOrientationData::EOrientationDisplayRightUp)///this is wrong
#endif*/
#else
    if(ETrue/*iDeviceOrientation*/)//if no accelerometer, use always landscape
#endif
        iContainer->CheckLimits(iPerspectiveCurrent, -KMaxAngleLandscape, KMaxAngleLandscape);  
    else
        iContainer->CheckLimits(iPerspectiveCurrent, -KMaxAnglePortrait, KMaxAnglePortrait);
    
    glRotatef(iPerspectiveCurrent, 0,1,0);//make perspective when moving on grid
    
    
    
    //Go to grid position ----------->
    //Center picture grid and set limit of pictures to be drawn
    float centerOffset=2.9;//bigger value -> less movement in Y-axis
    
#ifdef MOMENTUM_MOVE
    glTranslatef(-iDrawGridXY.iX-iDrawGridZoom*iPerspectiveCurrent/KMaxAngleLandscape,
            CImagicContainerBrowser::KSpacingY+(iDrawGridXY.iY-CImagicContainerBrowser::KSpacingY)/centerOffset, iDrawGridZoom);
#else
    glTranslatef(-iDrawGridX, KSpacingY+(iDrawGridY-KSpacingY)/centerOffset, iDrawGridZoom);
#endif
    
    //OpenGl Vertex data
    GLfixed vertices[8];
    glVertexPointer( 2, GL_FIXED, 0, vertices );
    
    //Bind into a null picture
    //iContainer->iCurrentBindedIndex = iContainer->iLoadingTextureIndex;
    //iContainer->iLoadingTextureIndex = 0;
    //glBindTexture( GL_TEXTURE_2D, iContainer->iLoadingTextureIndex);
    glBindTexture( GL_TEXTURE_2D, 0);
    
       
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   	
    // Set gray color for pictures that are not loaded yet
    glColor4f(0.3,0.3,0.3, 1);
    
    TInt drawMax = iContainer->GetCurrentIndex()+KDrawLimit;
    TInt drawMin = iContainer->GetCurrentIndex()-KDrawLimit;

    CImageData* currentImageData = NULL;
    TGridMode gridMode = iContainer->iIEngine->GetImageList().GetGridMode();
    if (gridMode != EGridModeTime)      
        currentImageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    
    //This loop draws all the textures in the grid ------------------------------->
    for(TInt i2=0; i2<iContainer->iIEngine->GetTotalNumOfImages(); i2++)
        {
        TInt i = (i2 + iContainer->GetCurrentIndex() + 1) % iContainer->iIEngine->GetTotalNumOfImages();
        CImageData* imageData = iContainer->iIEngine->GetImageData(i);
        
        //If we are in same picture index, ie. not moving, draw both sides equally
        if(drawMax > iContainer->iIEngine->GetTotalNumOfImages()-1)
            drawMax = iContainer->iIEngine->GetTotalNumOfImages()-1;
        if(drawMin < 0)
            drawMin = 0;
            
        //Do not draw all images, just enough to fill the screen
        if((Abs(imageData->iGridData.iRotationAngle-imageData->iGridData.iTargetRotationAngle) > 0.1) || //Draw all all images in grid if rotated
           (i>=drawMin && i<=drawMax))
            {
            // Calculate current coordinates
            //TInt x = i/iContainer->iGridSizeY;
            //TInt y = -(i%iContainer->iGridSizeY);// Y axis is inverted
            float z=0;
            //float scale=1;
            iScaleTarget = 1;
            
            
            //Highlight by scaling up selected picture on Grid
            if(iContainer->GetCurrentIndex()==i)
                {
                z=0.3;
                iScaleTarget = 1.5;
                }

            TBool dim = EFalse; //(currentImageData && !currentImageData->IsSamePath(*imageData));            
            //BubbleEffect(x, y, z);
            
            //Zoom out the grid if "camera target" is far away from current position >>>
            //if (Abs(iDrawGridX-iDrawGridTargetX) > 10*KSpacingX)
#ifdef MOMENTUM_MOVE
            float absDeltaX = Abs(iDrawGridXY.iX-iDrawGridTargetXY.iX);
            if((!iContainer->iMomentumMove && absDeltaX > 2*CImagicContainerBrowser::KSpacingX) ||
               ( iContainer->iMomentumMove && absDeltaX > 5*CImagicContainerBrowser::KSpacingX))
#else
            if(Abs(iDrawGridX-iDrawGridTargetX) > 2*KSpacingX)
#endif
                {
                z=KTargetZCoord;
                iContainer->Interpolate(imageData->iGridData.iZ, z, 0.05);
                }    
            else
                {
                iContainer->Interpolate(imageData->iGridData.iZ, z, 0.05);
                }
            
            iContainer->Interpolate(imageData->iGridData.iScale, iScaleTarget, 0.23);
            //Zoom out the grid if "camera target" is far away from current position <<<
            
            //Rotate picture in Grid if needed
            iContainer->HandleRotation(imageData->iGridData.iRotationAngle, imageData->iGridData.iTargetRotationAngle);
            
            //Calculate new picture vertices to fix picture aspect ratio
            iContainer->SetPictureVertices(imageData, vertices);
            
            //Move camera to picture position and rotate and scale it
            glPushMatrix();    
            glTranslatef(imageData->iGridData.iX, imageData->iGridData.iY, imageData->iGridData.iZ);// - (dim ? 1.0 : 0));
            glRotatef(imageData->iGridData.iRotationAngle, 0,0,1);
            TReal scale = imageData->iGridData.iScale;
            glScalef(scale, scale, scale);
            
#ifdef SHADOW_PHOTOS
            // Draw shadow behind image
            glPushMatrix();
            glBindTexture( GL_TEXTURE_2D, iContainer->iShadowTextureIndex);
            glTranslatef(0, 0, -0.15);
            glScalef(1.2, 1.2, 1);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glPopMatrix();
#endif                     
            
#ifdef DRAW_FRAME
            // Draw frame around selected image
            if (iContainer->GetCurrentIndex()==i)
                DrawFrame(i);
#endif
                    
            // Calculate color for not loaded picture, more far from selection-> darker box
            float color = 
#ifdef EMPTY_IMAGE_AS_WIREFRAME
                0.05 + 0.2/
#else
                1.0/
#endif                
                (Abs(iDrawGridXY.iX - imageData->iGridData.iX)/2 + .2);
            if (color > 0.5)
                color = 0.5;
            glColor4f(color,color,color, 1);
            
            //Bind to a new picture only if needed, ie. we really have new picture
            if(imageData->iGridData.iGlLQ128TextIndex != iLastGridTexture ||
               imageData->iGridData.iGlLQ32TextIndex != iLastGridTexture)
                {
                if(imageData->iGridData.iGlLQ128TextIndex)
                    iLastGridTexture = imageData->iGridData.iGlLQ128TextIndex;
                else if(imageData->iGridData.iGlLQ32TextIndex)
                    iLastGridTexture = imageData->iGridData.iGlLQ32TextIndex;
                else
                    iLastGridTexture = iContainer->iLoadingTextureIndex;
                
                iContainer->iCurrentBindedIndex = iLastGridTexture;
                glBindTexture( GL_TEXTURE_2D, iLastGridTexture);
                
                // Picture is loaded, draw it with white color
                if (iLastGridTexture)  
                    {
                    // Draw as solid black frame
                    if (dim)
                        glColor4f(0.4,0.4,0.4, 1);
                    else
                        glColor4f(1,1,1, 1);
                    }
                }
            
            // Draw image
#ifdef EMPTY_IMAGE_AS_WIREFRAME            
            if (iLastGridTexture == 0 && iContainer->GetCurrentIndex() != i)
                {
                GLfixed p;
                p = vertices[6]; vertices[6] = vertices[4]; vertices[4] = p;
                p = vertices[7]; vertices[7] = vertices[5]; vertices[5] = p;
                glLineWidth(1.8f);  // TODO: depend on resolution
                glDrawArrays(GL_LINE_LOOP,0,4);
                p = vertices[6]; vertices[6] = vertices[4]; vertices[4] = p;
                p = vertices[7]; vertices[7] = vertices[5]; vertices[5] = p;
                }
            else
                {
                // Draw as solid black frame
                if (iLastGridTexture == 0)
                    glColor4f(0,0,0, 1);
#endif                
                glDrawArrays(GL_TRIANGLE_STRIP,0,4);
#ifdef EMPTY_IMAGE_AS_WIREFRAME                
                }
#endif            
           
            glPopMatrix();
            }//if
        
        //EGLint err = eglGetError();
        EGLint err = glGetError();
        //if(err != EGL_SUCCESS)
        while(err != GL_NO_ERROR)
            {
            CImageData* data = iContainer->iIEngine->GetImageData(i);
            //Delete all textures for this index just in case
            if(data->iGridData.iGlLQ128TextIndex)
                glDeleteTextures(1, &data->iGridData.iGlLQ128TextIndex);data->iGridData.iGlLQ32TextIndex = 0;
            if(data->iGridData.iGlLQ32TextIndex)
                glDeleteTextures(1, &data->iGridData.iGlLQ32TextIndex);data->iGridData.iGlLQ128TextIndex = 0;
            if(data->iGridData.iGlHQ512TextIndex)
                glDeleteTextures(1, &data->iGridData.iGlHQ512TextIndex);data->iGridData.iGlHQ512TextIndex = 0;
            if(data->iGridData.iGlSuperHQTextIndex)
                glDeleteTextures(1, &data->iGridData.iGlSuperHQTextIndex);data->iGridData.iGlSuperHQTextIndex = 0;
            
            err = eglGetError();
            }
                        
        }//for
    
#if 0
    if(iMenuAlpha < 1)
        {
        iMenuAlpha-=0.15;
        if(iMenuAlpha < 0)
            iMenuAlpha = 0;
        }

    iContainer->iDrawUtility->DrawIcon2(iContainer->Size(), iContainer->iMenuTextureIndex, iMenuAlpha);
#endif
    
    iContainer->DynamicLoadingL();
    }


TBool CDrawGrid::IsDrawingNeededGrid()
    {
    
    CImageData* imageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    
    iContainer->SetMinMagFilterLinear(EFalse);
#if 0
    if(iMenuAlpha != 0)
        {
        return ETrue;
        }
#endif
    if(Abs(iPerspectiveCurrent) < CImagicContainerBrowser::KAngle2Start128Loading && Abs(iPerspectiveCurrent) > 0.001)
        {
        //DP0_IMAGIC(_L("CDrawGrid::IsDrawingNeededGrid #0"));
        iContainer->iDynamicLoadingOn = ETrue;
        }
    
    
    //Make sure that all visible images are rotated
    //this causes too much processor load
    /*for(TInt i=0; i<=CImagicContainerBrowser::K128TNImageBuffer; i++)
        {
        for(TInt j=0; j<2; j++)
            {
            TInt index = iContainer->GetCurrentIndex() + (j ?  i : -i);
            
            CImageData* imageData = iContainer->iIEngine->GetImageData(index);
            
            if(imageData != NULL)
                if(Abs(imageData->iGridData.iRotationAngle - imageData->iGridData.iTargetRotationAngle) > 0.01)
                    {
                    DP0_IMAGIC(_L("CDrawGrid::IsDrawingNeededGrid #0.1"));
                    return ETrue;
                    }
                else if(imageData->iGridData.iRotationAngle != imageData->iGridData.iTargetRotationAngle)
                    {
                    DP0_IMAGIC(_L("CDrawGrid::IsDrawingNeededGrid #0.2"));
                    imageData->iGridData.iRotationAngle = imageData->iGridData.iTargetRotationAngle;
                    return ETrue;
                    }
            }
        }*/
    
    //CImageData* imageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    
    if(imageData)
        if(Abs(imageData->iGridData.iScale - iScaleTarget) > 0.1)
            {
            return ETrue;
            }
        else if(imageData->iGridData.iScale != iScaleTarget)
            {
            imageData->iGridData.iScale = iScaleTarget;
            return ETrue;
            }
    
    
#ifdef ENABLE_DISPLAY_ROTATION
    if(Abs(iContainer->GetDisplayRotAngle() - iContainer->GetDisplayRotTargetAngle()) > 1)
        {
        DP0_IMAGIC(_L("CDrawGrid::IsDrawingNeededGrid #2"));
        return ETrue;
        }
    else if(iContainer->GetDisplayRotAngle() != iContainer->GetDisplayRotTargetAngle())
        {
        iContainer->SetDisplayRotAngle(iContainer->GetDisplayRotTargetAngle());
        return ETrue;
        }
#endif
    
    if(imageData)
        if(Abs(imageData->iGridData.iRotationAngle-imageData->iGridData.iTargetRotationAngle) > 1)
            {
            DP0_IMAGIC(_L("CDrawGrid::IsDrawingNeededGrid #3"));
            return ETrue;
            }
        else if(imageData->iGridData.iRotationAngle != imageData->iGridData.iTargetRotationAngle)
            {
            imageData->iGridData.iRotationAngle = imageData->iGridData.iTargetRotationAngle;
            //iContainer->iDrawNow = ETrue;
            return ETrue;
            }
    
#ifdef ENABLE_GRID_ZOOM
    if(Abs(iDrawGridZoom - iDrawGridTargetZoom) > 0.01)
        {
        DP0_IMAGIC(_L("CDrawGrid::IsDrawingNeededGrid #4"));
        return ETrue;
        }
#endif
    
    if(Abs(iDrawGridXY.iX - iDrawGridTargetXY.iX) > 0.02)
        {
        DP0_IMAGIC(_L("CDrawGrid::IsDrawingNeededGrid #5"));
        return ETrue;
        }
    else if(iDrawGridXY.iX != iDrawGridTargetXY.iX)
        {
        iDrawGridXY.iX = iDrawGridTargetXY.iX;
        }
    
    if(Abs(iDrawGridXY.iY - iDrawGridTargetXY.iY) > 0.02)
        {
        DP0_IMAGIC(_L("CDrawGrid::IsDrawingNeededGrid #6"));
        return ETrue;
        }
    else if(iDrawGridXY.iY != iDrawGridTargetXY.iY)
        {
        iDrawGridXY.iY = iDrawGridTargetXY.iY;
        }
    
    if(Abs(iPerspectiveCurrent - iTargetPerspective) > 0.1)
        {
        DP0_IMAGIC(_L("CDrawGrid::IsDrawingNeededGrid #7"));
        return ETrue;
        }
    else if(iPerspectiveCurrent != iTargetPerspective)
        {
        DP0_IMAGIC(_L("CDrawGrid::IsDrawingNeededGrid #7.1"));
        iPerspectiveCurrent = iTargetPerspective;
        return ETrue;
        }
    
    if(iContainer->iDrawNow)
        {
        DP0_IMAGIC(_L("CDrawGrid::IsDrawingNeededGrid #1"));
        iContainer->iDrawNow = EFalse;
        return ETrue;
        }
    
    return EFalse;
    }


void CDrawGrid::MovingDirection()
    {
    if(iContainer->GetPrevIndex() < iContainer->GetCurrentIndex())
        {
        iMovingRight = ETrue;
        iMovingLeft = EFalse;
        }
    else if(iContainer->GetPrevIndex() > iContainer->GetCurrentIndex())
        {
        iMovingLeft = ETrue;
        iMovingRight = EFalse;
        }
    else if(iContainer->GetPrevIndex() == iContainer->GetCurrentIndex())
        {
        iMovingRight = EFalse; 
        iMovingLeft = EFalse;
        }
    }

void CDrawGrid::DisplayDate()
    {
    CImageData* imageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    
    if(imageData)
        {
        TDateTime dateTime = imageData->GetCreatedTime().DateTime();
        if(iPrevDateTime.Month() != dateTime.Month())
            {
            TMonth month = dateTime.Month();
            iContainer->iImagicAppUi->GetImagicUtils()->DisplayYearAndMonth(0, dateTime);
            }
        iPrevDateTime = dateTime;
        }
    }

/*----------------------------------------------------------------------*/
// Calculates widht and height with aspect ratio
//
void CDrawGrid::DrawFrame(TInt aIndex)
    {
    // Draw frame around selected image
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    
    glDisable(GL_DEPTH_TEST);
    
    //Frame size
    float scale=1.09;        
#ifdef FRAME_COLOR_CHANGE
    if(iSelectionFrameColor >= 1.0)
        iSelectionFrameColor-=0.05;
        
    if(iSelectionFrameColor <= 0.3)
        iSelectionFrameColor+= 0.05;

    if(iTNCreationComplete)
        glColor4f(iSelectionFrameColor,iSelectionFrameColor,iSelectionFrameColor, 1);
    else
        glColor4f(1,iSelectionFrameColor,iSelectionFrameColor, 1);
#else
#ifdef SHADOW_PHOTOS
    glColor4f(1,0,0, 1);        // red frame
#else    
    glColor4f(1,1,1, 1);        // white frame
#endif    
#endif
    
    glTranslatef(0,0,-0.03);
    glScalef(scale,scale,scale);
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
    
    //glDisable(GL_BLEND);
    //glDisable(GL_DEPTH_TEST);
    
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
    }

/*----------------------------------------------------------------------*/
// Calculates widht and height with aspect ratio
//
void CDrawGrid::BubbleEffect(TInt& x, TInt& y, float& z)
    {
    if(iBubbleEffect)
        {
        // Selected image in coordinates x,y
        TInt selectedX = iContainer->GetCurrentIndex()/3 /*iGridSizeY*/;
        TInt selectedY =- (iContainer->GetCurrentIndex()%3 /*iGridSizeY*/);
                    
        // Distance to selected
        iDistanceX = selectedX-x;
        iDistanceY = selectedY-y;
        // Squared
        if(iDistanceX<0) iDistanceX*= -1;
        if(iDistanceY<0) iDistanceY*= -1;
        
        // Distance
        iDiff=iDistanceX+iDistanceY;
        // Convert distance to depth
        // http://en.wikipedia.org/wiki/Gaussian_function
        if (iDiff==0) z=5.0;
        if (iDiff==1) z=4.2;
        if (iDiff==2) z=3.4;
        if (iDiff==3) z=2.1;
        if (iDiff==4) z=1.3;
        if (iDiff==5) z=0.8;
        if (iDiff==6) z=0.4;
        if (iDiff==7) z=0.3;
        if (iDiff>7) z=0.3;
        //if (iDiff==8) z=0.1;
        //if (iDiff==9) z=0.05;
        //if (iDiff==10) z=0.01;
        //if (iDiff>10) z=0;
        }
    }


