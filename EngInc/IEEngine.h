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

#ifndef __IEENGINE_H__
#define __IEENGINE_H__

#define _FACEBROWSING

// Include files
#include <e32base.h>
#include <fbs.h>

#include "ImagicConsts.h"
#include "debug.h"
#include "IEImage.h"
#include "IEImageProcessing.h" //thumbnail client
#include "IESensorMonitor.h"
#include "IEImageList.h"

#ifdef _S60_5x_ACCELEROMETER_
#include <sensrvorientationsensor.h>
#endif

//Structures

// Class declaration, AppUI class implements
class MIEEngineObserver
{
public:
    virtual void ImagesLoadedL(TInt aError) = 0;
	virtual void TNCreationCompleteL(TThumbSize aTnRes) = 0;
	virtual void SingleTNCreationCompletedL(TInt index, TThumbSize aTnRes) = 0;
	virtual void FaceDetectionComplete() = 0;
	virtual void SingleFaceDetectionComplete() = 0;
	virtual TInt GetImageIndex() = 0;
	virtual void AllFilesScanned() = 0;
	virtual void ImageListChanged(TInt aIndex, TBool bAdded) = 0;
#ifdef _ACCELEROMETER_SUPPORTED_
	virtual void ImageRotated(TImagicDeviceOrientation aDeviceOrientation) = 0;
#endif
	virtual TInt GetGleMaxRes() = 0;
	
};

class CIEEngine : public CBase
{
public:
    
	IMPORT_C static CIEEngine* NewL(MIEEngineObserver& aObserver);
	IMPORT_C virtual ~CIEEngine();
	IMPORT_C virtual TInt GetTotalNumOfImages() = 0;
	IMPORT_C virtual void GetTotalNumOfImages(TInt& aNumOfImages, TInt& aNumOfFaces) = 0;
	IMPORT_C virtual TInt DeleteFile(TInt aIndex) = 0;
	IMPORT_C virtual TInt GetImageName(const TInt aIndex, TFileName& aFileName, TThumbSize aThumbRes) = 0;
	IMPORT_C virtual void GetBitmapL(CImageData* aImageData, CFbsBitmap* aBitmap, TThumbSize aThumbRes) = 0;
	IMPORT_C virtual void AppUIReady() = 0;
	IMPORT_C virtual void CancelFullSizeLoading() = 0;
#ifdef _ACCELEROMETER_SUPPORTED_
	IMPORT_C virtual TImagicDeviceOrientation GetDeviceOrientation() = 0;
	IMPORT_C virtual void SetDeviceOrientation(TImagicDeviceOrientation aOrientation) = 0;
	IMPORT_C virtual void StartAccSensorMonitoring() = 0;
    IMPORT_C virtual void StopAccSensorMonitoring() = 0;
#endif
	IMPORT_C virtual TBool IsAccelerometerExists() = 0;
	IMPORT_C virtual void SensorDataAvailable(TImagicDeviceOrientation aOrientation, TBool aValue) = 0;
	IMPORT_C virtual void SetImageRotation(TInt aIndex) = 0;
	IMPORT_C virtual void SetDBChanged(CImageData* aImageData) = 0;
	
	//New functions for handling UI access to Filename array
	IMPORT_C virtual CImageData* GetImageData(TInt aIndex/*, TImageArrayMode aMode*/) = 0;
	IMPORT_C virtual void SetImageData(TInt aIndex, CImageData* aGridData) = 0;
	IMPORT_C virtual TBool IsScanningFiles() const = 0;
	IMPORT_C virtual CIEImageList& GetImageList() = 0;	

	// TN related functions
	IMPORT_C virtual void GetFileNameL(const TInt aIndex, TThumbSize aThumbRes, TFileName& aFilename) = 0;
	IMPORT_C virtual void StopTNGeneration(TInt &aValue) = 0;
	IMPORT_C virtual TReal GetAspectRatio(TInt aIndex) = 0;
	IMPORT_C virtual TReal GetFacesAspectRatio(TInt aIndex) = 0;
	IMPORT_C virtual void GenerateThumbNailL(const TDes &aOrgFile, TThumbSize aTNResolution) = 0;
	
	// Newly added face Detection functions
	IMPORT_C virtual void GetFaceCoordinates(const TFileName a128x128TNFileName, RArray<TRect>& aFaceCoordinateArray) = 0;
    IMPORT_C virtual void GetSingleFaceCoordinates(TInt aIndex, const TFileName aTNFileName, RArray<TRect>& aFaceCoordinateArray) = 0;
    IMPORT_C virtual void SetImageDataMode(TImageArrayMode aMode) = 0;
    
    IMPORT_C virtual void Stop() = 0;
    IMPORT_C virtual TBool IsRunning() = 0;
};

#endif // __IEENGINE_H__
