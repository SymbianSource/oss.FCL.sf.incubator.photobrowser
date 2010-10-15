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

#ifndef __IEBGPSCONTROLLER_H__
#define __IEBGPSCONTROLLER_H__

// Include files
#include <e32base.h>
#include <IEImageProcessing.h>
#include <IEImage.h>

#include "IEFileloader.h"
#include "IEEngineUtils.h"


// Forward Class Declaration
class CIEFileLoader;
class CIEEngineUtils;

// Class Declartion
class MIEBgpsControllerObserver
    {
public:
    //virtual void Something() = 0;
    virtual void TNGenerationComplete(TThumbSize aTNRes) = 0;
    virtual void SingleTNGenerationComplete(TInt aIndex, TThumbSize aTNRes) = 0;
    virtual void FaceDetectionComplete() = 0;
    virtual void SingleFaceDetectionComplete() = 0;
    virtual TInt GetSelectedImageIndex() = 0;
    virtual CImageData* GetImageData(TInt aIndex) = 0;
    };

class CIEBgpsController : public CBase, public MIETNObserver
{
public: // First phase constructor and destructor
    static CIEBgpsController* NewL(
            RFs& aFileServer,
            MIEBgpsControllerObserver& aIEBgpsControllerObserver, 
            CIEEngineUtils& aEngineUtils,
            RCriticalSection* aCritical);
    ~CIEBgpsController();
    
private: // Second phase constructor and C++ default constructor
    void ConstructL();
    CIEBgpsController(
            RFs& aFileServer,
            MIEBgpsControllerObserver& aIEBgpsControllerObserver, 
            CIEEngineUtils& aEngineUtils,
            RCriticalSection* aCritical);
    
public: // From MIETNObserver 
    void ThumbnailGenerationCompleted(TInt aErrorCode);
    void ThumbnailGenerationCancelled(TInt aErrorCode);
    void HandleError(TInt aError);
    
public: // New functions
    // TN generation related
    void SetFileLoader(CIEFileLoader* aFileLoader);
    void CreateImageProcessing();
    //void StartTNGeneration(RArray<CImageData*>& aImageArray);
    //void StopTNGeneration(TInt &aValue);
    void GenerateThumbNailL(const TDes &aOrgFile, TThumbSize /*aTNResolution*/);
    void AllFilesAddedToFilenameArrayL();
    TReal GetAspectRatio(TInt aIndex);
    TReal GetFacesAspectRatio(TInt aIndex);
    void GenerateTNForEditedImage(const TFileName aEditedFileName, const TReal aAspectRatio);
    void FilenameArrayCountChanged(const RArray<CImageData*>& aImageDataArray);
    CImageData* GetImageData(const TInt aIndex);
    
    // Face Detection related
    void GetFaceCoordinates(const TFileName a128x128TNFileName, RArray<TRect>& aFaceCoordinateArray);  
    void StartFaceCropping(TInt aIndex, RArray<TFileName>& aFilenames);
    void RemoveFaceCoordinate(const TFileName a128x128TNFileName, RArray<TRect>& aCordArray);
    void AddFaceCoordinate(const TFileName a128x128TNFileName, RArray<TRect>& aCordArray);
    void GetSingleFaceCoordinates(TInt aIndex, const TFileName aTNFileName, RArray<TRect>& aFaceCoordinateArray);
    void CancelTNGeneration();
    
private: // Private functions
    void CreateSingleTNL();    
    void DeleteCorruptedThumbNailFile(TFileName aFileName);
    void CheckOptimalFileFor128x128TnCreation(TInt aIndex, TFileName& aFilename);
    void CheckOptimalFileFor32x32TnCreation(TInt aIndex, TFileName& aFilename);
    
    void Generate128x128Thumbnails(TInt aIndex);
    void Generate32x32Thumbnails(TInt aIndex);
    void Generate512x512Thumbnails(TInt aIndex);
    
    //TReal ReadAspectRatioL(TFileName& aFileName);
    //void CheckFileNamesExits(TInt aIndex, TDes& aFilename);
    void StartTNCreatorL();
    TInt FindMissingTN(TThumbSize& aRes);
    
private: // Data members
    RFs& iFileServer;
    MIEBgpsControllerObserver& iIEBgpsControllerObserver;
    CIEImageProcessing* iIEBgpsClient;
    CIEFileLoader*         iFileLoader;
    CIEEngineUtils&        iIEEngineUtils;
    
    RArray<CImageData*> iImageDataArray;
    RArray<CImageData*> iFaceCropImageDataArray;
    
    TBool i128x128TNCreationOn;
    TBool i32x32TNCreationOn;
    TBool i512x512TNCreationOn;
    
    
    TBool iSingleTNGeneration;
    
    TBool iSingleFaceDetectionOn;
    TBool iBackGroundFaceDetectionOn;
    TBool iBackGroundFaceDetectionComplete;
    
    TSize iTNSize;
    
    TInt iImageIndex;
    TInt iStopTN;
    
    TReal iAspectRatio;
    
    TFileName iTNFilename;
    TFileName iJpegFilename;
    TFileName iSavedFileName;    
        
    TThumbSize iLatestCreatedTNSize;
    CImageData* iTmpImageData;
    CImageData* iImageData;
    
    TInt iSingleFBIndex;
    RArray<TRect>* iSingleFBCoordinateArray;
    RArray<TRect> iFBCoordinateArray;
    
    TInt iTnCreationIndex; 
    TBool iAllTNsDone;
    TBool iTnCreationCancelled;
    RCriticalSection*  iCritical;
    CFbsBitmap* i512x512TnBitmap;
};

#endif // __IEBGPSCONTROLLER_H__
