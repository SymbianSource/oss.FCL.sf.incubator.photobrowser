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
#include <e32debug.h>
#include <e32math.h>
#include <exifmodify.h>
#include <BAUTILS.H>
#include "IEFaceBrowser.h"
#include "IEBgpsInfo.h"
#include "IEImageData.h"
#include "ImagicConsts.h"
#include "debug.h"
#include <hal.h>

// ================= MEMBER FUNCTIONS ================================ //
CFaceBrowser* CFaceBrowser::NewLC(
        RFs& aFileServer, 
        MIEFaceBrowserObserver& aFaceBrowserObserver)
    {
    DP0_IMAGIC(_L("CFaceBrowser::NewLC ++"));
    
    CFaceBrowser* self = new (ELeave) CFaceBrowser(aFileServer, aFaceBrowserObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    
    DP0_IMAGIC(_L("CFaceBrowser::NewLC --"));
    
    return self;
    }

CFaceBrowser::CFaceBrowser(
        RFs& aFileServer, 
        MIEFaceBrowserObserver& aFaceBrowserObserver) :
        iUtils(iFileServer),
        iFileServer(aFileServer), 
        iFaceBrowserObserver(aFaceBrowserObserver), 
        CActive(EPriorityIdle)
    {
    
    }

CFaceBrowser::~CFaceBrowser()
    {
    DP0_IMAGIC(_L("CFaceBrowser::~CFaceBrowser ++"));
    
    if(iImageDecoder)
        {
        iImageDecoder->CancelDecoding();
        
        delete iImageDecoder;
        iImageDecoder = NULL;
        }
    
    if(iImageEncoder)
        {
        iImageEncoder->CancelEncoding();
        
        delete iImageEncoder;
        iImageEncoder = NULL;
        }
    
    if(iSymbianImageDecoder)
    {
        delete iSymbianImageDecoder;
        iSymbianImageDecoder = NULL;   
    }
    
    if(IsActive())
        Cancel();
    
#ifdef IDL_BGPS
    if(iIDLImageProcessor)
        {
        delete iIDLImageProcessor;
        iIDLImageProcessor = NULL;
        }
#endif
    
    if(iInputBuffer)
        {
        delete iInputBuffer;
        iInputBuffer = NULL;
        }
    
    if(iOutputBuffer)
        {
        delete iOutputBuffer;
        iOutputBuffer = NULL;
        }
    
    if(iFaceYuvDataArray.Count() > 0)
        {
        TCroppedFaces croppedFace;
        HBufC8* buffer = NULL;
        TInt count = iFaceYuvDataArray.Count();
        for(TInt i=0; i<count; i++)
            {
            croppedFace = iFaceYuvDataArray[0];
            buffer = croppedFace.iYuvdata;
            
            iFaceYuvDataArray.Remove(0);
            
            delete buffer;
            buffer = NULL;
            }
        }
    
    iFaceYuvDataArray.Close();
    
    if(iImageDataArray.Count() > 0)
        {
        TInt count = iImageDataArray.Count();
        for(TInt i=0; i<count; i++)
            iImageDataArray.Remove(0);
        }

    iImageDataArray.Close();
    
    iFaceCoordinates.Close();
    if (iTempFaceCoordinates)
        iTempFaceCoordinates->Close();
   
    DP0_IMAGIC(_L("CFaceBrowser::~CFaceBrowser --"));    
    }

void CFaceBrowser::ConstructL()
    {
    TInt error = KErrNone;
    
    DP0_IMAGIC(_L("CFaceBrowser::ConstructL ++"));
    
    CActiveScheduler::Add(this);
    
    iImageDecoder = CIEImageDecoder::NewL(iFileServer, *this);
    
    iImageEncoder = CIEImageEncoder::NewL(iFileServer, *this);
    
#ifdef IDLBGPS
    iIDLImageProcessor = CIDLImageProcessing::NewL(*this);
#endif
    
    iBrowsingState = EStateIdle;
    
    iNumberOfImages = 0;
    iNumberOfImagesBrowsed = 0;
    iNumberOfFacesCropped = 0;    
    iNumberOfFaces = 0;
    iTotalNumberOfFaces = 0;
      
    //RDebug::Print(_L("IDL_Engine_Create error = %d"), error);
    
    if(error != KErrNone)
        User::Leave(error);
    
    DP0_IMAGIC(_L("CFaceBrowser::ConstructL --"));    
    }

void CFaceBrowser::RunL()
    {
    DP0_IMAGIC(_L("CFaceBrowser::RunL ++"));
    
    TInt error = KErrNone;
    
    switch(iBrowsingState)
        {
        case ECreatingBitmap:
            {
            DP0_IMAGIC(_L("CFaceBrowser::RunL() - ECreatingBitmap"));
            TInt error = iStatus.Int();
            if(iSymbianImageDecoder)
                {
                delete iSymbianImageDecoder;
                iSymbianImageDecoder = NULL;   
                }
#ifdef IDL_BGPS
            ContinueFBAfterImageConversionL();
#else
            //ExecuteFaceDetectionL();
#endif
            }
            break;
        
        case EFaceBrowsingRunning:
            {
            DP0_IMAGIC(_L("CFaceBrowser::RunL() - EFaceBrowsingRunning"));
            
            TRAP(error, BrowseFacesL(iImageDataArray[iNumberOfImagesBrowsed]));
            
            if(error != KErrNone)
                {
                Cleanup();
                iFaceBrowserObserver.FaceBrowsingError(error);
                }                
            }
            break;
        
        case ESingleFaceBrowsingRunning:
            {
            DP0_IMAGIC(_L("CFaceBrowser::RunL() - ESingleFaceBrowsingRunning"));
            
            //TRAP(error, BrowseFacesL(iImageDataArray[iSingleFaceBrowsingIndex]));
            TRAP(error, BrowseFacesL(iCurrentImageData));
            
            if(error != KErrNone)
                {
                Cleanup();
                iFaceBrowserObserver.FaceBrowsingError(error);
                }                
            }
            break;

        case EFaceBrowsingPaused:
            {
            DP0_IMAGIC(_L("CFaceBrowser::RunL() - EFaceBrowsingPaused"));
            }
            break;
            
        case EFaceBrowsingStopped:
            {
            DP0_IMAGIC(_L("CFaceBrowser::RunL() - EFaceBrowsingStopped"));
            
            Cleanup();
            }
            break;
            
        case EFaceBrowsingCompleted:
            {
            DP0_IMAGIC(_L("CFaceBrowser::RunL() - EFaceBrowsingCompleted"));
            
            Cleanup();
            iFaceBrowserObserver.FaceBrowsingComplete();
            }
            break;
            
        case ESingleFaceBrowsingComplete:
            {
            DP0_IMAGIC(_L("CFaceBrowser::RunL() - ESingleFaceBrowsingComplete"));
            
            Cleanup2();
            iFaceBrowserObserver.FaceSingleFaceBrowsingComplete();
            }
            break;
                               
        default:
            break;
        }
    
    DP0_IMAGIC(_L("CFaceBrowser::RunL --"));    
    }


void CFaceBrowser::CheckCroppedFaceFileNames()
    {
    DP0_IMAGIC(_L("CFaceBrowser::CheckCroppedFaceFileNames"));
    
    TInt count = iFaceYuvDataArray.Count();
    //Check that file was really cropped
    for(TInt i=0; i<count; i++)
        {
        if(BaflUtils::FileExists(iFileServer, iFaceYuvDataArray[i].iFileName))
            {
            //do nothing
            }
        else
            {
            iFaceYuvDataArray.Remove(i);
            }
        //CImageData* data = new CImageData;
        //data->iMGFilename = iFaceYuvDataArray[i].iFileName;
        //iImageDataArray.Append(data);
        }
    }
            

void CFaceBrowser::DoCancel()
    {
    DP0_IMAGIC(_L("CFaceBrowser::DoCancel ++"));
    
    iImageDecoder->CancelDecoding();
    iImageEncoder->CancelEncoding();
    
    DP0_IMAGIC(_L("CFaceBrowser::DoCancel --"));
    }

TInt CFaceBrowser::RunError(TInt aError)
    {
    DP1_IMAGIC(_L("CFaceBrowser::RunError - Error: %d"), aError);
    return aError;
    }

void CFaceBrowser::BitmapReady(TInt /*aError*/)
    {
    
    }

// =============================== PUBLIC FUNCTIONS ============================== //

void CFaceBrowser::StartFaceBrowsing(RArray<CImageData*> aImageDataArray)
    {
    DP0_IMAGIC(_L("CFaceBrowser::StartFaceBrowsing ++"));
    
    if(iBrowsingState != EStateIdle)
        iFaceBrowserObserver.FaceBrowsingError(KErrInUse); // Better error code ?
    
    if(aImageDataArray.Count() == 0)
        {
        DP0_IMAGIC(_L("ImageDataArray is empty!!!!"));
        iFaceBrowserObserver.FaceBrowsingError(KErrArgument);
        }
    else
        {
        iBrowsingState = EFaceBrowsingRunning;
        //Take local copy of imagedata array
        for(TInt i=0; i<aImageDataArray.Count(); i++)
            {
            if(!aImageDataArray[i]->iGridData.iCorrupted)
                iImageDataArray.Append(aImageDataArray[i]);
            }
        
        iNumberOfImages = iImageDataArray.Count();
        
        DP1_IMAGIC(_L("Number of images: %d"), iNumberOfImages);        
        ContinueLoop();        
        }
    
    DP0_IMAGIC(_L("CFaceBrowser::StartFaceBrowsing --"));    
    }

//void CFaceBrowser::StartSingleFaceDetection(TInt aIndex, RArray<TRect>* aImageCoordArray)
//void CFaceBrowser::StartSingleFaceDetection(TInt aIndex, RArray<TRect>* aImageCoordArray, RArray<CImageData*> aImageDataArray)
void CFaceBrowser::StartSingleFaceBrowsing(TInt aIndex, RArray<TRect>* aImageCoordArray, CImageData* aImageData)
    {
    DP0_IMAGIC(_L("CFaceBrowser::StartSingleFaceBrowsing ++"));
    
    iCurrentImageData = aImageData;
    iSingleFaceBrowsingIndex = aIndex;
    iTempFaceCoordinates = aImageCoordArray;
    
    if(iBrowsingState != EStateIdle)
        iFaceBrowserObserver.FaceBrowsingError(KErrInUse); // Better error code ?
    
    iBrowsingState = ESingleFaceBrowsingRunning;
    
    /*//Take local copy of imagedata array
    for(TInt i=0; i<aImageDataArray.Count(); i++)//TODO, copy only one array element, not all in alrray
        {
        if(!aImageDataArray[i]->iGridData.iCorrupted)
            iImageDataArray.Append(aImageDataArray[i]);
        }
    */
    iNumberOfImages = iImageDataArray.Count();
  
    DP0_IMAGIC(_L("CFaceBrowser::StartSingleFaceBrowsing --"));
    ContinueLoop();        
    }

void CFaceBrowser::CancelFaceBrowsing()
    {
    DP0_IMAGIC(_L("CFaceBrowser::CancelFaceBrowsing ++"));
    
    Cleanup2();
    
    if(IsActive())
        Cancel();
    
    //Cleanup();//TODO, check if this is needed
    
    DP0_IMAGIC(_L("CFaceBrowser::CancelFaceBrowsing ++"));
    }

TInt CFaceBrowser::FindFaces(const TFileName a512x512TNFileName, RArray<TRect>& aCordArray)
    {
    DP1_IMAGIC(_L("CFaceBrowser::FindFaces, a512x512TNFileName = %S ++"), &a512x512TNFileName);
    
    TInt error = KErrNone;
    
    TRAP(error, iUtils.ReadFaceCoordinatesL(a512x512TNFileName, aCordArray));
    if(error != KErrNone) return error;
    
    DP0_IMAGIC(_L("CFaceBrowser::FindFaces --"));
    
    return error;    
    }

TInt CFaceBrowser::GetNumberOfFaces(const TFileName /*aFile*/)
    {
    return 0;
    }

//void CIEEngineImp::RemoveExifFaceCoordsL(const TDes& aFilename, TPoint aTlCoord, TPoint aBrCoord, TInt aFaceNumber)


TInt CFaceBrowser::RemoveFaceCoordinate(const TFileName a512x512TNFileName, RArray<TRect>& aCordArray)
    {
    DP0_IMAGIC(_L("CFaceBrowser::RemoveFaceCoordinate++"));
    
     //Read first current make note
     TInt makerNoteSize;
     HBufC8* makerNote = ReadExifMakerNoteL(a512x512TNFileName, makerNoteSize);
     
     //Read first current maker note to new array from given file
     //RArray<TRect> newCordArray;
     //ReadFaceCoordinatesL(a512x512TNFileName, newCordArray);
     
     //Allocate buffer for coords to be removed
     HBufC8* heapComment = HBufC8::NewL(100);
     TPtr8 ptrCoords = heapComment->Des();
     //Copy coords to be removed to descriptor
     for(TInt i=0; i < aCordArray.Count(); i++)
         {
         ptrCoords.AppendNum(aCordArray[i].iTl.iX);
         ptrCoords.Append(' ');
         ptrCoords.AppendNum(aCordArray[i].iTl.iY);
         ptrCoords.Append(' ');
         ptrCoords.AppendNum(aCordArray[i].iBr.iX );
         ptrCoords.Append(' ');
         ptrCoords.AppendNum(aCordArray[i].iBr.iY);
         ptrCoords.Trim();
         }
     
     //Find coordinates from maker note
     TPtr8 tmpPtr = makerNote->Des();
     TInt res = tmpPtr.Find(ptrCoords);
     
     if(res == KErrNotFound)
         return res;
         
     //Remove coordinates from maker note
     TInt l = ptrCoords.Length();
     tmpPtr.Delete(res, ptrCoords.Length()+1);
     
     //Find number of faces from maker note and update it
     _LIT8(KNumberOfFace, "#");
     res = tmpPtr.Find(KNumberOfFace);
     
     TLex8 lex(makerNote->Ptr());
     lex.SkipAndMark(res+1);
     TInt faceCount = 0;
     lex.Val(faceCount);
     
     //Check lenght of number of faces string
     TInt length = 0;
     //TInt aFaceNumber = 1;
     if(faceCount < 10)
         length = 1;
     else
         length = 2;
          
     HBufC8* numberOfFaces = HBufC8::NewL(length);
     TPtr8 FaceNroPtr = numberOfFaces->Des();
     FaceNroPtr.AppendNum(faceCount-1);
              
     tmpPtr.Replace(res+1, length, FaceNroPtr);
     //TPtr8 numberOfFaces;
     
     delete numberOfFaces;
     //numberOfFaces.Copy();
     
     
     // 1. Read JPEG image from the file to a buffer...
     RFile file;
     User::LeaveIfError( file.Open( iFileServer, a512x512TNFileName, EFileWrite ) );
     CleanupClosePushL( file );
     TInt size = 0;
     file.Size(size);
     HBufC8* jpegImage = HBufC8::NewL( size );
     CleanupStack::PushL( jpegImage );
     TPtr8 bufferDes( jpegImage->Des() );
     User::LeaveIfError( file.Read( bufferDes ) );
     CleanupStack::Pop( jpegImage );
     CleanupStack::PopAndDestroy();
     CleanupStack::PushL( jpegImage );
     
     file.Close();
     
     // 2. Instantiate Exif modifier in ECreate mode...
     CExifModify* modify = CExifModify::NewL( jpegImage->Des(), CExifModify::EModify );
     CleanupStack::PushL( modify );
     
     //3. Insert (Set) at least the mandatory Exif data.
     //TInt descSize = 300;
     //HBufC8* heapComment = HBufC8::NewL(descSize);
     TPtr8 ptr = makerNote->Des();
     
     modify->SetMakerNoteL(ptr);
     //modify->SetMakerNoteL(makerNote->Des());
     
     
     // 4. Get the new Exif image...
     // If zero length descriptor is given instead of jpeg->Des(), then only the
     // Exif meta data is returned.
     //HBufC8* newExif = modify->WriteDataL( jpegImage->Des() );
     HBufC8* newExif;
     TRAPD(err, newExif = modify->WriteDataL( jpegImage->Des() ));
     
     if(err != KErrNone)
         {
         TInt i=0;
         }
     
     //TPtr8 tmp = newExif->Des();
     
     User::LeaveIfError( file.Replace( iFileServer, a512x512TNFileName, EFileWrite ) );
     //Write Exif and jpeg image back to jpeg file
     User::LeaveIfError(file.Write(*newExif));
     
     // Process the new Exif data
     delete newExif;
     newExif = NULL;
     
     // 5. Delete the modifier instance...
     CleanupStack::PopAndDestroy( modify );
     CleanupStack::PopAndDestroy( jpegImage );
     
     file.Close();
    
    DP0_IMAGIC(_L("CFaceBrowser::RemoveFaceCoordinate--"));
    return KErrNone;
    }


//Writes face coordinates to Exif data if faces was found
TInt CFaceBrowser::AddFaceCoordinate(const TFileName aFilename, RArray<TRect>& aCordArray)
    {
    DP0_IMAGIC(_L("CFaceBrowser::AddFaceCoordinate++"));
    //Read first current maker note to new array from given file
    RArray<TRect> newCordArray;
    iUtils.ReadFaceCoordinatesL(aFilename, newCordArray);
     
    //Append existing coords to new coords array
    for(TInt i=0; i<newCordArray.Count(); i++)
        {
        aCordArray.Append(newCordArray[i]);
        }
              
     //Write all coords to file exif data manufactorer note
    iUtils.WriteFaceCoordinatesL(aFilename, aCordArray);
     
    newCordArray.Close();
    
    DP0_IMAGIC(_L("CFaceBrowser::AddFaceCoordinate--"));

    return KErrNone;
    }


HBufC8* CFaceBrowser::ReadExifMakerNoteL(const TDes &aFileName, TInt& aSize)
    {
    DP0_IMAGIC(_L("CFaceBrowser::ReadExifMakerNoteL++"));
    HBufC8* makerNote;
    
    // 1. Read Exif image from the file to a buffer...
    RFile file;
    User::LeaveIfError( file.Open( iFileServer, aFileName , EFileRead ) );
    CleanupClosePushL( file );
    TInt size = 65536;
        
    HBufC8* exif = HBufC8::NewL( size );
    CleanupStack::PushL( exif );
    TPtr8 bufferDes( exif->Des() ); 
    TInt err1 = file.Read( bufferDes );
    TInt length = bufferDes.Length();
    
    if(length <= 0 )
        {
        CleanupStack::Pop( exif );
        CleanupStack::PopAndDestroy();
        //return NULL;
        }
    else
        {
        CleanupStack::Pop( exif );
        CleanupStack::PopAndDestroy();
        CleanupStack::PushL( exif );
        
        // 2. Instantiate Exif reader...
        CExifRead* ExifRead;
        TInt err = 0;
        TRAP(err, ExifRead = CExifRead::NewL( exif->Des(),CExifRead::ENoJpeg ));//CExifRead::ENoTagChecking | CExifRead::ENoJpeg
        
        //HBufC8* comment = NULL;
        if(err != KErrNone)
            {
            }
        else
            {
            CleanupStack::PushL( ExifRead );
        
            // 3. Get required data from the Exif image...
            TUint32  xRes;
            TUint32  yRes;
            ExifRead->GetPixelXDimension(xRes);
            ExifRead->GetPixelYDimension(yRes);  
                    
            makerNote = ExifRead->GetMakerNoteL(); 
            
            // Delete the reader instance...
            CleanupStack::PopAndDestroy( ExifRead );
            }
       
        file.Close();
        CleanupStack::PopAndDestroy( exif );
        //DP0_IMAGIC(_L("CIEEngineImp::ReadExifData--"));
        
        if(makerNote == NULL)
            {
            User::Leave(KErrNotFound);
            }
        else
            {
            aSize = makerNote->Length();
            //return (TUint8*)makerNote->Des().Ptr();
            return makerNote;
            }
        //return comment->Des()->Ptr();
        
        }
    DP0_IMAGIC(_L("CFaceBrowser::ReadExifMakerNoteL--"));
    
    return NULL;
    }

    
// ================================ FACE BROWSING RELATED FUNCTIONS =============================== //

//This is called from RunL when face browsing is started
void CFaceBrowser::BrowseFacesL(CImageData* aImageData)
    {
    DP0_IMAGIC(_L("CFaceBrowser::BrowseFacesL++"));
    
    if(iBrowsingState != EFaceBrowsingRunning && iBrowsingState != ESingleFaceBrowsingRunning)
        User::Leave(KErrNotSupported);
    
    TBool coordnatesExists = EFalse;
    iCurrentImageData = aImageData;
    
    TFileName filename;
    iCurrentImageData->GetFileName(filename, ESize512x512);
    DP1_IMAGIC(_L("CFaceBrowser::BrowseFacesL 512x512TNFile = %S ++"), &filename);
    
    // 512x512 TN are supposed to be created in TN generation phase and should be present
    if(!iCurrentImageData->IsImageReady(ESize512x512))
        {
        Cleanup();
        User::Leave(KErrNotFound);
        }        
    else
        {
        iCurrentImageData->GetFileName(iCurrent512x512TNFileName, ESize512x512); //target file to write face coords
        }
    
    //If coordinates exist we do not process image anymore
    TRAPD(error, iUtils.ReadFaceCoordinatesL(iCurrent512x512TNFileName, iFaceCoordinates));
    if (error == KErrNone)
        {
        if(iBrowsingState == ESingleFaceBrowsingRunning)
            {
            if(iBrowsingState == ESingleFaceBrowsingRunning)
                 for(TInt i = 0; i<iFaceCoordinates.Count(); i++)
                    iTempFaceCoordinates->Append(iFaceCoordinates[i]);
            
            iBrowsingState = ESingleFaceBrowsingComplete;
            }
        else
            {
            //Increment to the next image index
            iNumberOfImagesBrowsed++;
            //Check if we are at the end of image array
            if(iNumberOfImagesBrowsed == iNumberOfImages)
                {
                iBrowsingState = EFaceBrowsingCompleted;
                }
            }
        
        //and continue to next image
        ContinueLoop();
        }
    //Here we have image which has to be processed for face detection
    else
        {
        iPrevBrowsingState = iBrowsingState;
        iBrowsingState = ECreatingBitmap;
        
        TSize size;
        if(aImageData->GetAspectRatio() > 1)
            {
            size.iWidth=320;
            size.iHeight=320/aImageData->GetAspectRatio();
            }
        else
            {
            size.iWidth=320*aImageData->GetAspectRatio();
            size.iHeight=320;
            }
        
        if(size.iWidth%2 != 0)
            size.iWidth++;
        if(size.iHeight%2 != 0)
            size.iHeight++;
        
        iBitmap = new (ELeave) CFbsBitmap();
        iBitmap->Create(size, EColor16M);
        
        iSymbianImageDecoder = CImageDecoder::FileNewL(iFileServer, iCurrent512x512TNFileName, CImageDecoder::EPreferFastDecode);
        iSymbianImageDecoder->Convert(&iStatus, *iBitmap, 0);
        
        if(!IsActive())
            SetActive();
        
        }
    
    DP0_IMAGIC(_L("CFaceBrowser::BrowseFacesL --"));
    }

#ifdef IDLBGPS
void CFaceBrowser::ContinueFBAfterImageConversionL()
    {
    DP0_IMAGIC(_L("CFaceBrowser::ContinueFBAfterImageConversionL ++"));
    
    iBrowsingState = iPrevBrowsingState;
    
    TSize size = iBitmap->SizeInPixels();
    
    //Init IDL
    TInt value = 0;
    InitializeL(EIDLFeatureFaceDetection, size, size, &value, ETrue);
    
    // Code for the previous face detection which needed YUV input
    //TUint8* yuvArray;
    //TInt yuvDataSize = size.iHeight * size.iWidth * 3 / 2;
    //yuvArray = new TUint8 [yuvDataSize];
    //
    //ConvertRgb2Yuv(iBitmap, yuvArray, 3/*aBytesPerPixel*/, size);
    // 
    //iInputBuffer->Des().Copy(yuvArray, yuvDataSize);
    
    TInt bitmapSize = size.iHeight * size.iWidth * 3;
    
    // This is in the BGR order
    iInputBuffer->Des().Copy((TUint8 *)iBitmap->DataAddress(), bitmapSize);
    //delete yuvArray;
#if 0
    //just for testing --->
    _LIT(KTestPath, "C:\\Images\\RGB2YUV.YUV");
    TFileName temp;
    temp.Copy(KTestPath);
    
    RFile file;
    User::LeaveIfError(file.Replace(iFileServer, temp, EFileWrite));
    TInt dataSize = iInputBuffer->Size();
    file.Write(iInputBuffer->Des());
    //file.Write(yuvArray);
    file.Flush();
    file.Close();    
    //TODO, just for testing <---
#endif

#if 0
    //just for testing ---> 
    _LIT(KTestPath, "C:\\Images\\RawRGB.rgb");
    TFileName temp;
    temp.Copy(KTestPath);
        
    RFile file;
    User::LeaveIfError(file.Replace(iFileServer, temp, EFileWrite));
    TInt dataSize = iInputBuffer->Size();
    file.Write(iInputBuffer->Des());
    file.Flush();
    file.Close();    
    //just for testing <---
#endif

        
    TInt error = KErrNone;
    TRAP(error, BrowseFacesL(iCurrent512x512TNFileName, iFaceCoordinates));
    
    if(error != KErrNone)
        {
        Cleanup();
        iFaceBrowserObserver.FaceBrowsingError(error);
        }
    
#if 0
    //only for debug
    _LIT(KTestPath, "C:\\Images\\RGB2YUV.MBM");
    TFileName temp;
    temp.Copy(KTestPath);
    iBitmap->Save(temp);
#endif
    
    iBitmap->Reset();
    delete iBitmap;
    
    if(iBrowsingState == ESingleFaceBrowsingRunning)
        iBrowsingState = ESingleFaceDetectionComplete;
    
    ContinueLoop();
    
    DP0_IMAGIC(_L("CFaceBrowser::ContinueFBAfterImageConversionL --"));
    }
#endif

//http://wiki.forum.nokia.com/index.php/TSS001195_-_RGB_to_YUV_conversion
void CFaceBrowser::ConvertRgb2Yuv(CFbsBitmap* aSourceBitmap, TUint8* aYuv, TInt aBytesPerPixel, const TSize aSize)
    {
    DP0_IMAGIC(_L("CFaceBrowser::ConvertRgb2Yuv++"));
    
    // Definitions that help access each colour component in source bitmap
    #define   sR ((TInt32)(s[2]))
    #define   sG ((TInt32)(s[1]))
    #define   sB ((TInt32)(s[0]))
     
    const TInt KImageNumPixels = aSize.iHeight*aSize.iWidth;
     
    // Lock source bitmap (CFbsBitmap)
    aSourceBitmap->LockHeap(EFalse);
    TUint8* s = (TUint8*)aSourceBitmap->DataAddress();
        
    TInt i = 0;
    TInt ui = KImageNumPixels;
    TInt vi = KImageNumPixels + KImageNumPixels/4;
       
    //iYuv is an array of TUint8 values, length (KImageNumPixels*3/2)
         
    for(TInt j=0; j < aSize.iHeight; j++)
        {
        for(TInt k=0; k < aSize.iWidth; k++)
            {
            // Y value is generated for each pixel
            aYuv[i] = (TUint8)( (  66*sR + 129*sG +  25*sB + 128) >> 8 ) + 16;
              
            // U, V values are generated for every other pixel on every other scanline 
            if(0 == j%2 && 0 == k%2)
                {
                aYuv[ui++] = (TUint8)( (-38*sR - 74*sG + 112*sB + 128) >> 8 ) + 128;
                aYuv[vi++] = (TUint8)( (112*sR - 94*sG - 18*sB + 128) >> 8 ) + 128;
                }
            i++; 
            s+=aBytesPerPixel; // Number of bytes representing one pixel in source
                               // bitmap e.g. if bitmap display mode == EColor16M 
                               // (24bits/pixel), then iBytesPerPixel == 3
            }
        }
       
    aSourceBitmap->UnlockHeap(EFalse);
    // iYuv now contains the source frame converted to YUV420p format
    
    DP0_IMAGIC(_L("CFaceBrowser::ConvertRgb2Yuv--"));
    }

#ifdef IDLBGPS
//This is called after YUV data has been completed
void CFaceBrowser::BrowseFacesL(const TFileName a512x512TNFileName, RArray<TRect>& aFaceCoordinates)
    {
    DP0_IMAGIC(_L("CFaceBrowser::BrowseFacesL ++"));
    
       
    TPtr8 inBuffer = iInputBuffer->Des();
    TPtr8 outBuffer = iOutputBuffer->Des();
    
    iIDLImageProcessor->SetInOutDataL(inBuffer, outBuffer);
    iIDLImageProcessor->ProcessImageL();
    
    TInt count = aFaceCoordinates.Count();
    for(TInt i=0; i<count; i++)
        aFaceCoordinates.Remove(0);
    
    GetFaceCoordinates(iNumberOfFaces, aFaceCoordinates);
    
    //Add number of faces to image data
    iCurrentImageData->SetNumberOfFaces(iNumberOfFaces);
    
    if(iBrowsingState == ESingleFaceBrowsingRunning)
         for(TInt i = 0; i<aFaceCoordinates.Count(); i++)
            iTempFaceCoordinates->Append(aFaceCoordinates[i]);
 
    iUtils.WriteFaceCoordinatesL(a512x512TNFileName, aFaceCoordinates);
    
    if(iBrowsingState == EFaceBrowsingRunning)
        {
        if(iBrowsingState == EFaceBrowsingRunning)
            iNumberOfImagesBrowsed++;
        
        if(iNumberOfImagesBrowsed == iNumberOfImages)
            iBrowsingState = EFaceBrowsingCompleted;
        }
    else if(iBrowsingState == ESingleFaceBrowsingRunning)
        {
        iBrowsingState = ESingleFaceDetectionComplete;
        }
    
        
    DP0_IMAGIC(_L("CFaceBrowser::BrowseFacesL --"));
    }

void CFaceBrowser::GetFaceCoordinates(TInt& aNumberOfFaces, RArray<TRect>& aCordArray)
    {
    DP0_IMAGIC(_L("CFaceBrowser::GetFaceCoordinates ++"));
    
    iIDLImageProcessor->GetFacesDetected(aNumberOfFaces);
    
    if(aNumberOfFaces <= 0)
        DP0_IMAGIC(_L("No faces found!!!"));
    else
        {
        DP1_IMAGIC(_L("Number of faces found: %d"), aNumberOfFaces);
        
        // Clean up the coordinate array
        if(aCordArray.Count() > 0)
            {
            TInt count = aCordArray.Count();
            for(TInt i=0; i<count; i++)
                aCordArray.Remove(0);
            }
        
        iIDLImageProcessor->GetFaceCoordinates(aCordArray);
        aCordArray.SortSigned();
        }
    
    DP0_IMAGIC(_L("CFaceBrowser::GetFaceCoordinates --"));
    }
#endif

// =============================== FACE CROPPING RELATED FUNCTIONS ============================ //

void CFaceBrowser::CropFacesL(CImageData* aImageData)
    {
    DP0_IMAGIC(_L("CFaceBrowser::CropFacesL ++"));
    
    TInt error = KErrNone;
    iCurrentImageData = aImageData;
    iNumberOfFacesCropped = 0;
    
    TFileName imageFileName;
    aImageData->GetFileName(imageFileName, EFullSize);
    error = MakeFacesDir(imageFileName);
    
    if(error != KErrNone && error != KErrAlreadyExists)
        User::Leave(error);
    
    // 512x512 and 320x320 TN are supposed to be created in TN generation phase and should be present
    if(!iCurrentImageData->IsImageReady(ESize512x512) || !iCurrentImageData->IsImageReady(EFullSize))
        {
        Cleanup();
        User::Leave(KErrNotFound);
        }        
    else
        {
        iCurrentImageData->GetFileName(iCurrent512x512TNFileName, ESize512x512);
        iCurrentImageData->GetFileName(iCurrentImageFileName, EFullSize);
        }
    
    // We assume that all the images were searched for faces before face cropping started
    // Hence not checking if face coordinates exists or n ot
    iUtils.ReadFaceCoordinatesL(iCurrent512x512TNFileName, iFaceCoordinates);
    
    if(iFaceCoordinates.Count() == 0)
        {
        iNumberOfImagesBrowsed++;
        
        if(iNumberOfImagesBrowsed == iNumberOfImages)
            iBrowsingState = EFaceCroppingCompleted;
                
        ContinueLoop();
        }
    else
        {
        iTotalNumberOfFaces +=  iFaceCoordinates.Count();
        DP1_IMAGIC(_L("iTotalNumberOfFaces = %d"), iTotalNumberOfFaces);
        
        iImageDecoder->GetImageSizeL(iCurrentImageFileName, iSize);
        
        if(!CheckOddSize(iSize))
            {
            TInt size = iSize.iWidth * iSize.iHeight * 3 / 2;
                    
            PrepareInOutBuffersL(ETrue, size, EFalse, 0);
            
            iImageDecoder->ConvertJpeg2YuvL(iCurrentImageFileName, *iInputBuffer);
            }
        else
            {
            iNumberOfImagesBrowsed++;
            
            if(iNumberOfImagesBrowsed == iNumberOfImages)
                iBrowsingState = EFaceCroppingCompleted;
                            
            ContinueLoop();
            }        
        }
    
    DP0_IMAGIC(_L("CFaceBrowser::CropFacesL --"));
    }


void CFaceBrowser::CropFacesL(const TFileName /*aImageFileName*/, RArray<TRect>& aFaceCoordinates)
    {
    DP1_IMAGIC(_L("CFaceBrowser::CropFacesL++, number of faces: %d"), aFaceCoordinates.Count());

#ifdef IDLBGPS_CROP

    TRect rect(0, 0, 0, 0);
    TParse parser;
    parser.Set(aImageFileName, NULL, NULL);
        
    iNumberOfImagesBrowsed++;
    
    // Clean the face data array
    if(iFaceYuvDataArray.Count() > 0)
        {
        TCroppedFaces temp;
        HBufC8* buffer = NULL;
        TInt count  = iFaceYuvDataArray.Count();
        
        for(TInt i=0; i<count; i++)
            {
            temp = iFaceYuvDataArray[0];
            
            iFaceYuvDataArray.Remove(0);
            
            buffer = temp.iYuvdata;
            
            delete buffer;
            buffer = NULL;
            }
        }
    
    for(TInt faceIndex=0; faceIndex<aFaceCoordinates.Count(); faceIndex++)
        {
        TSize tnSize;
        TFileName thumbnailFileName;
        
        iCurrentImageData->GetFileName(thumbnailFileName, ESize32x32);
        iImageDecoder->GetImageSizeL(thumbnailFileName, tnSize);
        
        rect = GetFaceRect(iSize, tnSize, aFaceCoordinates[faceIndex]);
        
        InitializeL(EIDLFeatureCrop, iSize, rect.Size(), &rect, EFalse);
        
        TPtr8 inBuffer = iInputBuffer->Des();
        TPtr8 outBuffer = iOutputBuffer->Des();
        
        iIDLImageProcessor->SetInOutDataL(inBuffer, outBuffer);
        iIDLImageProcessor->ProcessImageL();
        
        DP1_IMAGIC(_L("Face cropped: %d"), faceIndex);
        
        TCroppedFaces croppedFace;
        croppedFace.iYuvdata = HBufC8::NewL(iOutputBuffer->Size());
        croppedFace.iYuvdata->Des().Copy(iOutputBuffer->Des());
        
        croppedFace.iFileName.Append(parser.DriveAndPath());
        croppedFace.iFileName.Append(KFaces);
        croppedFace.iFileName.Append(parser.Name());
        croppedFace.iFileName.Append(KUnderScr);
        croppedFace.iFileName.AppendNum(faceIndex);
        croppedFace.iFileName.Append(parser.Ext());
        
        croppedFace.iCroppedSize = rect.Size();        
        
        iFaceYuvDataArray.Append(croppedFace);
        iCroppedFilenames->Append(croppedFace.iFileName);
        }
#endif
    
    iBrowsingState = EEncodingFaces;
    
    DP0_IMAGIC(_L("CFaceBrowser::CropFacesL --"));
    }

TRect CFaceBrowser::GetFaceRect(const TSize aOrgImageSize, const TSize aRelImageSize, const TRect aFaceRect)
    {
    DP0_IMAGIC(_L("CFaceBrowser::GetFaceRect ++"));

    TRect faceRect(0, 0, 0, 0);
    
    faceRect.iTl = aFaceRect.iTl;
    faceRect.iBr = aFaceRect.iBr;
    
    TReal width = 0;
    TReal height = 0;
    TReal aspectRatioX = 0;
    TReal aspectRatioY = 0;
    
    //Converting the face rect to original image size
    aspectRatioX = (TReal)aOrgImageSize.iWidth / (TReal)aRelImageSize.iWidth;
    aspectRatioY = (TReal)aOrgImageSize.iHeight / (TReal)aRelImageSize.iHeight;
    
    //Make cropped rect bigger
    //faceRect.Grow(faceRect.Width()/4, (faceRect.Height()/2));
    faceRect.Grow(faceRect.Width()/2.5, (faceRect.Height()/1.5));
    //And move ract bit higher
    //faceRect.Move(0, -(faceRect.Height()/6));
    faceRect.Move(0, -(faceRect.Height()/8));
    
    if(aspectRatioX != 1.0 || aspectRatioY != 1.0)
        {
        //Scale cropping rect size to original  
        faceRect.iTl.iX *= aspectRatioX;
        faceRect.iTl.iY *= aspectRatioY;
        faceRect.iBr.iX *= aspectRatioX;
        faceRect.iBr.iY *= aspectRatioY;
        
        // Check for extreme values and negative values
        // Any invalid values will be made max valid values
        if(faceRect.iTl.iX < 0) faceRect.iTl.iX = 0;
        if(faceRect.iTl.iY < 0) faceRect.iTl.iY = 0;
        if(faceRect.iTl.iX >= aOrgImageSize.iWidth) faceRect.iBr.iX = aOrgImageSize.iWidth-1;
        if(faceRect.iTl.iY >= aOrgImageSize.iHeight) faceRect.iBr.iY = aOrgImageSize.iHeight-1;
        if(faceRect.iBr.iX > aOrgImageSize.iWidth) faceRect.iBr.iX = aOrgImageSize.iWidth;
        if(faceRect.iBr.iY > aOrgImageSize.iHeight) faceRect.iBr.iY = aOrgImageSize.iHeight;
        
        }    
    
    // Make sure that the width and height are divisible by 2, else encoder/decoder will give -10 error
    TReal remainder = 0;
    
    Math::Mod(remainder, faceRect.Size().iWidth, 2);
    
    if(remainder != 0)
        faceRect.iBr.iX--;
    
    Math::Mod(remainder, faceRect.Size().iHeight, 2);
    
    if(remainder != 0)
        faceRect.iBr.iY--;

    DP4_IMAGIC(_L("CFaceBrowser::GetFaceRect faceRect(%d, %d, %d, %d)--"), faceRect.iTl.iX, faceRect.iTl.iY, faceRect.iBr.iX, faceRect.iBr.iY);
    
    return faceRect;
    }

TInt CFaceBrowser::MakeFacesDir(const TFileName aImageName)
    {
    DP0_IMAGIC(_L("CFaceBrowser::MakeFacesDir ++"));
    
    TInt error = KErrNone;
    
    TParse parser;
    parser.Set(aImageName, NULL, NULL);
    
    TFileName faceDir = parser.DriveAndPath();
    faceDir.Append(KFaces);
    
    if(BaflUtils::PathExists(iFileServer, faceDir))
        error = KErrAlreadyExists;
    else
        {
        error = iFileServer.MkDir(faceDir);
        
        if(error == KErrNone)
            error = iFileServer.SetAtt(faceDir, KEntryAttNormal, KEntryAttNormal);
        }
    
    DP0_IMAGIC(_L("CFaceBrowser::MakeFacesDir --"));
    
    return error;
    }

void CFaceBrowser::EncodeFaceL(const TCroppedFaces aCroppedFace)
    {
    DP0_IMAGIC(_L("CFaceBrowser::EncodeFaceL ++"));
    
    HBufC8* buffer = aCroppedFace.iYuvdata;
   
    TFileName fileName = aCroppedFace.iFileName;
    TSize size = aCroppedFace.iCroppedSize;
    
    DP1_IMAGIC(_L("Encoding Image: %S"), &fileName);
    DP2_IMAGIC(_L("Size: %dx%d"), size.iWidth, size.iHeight);
    
    iImageEncoder->ConvertYuv2JpegL(fileName, *buffer, size);
    
    DP0_IMAGIC(_L("CFaceBrowser::EncodeFaceL --"));
    }

// =============================== COMMON FUNCTIONS ============================================== //

#ifdef IDLBGPS
void CFaceBrowser::InitializeL(const TIDLFeatures aIDLFeature, const TSize aInSize, const TSize aOutSize, TAny* aValue, TBool aInBufferCreate)
    {
    DP4_IMAGIC(_L("CFaceBrowser::InitializeL, aInsize = %dx%d, aOutSize = %dx%d ++"), aInSize.iWidth, aInSize.iHeight, aOutSize.iWidth, aOutSize.iHeight);
    
//    TInt value = 0;
    TInt outputBufferSize = 0;
    //TInt inputBufferSize = (aInSize.iWidth * aInSize.iHeight * 3 / 2);
    TInt inputBufferSize = aInSize.iWidth * aInSize.iHeight * 3;
    
    iIDLImageProcessor->SetFeatureL(aIDLFeature, aValue);
    //iIDLImageProcessor->InitializeFeatureL(aInSize, 
    //                                        aOutSize, 
    //                                        EIDLFormatYUV420, 
    //                                        EIDLFormatYUV420);
    iIDLImageProcessor->InitializeFeatureL(aInSize, 
                                                aOutSize, 
                                                EIDLFormatRGB, 
                                                EIDLFormatRGB);
    
    iIDLImageProcessor->AllocateBuffersL(outputBufferSize);
    
    PrepareInOutBuffersL(aInBufferCreate, inputBufferSize, ETrue, outputBufferSize);
    
    DP0_IMAGIC(_L("CFaceBrowser::InitializeL --"));
    }
#endif

void CFaceBrowser::PrepareInOutBuffersL(TBool aInBufferCreate, const TInt aInBufSize, TBool aOutBufferCreate, const TInt aOutBufSize)
    {
    DP0_IMAGIC(_L("CFaceBrowser::PrepareInOutBuffersL ++"));
    
    if(aInBufferCreate)
        {
        if(aInBufSize <= 0)
            User::Leave(KErrArgument);
        
        if(iInputBuffer)
            {
            delete iInputBuffer;
            iInputBuffer = NULL;
            }
        
        iInputBuffer = HBufC8::NewL(aInBufSize);
        }
    
    if(aOutBufferCreate)
        {
        if(aOutBufSize <= 0)
            User::Leave(KErrArgument);
        
        if(iOutputBuffer)
            {
            delete iOutputBuffer;
            iOutputBuffer = NULL;
            }
        
        iOutputBuffer = HBufC8::NewL(aOutBufSize);
        }
    
    DP0_IMAGIC(_L("CFaceBrowser::PrepareInOutBuffersL --"));
    }

TBool CFaceBrowser::CheckOddSize(const TSize aSize)
    {
    DP0_IMAGIC(_L("CFaceBrowser::CheckOddSize ++"));
    
    TReal remainder = 0;
    
    Math::Mod(remainder, aSize.iWidth, 2);
    
    if(remainder != 0) return ETrue;
    
    Math::Mod(remainder, aSize.iHeight, 2);
    
    if(remainder != 0) return ETrue;
    
    DP0_IMAGIC(_L("CFaceBrowser::CheckOddSize --"));
    
    return EFalse;
    }

#if 0
TFileName CFaceBrowser::MakeTNFileName(const TFileName aImageFileName, TBool a512TNFile, TBool /*a320TNFileName*/)
    {
    DP1_IMAGIC(_L("CFaceBrowser::Make512x512TNFileName, aImageFileName = %S ++"), &aImageFileName);
    
    TParse fileNameParser;
    fileNameParser.Set(aImageFileName, NULL, NULL);
    
    TFileName tnFileName;
    
    tnFileName = fileNameParser.DriveAndPath();
    
    if(a512TNFile /*&& !a320TNFileName*/)
        {
        tnFileName.Append(K512x512TNFilePath);
        tnFileName.Append(fileNameParser.Name());
        tnFileName.Append(KTNExt);
        }
/*    if(!a512TNFile && a320TNFileName)
        {
        tnFileName.Append(K320TNFilePath);
        tnFileName.Append(fileNameParser.Name());
        tnFileName.Append(K320TNFileExt);
        }*/
    
    DP1_IMAGIC(_L("CFaceBrowser::Make512x512TNFileName fileName512x512TN = %S --"), &tnFileName);
    
    return tnFileName;
    }
#endif    

void CFaceBrowser::Cleanup()
    {
    DP0_IMAGIC(_L("CFaceBrowser::Cleanup ++"));
    
    if(iImageDecoder)
            iImageDecoder->CancelDecoding();
            
    if(iImageEncoder)
        iImageEncoder->CancelEncoding();
        
    if(iFaceCoordinates.Count() > 0)
        {
        TInt count = iFaceCoordinates.Count();
        for(TInt i=0; i<count; i++)
            iFaceCoordinates.Remove(0);
        }
    
    if(iFaceYuvDataArray.Count() > 0)
        {
        TCroppedFaces croppedFaces;
        HBufC8* buffer = NULL;
        TInt count = iFaceYuvDataArray.Count();
        
        for(TInt i=0; i<count; i++)
            {
            croppedFaces = iFaceYuvDataArray[0];
            iFaceYuvDataArray.Remove(0);
            
            buffer = croppedFaces.iYuvdata;
            
            delete buffer;
            buffer = NULL;
            }
        }
/*    
    if(iImageDataArray.Count() > 0)
        {
        TInt count = iImageDataArray.Count();
        for(TInt i=0; i<count; i++)
            iImageDataArray.Remove(0);
        }
 */   
    iNumberOfFaces = 0;
    iNumberOfImages = 0;
    iNumberOfImagesBrowsed = 0;
    iNumberOfFacesCropped = 0;
    
    iCurrentImageData = NULL;
    
    iCurrentImageFileName = KEmptyString;
    iCurrent512x512TNFileName = KEmptyString;
    //iCurrent320x320TNFileName = KEmptyString;
    
    iBrowsingState = EStateIdle;
    
    DP0_IMAGIC(_L("CFaceBrowser::Cleanup --"));
    }

void CFaceBrowser::Cleanup2()
    {
    DP0_IMAGIC(_L("CFaceBrowser::Cleanup2 ++"));
    
    if(iImageDecoder)
        iImageDecoder->CancelDecoding();
        
    if(iImageEncoder)
        iImageEncoder->CancelEncoding();
    
    if(iFaceYuvDataArray.Count() > 0)
        {
        TCroppedFaces croppedFaces;
        HBufC8* buffer = NULL;
        TInt count = iFaceYuvDataArray.Count();
        
        for(TInt i=0; i<count; i++)
            {
            croppedFaces = iFaceYuvDataArray[0];
            iFaceYuvDataArray.Remove(0);
            
            buffer = croppedFaces.iYuvdata;
            
            delete buffer;
            buffer = NULL;
            }
        }

    iBrowsingState = EStateIdle;
    
    DP0_IMAGIC(_L("CFaceBrowser::Cleanup2 --"));
    }

void CFaceBrowser::ContinueLoop()
    {
    DP0_IMAGIC(_L("CFaceBrowser::ContinueLoop++"));
    
    if(!IsActive())
        {
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);            
        }
    
    DP0_IMAGIC(_L("CFaceBrowser::ContinueLoop--"));
    }

void CFaceBrowser::WriteFaceCoordToExif(TInt numOfFaces, RArray<TRect> faceCoordinates) {
    //Add number of faces to image data
    iCurrentImageData->SetNumberOfFaces(numOfFaces);
    
    if(iBrowsingState == ESingleFaceBrowsingRunning)
         for(TInt i = 0; i<iFaceCoordinates.Count(); i++)
            iTempFaceCoordinates->Append(iFaceCoordinates[i]);
 
    iUtils.WriteFaceCoordinatesL(iCurrent512x512TNFileName, faceCoordinates);
}

// EOF
