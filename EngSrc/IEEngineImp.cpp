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

// Include files
#include <e32uid.h>
#include <BAUTILS.H>
#include <IEImageProcessing.h>//thumbnail client
#include "IEEngineImp.h"
#ifdef IMAGE_EDITOR
#include "IEEditor.h"
#endif
#include "IEFileLoader.h"
#include "IEImageList.h"
#include "ImageMonitorAO.h"
#include "IEImageList.h"
#include "IEEngineUtils.h"


// ============================ MEMBER FUNCTIONS =========================== //

EXPORT_C CIEEngine* CIEEngine::NewL(MIEEngineObserver& aObserver)
{
    DP0_IMAGIC(_L("CIEEngine::NewL"));
	return CIEEngineImp::NewL(aObserver);
}

EXPORT_C CIEEngine::~CIEEngine()
    {
    
    }

CIEEngineImp* CIEEngineImp::NewL(MIEEngineObserver& aObserver)
{
    DP0_IMAGIC(_L("CIEEngine::NewL"));
	CIEEngineImp* self = new (ELeave) CIEEngineImp(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
}

CIEEngineImp::~CIEEngineImp()
    {
    DP0_IMAGIC(_L("CIEEngine::~CIEEngineImp++"));

#if 0
   if(iIEImageProcessing)
       {
       delete iIEImageProcessing;
       iIEImageProcessing = NULL;
       }
#endif

#ifdef IMAGE_EDITOR
     if(iImageEditor)
	 {
         DP0_IMAGIC(_L("CIEEngine::~CIEEngineImp - delete iImageEditor"));
         delete iImageEditor;
         iImageEditor = NULL;
	 }
#endif
   
	 if(iFileLoader)
	     {
	     DP0_IMAGIC(_L("CIEEngine::~CIEEngineImp - delete iFileLoader"));
	     delete iFileLoader;
	     iFileLoader = NULL;
	     }
	
	 if(iBitmapLoader)
	     {
	     DP0_IMAGIC(_L("CIEEngine::~CIEEngineImp - delete iBitmapLoader"));
	     iBitmapLoader->CancelFullSizeLoading();
	     DP0_IMAGIC(_L("CIEEngine::~CIEEngineImp - delete iBitmapLoader 2"));
	     delete iBitmapLoader;
	     iBitmapLoader = NULL;
	     }
	
	 if(iIEBgpsController)
	     {
	     DP0_IMAGIC(_L("CIEEngine::~CIEEngineImp - delete iIEBgpsController"));
	     delete iIEBgpsController;
	     iIEBgpsController = NULL;
	     }
#ifdef _ACCELEROMETER_SUPPORTED_
	 if(iSensorMonitor)
	     {
	     iSensorMonitor->StopMonitoring();
	     delete iSensorMonitor;
	     iSensorMonitor = NULL;
	     }
#endif //_ACCELEROMETER_SUPPORTED_

	iFileServer.Close();
	iCritical.Close();
	
	DP0_IMAGIC(_L("CIEEngine::~CIEEngineImp--"));
    }

CIEEngineImp::CIEEngineImp(MIEEngineObserver& aObserver) :
    iEngineObserver(aObserver),
    iIEEngineUtils(iFileServer)
    {
    }

void CIEEngineImp::ConstructL()
    {
    DP0_IMAGIC(_L("CIEEngine::ConstructL++"));
    
    iCritical.CreateLocal();
    
    User::LeaveIfError(iFileServer.Connect());
	
	/*Creating Engine Utility class pointer */
	iCurrentEditingMode = EEditModeNone;
	iEditingMode = EEditModeNone;
	iImageEdited = EFalse;
	iImageArrayMode = EImages;
	iAllFilesScanned = EFalse;
	iPrevDeviceOrientation = EOrientationDisplayLeftUp;
	

	TInt err = KErrNone;
	//Create Bitmap loader
	iBitmapLoader = CIEBitmapLoader::NewL(iFileServer, *this, &iCritical);
	//Create BGPS controller
	iIEBgpsController = CIEBgpsController::NewL(iFileServer, *this, iIEEngineUtils, &iCritical);
	//Create Fileloader
	TRAP(err, iFileLoader = CIEFileLoader::NewL(iFileServer, this, &iCritical));
    if(err != KErrNone)
        {
        DP1_IMAGIC(_L("CIEEngine::ConstructL - Error creating file loader, err: %d"),err);
        User::Leave(err);
        }
    
#ifdef _ACCELEROMETER_SUPPORTED_
    iDeviceOrientation = EOrientationDisplayLeftUp;
    err = KErrNone;
    TRAP(err, iSensorMonitor = CIESensorMonitor::NewL(*this));
    DP1_IMAGIC(_L("CIEEngine::ConstructL - iSensorMonitor error: %d"), err);
    if(err == KErrNone)
        {
        DP0_IMAGIC(_L("CIEEngine::ConstructL - iSensorMonitor->StartMonitoring()"));
        iSensorMonitor->StartMonitoring();
        }
    else
        {
        iSensorMonitor = NULL;
        }

#endif

    iIEBgpsController->SetFileLoader(iFileLoader); 
   
    DP0_IMAGIC(_L("CIEEngine::ConstructL--"));
}

void CIEEngineImp::SetDBChanged(CImageData* aImageData)
    {
    TFileName filename;
    aImageData->GetFileName(filename, EFullSize);
    iFileLoader->GetImageList().SetChanged(filename); 
    }

CIEImageList& CIEEngineImp::GetImageList()
    {
    return iFileLoader->GetImageList();
    }

void CIEEngineImp::AppUIReady()
    {
    DP0_IMAGIC(_L("CIEEngine::AppUIReady"));
    
    //Create ImageProcessing
    iIEBgpsController->CreateImageProcessing();
    }

void CIEEngineImp::CancelFullSizeLoading()
    {
    DP0_IMAGIC(_L("CIEEngine::CancelLoading"));
    
    iBitmapLoader->CancelFullSizeLoading();
    }


void CIEEngineImp::SetImageDataMode(TImageArrayMode aMode)
    {
    iImageArrayMode = aMode;
    
    iBitmapLoader->SetImageDataMode(aMode);
    }

MIEEngineObserver& CIEEngineImp::GetObserver() 
    {
    return iEngineObserver;
    }

void CIEEngineImp::TNGenerationComplete(TThumbSize aTNRes)
    {
    DP0_IMAGIC(_L("CIEEngine::TNGenerationComplete++"));
    
    iEngineObserver.TNCreationCompleteL(aTNRes);
    
    DP0_IMAGIC(_L("CIEEngine::TNGenerationComplete--"));
    }

void CIEEngineImp::SingleTNGenerationComplete(TInt aIndex, TThumbSize aTNRes)
    {
    DP0_IMAGIC(_L("CIEEngine::TNGenerationComplete++"));
    
    iEngineObserver.SingleTNCreationCompletedL(aIndex, aTNRes);
    
    DP0_IMAGIC(_L("CIEEngine::TNGenerationComplete--"));
    }

void CIEEngineImp::FaceDetectionComplete()
    {
    DP0_IMAGIC(_L("CIEEngine::FaceDetectionComplete"));
    iEngineObserver.FaceDetectionComplete();
    }

void CIEEngineImp::SingleFaceDetectionComplete()
    {
    DP0_IMAGIC(_L("CIEEngine::SingleFaceDetectionComplete"));
    
    iEngineObserver.SingleFaceDetectionComplete();
    }

void CIEEngineImp::AllFilesAddedToFilenameArrayL()
    {
    DP0_IMAGIC(_L("CIEEngine::AllFilesAddedToFilenameArrayL"));
    
    iIEBgpsController->AllFilesAddedToFilenameArrayL();
    iEngineObserver.AllFilesScanned();
    iAllFilesScanned = ETrue;
    }

CIEEngineUtils * CIEEngineImp::GetEngineUtils()
    {
    DP0_IMAGIC(_L("CIEEngine::GetEngineUtils"));
    
    return &iIEEngineUtils;
    }

CIEFileLoader* CIEEngineImp::GetFileLoader()
    {
    DP0_IMAGIC(_L("CIEEngineImp::GetFileLoader"));
    
    return iFileLoader;
    }

void CIEEngineImp::BitmapsLoadedL(TInt aError)
{
    DP0_IMAGIC(_L("CIEEngineImp::BitmapsLoaded"));
    
	iEngineObserver.ImagesLoadedL(aError);
}

TBool CIEEngineImp::IsScanningFiles() const
    {
    return !iAllFilesScanned;
    }

void CIEEngineImp::GetTotalNumOfImages(TInt& aNumOfImages, TInt& aNumOfFaces)
    {
    //DP0_IMAGIC(_L("CIEEngineImp::GetTotalNumOfImages"));
    
    iFileLoader->GetTotalNumOfImages(aNumOfImages, aNumOfFaces);
    }

TInt CIEEngineImp::GetTotalNumOfImages()
    {
    //DP0_IMAGIC(_L("CIEEngineImp::GetTotalNumOfImages"));
    //return iFileLoader->GetTotalNumOfImages();
    
    TInt numOfImages, numOfFaces;
    if(iImageArrayMode == EImages)
        {
        iFileLoader->GetTotalNumOfImages(numOfImages, numOfFaces);
        return numOfImages;
        }
        
    else if(iImageArrayMode == EFaces)
        {
        iFileLoader->GetTotalNumOfImages(numOfImages, numOfFaces);
        return numOfFaces;
        }
    else
        return 0;
    
    }

TInt CIEEngineImp::DeleteFile(TInt aIndex)
    {  
    DP0_IMAGIC(_L("CIEEngineImp::DeleteFile++"));
    TInt err;
    if(iImageArrayMode == EImages)
        {
        err = iFileLoader->DeleteFile(aIndex);
        iIEBgpsController->FilenameArrayCountChanged(iFileLoader->GetFileNameArray());
        }
    else//EFaces
        {
        err = iFileLoader->DeleteFaceFile(aIndex);
        TInt numOfImages = 0;
        TInt numOfFaces = 0;
        iFileLoader->GetUpdatedNumOfImages(numOfImages, numOfFaces);
        }
    DP1_IMAGIC(_L("CIEEngineImp::DeleteFile-- %d"), err);
    return err;
    }

TInt CIEEngineImp::GetImageName(const TInt aIndex, TFileName& aFileName, TThumbSize aThumbRes)
    {
    DP0_IMAGIC(_L("CIEEngineImp::GetImageName"));
    
    TInt error = KErrNone;
    TRAP(error, iFileLoader->GetFileNameL(aIndex, aFileName, aThumbRes));
    return error;
    }

void CIEEngineImp::GetBitmapL(CImageData* aImageData, CFbsBitmap* aBitmap, TThumbSize aThumbRes)
    {
    DP0_IMAGIC(_L("CIEEngineImp::GetBitmapL"));
    
    iBitmapLoader->GetOneBitmapL(aImageData, aBitmap, aThumbRes);
    }

void CIEEngineImp::GetFileNameL(const TInt aIndex, TThumbSize aThumbRes, TFileName& aFilename)
    {
    iFileLoader->GetFileNameL(aIndex, aFilename, aThumbRes);
    }

void CIEEngineImp::StopTNGeneration(TInt& /*aValue*/)
    {
    //iIEBgpsController->StopTNGeneration(aValue);
    iIEBgpsController->CancelTNGeneration();
    }

TReal CIEEngineImp::GetAspectRatio(TInt aIndex)
    {
    return iIEBgpsController->GetAspectRatio(aIndex);
    }

TReal CIEEngineImp::GetFacesAspectRatio(TInt aIndex)
    {
    return iIEBgpsController->GetFacesAspectRatio(aIndex);
    }

//This function is called in editing mode only when we did not have 320x320 TN ready
void CIEEngineImp::GenerateThumbNailL(const TDes &aOrgFile, TThumbSize aTNResolution)
    {
    DP0_IMAGIC(_L("CIEEngineImp::GenerateThumbNail++"));
    
    iIEBgpsController->GenerateThumbNailL(aOrgFile, aTNResolution);

    DP0_IMAGIC(_L("CIEEngineImp::GenerateThumbNail--"));
    }

//This is used to get face coords from already processed image
void CIEEngineImp::GetFaceCoordinates(const TFileName aTNFileName, RArray<TRect>& aFaceCoordinateArray)
    {
    DP0_IMAGIC(_L("CIEEngineImp::GetFaceCoordinates"));
    aFaceCoordinateArray.Reset();
    //iIEBgpsController->GetFaceCoordinates(aTNFileName, aFaceCoordinateArray);
    }

void CIEEngineImp::AddImageToFaceNameArray()
    {
    DP0_IMAGIC(_L("CIEEngineImp::AddImageToFaceNameArray++"));
    
    //Tarkista etta jos kuva on jo olemassa sita ei lisata enaa uudelleen 
    
    for(TInt i=0; i<iCroppedFilenames.Count(); i++)
        {
        //Add newly cropped filenames to filename array
        CImageData* tmpImageData = CImageData::NewL(EFullSize);
        
        TRAP_IGNORE(tmpImageData->SetFileNameL(iCroppedFilenames[i]));
        
        DP1_IMAGIC(_L("CIEEngineImp::FaceCroppingComplete - Add face, Filename: %S"), &iCroppedFilenames[i] );
        
        TSize size;
        TRAPD(err, iIEEngineUtils.GetImageSizeL(iCroppedFilenames[i], size));
        if (err == KErrNone)
            tmpImageData->SetSize(size);
        
        iFileLoader->AddNewFaceCropImage(tmpImageData, 0);    
        }
    
    TInt numOfImages, numOfFaces;
    iFileLoader->GetUpdatedNumOfImages(numOfImages, numOfFaces);
    
    //TotalNumberOfFaceImagesChanged(numOfFaces);
    
    //Empty data from array after using it
    TInt count = iCroppedFilenames.Count();
    for(TInt i=0; i<count; i++)
        {
        iCroppedFilenames.Remove(0);
        }
    //iFileLoader->StopFileSystemMonitoring();
    
    DP0_IMAGIC(_L("CIEEngineImp::AddImageToFaceNameArray--"));
    }

//If background face Detection is still continuing, we have to use GetSingleFaceCoordinates() function to get face coordinates
void CIEEngineImp::GetSingleFaceCoordinates(TInt aIndex, const TFileName aTNFileName, RArray<TRect>& aFaceCoordinateArray)
    {
    DP0_IMAGIC(_L("CIEEngineImp::GetSingleFaceCoordinates"));
    
    iIEBgpsController->GetSingleFaceCoordinates(aIndex, aTNFileName, aFaceCoordinateArray);
    }


CImageData* CIEEngineImp::GetImageData(TInt aIndex/*, TImageArrayMode aMode*/)
    {
    return iFileLoader->GetImageData(aIndex/*, aMode*/);
    }

void CIEEngineImp::SetImageData(TInt aIndex, CImageData* aGridData)
    {
    iFileLoader->SetImageData(aIndex, aGridData);
    }

TInt CIEEngineImp::GetSelectedImageIndex()
    {
    return iEngineObserver.GetImageIndex();
    }
    


#ifdef _ACCELEROMETER_SUPPORTED_

void CIEEngineImp::SensorDataAvailable(TImagicDeviceOrientation aOrientation, TBool aValue)
    {
    //DP0_IMAGIC(_L("CIEEngineImp::SensorDataAvailable++"));
    
    if(iPrevDeviceOrientation == aOrientation)
        {
        //DP0_IMAGIC(_L("CIEEngineImp::SensorDataAvailable - Orientation not changed--"));
        return;
        }

    iPrevDeviceOrientation = aOrientation;
    
    CImageData* currentImage = NULL ; //GetGridData(GetSelectedImageIndex(), EImages);
    TInt totalImages = GetTotalNumOfImages();
    
    switch ( aOrientation )
        {
#ifdef _S60_3x_ACCELEROMETER_
        case EOrientationDisplayDown: // Portrait Up
#else if _S60_5x_ACCELEROMETER_
        case TSensrvOrientationData::EOrientationDisplayUp: // Portrait Up
        //case TSensrvOrientationData::EOrientationDisplayDown: // Portrait Up
#endif            
            {               
            DP0_IMAGIC(_L("CIEEngineImp::SensorDataAvailable - EOrientationDisplayUp"));
            
            for ( TInt imageIndex = 0 ; imageIndex < totalImages ; imageIndex++ )
                {
                currentImage = GetImageData(imageIndex); 
                currentImage->iGridData.iTargetRotationAngle = (currentImage->GetOrientation() + 90)%360;
                
                TReal targetA = currentImage->iGridData.iTargetRotationAngle;
                TReal currentA = currentImage->iGridData.iRotationAngle;
                
                currentImage->iGridData.iTargetRotationAngle = targetA;
                
                }
            
            iPrevDeviceOrientation = iDeviceOrientation = EOrientationDisplayDown;
            break;
            }

#ifdef _S60_3x_ACCELEROMETER_
        case EOrientationDisplayLeftUp: // Landscape Up
#else if _S60_5x_ACCELEROMETER_
        case TSensrvOrientationData::EOrientationDisplayRightUp: // Landscape Up
        //case TSensrvOrientationData::EOrientationDisplayLeftUp: // Landscape Up
#endif            
                {
            DP0_IMAGIC(_L("CIEEngineImp::SensorDataAvailable - EOrientationDisplayLeftUp"));
            
            for ( TInt imageIndex = 0 ; imageIndex < totalImages ; imageIndex++ )
                {
                currentImage = GetImageData(imageIndex); 
                if ( currentImage->iGridData.iRotationAngle == 270 )
                    {
                    currentImage->iGridData.iRotationAngle = -90; // Rotate
                    }
                currentImage->iGridData.iTargetRotationAngle = (currentImage->GetOrientation() )%360;
                
                TReal targetA = currentImage->iGridData.iTargetRotationAngle;
                TReal currentA = currentImage->iGridData.iRotationAngle;
                
                currentImage->iGridData.iTargetRotationAngle = targetA;
                }              
  
            iPrevDeviceOrientation = iDeviceOrientation = EOrientationDisplayLeftUp;
            break;
            }

            default:
                {
                DP1_IMAGIC(_L("CIEEngineImp::SensorDataAvailable - Ignored orientation: %d"),aOrientation);
                //progressBuf.Append( _L( "Unknown orientation" ) );
                break;
                }
            }
    iEngineObserver.ImageRotated(iDeviceOrientation);
    
    DP0_IMAGIC(_L("CIEEngineImp::SensorDataAvailable--"));
   }

void CIEEngineImp::SetImageRotation(TInt aIndex)
    {
    CImageData* currentImage = NULL ; //GetGridData(GetSelectedImageIndex(), EImages);
    //TInt totalImages = GetTotalNumOfImages();
    
    currentImage = GetImageData(aIndex); 
    
    TReal targetAngle = (currentImage->GetOrientation())%360;
    //TReal currentAngle = currentImage->iGridData.iRotationAngle;
    
    currentImage->iGridData.iTargetRotationAngle = targetAngle;
    
    }
    

TImagicDeviceOrientation CIEEngineImp::GetDeviceOrientation()
    {
    DP1_IMAGIC(_L("CIEEngineImp::GetDeviceOrientation: %d"),iDeviceOrientation);
    return iDeviceOrientation;
   }

void CIEEngineImp::SetDeviceOrientation(TImagicDeviceOrientation aOrientation)
    {
    DP1_IMAGIC(_L("CIEEngineImp::SetDeviceOrientation: %d"),aOrientation);
    
    SensorDataAvailable(aOrientation, EFalse);
   }

void CIEEngineImp::StartAccSensorMonitoring()
    {
    if(iSensorMonitor)
        {
        DP0_IMAGIC(_L("CIEEngineImp::StartAccSensorMonitoring"));
        iSensorMonitor->StartMonitoring();
        }
        
    }

void CIEEngineImp::StopAccSensorMonitoring()
    {
    if(iSensorMonitor)
        {
        DP0_IMAGIC(_L("CIEEngineImp::StopAccSensorMonitoring"));
        iSensorMonitor->StopMonitoring();
        }
    }

#endif

void CIEEngineImp::Stop()
    {
    /*TInt err = KErrNone;
    StopTNGeneration(err); 
    StopFaceDetection(err);*/
    iFileLoader->StopImageFinder();
    }

TBool CIEEngineImp::IsRunning()
    {
    //iIEBgpsController->
    return (iFileLoader->ImageFinderState() != CIEFileLoader::EImageFinderStopped);
    //return ETrue;
    }
 
TBool CIEEngineImp::IsAccelerometerExists()
    {
    DP0_IMAGIC(_L("CIEEngineImp::IsAccelerometerExists"));
    
#ifdef _S60_3x_ACCELEROMETER_
#ifdef SENSOR_API_LOAD_DYNAMICALLY
    DP1_IMAGIC(_L("CIEEngineImp::IsAccelerometerExists - iSensorMonitor exists: %d"),iSensorMonitor);
    return (iSensorMonitor != NULL);
#endif
#endif
    DP0_IMAGIC(_L("CIEEngineImp::IsAccelerometerExists - iSensorMonitor not exists"));
    return EFalse;
    }

TInt CIEEngineImp::GetGleMaxRes()
    {
    return iEngineObserver.GetGleMaxRes();
    }

    
// EOF
 
