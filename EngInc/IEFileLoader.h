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

#ifndef __IEFILELOADER_H__
#define __IEFILELOADER_H__

// Include files
#include <e32base.h>
#include <f32file.h>
#include <IEEngine.h>
#include "IEBitmapLoader.h"
#include "IEThreadEngine.h"
#include "IEImageList.h"
//#include "FileSystemMonitorAO.h"
//#include "IEEngineImp.h"

// Forward class declarations
class CIEBitmapLoader;
class IEImageFinderAO;
class CIEEngineImp;
class IEImageFinderAO;
class CFileFinderThreadAO;
class CImageMonitorAO;

// Class declaration
class CIEFileLoader : public CBase
{
public:
	static 	CIEFileLoader* NewL(RFs& aFileServer, CIEEngineImp* aEngImp, RCriticalSection* aCritical);
	~CIEFileLoader();
	
	enum TImageFinderState {
        EImageFinderRunning,
        EImageFinderStopping,
        EImageFinderStopped
	};

private:
	void ConstructL();
	CIEFileLoader(RFs& aFileServer, CIEEngineImp* aEngImp, RCriticalSection* aCritical);
	              
private:
	void LoadImageNamesL();
	void Load320x320ThumbImageNamesL();
	void FindThumbnailFolderL(TBuf<KMaxFileName> folderName, RFs& session);
	void FillImageDataArrayL(TBuf<KMaxFileName> folder, RFs& session, TBool IsThumbnail);
	TInt ScanDirFileCountL(const TDesC& aDir, const TDesC& aWild);
	void SearchFileCountL(const TDesC& aRootPath, const TDesC& aSearchName, TInt& imageFileCount, TInt& facesFileCount);
	
public:
	void GetFileNameL(const TInt aFileIndex, TFileName& aFileName, TThumbSize aThumbRes);
	void SaveFileL(const TDesC& aFileName, const TDesC8& aData);
	TInt GetImageCount(TThumbSize aThumbRes);
	TInt GetTotalNumOfImages();
	void GetTotalNumOfImages(TInt& aNumOfImages, TInt& aNumOfFaces);
	RArray<CImageData*>& GetFileNameArray();
	RArray<CImageData*>& GetFacesFileNameArray();
	/*void SetAll640x480TNsDone(TBool aValue);
	TBool GetAll640x480TNsDone();
	void SetAll128x128TNsDone(TBool aValue);
    TBool GetAll128x128TNsDone();*/
	void AllFilesAddedToFilenameArray();
	TInt DeleteFile(TInt aIndex);
	TInt DeleteFaceFile(TInt aIndex);
	void AddNewImage(CImageData* aTmpImageData, TInt iImageIndex);
	void AddNewFaceCropImage(CImageData* aTmpImageData, TInt aImageIndex);
	void ModifyImageData(CImageData* aTmpImageData, TInt aImageIndex);
	void ImageListChanged(TInt aIndex, TBool aAdded);
	CIEEngineImp* GetEngineImpPtr();
	/*void StartFileSystemMonitoring();
	void StopFileSystemMonitoring();
	void FileSystemChanged();*/
	void GetUpdatedNumOfImages(TInt& aNumOfImages, TInt& aNumOfFaces);
	
	// Functions for handling UI access to Filename array
	CImageData* GetImageData(TInt aIndex/*, TImageArrayMode aMode*/);
	//CImageData* GetImageData(TInt aIndex);
	void SetImageData(TInt aIndex, CImageData* aGridData);
	CIEImageList& GetImageList();
	TInt DeleteFile(const CImageData* aImageData);
	void StopImageFinder();
	void ImageFinderStopped(); // callback
	TImageFinderState ImageFinderState() const;
#ifdef _ACCELEROMETER_SUPPORTED_
	TImagicDeviceOrientation CIEFileLoader::DeviceOrientation();
#endif
	
private: // Data
    TBuf<KMaxFileName>     iTotalRootFolder;
    
    //CFileSystemMonitorAO*   iFileSystemMonitor;
	RFs&                   iFileServer;
	CIEEngineImp*          iEngImp;
	TInt                   iCurrentFileIndex;
	RArray<CImageData*>    iFileNameData;
	RArray<CImageData*>    iFaceFilenameData;
	CIEImageList*          iImageList;
	
	//For file searching
	RFile                  iFile;
	TInt                   iOffSet;
	TInt                   iNumberOfImages;
	TInt                   iNumberOfFaces;
	CFileFinderThread*     iImageFinderThread;
	RCriticalSection*      iCritical;
	CImageMonitorAO*       iImageFinderMonitor;
	TThreadId              iMainThreadId;
	//TBool                  iJpgFileCountingComplete;
    TImageFinderState      iImageFinderState;	
};

#endif // __IEFILELOADER_H__
