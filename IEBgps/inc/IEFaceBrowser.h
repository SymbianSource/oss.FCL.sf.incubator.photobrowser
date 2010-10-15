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

#ifndef __IEFACEBROWSER_H__
#define __IEFACEBROWSER_H__

#include <e32base.h>
#include <f32file.h>
#include <f32file.h>
//#include <IDLImageProcessing.h>
#include <ImageConversion.h>
//#include <IclExtJpegApi.h>

#include "IEBgpsInfo.h"
#include "IEImageData.h"
#include "IEImageEncoder.h"
#include "IEImageDecoder.h"
#include "IEEngineUtils.h"

// Foraward class declaration
class CIEImageDecoder;
class CIEImageEncoder;

// Class declaration
class MIEFaceBrowserObserver
{
public:
    virtual void FaceBrowsingComplete() = 0;
    virtual void FaceBrowsingError(TInt aError) = 0;
    virtual void FaceSingleFaceBrowsingComplete() = 0;
    
};

class CFaceBrowser : public CActive, 
                        public MDecodingObserver,
                        public MEncodingObserver
#ifdef IDL_BGPS                        
                        , public MIDLObserver
#endif
{
private:
    enum TFaceBrowsingState
        {
        EStateIdle = 0,
        EFaceBrowsingRunning,
        ESingleFaceBrowsingRunning,
        EFaceBrowsingPaused,
        EFaceBrowsingStopped,
        EFaceBrowsingCompleted,
        EFaceCroppingRunning,
        EEncodingFaces,
        EFaceCroppingCompleted,
        ESingleFaceBrowsingComplete,
        ECreatingBitmap
        };
/*
    enum TFaceBrowsingMode
        {
        EBrowseModeNone = 0,
        EBrowseModeSingleImage,
        EBrowseModeBulkImages,
        EModeCroppingFaces        
        };
*/
    class TCroppedFaces
        {
        public:
            HBufC8* iYuvdata;
            TFileName iFileName;
            TSize iCroppedSize;
        };
public: // First phase constructor and destructor
    static CFaceBrowser* NewLC(RFs& aFileServer, MIEFaceBrowserObserver& aFaceBrowserObserver);
    virtual ~CFaceBrowser();

private: // Second phase constuctor and C++ default constructor
    void ConstructL();
    CFaceBrowser(RFs& aFileServer, MIEFaceBrowserObserver& aFaceBrowserObserver);

protected: // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    
public: // From MDecodingObserver
    void YuvImageReady(TInt aError);
    void BitmapReady(TInt aError);

public: // From MEncodingObserver
    void JpegImageReady(TInt aError);
    
public: // From MIDLObserver
    inline void ProcessingComplete(TDesC8& /*aData*/){};
    inline void HandleError(TInt /*aError*/){}; 

public: // New public functions
    void StartFaceBrowsing(RArray<CImageData*> aImageDataArray);
    void StartSingleFaceBrowsing(TInt aIndex, RArray<TRect>* aImageCoordArray, CImageData* aImageData);
    void CancelFaceBrowsing();
    TInt FindFaces(const TFileName a128x128TNFileName, RArray<TRect>& aCordArray);
    TInt GetNumberOfFaces(const TFileName aFile);
    void StartFaceCropping(RArray<CImageData*> aImageDataArray, RArray<TFileName>* aCroppedFilenames);
    void CancelFaceCropping();
    TInt AddFaceCoordinate(const TFileName a128x128TNFileName, RArray<TRect>& aCordArray);
    TInt RemoveFaceCoordinate(const TFileName a128x128TNFileName, RArray<TRect>& aCordArray);
    HBufC8* ReadExifMakerNoteL(const TDes &aFileName, TInt& aSize);
    
    
private: // Internal functions
    
    // Common functions
#ifdef IDL_BGPS
    void InitializeL(const TIDLFeatures aIDLFeature, const TSize aInSize, const TSize aOutSize, TAny* aValue, TBool aInBufferCreate);
#endif
    void PrepareInOutBuffersL(TBool aInBufferCreate, const TInt aInBufSize, TBool aOutBufferCreate, const TInt aOutBufSize);
    TFileName MakeTNFileName(const TFileName aImageFileName, TBool a128TNFile, TBool a320TNFileName);
    void Cleanup();
    void Cleanup2();
    void ContinueLoop();
    TBool CheckOddSize(const TSize aSize);
    
    //New function to handle RGB conversion
    void ContinueFBAfterImageConversionL();
    void ConvertRgb2Yuv(CFbsBitmap* aSourceBitmap, TUint8* aYuv, TInt aBytesPerPixel, const TSize aSize);
        
    // Face browsing related functions
    void BrowseFacesL(CImageData* aImageData);
#ifdef IDL_BGPS
    void BrowseFacesL(TFileName a128x128TNFileName, RArray<TRect>& aFaceCoordinates);
#endif
    void GetFaceCoordinates(TInt& aNumberOfFaces, RArray<TRect>& aCordArray);
    //void WriteFaceCoordinatesToExifDataL(const TFileName a128x128TNFileName, RArray<TRect>& aCordArray);
    //void IsCoordinateExistsL(const TFileName a128x128TNFileName, TBool& aBool);
    //void ReadFaceCoordinatesL(const TFileName a128x128TNFileName, RArray<TRect>& aCordArray);
        
    // Face cropping related functions    
    void CropFacesL(CImageData* aImageData);
    void CropFacesL(const TFileName aImageFileName, RArray<TRect>& aFaceCoordinates);
    TInt MakeFacesDir(const TFileName aImageName);
    TRect GetFaceRect(const TSize aOrgImageSize, const TSize aRelImageSize, const TRect aFaceRect);    
    void EncodeFaceL(const TCroppedFaces aCroppedFace);
    void CheckCroppedFaceFileNames();
    
    // Face detection
    void WriteFaceCoordToExif(TInt numOfFaces, RArray<TRect> faceCoordinates);

private: // Data members
    RFs& iFileServer;
    MIEFaceBrowserObserver& iFaceBrowserObserver;

    RArray<CImageData*> iImageDataArray;
    RArray<TCroppedFaces> iFaceYuvDataArray;
    RArray<TRect> iFaceCoordinates;
    RArray<TRect>* iTempFaceCoordinates;
    
    CIEImageDecoder* iImageDecoder;
    CIEImageEncoder* iImageEncoder;
    CImageDecoder*   iSymbianImageDecoder;
    
#ifdef IDL_BGPS
    CIDLImageProcessing* iIDLImageProcessor;
#endif
    
    HBufC8* iInputBuffer;
    HBufC8* iOutputBuffer;
    
    CImageData* iCurrentImageData;
    
    TFaceBrowsingState iBrowsingState;
    TFaceBrowsingState iPrevBrowsingState;
//    TFaceBrowsingMode iBrowseMode;
    
    TSize iSize;
    
    TInt iNumberOfImages;
    TInt iNumberOfImagesBrowsed;
    TInt iSingleFaceBrowsingIndex;
    TInt iNumberOfFacesCropped;    
    TInt iNumberOfFaces;   
    
    TFileName iCurrentImageFileName;
    TFileName iCurrent512x512TNFileName;
    TInt        iTotalNumberOfFaces;
    CIEEngineUtils iUtils;
    
    RArray<TFileName>* iCroppedFilenames;
    //RArray<TRect> iImageCoordArray;
    CFbsBitmap*     iBitmap;
};

#endif // __IEFACEBROWSER_H__
