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

#include "CDrawMagGlass.h"
#include "TextureLoader.h"
#include "DrawUtility.h"
#include "ImagicConsts.h"


GLfixed CDrawMagGlass::iMagGlassVertices[VERTICES_PER_LINE*VERTICES_PER_LINE*3];
GLfixed CDrawMagGlass::iMagGlassTex[VERTICES_PER_LINE*VERTICES_PER_LINE*2];
    
const TInt CDrawMagGlass::iMagGlassTriCount=(VERTICES_PER_LINE-1)*(VERTICES_PER_LINE-1)*2;
GLushort CDrawMagGlass::iMagGlassIndices[CDrawMagGlass::iMagGlassTriCount*3];



// Texture coordinate data
const GLfixed CDrawMagGlass::iGlobalTexCoords[] =
   {
   //bitmap has to be flipped over
   0,       1<<16,
   1<<16,   1<<16,
   0,       0,
   1<<16,   0
   };

CDrawMagGlass::CDrawMagGlass()
    {
    // No implementation required
    }

CDrawMagGlass::~CDrawMagGlass()
    {
    }

CDrawMagGlass* CDrawMagGlass::NewLC(CImagicContainerBrowser* aContainer, CDrawOneByOne* aDrawOneByOne)
    {
    CDrawMagGlass* self = new (ELeave) CDrawMagGlass();
    CleanupStack::PushL(self);
    self->ConstructL(aContainer,aDrawOneByOne);
    return self;
    }

CDrawMagGlass* CDrawMagGlass::NewL(CImagicContainerBrowser* aContainer, CDrawOneByOne* aDrawOneByOne)
    {
    CDrawMagGlass* self = CDrawMagGlass::NewLC(aContainer,aDrawOneByOne);
    CleanupStack::Pop(); // self;
    return self;
    }

void CDrawMagGlass::ConstructL(CImagicContainerBrowser* aContainer, CDrawOneByOne* aDrawOneByOne)
    {
    iContainer = aContainer;
    iDrawOneByOne = aDrawOneByOne;
    }

TReal CDrawMagGlass::GetMagGlassZoomFactor()
    {
    DP1_IMAGIC(_L("CDrawMagGlass::GetMagGlassZoomFactor - magGlass: %f"), iMagGlassZoomFactor);
    return iMagGlassZoomFactor;
    }

/*----------------------------------------------------------------------*/
// Interpolates given value into target value with step
//
void CDrawMagGlass::Interpolate(float &aValue, const float aTarget, const float aStep)
    {
    //DP0_IMAGIC(_L("CImagicContainerBrowser::Interpolate"));
    // Calculate new value
    float diff = aTarget-aValue;
    aValue += diff * aStep * 0.2/*iTimeDiff*/ * 30; 
    //float timediff = Min(0.1f, iTimeDiff); // so max value of timediff is 100tick (100ms)
    //aValue += diff * aStep * timediff * 30; 
    
    // Check that value is in range
    if (aValue > aTarget && diff > 0)
        aValue = aTarget;
    if (aValue < aTarget && diff < 0)
        aValue = aTarget;
    }

void CDrawMagGlass::InitDrawMagGlass()
    {
    iMagGlassZoomFactor = 1;
    
    TInt pos=0;
    for (TInt y=0; y<VERTICES_PER_LINE-1; y++)
        {
        TInt startvert=y*VERTICES_PER_LINE;
        for (TInt x=0; x<VERTICES_PER_LINE-1; x++, pos+=6)
            {
            // First triangle
            iMagGlassIndices[pos+0]=startvert+x;
            iMagGlassIndices[pos+1]=startvert+x+1;
            iMagGlassIndices[pos+2]=startvert+x+VERTICES_PER_LINE;
            
            // Second triangle
            iMagGlassIndices[pos+3]=startvert+x+VERTICES_PER_LINE;
            iMagGlassIndices[pos+4]=startvert+x+1;
            iMagGlassIndices[pos+5]=startvert+x+VERTICES_PER_LINE+1;
            }
        }
    }
    
void CDrawMagGlass::DrawMagGlass(const TSize &aScreenPhysicalSize, TReal aImageAspectRatio)
    {
    TInt pos=0;
    
#ifdef __WINS__ 
    // Fake touhcpoint for emulator
    iContainer->iLastTouchPoint.iX=320/2;
    iContainer->iLastTouchPoint.iY=240/2;
#endif
        
    // Calculate finger position on screen
    // First calculate position on 0-1 range
    GLfixed xFinger;
    GLfixed yFinger;
    if(iContainer->GetScreenOrientation())
        {
        const TInt fingerOffset = 100;
        xFinger=iContainer->iLastTouchPoint.iX*(1<<16) / aScreenPhysicalSize.iWidth;
        yFinger=(iContainer->iLastTouchPoint.iY-fingerOffset)*(1<<16) / aScreenPhysicalSize.iHeight;
        }
    else
        {
        const TInt fingerOffset = 100;
        xFinger=(iContainer->iLastTouchPoint.iX-fingerOffset)*(1<<16) / aScreenPhysicalSize.iWidth;
        yFinger=(iContainer->iLastTouchPoint.iY)*(1<<16) / aScreenPhysicalSize.iHeight;
        }
    
    // Move center to match OpenGL center
    xFinger=-(0.5*(1<<16)-xFinger);
    yFinger=0.5*(1<<16)-yFinger;
    // Then scale it to opengl window size
    /*xFinger=xFinger*iContainer->iDrawOnebyOneW*2;
    yFinger=yFinger*iContainer->iDrawOnebyOneH*2;*/
    xFinger=xFinger*iDrawOneByOne->GetDrawOneByOneWidth()*2;
    yFinger=yFinger*iDrawOneByOne->GetDrawOneByOneHeight()*2;
        
    
    CImageData* data=iContainer->iIEngine->GetImageData(iContainer->GetCurrentIndex());
    
    if((data->GetOrientation() == 0 || data->GetOrientation() == 180) && !iContainer->GetScreenOrientation())
        {
        GLfixed tmpX=xFinger;
        GLfixed tmpY=yFinger;
        xFinger = tmpY;
        yFinger = tmpX * -1;
        }
    else if(data->GetOrientation() == 0)
        ;
    else if((data->GetOrientation() == 90 || data->GetOrientation() == 270) && iContainer->GetScreenOrientation())
        {
        GLfixed tmpX=xFinger;
        GLfixed tmpY=yFinger;
        xFinger = tmpY;
        yFinger = tmpX * -1;
        }
    else if((data->GetOrientation() == 90 || data->GetOrientation() == 270) && !iContainer->GetScreenOrientation())
        {
        xFinger = xFinger * -1;
        yFinger = yFinger * -1;
        }
    
    
    //Interpolate(iMagGlassZoomFactor, 1.8, 0.05);
    //const float step=0.3; 
    //const float step=0.2;
    /*
    if (iMagGlassZoomFactor < CDrawOneByOne::KMaxMagGlassZoomFactor-step)
        //iDrawOneByOne->IncMagGlassZoomFactor(step);
        iMagGlassZoomFactor += step;
    else
        //iDrawOneByOne->SetMagGlassZoomFactor(CDrawOneByOne::KMaxMagGlassZoomFactor);
        iMagGlassZoomFactor = CDrawOneByOne::KMaxMagGlassZoomFactor;
    */
    Interpolate(iMagGlassZoomFactor, CDrawOneByOne::KMaxMagGlassZoomFactor, 0.1);
    DP1_IMAGIC(_L("CDrawMagGlass::DrawMagGlass - magGlass: %f"), iMagGlassZoomFactor);
    
    /*CImageData* data=iIEngine->GetImageData(iCurrentIndex, iImagicAppUi->GetUIDrawMode());
    if(data->GetOrientation() == 90 || data->GetOrientation() == 270)
        aImageAspectRatio = 1/aImageAspectRatio;*/
    
    // Create vertices
    pos=0;
    TInt texPos=0;
    for (TInt y=0; y<VERTICES_PER_LINE; y++)
        for (TInt x=0; x<VERTICES_PER_LINE; x++,pos+=3,texPos+=2)
            {
            // Calculate new vertex coordinates
            // First, calculate center
            GLfixed xCoord= x;
            GLfixed yCoord=-y;
            GLfixed zCoord= 0;
            // Then convert to fixed point
            xCoord*=(1<<16)/(VERTICES_PER_LINE-1);
            yCoord*=(1<<16)/(VERTICES_PER_LINE-1);
            // shift 0.5 to locate image center at (0,0) 
            xCoord -= (1<<15);
            yCoord += (1<<15);
            
            // Set aspect ratio
            if (aImageAspectRatio > 1)
                yCoord *= (1/aImageAspectRatio);
            else
                xCoord *= (1*aImageAspectRatio);
            
            
            // Zoom ball sizes
            const float ballSize=0.35;      // Zoom ball size
            const float zoomSize=0.2;     // Sharp zoom size, has to be at least 0.1 smaller bigger than ball size
            // Calculate squared values
            const GLfixed ballSizeSquared=(ballSize*(1<<8))*(ballSize*(1<<8));
            const GLfixed zoomSizeSquared=(zoomSize*(1<<8))*(zoomSize*(1<<8));
            
            // Calculate distance to finger with both axis
            GLfixed xDist=xFinger-xCoord;
            GLfixed yDist=yFinger-yCoord;
            // Calculate squared distance with phytagoras
            GLfixed dist=(xDist/(1<<8))*(xDist/(1<<8)) + (yDist/(1<<8))*(yDist/(1<<8));
            
            // Are we close enough
            if (dist < ballSizeSquared)
                {
                // Zoom factor
                //const float iMagGlassZoomFactor=1.8;
                
                // Determine proper zoom for this vertex
                //float zoom=iContainer->iMagGlassZoomFactor;
                float zoom=iMagGlassZoomFactor;
                
                if (dist > zoomSizeSquared)
                    {
                    // We are between sharp zoom and ball edge
                    // Calculat escale factor for zoom
                    float scale=((float)(dist-zoomSizeSquared))/(ballSizeSquared-zoomSizeSquared);
                    zoom=(zoom-1)*(1.0-scale)+1;
                    }
                
                // Calculate new axis distances with zoom
                xDist*=zoom;
                yDist*=zoom;
                // Calculate new coordinates based on distances
                xCoord=xFinger-xDist;
                yCoord=yFinger-yDist;
                zCoord=128;     // Set coordinate a bit above of original picture
                }
            
            // Store vertex data
            iMagGlassVertices[pos+0]=xCoord;
            iMagGlassVertices[pos+1]=yCoord;
            iMagGlassVertices[pos+2]=zCoord;
            
            // Set texture coodrinates
            iMagGlassTex[texPos+0]=x*(1<<16)/(VERTICES_PER_LINE-1);
            iMagGlassTex[texPos+1]=y*(1<<16)/(VERTICES_PER_LINE-1);
            }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    
    // Set vertex and texture coordinates and draw
    glVertexPointer(3, GL_FIXED, 0, iMagGlassVertices);
    glTexCoordPointer(2, GL_FIXED, 0, iMagGlassTex);
    glDrawElements(GL_TRIANGLES,iMagGlassTriCount*3,GL_UNSIGNED_SHORT,iMagGlassIndices);
    //glDrawElements(GL_LINES,triCount*3,GL_UNSIGNED_SHORT,indices);
    
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    }

