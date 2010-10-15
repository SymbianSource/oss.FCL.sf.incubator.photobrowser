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

#ifndef __IEIMAGELIST_H_
#define __IEIMAGELIST_H_

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <bautils.h>
#include <s32file.h>
#include <ICLExif.h> 
#include <exifread.h>
#include <ExifUtility.h>

#include "IEImageProcessing.h"
#include "IEEngineUtils.h"

class CIEFileLoader;

enum TGridMode {
    EGridModeTime,
    EGridModeFolder,
    EGridModePeople
};

class CIEImageList
    {
public:
    IMPORT_C static CIEImageList* NewL(RArray<CImageData*>& aImageData, CIEFileLoader* aCallback);
    IMPORT_C ~CIEImageList();    
    IMPORT_C TBool IsImageViewableL(TDesC& aFileName, RFs& aFs) const;
    IMPORT_C void ReadDatabaseL();
    IMPORT_C void WriteDatabaseL();
    IMPORT_C CImageData* CreateImageDataL(
            const TFileName& aFileName, 
            const TTime& aCreatedTime, 
            TReal orientation);
    IMPORT_C void AddImage(CImageData* aImageData);
    IMPORT_C void Remove(TInt aIndex, RFs& aFs);
    IMPORT_C void RemoveNonExistImagesL(TDesC* aPath, RFs& aFs);     
    IMPORT_C TInt GetImageIndex(CImageData* aImageData);
    IMPORT_C CImageData* GetImageData(const TFileName& aFileName);
    IMPORT_C void SetChanged(TDesC& aPath);
    IMPORT_C void SetGridMode(TGridMode aGridMode);
    IMPORT_C TGridMode GetGridMode() const;    
    
private:        
    enum TImageListDrive {
        EImageListDriveC = 0,
        EImageListDriveE = 1,
        EImageListDriveF = 2,
    };
  
    CIEImageList(RArray<CImageData*>& aImageData, CIEFileLoader* aCallback);
    void ConstructL();
    CImageData* ReadImageDataL(RFileReadStream& readStream, RFs& aFs);
    void WriteDatabaseL(TImageListDrive aDrive, RFs& aFs);
    void GetDatabaseFileName(TFileName& aFileName, TImageListDrive aDrive);
    TBool IsImageBefore(CImageData* aImageData1, TInt aIndex) const;
    TInt GetNewImageIndex(CImageData* aImageData) const;
    void Rearrange(TInt aStartIndex);
    void SetChanged(CImageData* aImageData);
    static TImageListDrive GetPathDriveL(TDesC& aPath);
    
    CIEFileLoader*          iCallback; 
    TBool                   iDatabaseChanged[3];
    RArray<CImageData*>&    iImageDataList;    
    TGridMode               iGridMode;
    RCriticalSection        iCritical;
    };

#endif //__IEIMAGELIST_H_

// End of File
