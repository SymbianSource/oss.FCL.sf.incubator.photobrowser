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

#ifndef __IEEDITOR_H__
#define __IEEDITOR_H__

// Include files
#include <e32base.h>
#include <f32file.h>
#include <FBS.H>
#include <ImageConversion.h>
#include <IclExtJpegApi.h>
#include <IEImage.h>

#include <IDLImageProcessing.h>

#include "IEImageDecoder.h"
#include "IEImageEncoder.h"
#include "ImagicConsts.h"

#define _FACEBROWSING

// Forward class declaration
class CIEImageDecoder;
class CIEImageEncoder;

class TEditedImage
{
public:
	TIEFeature iEditedFeature;
	TInt iEditedValue;
	TIEColorParams iColorValue;
	HBufC8* iEditedYuvData;
};

class MIEObserver
{
public:
	virtual void WizardImagesLoadedL(TInt aError) = 0;
	virtual void FeatureCompleteL(TIEFeature aFeature, TInt aError) = 0;
	virtual void FeatureErrorL(TIEFeature aFeature, TInt aError) = 0;
	virtual void ImageSavedL(TInt aError, TReal aAspectRatio) = 0;
};

// Class declaration
class CIEEditor : public CBase, 
					public MDecodingObserver, 
					public MEncodingObserver, 
					public MIDLObserver
{
public:
	static CIEEditor* NewL(RFs& aFileServer, MIEObserver& aObserver);
	~CIEEditor();

private:
	void ConstructL();
	CIEEditor(RFs& aFileServer, MIEObserver& aObsaerver);

public: // From MDecodingObserver
	void YuvImageReadyL(TInt aError);
	void BitmapReadyL(TInt aError);

public: // From MEncodingObserver
	void JpegImageReadyL(TInt aError);
	
	TSize GetRotateImageSize();
	TInt GetRotationAngle();

public: // From MIDLObserver
	inline void ProcessingComplete(TDesC8& /*aData*/){};
	inline void HandleError(TInt /*aError*/){};	

public:
	void EditImageL(TIEImage* aImage, const TIEFeature aFeature, const TInt aValue);
	void EditImageWizardL(const TIEFeature aFeature, RArray<CFbsBitmap*>& aBitmapArray, const TInt aIndex = 0);
	void EditBrightnessL(CFbsBitmap& aBitmap, const TInt aValue);
	void EditContrastL(CFbsBitmap& aBitmap, const TInt aValue);
	void EditColorL(CFbsBitmap& aBitmap,
				const TInt aRedValue,
				const TInt aGreenValue,
				const TInt aBlueValue);
	void EditGammaL(CFbsBitmap& aBitmap, const TInt aValue);
	void EditEdgeEnhancementL(CFbsBitmap& aBitmap, const TInt aValue);
	void RotateImageL(CFbsBitmap& aBitmap, const TRotationAngle aRotationAngle);
	void CropImageL(CFbsBitmap& aBitmap, const TRect aRect);
	void EditLocalColorCorrectionL(CFbsBitmap& aBitmap, const TInt aValue);
	void SetImageAndModeL(const TDesC& aImageName, const TIEEditingMode aEditingMode);
	void CompleteWizardEditL(const TInt aIndex);
	void SaveEditedImageL(const TDesC& aSrcFileName, const TDesC& aTargetFileName);
	void CleanYuvDataArray();
	void DeleteInputYUVBuffer();
	void CleanBitmapArray();

 	
    	void DetectFaceL(const TDesC &aFileName);
    	void DetectFaceL();
    	void FaceDetectSetupL(const TIEFeature aFeature, const TIDLFeatures aIDLFeature);
    	void NoOfFacesDetected(TInt &aNoOfFaces);
    	
    	void NoOfFacesDetected(TInt &aNoOfFaces, RArray<TRect> &aFaceCoordinates);
    	
 
private:
	void PrepareOutputBuffersL(TInt aBuffers, TInt aSize);
	void CreateBitmapsL(TInt aBitmaps, TSize aSize);
	
	void SetupL(CFbsBitmap& aBitmap, 
			const TIEFeature aFeature, 
			const TIDLFeatures aIDLFeature,
			const TAny* aValue);

	//void InitializeFeatureL();
	void InitializeFeatureL(const TSize aInSize, const TSize aOutsize);
	void ProcessImageL(TDesC8& aInputData, TDesC8& aOutputData);
	
	void EditFeatureL(TIEFeature aFeature);
	void EditBrightnessL();
	void EditContrastL();
	void EditColorL();
	void EditSharpnessL();
	void EditGammaL();
	void RotateImageL();	
	void CropImageL();
	void EditLocalColorCorrectionL();
	
	void SaveImageL();
	
	void SaveDataToFileL(const TDesC& aFileName, TDesC8& aData);
	void GenerateFileName(TDes& aFileName, TDesC& aFeature, TInt aSuffix, TBool aIsYuv);
	
	void SetImageParams(const TIEFeature aFeature, const TAny* aFeatureValue);

private: // Data
	RFs& iFileServer;
	MIEObserver& iObserver;

	CIEImageDecoder* iImageDecoder;
	CIEImageEncoder* iImageEncoder;

	RArray<TEditedImage> iYuvDataArray;
	RArray<CFbsBitmap*>* iBitmapArrayPtr;
	RArray<CFbsBitmap*> iBitmapArray;
		
	HBufC8* iInBufferYuv;
	HBufC8* iOutBufferYuv;
	HBufC8* iJpegBuffer;
	
	CFbsBitmap* iBitmap;
	
	CIDLImageProcessing* iIDLImageProcessor;
	
	TSize iSize;
	TSize iFinalImageSize;
	TSize iRotatedSize;
	TSize iCroppedSize;
		
	TInt iBitmapArrayIndex;
	TInt iYuvDataArrayIndex;
	TInt iOrgImagePos;
		
	TUint8* iBufU;
	
	TIEImage iCurrentImage;
	TIEFeature iCurrentFeature;
	TIDLFeatures iCurrentIDLFeature;
	TInt iFeatureValue;
	TIEEditingMode iEditingMode;
	
	TRotationAngle iRotationAngle;
	TInt iAngle;
	TInt iNumberOfRotation;
	
	TRect iCropRect;
	
	TBool iImageEdited;
	TBool iEditComplete;
	
	TInt iBufferSize;
	float  iAspectRatio;
	
	TFileName iFileName;
		
#ifdef __SAVE_INTERMEDIATE_FILES__
	TInt       iCount;
#endif
	
};

#endif //__IEEDITOR_H__
