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

#include <bautils.h>
#include <IEBgpsInfo.h>


#include "IEEngineImp.h"
#include "IEFileLoader.h"
#include "IEImageFinder.h"
#include "ImageMonitorAO.h"

#ifdef _S60_5x_ACCELEROMETER_
#include "IESensorMonitor.h"
#endif

#define IMAGEFINDERMONITORAO

//_LIT(KFacesPath, "ImagicFaces");


CIEFileLoader* CIEFileLoader::NewL(
        RFs& aFileServer, 
        CIEEngineImp* aEngImp, 
        RCriticalSection* aCritical/*, TRequestStatus& aStatus*/)
    {
    DP0_IMAGIC(_L("CIEFileLoader::NewL++"));
    
	CIEFileLoader* self = new (ELeave) CIEFileLoader(aFileServer, aEngImp, aCritical/*, aStatus*/);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	DP0_IMAGIC(_L("CIEFileLoader::NewL--"));
	return self;
    }

CIEFileLoader::~CIEFileLoader()
    {
    DP0_IMAGIC(_L("CIEFileLoader::~CIEFileLoader++"));
    
    // Save database file if needed
    if (iImageList)
        TRAP_IGNORE(iImageList->WriteDatabaseL());
    
   	DP0_IMAGIC(_L("CIEFileLoader::~CIEFileLoader 1"));
#ifdef IMAGEFINDERMONITORAO
   	if(iImageFinderMonitor->IsActive())
   	    {
   	    DP0_IMAGIC(_L("CIEFileLoader::~CIEFileLoader 1.1"));
   	    iImageFinderMonitor->Cancel();
   	    }
   	
   	DP0_IMAGIC(_L("CIEFileLoader::~CIEFileLoader 2"));
   	delete iImageFinderMonitor;
   	iImageFinderMonitor = NULL;
#endif
   	
	// The object is created in ThreadEngine CreateThreadsL().
    // This is done because thread1 needs to cleanup before killing
    // the thread.
	iImageFinderThread->Stop();
	delete iImageFinderThread;
	iImageFinderThread = NULL;
	DP0_IMAGIC(_L("CIEFileLoader::~CIEFileLoader 3"));

	for(TInt i = 0;i < iFileNameData.Count();i++)
	    delete iFileNameData[i];
	
    for(TInt i = 0;i < iFaceFilenameData.Count();i++)
        delete iFaceFilenameData[i];
        
    iFileNameData.Reset();
    iFileNameData.Close();
    
    iFaceFilenameData.Reset();
    iFaceFilenameData.Close();
    
    delete iImageList;
	
	//delete iFileSystemMonitor;
	DP0_IMAGIC(_L("CIEFileLoader::~CIEFileLoader--"));
    }

CIEFileLoader::CIEFileLoader(
        RFs& aFileServer, 
        CIEEngineImp* aEngImp, 
        RCriticalSection* aCritical) :
        iFileServer(aFileServer),
        iEngImp(aEngImp),
        iCritical(aCritical)
    {
          
    }

void CIEFileLoader::ConstructL()
    {
    DP0_IMAGIC(_L("CIEFileLoader::ConstructL++"));
    
	iCurrentFileIndex = 0;
	//iJpgFileCountingComplete = EFalse;
	iImageFinderState = EImageFinderRunning;

	iImageList = CIEImageList::NewL(iFileNameData, this);
	
	TFileName rootPath;
    
	//iFileSystemMonitor = FileSystemMonitorAO::NewL(iFileServer, this);
	//iFileSystemMonitor->StartMonitoring();
#ifdef IMAGEFINDERMONITORAO		
	iImageFinderMonitor = CImageMonitorAO::NewL(iEngImp);
	iImageFinderMonitor->iStatus = KRequestPending; 
	iImageFinderMonitor->ActiveRequest();
#endif    
	//Create and start ImageFinder thread
	iImageFinderThread = CFileFinderThread::NewL(
	        this, 
	        iFileNameData, 
	        iFaceFilenameData, 
	        iCritical, 
	        rootPath);

	iImageFinderThread->StartL();
	
	iMainThreadId = RThread().Id();
	
	DP0_IMAGIC(_L("CIEFileLoader::ConstructL--"));
}

CIEFileLoader::TImageFinderState CIEFileLoader::ImageFinderState() const
    {
    return iImageFinderState;
    //return (iImageFinderThread != NULL);
    }

CIEImageList& CIEFileLoader::GetImageList()
    {
    return *iImageList;
    }

void CIEFileLoader::StopImageFinder()
    {
    DP0_IMAGIC(_L("CIEFileLoader::Stop++"));
    if (iImageFinderState == EImageFinderRunning)
        iImageFinderState = EImageFinderStopping;
    DP0_IMAGIC(_L("CIEFileLoader::Stop--"));
    }

void CIEFileLoader::ImageFinderStopped()
    {
    iImageFinderState = EImageFinderStopped;
    }

void CIEFileLoader::ImageListChanged(TInt aIndex, TBool aAdded) 
    {
    iEngImp->GetObserver().ImageListChanged(aIndex, aAdded);
    }

/* Added new image */
void CIEFileLoader::AddNewImage(CImageData* aTmpImageData, TInt aImageIndex)
    {
    //Insert new image data to filename array
    iFileNameData.Insert(aTmpImageData, aImageIndex);
    }   

/* Added new image */
void CIEFileLoader::AddNewFaceCropImage(CImageData* aTmpImageData, TInt aImageIndex)
    {
    
    //Insert new image data to filename array
    aTmpImageData->iGridData.iCorrupted = EFalse;
    aTmpImageData->iGridData.iRotationAngle = 0;//rotation angle of one image
    aTmpImageData->iGridData.iTargetRotationAngle = 0;//target rotation angle of one image
    aTmpImageData->iGridData.iX = aTmpImageData->iGridData.iY = aTmpImageData->iGridData.iZ = 0;//OpenGL Z coord
    aTmpImageData->iGridData.iScale = 0;//OpenGL iScale
    aTmpImageData->iGridData.iGlLQ32TextIndex = 0;//OpenGL 32x32 texture index
    aTmpImageData->iGridData.iGlLQ128TextIndex = 0;//OpenGL 128x128 texture index
    aTmpImageData->iGridData.iGlHQ512TextIndex = 0;//OpenGL 512x512 texture index
    aTmpImageData->iGridData.iGlSuperHQTextIndex = 0;//OpenGL 2048x2048 texture index
    
    iFaceFilenameData.Insert(aTmpImageData, aImageIndex);
    }   


/*
void CIEFileLoader::StartFileSystemMonitoring()
    {
    //iFileSystemMonitor->StartMonitoring();    
    }

void CIEFileLoader::StopFileSystemMonitoring()
    {
    //iFileSystemMonitor->StopMonitoring();    
    }

void CIEFileLoader::FileSystemChanged()
    {
    
    }
*/
CIEEngineImp* CIEFileLoader::GetEngineImpPtr()
    {
    return iEngImp;
    }
    
void CIEFileLoader::ModifyImageData(CImageData* aTmpImageData, TInt aImageIndex)
    {
    //Insert new image data to filename array
    iFileNameData[aImageIndex] = aTmpImageData;
    }


/* All files are added to fine name array */
void CIEFileLoader::AllFilesAddedToFilenameArray()
    {
#ifdef IMAGEFINDERMONITORAO        
    TRequestStatus* status = &iImageFinderMonitor->iStatus;
    RThread mainThread;
    mainThread.Open( iMainThreadId );
    mainThread.RequestComplete( status, KErrNone );
#else    
    iEngImp->AllFilesAddedToFilenameArrayL();
#endif    
    DP0_IMAGIC(_L("CIEFileLoader::AllFilesAddedToFilenameArray 1"));
    //JPG File counting completed
    //iJpgFileCountingComplete = ETrue;
    
    }

//Only used for getting number of TNs in file system 
#if 0
void CIEFileLoader::SearchFileCountL(const TDesC& aRootPath, const TDesC& aSearchName, TInt& aImageFileCount, TInt& aFacesFileCount)
    {
    DP0_IMAGIC(_L("CIEFileLoader::SearchFileCountL++"));
    TInt error = KErrNone;
    TInt tnFileCount = 0;
    
    CDirScan* dirScan = CDirScan::NewL(iFileServer);
    dirScan->SetScanDataL(aRootPath, KEntryAttDir|KEntryAttMatchExclusive, ESortNone, CDirScan::EScanDownTree);
    
    while(1)
        {
         CDir* dir = NULL;
         dirScan->NextL(dir);
         
         if(error || !dir)
          break;
         
         delete dir;
         
         TBool isFace = EFalse;
         TPtrC FacesDir = dirScan->FullPath();
         if(FacesDir.Find(KFacesPath) != KErrNotFound)
             {
             aFacesFileCount += ScanDirFileCountL(dirScan->FullPath(), aSearchName);    
             }
         else
             {
             aImageFileCount += ScanDirFileCountL(dirScan->FullPath(), aSearchName);    
             }
         } 
    
    delete dirScan;
    dirScan = NULL;
    
    DP0_IMAGIC(_L("CIEFileLoader::SearchFileCountL--"));
    }


//Only used for getting number of TNs in file system
TInt CIEFileLoader::ScanDirFileCountL(const TDesC& aDir, const TDesC& aWild)
{
    DP0_IMAGIC(_L("CIEFileLoader::ScanDirFileCountL++"));
    TParse parse;
    TInt count = 0;
    
    parse.Set(aWild, &aDir, NULL);
    TPtrC spec(parse.FullName());

    TFindFile findFile(iFileServer);
    CDir* dir;
    
    if (!findFile.FindWildByPath(parse.FullName(), NULL, dir))
    {
        CleanupStack::PushL(dir);
        
        count = dir->Count(); 

        CleanupStack::PopAndDestroy(dir);
    }
 
    return count;

}
#endif

/* Deleting all corresponding Thumbnails including Gallery TNs */
TInt CIEFileLoader::DeleteFile(TInt aIndex)
    {
    TInt err = KErrNotFound;
    if(aIndex < iFileNameData.Count())
        {
        err = DeleteFile(iFileNameData[aIndex]);
    
        if (err == KErrNone)
            {
            CImageData* imageData = iFileNameData[aIndex];
            iFileNameData.Remove(aIndex);
            delete imageData;
            iNumberOfImages = iFileNameData.Count();
            }
        }
    return err;
    }

TInt CIEFileLoader::DeleteFile(const CImageData* aImageData)
    {
    TFileName fileName;
    TInt err = KErrNone;

    if(aImageData->IsImageReady(EFullSize))
        {
        aImageData->GetFileName(fileName, EFullSize);            
        TInt err = iFileServer.Delete(fileName);
        if (err != KErrNone)
            return err;
        }
    
    //Here if condition is not required. If the file does not exists
    //then it returns not found system error code ?
    //There is no value addition for extra check any way we have to remove
    // that index from the array...( Cross check)
    if(aImageData->IsImageReady(ESize128x128)) 
        {
        aImageData->GetFileName(fileName, ESize128x128);
        iFileServer.Delete(fileName);
        }
        
    if(aImageData->IsImageReady(ESize512x512))
        {
        aImageData->GetFileName(fileName, ESize512x512);        
        iFileServer.Delete(fileName);
        }
            
    if(aImageData->IsImageReady(ESize32x32))
        {
        aImageData->GetFileName(fileName, ESize32x32);
        iFileServer.Delete(fileName);
        }
    
    // Delete standard gallery thumbnail files
    TFileName path, imageName;
    aImageData->GetPath(path);
    aImageData->GetFileName(imageName);
    
    const TPtrC thumbPaths[] = { 
        _L("_PAlbTN\\320x320\\"), _L("_320x320"),
        _L("_PAlbTN\\320x240\\"), _L(""),
        _L("_PAlbTN\\170x128\\"), _L("_170x128"),        
        _L("_PAlbTN\\"), _L("_128x96"),
        _L("_PAlbTN\\64x64dat\\"), _L(""),
        _L("_PAlbTN\\56x42\\"), _L("_56x42")
    };
    
    for (TInt i = 0;i < sizeof(thumbPaths) / sizeof(TPtrC) ;i+=2)
        {
        fileName = path;
        fileName.Append(thumbPaths[i]);
        fileName.Append(imageName);
        fileName.Append(thumbPaths[i + 1]);
        iFileServer.Delete(fileName);
        }

    return err;
    }

/* Deleting all corresponding Thumbnails including Gallery TNs */
TInt CIEFileLoader::DeleteFaceFile(TInt aIndex)
    {
    TInt err = KErrNotFound;
    if(aIndex < iFaceFilenameData.Count())
        {
        //Here if condition is not required. If the file does not exists
        //then it returns not found system error code ?
        //There is no value addition for extra check any way we have to remove
        // that index from the array...( Cross check)
        err = DeleteFile(iFaceFilenameData[aIndex]);
        CImageData* imageData = iFaceFilenameData[aIndex];
        iFaceFilenameData.Remove(aIndex);
        delete imageData;
        iNumberOfFaces = iFaceFilenameData.Count();
        }
    return err;
    }

 
RArray<CImageData*>& CIEFileLoader::GetFileNameArray()
{
    return iFileNameData;
}

RArray<CImageData*>& CIEFileLoader::GetFacesFileNameArray()
{
    return iFaceFilenameData;
}

TInt CIEFileLoader::GetTotalNumOfImages()
{
    //return iNumberOfImages;
    return iFileNameData.Count();
}

void CIEFileLoader::GetTotalNumOfImages(TInt& aNumOfImages, TInt& aNumOfFaces)
    {
    aNumOfImages = iFileNameData.Count();
    aNumOfFaces = iFaceFilenameData.Count();
    }

void CIEFileLoader::GetUpdatedNumOfImages(TInt& aNumOfImages, TInt& aNumOfFaces)
    {
    aNumOfImages = iFileNameData.Count();
    aNumOfFaces = iFaceFilenameData.Count();
    }

void CIEFileLoader::GetFileNameL(TInt aFileIndex, TFileName& aFileName, TThumbSize aThumbRes)
{
	if(aFileIndex < 0 || aFileIndex > iFileNameData.Count())
	    {
		User::Leave(KErrArgument);
	    }
	else
	    {
        iFileNameData[aFileIndex]->GetFileName(aFileName, aThumbRes);
    	}
}

/*CImageData* CIEFileLoader::GetImageData(TInt aIndex)
    {
    return (aIndex < iFileNameData.Count()) ? iFileNameData[aIndex] : NULL;
    }*/

CImageData* CIEFileLoader::GetImageData(TInt aIndex/*, TImageArrayMode aMode*/)
    {
    return (aIndex >= 0 && aIndex < iFileNameData.Count()) ? iFileNameData[aIndex] : NULL;
    }

void CIEFileLoader::SetImageData(TInt aIndex, CImageData* aGridData)
    {
    if(aIndex < iFileNameData.Count())
        iFileNameData[aIndex] = aGridData;
    }

#ifdef _ACCELEROMETER_SUPPORTED_
TImagicDeviceOrientation CIEFileLoader::DeviceOrientation()
    {
    return iEngImp->GetDeviceOrientation();
    }
#endif

// EOF
