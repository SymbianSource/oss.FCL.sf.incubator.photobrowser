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

#ifndef IMAGIC_TEXTURE_LOADER_H
#define IMAGIC_TEXTURE_LOADER_H

// INCLUDES
#include "ImagicViewBrowser.h"
#include "ImagicContainerBrowser.h"
#include <BitmapTransforms.h>


// FORWARD DECLARATIONS
class CImagicViewBrowser;
class CImagicAppUi;

/*enum TTextureLoaderStatus
{
    EIdle = 0,
    EScaling = 1,
    EDecoding = 2,
    ECreateSmileTex = 3,
};*/

class CTextureLoader : public CActive
	{
	public:
		// Constructor and destructor
	    void ConstructL();
		CTextureLoader(CImagicAppUi* aImagicAppUi, CImagicContainerBrowser* aContainer, 
		               CImagicViewBrowser* aView, RCriticalSection* aDrawLock);
		~CTextureLoader();
		
		// Tells if loader is running at the moment
		inline const TBool IsRunning(void) const 
			{
			return (iData!=NULL);
			}
		
		// Loads picture and stores the OpenGL index into specified variable
		void LoadL(CImageData* aData, TThumbSize aResolution);
		
		// Unloads picture from specified index
		void ReleaseSuperHResTexture(CImageData* aGridData);
        void ReleaseHQ512Textures();
	    void UnloadLQ512Tex(CImageData* aData) const;
		void UnloadLQ128Tex(CImageData* aData) const;
		void UnloadLQ32Tex(CImageData* aData) const;
		
		// Called when image is loaded
		void ImageLoadedL(TInt aError, CFbsBitmap* aBitmap, TInt aGLMaxRes);
		
		// Creates OpenGL texture of given bitmap
		static TInt CreateTexture(CFbsBitmap* aBitmap, TBool aHighQuality);
		void CreateIconTextures(/*RArray<CFbsBitmap*> aBitmapArray*/);
		void LoadIcons();
		
		//void LoadAnimation();
		TBool IsActiveAndRunning();
		void GetPngL(TFileName& afilepath, CFbsBitmap* aBitmap);
		
	private:
        // Active object interface
        void RunL();
        void DoCancel();
        void RunError();
        void CreateThumbnailTexture(CFbsBitmap* aBitmap);        
		
	private:
		// Scales given value to power of two
        TInt ScaleDown(TInt aSize);
        TBool IsScalingNeeded(TSize aImageSize);
		
	private:
		CImagicViewBrowser*   iView;		// Pointer to view
		RCriticalSection*     iDrawLock;	// Drawing mutex
		CImagicContainerBrowser* iContainer; // Container class
        CFbsBitmap*           iBitmap;          // Bitmap for scaled picture
        CFbsBitmap*           iSmileBitmap;          // Bitmap for scaled picture
        CFbsBitmap*           iZoomBitmap;          // Bitmap for scaled picture
        RArray<CFbsBitmap*>   iBitmapArray;          // Bitmap for scaled picture
        CBitmapScaler*        iBitmapScaler;    // Bitmap scaler object
        CImageDecoder*        iImageDecoder;
        
        CImageData*           iData;		// Pointer to one grid data
		TBool                 iHighQuality;	// Is image supposed to be high quality
		
		TUint                 iNewIndex; 				// Texture index
		CImageData*           iPreviousData;                // Texture index

		
	    TBool                 iCreateAnimation;
		GLuint                iSmileTexIndex;
		RArray<GLuint>        iIconTextureIndexes;
		TSize                 iImageSize;
        TBool                 iScalingNeeded;
        CImagicAppUi*         iImagicAppUi;
        TInt                  iGLMaxRes;
        TThumbSize            iResolution;
        TBool                 iClearCurrentLoading;
        TBool                 iRGB2BGRDone;
	};

#endif // IMAGIC_TEXTURE_LOADER_H
