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

#include "CDrawFaceBrowsing.h"
#include "TextureLoader.h"
#include "DrawUtility.h"
#include "ImagicConsts.h"

#define IS_ALMOST_ZERO (0.001)


CDrawFaceBrowsing::CDrawFaceBrowsing(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex):
    iContainer(aContainer)//,
    //iCurrentIndex(aCurrentIndex)
    {
    // No implementation required
    }

CDrawFaceBrowsing::~CDrawFaceBrowsing()
    {
    iCoordinates.Close();
    iFloatCoordinates.Close();
    }

CDrawFaceBrowsing* CDrawFaceBrowsing::NewLC(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex)
    {
    CDrawFaceBrowsing* self = new (ELeave) CDrawFaceBrowsing(aContainer,aCurrentIndex);
    CleanupStack::PushL(self);
    self->ConstructL(aContainer,aCurrentIndex);
    return self;
    }

CDrawFaceBrowsing* CDrawFaceBrowsing::NewL(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex)
    {
    CDrawFaceBrowsing* self = CDrawFaceBrowsing::NewLC(aContainer,aCurrentIndex);
    CleanupStack::Pop(); // self;
    return self;
    }

void CDrawFaceBrowsing::ConstructL(CImagicContainerBrowser* aContainer, TInt& aCurrentIndex)
    {
    //iContainer = aContainer;
    }

void CDrawFaceBrowsing::KeyPressed()
    {
    iMenuAlpha = 1;
    }

void CDrawFaceBrowsing::KeyReleased()
    {
    iMenuAlpha = 0.99;
    }

void CDrawFaceBrowsing::KeyEvent()
    {
    //iMenuAlpha = 1;
    }

void CDrawFaceBrowsing::GetFBZoomAndLocation(TReal& aDrawZoom, TReal& aInPictureX, TReal& aInPictureY)
    {
    aDrawZoom = iDrawZoom;
    aInPictureX = iInPictureX;
    aInPictureY = iInPictureY;
    }



void CDrawFaceBrowsing::InitFaceBrowsing()
    {
    //iFBMovingSpeed = 1.4*iContainer->iTimeDiff;
    iFBMovingSpeed = 0.1;
    iFBRectCounter = 0;
    //iContainer->iDrawOneByOneTargetZoom=1;
    iDrawZoom=1;
    iDrawFBTargetZoom=1;
    iFBZoomingSpeed = 1.2*iContainer->iTimeDiff;
    iDrawX=0;
    iDrawY=0;
    iDrawTargetX=0;
    iDrawTargetY=0;
    //iInPictureX, iInPictureY
    
    }


/*----------------------------------------------------------------------*/
// Draws FaceBrowser view
//
void CDrawFaceBrowsing::DrawFaceBrowsing(const TSize &aSize)
    {
    DP0_IMAGIC(_L("CDrawFaceBrowsing::DrawFaceBrowsing"));

    CImageData* imageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    
    
#ifdef SUPERZOOM
    if(imageData->iGridData.iGlSuperHQTextIndex == 0)
        {
        TRAPD(err, iContainer->iTextureLoader->LoadL(imageData, EFullSize));
        }
#endif
    
    //Calculate screen size
    iContainer->CalculateImageSize(iImageWidth, iImageHeight, (float)aSize.iWidth/(float)aSize.iHeight);
    
    if(imageData->iGridData.iRotationAngle != imageData->iGridData.iTargetRotationAngle)
        iContainer->HandleRotation(imageData->iGridData.iRotationAngle, imageData->iGridData.iTargetRotationAngle);
        
    
    //Interpolate current screen size into the new one
    iContainer->Interpolate(iDrawWidth, iImageWidth, 0.75);
    iContainer->Interpolate(iDrawHeight, iImageHeight, 0.75);
    
    //Set orthigonal projection
    glLoadIdentity();
    glOrthof(-iDrawWidth,iDrawWidth, -iDrawHeight, iDrawHeight, -1,1);
    
    //iContainer->Interpolate(iContainer->iDisplayRotation, iContainer->iDisplayRotationTarget, 0.2);
    float tmp = iContainer->GetDisplayRotAngle();
    iContainer->Interpolate(tmp, iContainer->GetDisplayRotTargetAngle(), 0.2);
    iContainer->SetDisplayRotAngle(tmp);
    glRotatef(iContainer->GetDisplayRotAngle(), 0,0,1);
    
    
    //Handle coordinates----------------------->
    if(iFaceNro < 0)
        iFaceNro = iCoordinates.Count()-1;
    if(iFaceNro >= iCoordinates.Count())
        iFaceNro = 0;
    
    
    // Interpolate to new zoom value
    if( iContainer->iView->GetFaceBrowsingMode() != EFaceBrowsingShowRect )
        iContainer->Interpolate(iDrawZoom, iDrawFBTargetZoom, iFBZoomingSpeed);
    
    //Convert coordinates from corner to center of screen
    if(iCoordinates.Count() >= 1)
        {
        //Convert integer coords to OpenGL float coords and fill array
        for(TInt i=0; iFloatCoordinates.Count() < iCoordinates.Count();i++)
            {
            iFloatCoordinates.Append(ConvertCoordsFromAlgo2OGl(iCoordIndex));
            iCoordIndex++;
            }
        
        iDrawTargetX = iFloatCoordinates[iFaceNro].iX;
        iDrawTargetY = iFloatCoordinates[iFaceNro].iY;
        
        //Calculate face width and use that for zooming in factor
        float faceWidth = (iCoordinates[iFaceNro].iBr.iX - iCoordinates[iFaceNro].iTl.iX);
        float zoomFactor = faceWidth/20;
        
        if(iContainer->GetScreenOrientation())
            iDrawFBTargetZoom = 4.6;
        else
            iDrawFBTargetZoom = 5.4;

        iDrawFBTargetZoom /= zoomFactor;
        if(iDrawFBTargetZoom <=1.6)
            iDrawFBTargetZoom = 1.6;
        
        }
    
    /*iContainer->iTouchMoveData.iX=0;
    iContainer->iTouchMoveData.iY=0;*///mika. to test doe this make anything at all????
    
    
    // Calculate picture size
    iContainer->CalculateImageSize(iImageWidth, iImageHeight, imageData->GetAspectRatio());
    
    iImageWidth*=iDrawFBTargetZoom;
    iImageHeight*=iDrawFBTargetZoom;
    
    //Calculate location/coordinates in screen
    iInPictureX = iDrawTargetX * iDrawFBTargetZoom;
    iInPictureY = iDrawTargetY * iDrawFBTargetZoom;
    
    iDrawTargetX = iInPictureX / iDrawFBTargetZoom;
    iDrawTargetY = iInPictureY / iDrawFBTargetZoom;
    
    if(!iContainer->GetScreenOrientation())
        {
        float temp = iDrawTargetX;
        iDrawTargetX = iDrawTargetY;
        iDrawTargetY = -temp ;
        }
        
    // Move in picture------------------------>
    if( iContainer->iView->GetFaceBrowsingMode() != EFaceBrowsingShowRect )
        {
        TInt imageRotation = 0 - (TReal)imageData->GetOrientation();
        imageRotation%=360;
        
        if(imageRotation == 0)
            {
            iContainer->Interpolate(iDrawX,iDrawTargetX, iFBMovingSpeed);
            iContainer->Interpolate(iDrawY,iDrawTargetY, iFBMovingSpeed);
            }
        else if(imageRotation == -90)
            {
            iContainer->Interpolate(iDrawX,iDrawTargetY, iFBMovingSpeed);
            iContainer->Interpolate(iDrawY,-iDrawTargetX, iFBMovingSpeed);
            }
        else if(imageRotation == -180)
            {
            iContainer->Interpolate(iDrawX,-iDrawTargetX, iFBMovingSpeed);
            iContainer->Interpolate(iDrawY,-iDrawTargetY, iFBMovingSpeed);
            }
        else if(imageRotation == -270)
            {
            iContainer->Interpolate(iDrawX,-iDrawTargetY, iFBMovingSpeed);
            iContainer->Interpolate(iDrawY,iDrawTargetX, iFBMovingSpeed);
            }
        }
    
    iContainer->iCurrentBindedIndex = imageData->iGridData.BestImage();
    glBindTexture( GL_TEXTURE_2D, iContainer->iCurrentBindedIndex);
    iContainer->SetMinMagFilterLinearDo(iContainer->iMinMagFilterSetting);
    
    // Calculate picture vertices
    GLfixed vertices[8];
    iContainer->SetPictureVertices(imageData, vertices);
    glVertexPointer( 2, GL_FIXED, 0, vertices );
    glColor4f(1,1,1, 1);
    glPushMatrix();
    glScalef(iDrawZoom, iDrawZoom, iDrawZoom);
    glTranslatef(-iDrawX, iDrawY, 0);
    glRotatef(imageData->iGridData.iRotationAngle, 0,0,1);
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);

#ifdef RD_FACEFRAME
    if( iContainer->iView->GetFaceBrowsingMode() == EFaceBrowsingShowRect )
        DrawFaceFrame(-1);
        
    if(ShowUtilities())
        DrawFaceFrame(iFaceNro);
    
    //Draw face rects for ~ 0.5s time
    if(iFBRectCounter > 6)
        iContainer->iView->SetFaceBrowsingMode(EFaceBrowsing);
    else
        iFBRectCounter++;
#endif

    glPopMatrix();
    
#ifdef RD_ZOOMICON
    if(ShowUtilities())
        {
        //Draw moving direction arrays
        if(iFloatCoordinates.Count() >1)
            {
            TInt imageRotation = 0 - (TReal)imageData->GetOrientation();
            
            if(iContainer->GetScreenOrientation())
                {
                if(imageRotation == 0 || imageRotation == -180)
                    {
                    iContainer->iDrawUtility->DrawMovingArrow(ETrue, EFalse, iContainer->Size());
                    }
                else if(imageRotation == -90 || imageRotation == -270)
                    {
                    iContainer->iDrawUtility->DrawMovingArrow(EFalse, ETrue, iContainer->Size());
                    }    
                }
            
            if(!iContainer->GetScreenOrientation())
                {
                if(imageRotation == 0 || imageRotation == -180)
                    {
                    iContainer->iDrawUtility->DrawMovingArrow(EFalse, ETrue, iContainer->Size());
                    }
                else if(imageRotation == -90 || imageRotation == -270)
                    {
                    iContainer->iDrawUtility->DrawMovingArrow(EFalse, ETrue, iContainer->Size());
                    }    
                }
            
            //iContainer->iDrawUtility->DrawMovingArrow(iContainer->GetScreenOrientation(), !iContainer->GetScreenOrientation(), iContainer->Size());
            }
            
            
        iContainer->iDrawUtility->DrawZoomIcon( imageData,
                                                iContainer->Size(), 
                                                -iDrawX, 
                                                iDrawY,
                                                iDrawWidth/iImageWidth, 
                                                iDrawHeight/iImageHeight,
                                                iContainer->iView->GetFaceBrowsingMode()==EFaceBrowsing);
        }
    
#endif
    
#if 0
    if(iMenuAlpha < 1)
        {
        iMenuAlpha-=0.2;
        if(iMenuAlpha < 0)
            iMenuAlpha = 0;
        }

    iContainer->iDrawUtility->DrawIcon2(iContainer->Size(), iContainer->iLoadingTextureIndex, iMenuAlpha);
#endif   
    }


TBool CDrawFaceBrowsing::ShowUtilities()
    {
    CImageData* imageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    TInt imageRotation = 0 - (TReal)imageData->GetOrientation();
    
    if(imageRotation == -90 || imageRotation == -270)
        {
        if(Abs(iDrawX) != Abs(iDrawTargetY) || 
           Abs(iDrawY) != Abs(iDrawTargetX) || 
           iDrawZoom != iDrawFBTargetZoom ||
           Abs(imageData->iGridData.iRotationAngle) != Abs(imageData->iGridData.iTargetRotationAngle) )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    else
        {
        if(Abs(iDrawX) != Abs(iDrawTargetX) || 
           Abs(iDrawY) != Abs(iDrawTargetY) || 
           iDrawZoom != iDrawFBTargetZoom ||
           Abs(imageData->iGridData.iRotationAngle) != Abs(imageData->iGridData.iTargetRotationAngle) )
            {
            return ETrue;
            }
        else
            {
            return EFalse;
            }
        }
    }

void CDrawFaceBrowsing::DrawFaceFrame(TInt aFace2bDrawn)
    {
    DP0_IMAGIC(_L("CDrawFaceBrowsing::DrawFaceFrame++"));

    
    CImageData* imageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    RArray<TRect> facecoords;
    facecoords.Reset();
    
    //if(imageData->IsImageReady(ESize512x512))
        {
        for(TInt i=0; i< iCoordinates.Count();i++)
            facecoords.Append(iCoordinates[i]);    
        
        
        GLfixed vertices[8];
        glPushMatrix();
        glDisable(GL_TEXTURE_2D);
        glTranslatef(0, 0, 0.01f);
        glColor4f(0,0.7,0,1);
        glLineWidth(3.0f);
        glVertexPointer(2, GL_FIXED, 0, vertices);

        for(TInt i=0; i< facecoords.Count();i++)
            {
            GLfixed vx = (1<<15) * Abs(facecoords[i].iBr.iX + facecoords[i].iTl.iX);
            GLfixed vy = (1<<15) * Abs(facecoords[i].iBr.iY + facecoords[i].iTl.iY);
            GLfixed fW = (1<<16) * Abs(facecoords[i].iBr.iX - facecoords[i].iTl.iX);
            GLfixed fH = (1<<16) * Abs(facecoords[i].iBr.iY - facecoords[i].iTl.iY);

            DP5_IMAGIC(_L("CDrawFaceBrowsing::DrawFaceFrame face:%d (%d,%d), W/H=%d,%d"), i, vx, vy, fW, fH);
            
            vx /= 320; // Coords are given in 320x320 coords
            vy /= 320; // convert to OpenGL 1.0x1.0 coords
            fW /= 320;
            fH /= 320;

            float ar = imageData->GetAspectRatio();

            if(ar > 1)
                {
                vx -= (1<<15);
                vy -= (0.5/ar)*(1<<16);
                }
            else
                {
                vx -= (0.5*ar)*(1<<16);
                vy -= (1<<15);
                }

            GLfixed x1 =  vx-fW/2, x2 =  vx+fW/2;
            GLfixed y1 = -vy-fH/2, y2 = -vy+fH/2; // -vy since y-coord in OpenGL is so

            DP5_IMAGIC(_L("CDrawFaceBrowsing::DrawFaceFrame face:%d (%d,%d), W/H=%d,%d"), i, vx, vy, fW, fH);
            DP4_IMAGIC(_L("CDrawFaceBrowsing::DrawFaceFrame xy12 (%d,%d), (%d,%d)"), x1, y1, x2, y2);

            vertices[0*2+0] = x1;    vertices[0*2+1] = y1;
            vertices[1*2+0] = x2;    vertices[1*2+1] = y1;
            vertices[2*2+0] = x2;    vertices[2*2+1] = y2;
            vertices[3*2+0] = x1;    vertices[3*2+1] = y2;
            
            if(i == aFace2bDrawn || aFace2bDrawn == -1)
                glDrawArrays(GL_LINE_LOOP,0,4);
            }

        glColor4f(1,1,1,1);
        glEnable(GL_TEXTURE_2D);
        glPopMatrix();
        }

    DP0_IMAGIC(_L("CDrawFaceBrowsing::DrawFaceFrame--"));
    }


TBool CDrawFaceBrowsing::IsDrawingNeededFaceBrowsing()
    {
    CImageData* imageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    
    if(Abs(iContainer->GetDisplayRotAngle() - iContainer->GetDisplayRotTargetAngle()) > 1)
        {
        DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 6"));
        iContainer->SetMinMagFilterLinear(EFalse);
        return ETrue;
        }
    else if(iContainer->GetDisplayRotAngle() != iContainer->GetDisplayRotTargetAngle())
        {
        DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 6.1"));
        //iContainer->iDisplayRotation = iContainer->iDisplayRotationTarget;
        iContainer->SetDisplayRotAngle(iContainer->GetDisplayRotTargetAngle());
        //iContainer->iDrawOneMoreTime = ETrue;
        iContainer->SetMinMagFilterLinear(ETrue);
        return ETrue;
        }
    
    if(Abs(imageData->iGridData.iRotationAngle - imageData->iGridData.iTargetRotationAngle) > 1)
        {
        DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 7"));
        iContainer->SetMinMagFilterLinear(EFalse);
        return ETrue;
        }
    else if(imageData->iGridData.iRotationAngle != imageData->iGridData.iTargetRotationAngle)
        {
        DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 7.1"));
        imageData->iGridData.iRotationAngle = imageData->iGridData.iTargetRotationAngle;
        iContainer->SetMinMagFilterLinear(ETrue);
        //iContainer->iDrawOneMoreTime = ETrue;
        return ETrue;
        }
    
    if(Abs(iDrawZoom - iDrawFBTargetZoom) > 0.01)
        {
        DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 10"));
        iContainer->SetMinMagFilterLinear(EFalse);
        return ETrue;
        }
    else if(iDrawZoom != iDrawFBTargetZoom)
        {
        DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 10.1"));
        iDrawZoom = iDrawFBTargetZoom;
        //iContainer->iDrawOneMoreTime = ETrue;
        iContainer->SetMinMagFilterLinear(ETrue);
        return ETrue;
        }
    
    
    TInt imageRotation = 0 - (TReal)imageData->GetOrientation();
    
    if(imageRotation == -90 || imageRotation == -270)
        {
        if(Abs(Abs(iDrawX)-Abs(iDrawTargetY)) > 0.001)
            {
            DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 11"));
            iContainer->SetMinMagFilterLinear(EFalse);
            return ETrue;
            }
        else if(Abs(iDrawX) != Abs(iDrawTargetY))
            {
            DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 11.1"));
            if(iDrawX < 0)
                iDrawX = 0-Abs(iDrawTargetY);
            else
                iDrawX = Abs(iDrawTargetY);
            //iContainer->iDrawOneMoreTime = ETrue;
            iContainer->SetMinMagFilterLinear(ETrue);
            return ETrue;
            }
    
        if(Abs(Abs(iDrawY)-Abs(iDrawTargetX)) > 0.001)
            {
            DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 12"));
            iContainer->SetMinMagFilterLinear(EFalse);
            return ETrue;
            }
        else if(Abs(iDrawY) != Abs(iDrawTargetX))
            {
            DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 12.1"));
            if(iDrawY < 0)
                iDrawY = 0-Abs(iDrawTargetX);
            else
                iDrawY = Abs(iDrawTargetX);
            //iContainer->iDrawOneMoreTime = ETrue;
            iContainer->SetMinMagFilterLinear(ETrue);
            return ETrue;
            }
        }
    else
        {
        if(Abs(iDrawX-iDrawTargetX) > 0.001)
            {
            DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 11"));
            iContainer->SetMinMagFilterLinear(EFalse);
            return ETrue;
            }
        else if(iDrawX != iDrawTargetX)
            {
            DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 11.1"));
            iDrawX = iDrawTargetX;
            //iContainer->iDrawOneMoreTime = ETrue;
            iContainer->SetMinMagFilterLinear(ETrue);
            return ETrue;
            }
    
        if(Abs(iDrawY-iDrawTargetY) > 0.001)
            {
            DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 12"));
            iContainer->SetMinMagFilterLinear(EFalse);
            return ETrue;
            }
        else if(iDrawY != iDrawTargetY)
            {
            DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 12.1"));
            iDrawY = iDrawTargetY;
            //iContainer->iDrawOneMoreTime = ETrue;
            iContainer->SetMinMagFilterLinear(ETrue);
            return ETrue;
            }
        }
    
    /*if(iContainer->iDrawOneMoreTime)
        {
        DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 14"));
        iContainer->iDrawOneMoreTime = EFalse;
        iContainer->SetMinMagFilterLinear(ETrue);//Draw one more time to change for Linear rendering mode
        return ETrue;
        }*/
    
    if(!iContainer->iMagFilterLinear)
        {
        DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 15"));
        iContainer->SetMinMagFilterLinear(ETrue);
        //iContainer->iDrawOneMoreTime = ETrue;
        }
        
    if(iContainer->iDrawNow)
        {
        DP0_IMAGIC(_L("CDrawFaceBrowsing::IsDrawingNeededOneByOne - 1"));
        /*if(iContainer->iMagGlassOn)
            {
            if(!iContainer->iTouchPointThreshold)//moved >2 pixel
                iContainer->SetMinMagFilterLinear(ETrue);
            else
                iContainer->SetMinMagFilterLinear(EFalse);
            }
        else*/
            {
            iContainer->SetMinMagFilterLinear(ETrue);
            }
        
        //iDrawOneMoreTime = ETrue;
        iContainer->iDrawNow = EFalse;
        return ETrue;
        }
    
    return EFalse;
    }


/*----------------------------------------------------------------------*/
// Calculate coords coords from alogorith to OpenGL -0.5 - +0.5 coords
//
FloatCoords CDrawFaceBrowsing::ConvertCoordsFromAlgo2OGl(const TInt aFaceIndex)
    {
    CImageData* imageData = iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    
    TInt pictureWidth, pictureHeigth;
    //We use always qvga image for Face detection
    pictureWidth=320;
    pictureHeigth=320;
    //landscape
    if(imageData->GetAspectRatio() > 1)
        pictureHeigth=pictureWidth/imageData->GetAspectRatio();
    else//portrait
        pictureWidth=pictureHeigth*imageData->GetAspectRatio();
        
    //calculate midpoint from face rect
    TInt xAvegCoord = (iCoordinates[aFaceIndex].iTl.iX + iCoordinates[aFaceIndex].iBr.iX)/2;
    TInt yAvegCoord = (iCoordinates[aFaceIndex].iTl.iY + iCoordinates[aFaceIndex].iBr.iY)/2;
    //and then convert coordinate zero point to center of screen 
    xAvegCoord-=(pictureWidth/2);
    yAvegCoord-=(pictureHeigth/2);
    
    //Calculate coords coords from alogorith to OpenGL -0.5 - +0.5 coords and fill the array
    FloatCoords tmp;
    if(imageData->GetAspectRatio() > 1)
        {
        tmp.iX = (xAvegCoord * 0.5) / (pictureWidth/2);
        tmp.iY = (yAvegCoord * (0.5/imageData->GetAspectRatio())) / (pictureHeigth/2);
        }
    else//portrait
        {
        tmp.iX = (xAvegCoord * (0.5*imageData->GetAspectRatio())) / (pictureWidth/2);
        tmp.iY = (yAvegCoord * 0.5) / (pictureHeigth/2);
        }

    return tmp;
    }


/*----------------------------------------------------------------------*/
// Draws one by one view
//
void CDrawFaceBrowsing::GetFaceCoordinatesL(TRect& aRect, TFileName& aFilename)
    {
    iContainer->iImagicAppUi->GetEngine()->GetFileNameL(iContainer->GetCurrentIndex(), ESize512x512, aFilename);
    
    if(iFaceNro < iCoordinates.Count() && iFaceNro >= 0)
        {
        aRect.iTl.iX = iCoordinates[iFaceNro].iTl.iX;
        aRect.iTl.iY = iCoordinates[iFaceNro].iTl.iY;
        aRect.iBr.iX = iCoordinates[iFaceNro].iBr.iX;
        aRect.iBr.iY = iCoordinates[iFaceNro].iBr.iY;
        }
    }

/*----------------------------------------------------------------------*/
// Draws one by one view
//
TInt CDrawFaceBrowsing::GetNumberOfFaces()
    {
    return iCoordinates.Count();
    }

/*----------------------------------------------------------------------*/
// Draws one by one view
//
void CDrawFaceBrowsing::SetCurrentFaceNro(TInt aNro)
    {
    iFaceNro = aNro;
    
    if(iFaceNro < 0)
        iFaceNro = 0;
    if(iFaceNro >= iCoordinates.Count())
        iFaceNro = 0;
    }

/*----------------------------------------------------------------------*/
// Gets current face nro
//
TInt CDrawFaceBrowsing::GetCurrentFaceNro()
    {
    return iFaceNro;
    }

void CDrawFaceBrowsing::SetFaceCoords(RArray<TRect>& aCoordinates)
    {
    ClearFaceArray();
    
    for(TInt i = 0; i < aCoordinates.Count(); i++)
        {
        iCoordinates.Append(aCoordinates[i]);        
        }

    iDrawFBTargetZoom = 0;
    iFaceNro = 0;
    }

void CDrawFaceBrowsing::ClearFaceArray()
    {
    //iCoordinates.Reset();
    TInt tmp = iCoordinates.Count();
    //delete array if we had old data there
    for(TInt i = 0; i < tmp; i++)
        {
        iCoordinates.Remove(0);
        }
    
    tmp = iFloatCoordinates.Count();
    for(TInt i = 0; i < tmp; i++)
        {
        iFloatCoordinates.Remove(0);
        }
    iCoordIndex=0;
    }

TInt CDrawFaceBrowsing::GetFaceCount()
    {
    return iCoordinates.Count();
    }

void CDrawFaceBrowsing::IncFaceNumber()
    {
    iFaceNro++;
    }

void CDrawFaceBrowsing::DecFaceNumber()
    {
    iFaceNro--;
    }
