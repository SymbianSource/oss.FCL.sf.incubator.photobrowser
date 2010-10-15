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

#ifndef __IEENGINEIMP_H__
#define __IEENGINEIMP_H__

#define _FACEBROWSING

// Include files
#include <e32cons.h>
#include <e32base.h>
#include <F32file.h>
#include <FBS.h>

#include <IEEngine.h>
#include <IEImageProcessing.h>//thumbnail client
#include <exifmodify.h> 

#ifdef IMAGE_EDITOR
#include "IEEditor.h"
#endif
#include "IEBitmapLoader.h"
#include "IEFileloader.h"
#include "IEBgpsController.h"
#include "ImagicConsts.h"

#include "IEEngineUtils.h"
//#ifdef _S60_5x_ACCELEROMETER_
#include "IESensorMonitor.h"
//#endif

// Forward class declarations
#ifdef IMAGE_EDITOR
class CIEEditor;
#endif
class CIEFileLoader;
class CIEBitmapLoader;
class CIETNController;
class CIEImageProcessing;
class CImageData;
class CIEEngineUtils;
class MBitmapLoaderObserver;
//#ifdef _S60_5x_ACCELEROMETER_
//class TSensrvOrientationData;
//#endif

// Class declaration
class CIEEngineImp : public CIEEngine, public MBitmapLoaderObserver , public MIEBgpsControllerObserver 
#ifdef _ACCELEROMETER_SUPPORTED_
,public MIESensorMonitorObserver
#endif
#ifdef IMAGE_EDITOR
, public MIEObserver
#endif
{
public: // First phase constructor and destructor
	static CIEEngineImp* NewL(MIEEngineObserver& aObserver);
	~CIEEngineImp();

private: // Second phase constructot and C++ default constructor
	void ConstructL();
	   CIEEngineImp(MIEEngineObserver& aObserver);
	
public: // From CIEEngine
	// General Functions
    void SetDBChanged(CImageData* aImageData);
    TInt GetTotalNumOfImages();
    void GetTotalNumOfImages(TInt& aNumOfImages, TInt& aNumOfFaces);
	TInt DeleteFile(TInt aIndex);
	TInt GetImageName(const TInt aIndex, TFileName& aFileName, TThumbSize aThumbRes);
	void GetBitmapL(CImageData* aImageData, CFbsBitmap* aBitmap, TThumbSize aThumbRes);
	void SetImageDataMode(TImageArrayMode aMode);
	void AppUIReady();
	void CancelFullSizeLoading();
	//New functions for handling UI access to Filename array
    CImageData* GetImageData(TInt aIndex);
    void SetImageData(TInt aIndex, CImageData* aGridData);
	TBool IsScanningFiles() const;
	CIEImageList& GetImageList();
	
	// TN related functions
	void GetFileNameL(const TInt aIndex, TThumbSize aThumbRes, TFileName& aFilename);
	void StopTNGeneration(TInt &aValue);
	TReal GetAspectRatio(TInt aIndex);
	TReal GetFacesAspectRatio(TInt aIndex);
	void GenerateThumbNailL(const TDes &aOrgFile, TThumbSize aTNResolution);
	
	// Face Detection related functions
	void GetFaceCoordinates(const TFileName a128x128TNFileName, RArray<TRect>& aFaceCoordinateArray);  
    void RemoveFaceCoordinate(const TFileName a128x128TNFileName, RArray<TRect>& aCordArray);
    void GetSingleFaceCoordinates(TInt aIndex, const TFileName aTNFileName, RArray<TRect>& aFaceCoordinateArray);
    
public: //From MBitmapLoaderObserver
    CIEFileLoader* GetFileLoader();
    void BitmapsLoadedL(TInt aError);
    
public: // From
    inline void Something(){};
    void TNGenerationComplete(TThumbSize aTNRes);
    void SingleTNGenerationComplete(TInt aIndex, TThumbSize aTNRes);
    void FaceDetectionComplete();
    void SingleFaceDetectionComplete();
    void SetGridRotationAngle(TReal aAngle);
    TBool IsAccelerometerExists();

#ifdef _ACCELEROMETER_SUPPORTED_
    void SensorDataAvailable(TImagicDeviceOrientation aOrientation, TBool aValue);
    void SetImageRotation(TInt aIndex);
    TImagicDeviceOrientation GetDeviceOrientation();
    void SetDeviceOrientation(TImagicDeviceOrientation aOrientation);
    void StartAccSensorMonitoring();
    void StopAccSensorMonitoring();
#endif

public: // New public functions
	CIEEngineUtils *GetEngineUtils();
	void AllFilesAddedToFilenameArrayL();
	MIEEngineObserver& GetObserver();
	
	TInt GetSelectedImageIndex();
	void Stop();
	TBool IsRunning();
	TInt GetGleMaxRes();
	
private:
    void AddImageToFaceNameArray();
  
private:
	MIEEngineObserver&     iEngineObserver;
	RFs                    iFileServer;
#ifdef IMAGE_EDITOR
	CIEEditor*             iImageEditor;
#endif
	CIEFileLoader*         iFileLoader;
	CIEBitmapLoader*       iBitmapLoader;
    CIEEngineUtils         iIEEngineUtils;
    CIEBgpsController*     iIEBgpsController;
    TBool                  iAllFilesScanned;
#ifdef _ACCELEROMETER_SUPPORTED_
    CIESensorMonitor*       iSensorMonitor;
    TImagicDeviceOrientation      iPrevDeviceOrientation;
    TImagicDeviceOrientation      iDeviceOrientation;
#endif
	
	TIEEditingMode         iEditingMode;
	TIEEditingMode         iCurrentEditingMode;
		
	TBool                  iImageEdited;
	CIEImageProcessing*    iIEBgpsClient;
	RCriticalSection       iCritical;
	
	TFileName              iSavedFileName;
	
	TImageArrayMode        iImageArrayMode;
	RArray<TFileName>      iCroppedFilenames;
};

#endif // __IEENGINEIMP_H__
