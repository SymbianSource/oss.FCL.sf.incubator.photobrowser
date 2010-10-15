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

#ifndef __IEIMAGEFINDER_H_
#define __IEIMAGEFINDER_H_

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <IEEngine.h>
#include <bautils.h>

#include "IEFileLoader.h"
#include "IEImageList.h"
#include <ExifUtility.h> 
#include <ICLExif.h> 
#include "ImageMonitorAO.h"
#include "IEEngineImp.h"

#include <IEImageProcessing.h>
#include <exifread.h>

#include "IEEngineImp.h"
#include "IEEngineUtils.h"
#include "FileSystemMonitorAO.h"

// FORWARD DECLARATIONS
class CIEFileLoader;
class CFileSystemMonitorAO;

class CIEImageFinder : public CBase
    {
    public:
        static CIEImageFinder* NewL(
                CIEFileLoader* aCallback, 
                RArray<CImageData*>& aFileNameData, 
                RArray<CImageData*>& aFaceFileNameData, 
                RCriticalSection* aCritical);
        
        static CIEImageFinder* NewLC(
                CIEFileLoader* aCallback, 
                RArray<CImageData*>& aFileNameData, 
                RArray<CImageData*>& aFaceFileNameData, 
                RCriticalSection* aCritical);
        
        CIEImageFinder(CIEFileLoader* callback, 
                        RArray<CImageData*>& aFileNameData, 
                        RArray<CImageData*>& aFaceFileNameData, 
                        RCriticalSection* aCritical);
        void ConstructL();
        ~CIEImageFinder();
        void StartFinderL(const TDesC& aSearchName);
        void FileSystemChanged();
        void SetImageDataChanged();
        
    private:
        void ScanDirL(CDir* dir, const TDesC& aDir, const TDesC& aWild);
        void SearchFilesL(const TDesC& aSearchName);
        TInt CheckIfFileExist(TFileName& aFileName);
        float ReadAspectRatioL(TFileName& aFileName);
        void CheckCreatedThumbnails(CImageData& aImageData) const;
        void GetDatabaseFileName(TFileName & fileName) const;
        TBool IsSearching() const;
       
    private:
        //CFileSystemMonitorAO*   iFileSystemMonitor;
        CIEImageList*           iImageList;
        RFs                     iFileServer;
        CIEFileLoader*          iCallback;
        CIEEngineUtils          iIEEngineUtils;
        RArray<CImageData*>&    iFileNameData;
        RArray<CImageData*>&    iFaceFileNameData;
        RCriticalSection*       iCritical;
        TExifReaderUtility*     iExifReader;
    };

#endif //__IEIMAGEFINDER_H_

// End of File
