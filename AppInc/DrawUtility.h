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

#ifndef LOADING_ANIMATION_H
#define LOADING_ANIMATION_H

/*--------------------------------------------------------------------------*/

//#include "DrawableInterface.h"
#include "ImagicContainerBrowser.h"
//#include "ImagicAppUi.h"
#include <GLES\egl.h>

class CImagicContainerBrowser;
/*--------------------------------------------------------------------------*/

class CDrawUtility //: public CDrawableInterface
	{
	private:
		// 1st and 2nd stage constructors
		CDrawUtility();
		void ConstructL(CImagicContainerBrowser* aContainer);
	public:
		// NewL and destructor
		//static CDrawableInterface* NewL(void);
	    static CDrawUtility* NewL(CImagicContainerBrowser* aContainer);
		~CDrawUtility();
		
		// Update animation
		// Returns true if screen should be redrawn
		TBool Update(void);
		// Draw animation
		void Draw(const TSize &aScreenSize);
		void DrawIcon(const TSize &aScreenSize, GLuint aTexIndex);
		void DrawIcon2(const TSize &aScreenSize, GLuint aTexIndex, TReal aAlpha);
		void DrawZoomIcon(const CImageData* aImageData,
                          const TSize aScreenSize, 
                          float aDrawOneByOneX, 
                          float aDrawOneByOneY,
                          TReal aDrawOnebyOneW, 
                          TReal aDrawOnebyOneH,
                          TBool aShowLocationRect);
		void DrawZoomFrame(float aRotationTarget);
		void DrawFrame(TInt aIndex);
		void DrawMovingArrow(TBool aPrevArrow, TBool aUpDownArrow, const TSize& aScreenSize);
		void DrawMenuIndicators(const TSize& aScreenSize);
		
	private:
		// Makes sure that angle is on valid range
		void LimitAngle(TInt &Angle);
		void SetPictureVertices(GLfixed* aVertices, TReal aAspectRatio);
		
		
		// Rotation angles
		TInt iAngleX;
		TInt iAngleY;
		TInt iAngleZ;
		
		// Mesh data
		static const TInt iTriCount;
		static const GLfixed iVertices[4*3];
		static const GLfixed iNormals[4*3];
		static const GLushort iIndices[4*3];
		
		CImagicContainerBrowser* iContainer;
		float iRotation; 
		float iRotationTarget;
		TSize iScrSize;
		TSize iThumbSize;
		TSize iZoomRectSize;
		TReal iAspectRatio;
		float iDrawOneByOneX; 
        float iDrawOneByOneY;
        TReal iScrAspectratio;
        TReal iDrawOnebyOneW; 
        TReal iDrawOnebyOneH;
	};

/*--------------------------------------------------------------------------*/

#endif // LOADING_ANIMATION_H
