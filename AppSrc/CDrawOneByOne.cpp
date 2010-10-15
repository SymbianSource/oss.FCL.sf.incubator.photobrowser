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

#include "CDrawOneByOne.h"
#include "TextureLoader.h"
#include "DrawUtility.h"
#include "ImagicConsts.h"

#define IS_NOT_IN_ZOOM_ONEBYONE ((iDrawOneByOneTargetZoom) < (iContainer->KDoubleTapZoomOneByOne1 + 0.01f))
#define IS_ALMOST_ZERO (0.001)


const float CDrawOneByOne::KMaxMagGlassZoomFactor = 2.2;//2.0;
const float KOneByOneSlideSpeed = 0.24;


CDrawOneByOne::CDrawOneByOne(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex):
    iContainer(aContainer)//,
    {
    iMagGlassOn = EFalse;
    iMagGlassOnPrev = EFalse;
    }

CDrawOneByOne::~CDrawOneByOne()
    {
    delete iMagGlass;
    }

CDrawOneByOne* CDrawOneByOne::NewLC(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex)
    {
    CDrawOneByOne* self = new (ELeave) CDrawOneByOne(aContainer, aCurrentIndex);
    CleanupStack::PushL(self);
    self->ConstructL(aContainer, aCurrentIndex);
    return self;
    }

CDrawOneByOne* CDrawOneByOne::NewL(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex)
    {
    CDrawOneByOne* self = CDrawOneByOne::NewLC(aContainer, aCurrentIndex);
    CleanupStack::Pop(); // self;
    return self;
    }

void CDrawOneByOne::ConstructL(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex)
    {
    iMagGlass = CDrawMagGlass::NewL(iContainer, this);
    }

float CDrawOneByOne::GetMovingStep()
    {
    //iMovingStep = 0.01/(iDrawOneByOneZoom/8.0);
    iMovingStep = 0.015/(iDrawOneByOneZoom/8.0);
    return iMovingStep;
    }

float CDrawOneByOne::GetImgeFlowLocation()
    {
    return iOneByOneFlow;
    }

void CDrawOneByOne::SetImgeFlowLocation(float aValue)
    {
    iOneByOneFlow = aValue;
    }

TDrawOneByOneXY CDrawOneByOne::GetDrawOneByOneXY()
    {
    return iDrawOneByOneXY;
    }

TDrawOneByOneXY CDrawOneByOne::GetDrawOneByOneTargetXY()
    {
    return iDrawOneByOneTargetXY;
    }

void CDrawOneByOne::SetDrawOneByOneTargetXY(TDrawOneByOneXY aValue)
    {
    iDrawOneByOneTargetXY = aValue;
    }

void CDrawOneByOne::ChangeDrawOneByOneTargetX(float aValue)
    {
    iDrawOneByOneTargetXY.iX += aValue;
    }

void CDrawOneByOne::ChangeDrawOneByOneTargetY(float aValue)
    {
    iDrawOneByOneTargetXY.iY += aValue;
    }

float CDrawOneByOne::GetDrawOneByOneZoom()
    {
    return iDrawOneByOneZoom;
    }

void CDrawOneByOne::SetDrawOneByOneZoom(float aValue)
    {
    iDrawOneByOneZoom = aValue;
    }

float CDrawOneByOne::GetDrawOneByOneTargetZoom()
    {
    return iDrawOneByOneTargetZoom;
    }

void CDrawOneByOne::SetDrawOneByOneTargetZoom(float aValue)
    {
    iDrawOneByOneTargetZoom = aValue;
    }

float CDrawOneByOne::GetDrawOneByOneWidth()
    {
    return iDrawOnebyOneW;
    }

void CDrawOneByOne::SetDrawOneByOneWidth(float aValue)
    {
    iDrawOnebyOneW = aValue;
    }

float CDrawOneByOne::GetDrawOneByOneHeight()
    {
    return iDrawOnebyOneH;
    }

void CDrawOneByOne::SetDrawOneByOneHeight(float aValue)
    {
    iDrawOnebyOneH = aValue;
    }

TBool CDrawOneByOne::IsMagGlassOn()
    {
    return iMagGlassOn;
    }

void CDrawOneByOne::SetMagGlassStatus(TBool aValue)
    {
    iMagGlassOn = aValue;
    iMagGlass->InitDrawMagGlass();
    }

TBool CDrawOneByOne::IsMagGlassPrevStateOn()
    {
    return iMagGlassOnPrev;
    }

void CDrawOneByOne::SetMagGlassPrevStatus(TBool aValue)
    {
    iMagGlassOnPrev = aValue;
    }

CDrawMagGlass* CDrawOneByOne::GetMagGlass()
    {
    return iMagGlass;
    }

void CDrawOneByOne::KeyPressed()
    {
    iMenuAlpha = 1;
    }

void CDrawOneByOne::KeyReleased()
    {
    iMenuAlpha = 0.99;
    }

void CDrawOneByOne::KeyEvent()
    {
    //iMenuAlpha = 1;
    }

/*----------------------------------------------------------------------*/
// Initializes one by one view
//
void CDrawOneByOne::InitDrawOnebyOne(TReal aDrawZoom, TReal aInPictureX, TReal aInPictureY)
    {
    DP0_IMAGIC(_L("CDrawOneByOne::InitDrawOnebyOne"));
    
    //iDrawOneByOneXY.iX = 0;
    //iDrawOneByOneXY.iY = 0;
    iDrawOneByOneXY.iX = aInPictureX;
    iDrawOneByOneXY.iY = aInPictureY;
    iDrawOneByOneTargetXY.iX = 0;
    iDrawOneByOneTargetXY.iY = 0;
    
    //iDrawOneByOneZoom = 0.1;
    //if(aDrawZoom == 1)
        iDrawOneByOneZoom = aDrawZoom;
    /*else
        iDrawOneByOneZoom = 0.4;*/
    
    iDrawOneByOneTargetZoom = 1;
    TSize size = iContainer->Size();
    iContainer->CalculateImageSize(iDrawOnebyOneW, iDrawOnebyOneH, (float)size.iWidth/(float)size.iHeight);
    
    iZoomingStep = 0.1 * iDrawOneByOneTargetZoom;
    iZoomingSpeed = 0.2; // unno 2.5*iTimeDiff;
    iScalingSpeed = 0.3;
    iOneByOneFlow = 0;
    iMagGlassOn = EFalse;
    iMagGlassOnPrev = EFalse;
#ifdef HOLD_SELECTION_ONDRAG
    iContainer->iOneByOneSlideByDrag = EFalse;
#endif
    
    iBorder = 0.03;
    iScreenW = 0;
    iScreenH = 0;
    iInPictureX = 0;
    iInPictureY = 0;
    
    //iContainer->iDrawFunction = CImagicContainerBrowser::EOneByOne;
    iContainer->SetDrawMode(CImagicContainerBrowser::EOneByOne);
    iContainer->iKeyPressedDown=EFalse;
    //iContainer->iZoomInKey=EFalse;
    //iContainer->iZoomOutKey=EFalse;
    
    
    iMagGlass->InitDrawMagGlass();
    
    }
/*----------------------------------------------------------------------*/
//Draws one by one view
// 
void CDrawOneByOne::DrawOnebyOneL(const TSize &aSize)
    {
    DP0_IMAGIC(_L("CDrawOneByOne::DrawOnebyOne"));
    //RDebug::Print(_L("CDrawOneByOne::DrawOnebyOne"));
    
    CImageData* imageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    
    // Calculate screen size
    iContainer->CalculateImageSize(iImageWidth, iImageHeight, (float)aSize.iWidth/(float)aSize.iHeight);
    
    //Interpolate current screen size into the new one
    //if(iContainer->iLastEventFromKeys)
        {
        iContainer->Interpolate(iDrawOnebyOneW, iImageWidth, 0.55);
        iContainer->Interpolate(iDrawOnebyOneH, iImageHeight, 0.55);
        }
    /*else//faster interpolation for touch bcos slide speed is also faster
        {
        iContainer->Interpolate(iDrawOnebyOneW, iImageWidth, 0.85);
        iContainer->Interpolate(iDrawOnebyOneH, iImageHeight, 0.85);  
        }*/
    //iContainer->Interpolate(iDrawOnebyOneW, iImageWidth, 0.55);
    //iContainer->Interpolate(iDrawOnebyOneH, iImageHeight, 0.55);
    
    
    //Use orthogonal projection in OneByOne mode
    glLoadIdentity();
    glOrthof(-iDrawOnebyOneW, iDrawOnebyOneW, -iDrawOnebyOneH, iDrawOnebyOneH, -1,1);
    
    //iContainer->Interpolate(iContainer->iDisplayRotation, iContainer->iDisplayRotationTarget, 0.2);
    float tmp = iContainer->GetDisplayRotAngle();
    iContainer->Interpolate(tmp, iContainer->GetDisplayRotTargetAngle(), 0.2);
    iContainer->SetDisplayRotAngle(tmp);
    glRotatef(iContainer->GetDisplayRotAngle(), 0,0,1);
        
    // Handle keys----------------------->
    HandleMovingKeysOnebyOne();
    
    // Calculate picture size
    iContainer->CalculateImageSize(iImageWidth, iImageHeight, imageData->GetAspectRatio());
    
    //Here we check that we do not go over the picture boundaries
    CheckImageLocation();
    
    
    //Move in picture------------------------>
    iDrawOneByOneTargetXY.iX = iInPictureX / iDrawOneByOneTargetZoom;
    iDrawOneByOneTargetXY.iY = iInPictureY / iDrawOneByOneTargetZoom;

    //iMovingStep = 0.01/(iDrawOneByOneZoom/8);
    
    iContainer->Interpolate(iDrawOneByOneXY.iX,iDrawOneByOneTargetXY.iX, iMovingSpeed);
    iContainer->Interpolate(iDrawOneByOneXY.iY,iDrawOneByOneTargetXY.iY, iMovingSpeed);
    
    // Zooming-------------------------------->
    if(iContainer->iKeyPressedDown)
        {
        if(iContainer->GetKeyData().iZoomInKey)
            iDrawOneByOneTargetZoom += iZoomingStep*2;//zoom in
        else if(iContainer->GetKeyData().iZoomOutKey)
            iDrawOneByOneTargetZoom -= iZoomingStep*4;//zoom out
        }
    
    
    //Limit zooming range
    iContainer->CheckLimits(iDrawOneByOneTargetZoom, CImagicContainerBrowser::KMinOneByOneZoom, CImagicContainerBrowser::KMaxOneByOneZoom);
    
    // Interpolate to new zoom value
    if(iDrawOneByOneZoom < 0.99)
        iContainer->Interpolate(iDrawOneByOneZoom, iDrawOneByOneTargetZoom, iScalingSpeed);
    else
        iContainer->Interpolate(iDrawOneByOneZoom, iDrawOneByOneTargetZoom, iZoomingSpeed);
    
    // Rotation
    iContainer->HandleRotationKeys();
    iContainer->HandleRotation(imageData->iGridData.iRotationAngle, imageData->iGridData.iTargetRotationAngle);
    
    
    //Load image ----------------------------------->
    // Load high res picture if possible
    //if(iOneByOneFlow < IS_ALMOST_ZERO)//when not zero, images are moving
    if(iOneByOneFlow == 0)//when not zero, images are moving in oneByOne flow
        {
        iContainer->LoadHighResImage(imageData, iContainer->GetCurrentIndex());
        }
        
    
    //Bind and Draw ------------------------------------->
    //Bind to Grid low res texture index. If high res is not available, so we have always some picture to bind and draw it
    // Determine image indexes, -1 means no image
    TInt imageIndexes[3]={-1,iContainer->GetCurrentIndex(),-1};
    
    //Do not draw other images when not sliding and we are in zoomed into picture
    //if(iOneByOneFlow != 0)//when not zero, images are moving    
    //if(!iMagGlassOn || IS_NOT_IN_ZOOM_ONEBYONE)
        {
        if (iContainer->GetCurrentIndex()>0)
            imageIndexes[0]=iContainer->GetCurrentIndex()-1;
        if (iContainer->GetCurrentIndex() < iContainer->iIEngine->GetTotalNumOfImages()-1)
            imageIndexes[2]=iContainer->GetCurrentIndex()+1;
        }
    
    //Fade side images when flow goes 70% of image width
    if(Abs(iOneByOneFlow) > 0.25)
        {
        iFadeColor=1;
        glColor4f(iFadeColor,iFadeColor,iFadeColor, 1);
        }
    if(iOneByOneFlow < 0.5)
        {
        //if(iContainer->iLastEventFromKeys)
            iContainer->Interpolate(iFadeColor, 0, 0.25);
        /*else
            iContainer->Interpolate(iFadeColor, 0, 0.35);*/
        
        glColor4f(iFadeColor,iFadeColor,iFadeColor, 1);
        }
    
    /*if(iFadeColor == 0)
        {
        // Determine image indexes, -1 means no image
        TInt imageIndexes[3]={-1,iContainer->GetCurrentIndex(),-1};
        }*/
    
    //Move to zero coordinate, which is selected image coordinate
#ifdef HOLD_SELECTION_ONDRAG
    //Stop interpolation when user is dragging
    if(iContainer->iHoldSelection == EFalse)
        {
#endif
        if(iContainer->iLastEventFromKeys)
            iContainer->Interpolate(iOneByOneFlow, 0, KOneByOneSlideSpeed);//sliding speed
        else
            iContainer->Interpolate(iOneByOneFlow, 0, KOneByOneSlideSpeed*1.3);//sliding speed
        }
    
    //glColor4f(1,1,1, 1);
    glPushMatrix();
    glScalef(iDrawOneByOneZoom, iDrawOneByOneZoom, iDrawOneByOneZoom);
    //Move to first image location (= current-1)
    glTranslatef(iOneByOneFlow-iContainer->KOneByOneSpacing,0,0);
    
    TInt currentIndexTexture=0;
    for(TInt i=0; i<3; i++)
        {
        // Check that image index is valid
        if (imageIndexes[i]!=-1)
            {
            //Bind to best picture
            CImageData* data=iContainer->iIEngine->GetImageData(imageIndexes[i]);
            if(i==1)
                {
                iContainer->iCurrentBindedIndex = data->iGridData.BestImage();
                //Bind to 512 res image when not zoomed for better image scaling quality
                if(IS_NOT_IN_ZOOM_ONEBYONE && data->iGridData.iGlHQ512TextIndex!=0 && !iMagGlassOn) 
                    iContainer->iCurrentBindedIndex = data->iGridData.iGlHQ512TextIndex;
                }
            //Side images gets only 128x128 tn because of perf reason
            else
                iContainer->iCurrentBindedIndex = data->iGridData.iGlLQ128TextIndex;
            if(!iContainer->iCurrentBindedIndex)
                iContainer->iCurrentBindedIndex = data->iGridData.iGlLQ32TextIndex;
            
            
            //If no images ready, bind with loading tn
            if (iContainer->iCurrentBindedIndex == 0)
                iContainer->iCurrentBindedIndex = iContainer->iLoadingTextureIndex;
            
            glBindTexture(GL_TEXTURE_2D, iContainer->iCurrentBindedIndex);
            
            if(i==1){
                currentIndexTexture=iContainer->iCurrentBindedIndex;
            }
            iContainer->SetMinMagFilterLinearDo(iContainer->iMinMagFilterSetting);
            
            //Store matrix and rotate picture
            glPushMatrix();
               
            if(i==1 && iMagGlassOn)
                {
                glColor4f(1,1,1, 1);
                glTranslatef(iDrawOneByOneXY.iX, -iDrawOneByOneXY.iY, 0);
                glRotatef(data->iGridData.iRotationAngle, 0,0,1);
                iMagGlass->DrawMagGlass(aSize, data->GetAspectRatio());
                }
            else
                {
                //Set Draw color to white when drawing selection or if we drag images 
                if(i==1 || (iContainer->iHoldSelection && !iMagGlassOn))
                    glColor4f(1,1,1, 1);
                else//othervise "fade" images
                    glColor4f(iFadeColor,iFadeColor,iFadeColor, 1);
                
                glTranslatef(-iDrawOneByOneXY.iX, iDrawOneByOneXY.iY, 0);
                glRotatef(data->iGridData.iRotationAngle, 0,0,1);
                // Set vertixes and draw
                GLfixed vertices[8];
                iContainer->SetPictureVertices(data, vertices);
                glVertexPointer( 2, GL_FIXED, 0, vertices );
                glDrawArrays(GL_TRIANGLE_STRIP,0,4);
#ifdef EMPTY_IMAGE_AS_WIREFRAME
                // Draw black frame with white borders
                if (iContainer->iCurrentBindedIndex == 0)
                    {
                    glScalef(0.92, 0.92, 0.92);
                    glColor4f(0,0,0,1);
                    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
                    }
#endif                
                
                }

#ifdef RD_FACEFRAME
            //if (i==1 && iDrawFaceFrame) DrawFaceFrame();
#endif
            
            //Remove rotation
            glPopMatrix();
            }
        // Move to next image position
        glTranslatef(iContainer->KOneByOneSpacing,0,0);
        }
    glPopMatrix();

    if(currentIndexTexture!=iContainer->iCurrentBindedIndex)
        {
        iContainer->iCurrentBindedIndex = currentIndexTexture;
        glBindTexture(GL_TEXTURE_2D, iContainer->iCurrentBindedIndex);
        iContainer->SetMinMagFilterLinearDo(iContainer->iMinMagFilterSetting);
        }

#ifdef SCR_DRAW_DEBUG
    iDrawUtility->Update();
    iDrawUtility->Draw(Size());
#endif
    
    
    
    //iContainer->iDrawUtility->DrawMenuIndicators(iContainer->Size());
    
    
#ifdef RD_ZOOMICON
    //Draw moving direction arrays when sliding in OnByOne mode
    if(!iMagGlassOn && iFadeColor!=0 || iOneByOneFlow != 0)
        iContainer->iDrawUtility->DrawMovingArrow(ETrue, EFalse, iContainer->Size());
    
    //Draw moving direction arrays when zoomed in
    if (!IS_NOT_IN_ZOOM_ONEBYONE && 
        (iDrawOneByOneXY.iX != iDrawOneByOneTargetXY.iX || 
         iDrawOneByOneXY.iY != iDrawOneByOneTargetXY.iY || 
         iDrawOneByOneZoom != iDrawOneByOneTargetZoom ||
         imageData->iGridData.iRotationAngle != imageData->iGridData.iTargetRotationAngle ||
         iContainer->iOnTheEdge))
        {
        /*if(!iMagGlassOn)
            {
            iContainer->iDrawUtility->DrawMovingArrow(ETrue, !IS_NOT_IN_ZOOM_ONEBYONE, iContainer->Size());
            }*/
        
        iContainer->iDrawUtility->DrawZoomIcon( imageData,
                                                iContainer->Size(), 
                                                -iDrawOneByOneXY.iX, 
                                                iDrawOneByOneXY.iY,
                                                iDrawOnebyOneW/iImageWidth, 
                                                iDrawOnebyOneH/iImageHeight,
                                                ETrue);
        }
#endif    

#if 0
    if(iMenuAlpha < 1)
        {
        iMenuAlpha-=0.3;
        if(iMenuAlpha < 0)
            iMenuAlpha = 0;
        }

    iContainer->iDrawUtility->DrawIcon2(iContainer->Size(), iContainer->iMenuTextureIndex, iMenuAlpha);
#endif   
    iContainer->DynamicLoadingL();
    }


TBool CDrawOneByOne::IsDrawingNeededOneByOne()
    {
    CImageData* imageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    
#if 0
    if(iMenuAlpha != 0)
        {
        return ETrue;
        }
#endif
    
    if(Abs(iOneByOneFlow) > 0.01)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 2"));
        iContainer->SetMinMagFilterLinear(EFalse);
        return ETrue;
        }
    else if(iOneByOneFlow!=0)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 3"));
        iOneByOneFlow=0;
        //if(imageData->iGridData.iGlHQ512TextIndex != 0)
        //iContainer->iDrawOneMoreTime = ETrue;
        iContainer->SetMinMagFilterLinear(ETrue);
        return ETrue;
        }
    
    if(iMagGlassOnPrev != iMagGlassOn)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 4"));
        iMagGlassOnPrev = iMagGlassOn;
        //SetMinMagFilterLinear(ETrue);
        //iContainer->iDrawOneMoreTime = ETrue;
        return ETrue;
        }
    
    if(iMagGlassOn && (iMagGlass->GetMagGlassZoomFactor() < KMaxMagGlassZoomFactor-0.1))
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 5"));
        DP1_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - magGlass: %f"), iMagGlass->GetMagGlassZoomFactor());
        iMagGlassOnPrev = iMagGlassOn;
        iContainer->SetMinMagFilterLinear(EFalse);
        return ETrue;
        }
        
    if(Abs(iContainer->GetDisplayRotAngle() - iContainer->GetDisplayRotTargetAngle()) > 1)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 6"));
        iContainer->SetMinMagFilterLinear(EFalse);
        return ETrue;
        }
    else if(iContainer->GetDisplayRotAngle() != iContainer->GetDisplayRotTargetAngle())
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 6.1"));
        iContainer->SetDisplayRotAngle(iContainer->GetDisplayRotTargetAngle());
        iContainer->SetMinMagFilterLinear(ETrue);
        return ETrue;
        }
    
    if(Abs(imageData->iGridData.iRotationAngle - imageData->iGridData.iTargetRotationAngle) > 1)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 7"));
        iContainer->SetMinMagFilterLinear(EFalse);
        return ETrue;
        }
    else if(imageData->iGridData.iRotationAngle != imageData->iGridData.iTargetRotationAngle)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 7.1"));
        imageData->iGridData.iRotationAngle = imageData->iGridData.iTargetRotationAngle;
        iContainer->SetMinMagFilterLinear(ETrue);
        return ETrue;
        }
    
    if(Abs(iDrawOneByOneZoom - iDrawOneByOneTargetZoom) > 0.01)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 9"));
        iContainer->SetMinMagFilterLinear(EFalse);
        return ETrue;
        }
    else if(iDrawOneByOneZoom != iDrawOneByOneTargetZoom)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 9.1"));
        iDrawOneByOneZoom = iDrawOneByOneTargetZoom;
        //iContainer->iDrawOneMoreTime = ETrue;
        iContainer->SetMinMagFilterLinear(ETrue);
        return ETrue;
        }

    if(Abs(iDrawOneByOneXY.iX-iDrawOneByOneTargetXY.iX) > 0.001)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 11"));
        iContainer->SetMinMagFilterLinear(EFalse);
        return ETrue;
        }
    else if(iDrawOneByOneXY.iX != iDrawOneByOneTargetXY.iX)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 11.1"));
        iDrawOneByOneXY.iX = iDrawOneByOneTargetXY.iX;
        //iContainer->iDrawOneMoreTime = ETrue;
        iContainer->SetMinMagFilterLinear(ETrue);
        return ETrue;
        }
    
    if(Abs(iDrawOneByOneXY.iY-iDrawOneByOneTargetXY.iY) > 0.001)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 12"));
        iContainer->SetMinMagFilterLinear(EFalse);
        return ETrue;
        }
    else if(iDrawOneByOneXY.iY != iDrawOneByOneTargetXY.iY)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 12.1"));
        iDrawOneByOneXY.iY = iDrawOneByOneTargetXY.iY;
        //iContainer->iDrawOneMoreTime = ETrue;
        iContainer->SetMinMagFilterLinear(ETrue);
        return ETrue;
        }
    
    /*if(iPreviousTexNum != iCurrentTexNum && !iDrawOneMoreTime)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 13.0"));
        //iDrawOneMoreTime = ETrue;
        SetMinMagFilterLinear(ETrue);
        return ETrue;
        }*/
    
    /*if(imageData->GetAspectRatio() > iScreenAspectRatio && (iDrawOneByOneZoom-1) < 0.01)
        if(Abs(iDrawOnebyOneW-iImageWidth*2) > 0.01)
            {
            DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 13.1"));
            DP2_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - iDrawOnebyOneW: %f, iImageWidth: %f"),iDrawOnebyOneW, iImageWidth);
            SetMinMagFilterLinear(EFalse);
            //iDrawOneMoreTime = ETrue;
            return ETrue;
            }
        else if(iDrawOnebyOneW != iImageWidth*2)
            {
            DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 13.2"));
            DP2_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - iDrawOnebyOneW: %f, iImageWidth: %f"),iDrawOnebyOneW, iImageWidth);
            iDrawOnebyOneW = iImageWidth*2;
            iDrawOneMoreTime = ETrue;
            SetMinMagFilterLinear(ETrue);
            return ETrue;
            }*/
    
    /*
    if(imageData->GetAspectRatio() < iContainer->iScreenAspectRatio && (iDrawOneByOneZoom-1) < 0.01)
        if(Abs(iDrawOnebyOneH-iImageHeight) > 0.01)
            {
            DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 13.3"));
            DP2_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - iDrawOnebyOneH: %f, iImageHeight: %f"),iDrawOnebyOneH, iImageHeight);
            iContainer->SetMinMagFilterLinear(EFalse);
            //iDrawOneMoreTime = ETrue;
            return ETrue;
            }
        else if(Abs(iDrawOnebyOneH - iImageHeight) > IS_ALMOST_ZERO)
            {
            DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 13.4"));
            DP2_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - iDrawOnebyOneH: %f, iImageHeight: %f"),iDrawOnebyOneH, iImageHeight);
            iDrawOnebyOneH = iImageHeight;
            //iContainer->iDrawOneMoreTime = ETrue;
            iContainer->SetMinMagFilterLinear(ETrue);
            return ETrue;
            }
    */
    
    /*if(iContainer->iDrawOneMoreTime)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 14"));
        iContainer->iDrawOneMoreTime = EFalse;
        iContainer->SetMinMagFilterLinear(ETrue);//Draw one more time to change for Linear rendering mode
        return ETrue;
        }*/
    
    if(!iContainer->iMagFilterLinear)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 15"));
        iContainer->SetMinMagFilterLinear(ETrue);
        //iContainer->iDrawOneMoreTime = ETrue;
        }
        
    if(Abs(iOneByOneFlow) < 0.7)
        if(iFadeColor > 0.1)
            {
            DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 16"));
            iContainer->SetMinMagFilterLinear(EFalse);
            return ETrue;
            }
        else if(iFadeColor!=0)
            {
            DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 17"));
            iFadeColor = 0;
            iContainer->SetMinMagFilterLinear(ETrue);
            return ETrue;
            }

    if(iContainer->iDrawNow)
        {
        DP0_IMAGIC(_L("CDrawOneByOne::IsDrawingNeededOneByOne - 1"));
        if(iMagGlassOn)
            {
            if(!iContainer->IsTouchPointThresholdExeed())//moved >2 pixel
                iContainer->SetMinMagFilterLinear(ETrue);
            else
                iContainer->SetMinMagFilterLinear(EFalse);
            }
        else
            {
            iContainer->SetMinMagFilterLinear(ETrue);
            }
        
        iContainer->iDrawNow = EFalse;
        return ETrue;
        }
    
    return EFalse;
    }

//Here we check that we do not go over the picture boundaries
void CDrawOneByOne::CheckImageLocation(/*float& aImageWidth, float& aImageHeight*/)
    {
    
    CImageData* imageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    
    // Change landscape<->portrait if pic is rotated 90 or 270
    TInt angle = imageData->iGridData.iTargetRotationAngle;
    if(angle % 90 == 0 && angle % 180 != 0)
        {
        if(imageData->GetAspectRatio() > 1)
            {
            /*iImageWidth /= imageData->GetAspectRatio();
            iImageWidth -= (iImageWidth-iDrawOnebyOneW);*/
            iImageHeight *= imageData->GetAspectRatio();
            iImageHeight -= (iImageHeight-iDrawOnebyOneH);
            iImageWidth = iImageHeight / imageData->GetAspectRatio();
            
            }
        else
            {
            /*iImageWidth/= imageData->GetAspectRatio();
            iImageWidth-= (iImageWidth-iDrawOnebyOneW);*/
            iImageHeight *= imageData->GetAspectRatio();
            iImageHeight -= (iImageHeight-iDrawOnebyOneH);
            iImageWidth = iImageHeight / imageData->GetAspectRatio();
            }
        }
    
    
    iImageWidth *= iDrawOneByOneTargetZoom;
    iImageHeight *= iDrawOneByOneTargetZoom;
    
    //Calculate location/coordinates in screen
    iInPictureX = iDrawOneByOneTargetXY.iX * iDrawOneByOneTargetZoom; 
    iInPictureY = iDrawOneByOneTargetXY.iY * iDrawOneByOneTargetZoom;
        
    //Lets move in picture little bit over the border
    if(iDrawOneByOneTargetZoom==1)
        {
        iBorder=0;
        iDrawOneByOneTargetXY.iX = 0;
        iDrawOneByOneTargetXY.iY = 0;
        }
    else
		{
        iBorder=0.015; // was 0.1. Changed to 0 to limit just at the edge of image in zoom
		}
    
    //Calculate screen size
    iScreenW = iDrawOnebyOneW - iBorder;
    iScreenH = iDrawOnebyOneH - iBorder;
        
        
    if (iImageWidth > iScreenW)
        {
        if (iInPictureX-iScreenW < -iImageWidth)
            iInPictureX=-iImageWidth+iScreenW;
        if (iInPictureX+iScreenW > iImageWidth)
            iInPictureX=iImageWidth-iScreenW;
        }
    else
        {
        iInPictureX=0;
        }
    if (iImageHeight > iScreenH)
        {
        if (iInPictureY-iScreenH < -iImageHeight)
            iInPictureY=-iImageHeight+iScreenH;
        if (iInPictureY+iScreenH > iImageHeight)
            iInPictureY=iImageHeight-iScreenH;
        }
    else
        {
        iInPictureY=0;
        }
    
    if(IS_NOT_IN_ZOOM_ONEBYONE)
        {
        iInPictureY=0;
        iInPictureX=0;
        }
    }

void CDrawOneByOne::HandleMovingKeysOnebyOne()
    {
    // Handle keys----------------------->
    CKeyData keyData = iContainer->GetKeyData();
    CKeyData touchData = iContainer->GetTouchData();
    iFlowThresHold=0.8;
    
    //Handle keydata and touchdata
    ChangeDrawOneByOneTargetX(keyData.iRight * GetMovingStep());
    ChangeDrawOneByOneTargetX(-keyData.iLeft * GetMovingStep());
    ChangeDrawOneByOneTargetY(keyData.iDown * GetMovingStep());
    ChangeDrawOneByOneTargetY(-keyData.iUp * GetMovingStep());
    
    ChangeDrawOneByOneTargetX(touchData.iRight * GetMovingStep());
    ChangeDrawOneByOneTargetX(-touchData.iLeft * GetMovingStep());
    ChangeDrawOneByOneTargetY(touchData.iDown * GetMovingStep());
    ChangeDrawOneByOneTargetY(-touchData.iUp * GetMovingStep());
    
    if(touchData.iRight || touchData.iLeft || touchData.iDown || touchData.iUp)
        //iMovingSpeed = 2.2;
        iMovingSpeed = 100;
    else
        iMovingSpeed = 0.15;
        //iMovingSpeed = 0.01;
        
    //Calculate new index from movement keys to change picture in onebyone mode
    //but only if zooming is not done
    //And we have started sliding/moving
#ifdef HOLD_SELECTION_ONDRAG
    if(GetDrawOneByOneTargetZoom()==1 && 
       //((Abs(GetImgeFlowLocation())<0.99) || 
       ((Abs(GetImgeFlowLocation())<iFlowThresHold) ||
       iContainer->GetSlideByDragValue()) &&
       !IsMagGlassOn())
       
#else
    if(iDrawOneByOneTargetZoom==1 && Abs(iOneByOneFlow)<0.2 && !iMagGlassOn)
#endif
        {
        iContainer->SetCurrentIndex(iContainer->GetCurrentIndex() + keyData.iY);
        iContainer->SetCurrentIndex(iContainer->GetCurrentIndex() + keyData.iX);
        
        iContainer->SetCurrentIndex(iContainer->GetCurrentIndex() + touchData.iY);
        iContainer->SetCurrentIndex(iContainer->GetCurrentIndex() + touchData.iX);
        }
    
    // Check that index is in grid area
    while(iContainer->GetCurrentIndex() >= iContainer->iIEngine->GetTotalNumOfImages())
        iContainer->SetCurrentIndex(iContainer->GetCurrentIndex() - iContainer->iIEngine->GetTotalNumOfImages());
    while(iContainer->GetCurrentIndex() < 0)
        iContainer->SetCurrentIndex(iContainer->GetCurrentIndex() + iContainer->iIEngine->GetTotalNumOfImages());
    
    // Update selected picture index
    if(iContainer->GetPrevIndex() != iContainer->GetCurrentIndex())
        {
        //iContainer->iTextureLoader->ReleaseHQ512Textures();
        
        //iMagGlassOn = EFalse;
        SetMagGlassStatus(EFalse);
        
        iContainer->iImagicAppUi->SetImageIndex(iContainer->GetCurrentIndex());
        
        // Set new flow coordinate
        if (keyData.iLeft || touchData.iLeft)
            {
            float tmp = GetImgeFlowLocation();
            tmp-=CImagicContainerBrowser::KOneByOneSpacing;
            SetImgeFlowLocation(tmp);
            //iOneByOneFlow-=KOneByOneSpacing;
            }
        if (keyData.iRight || touchData.iRight)
            {
            float tmp = GetImgeFlowLocation();
            tmp+=CImagicContainerBrowser::KOneByOneSpacing;
            SetImgeFlowLocation(tmp);
            //iOneByOneFlow+=KOneByOneSpacing;
            }
        
        //Cancel SuperZoom image loading, if it was started
        iContainer->iIEngine->CancelFullSizeLoading();
        }
    iContainer->SetPrevIndex(iContainer->GetCurrentIndex());
    
    iContainer->ResetKeyData();
    iContainer->ResetTouchData();
    }
