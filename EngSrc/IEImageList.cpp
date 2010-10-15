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

// INCLUDE FILES

#include <eikenv.h>
#include <exifread.h>
#include <f32file.h>
#include <s32file.h>
#include <bautils.h>
#include "IEImageList.h"
#include "IEImageData.h"
#include "IEEngineImp.h"
#include "IEEngineUtils.h"
#include "ImageMonitorAO.h"
#include "IEFileLoader.h"
#ifdef _S60_5x_ACCELEROMETER_
#include "IESensorMonitor.h"
#endif

#define LATETHUMBCHECK
//#define GROUP_FOLDERS_BY_NAME
#define CHECK_IF_IMAGE_IS_VISIBLE

_LIT(KDatabaseFileName, "photobrowser.db");
_LIT8(KDatabaseId, "IMGC0008");
const TInt KNumOfDrives = 3;

EXPORT_C CIEImageList* CIEImageList::NewL(        
        RArray<CImageData*>& aImageData, 
        CIEFileLoader* aCallback)
    {
    CIEImageList* self = new (ELeave) CIEImageList(aImageData, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CIEImageList::CIEImageList(
        RArray<CImageData*>& aImageData, 
        CIEFileLoader* aCallback) :
    iCallback(aCallback),
    iImageDataList(aImageData),
    iGridMode(EGridModeTime)	
    {
    for (TInt i = 0;i < KNumOfDrives;i++)
        iDatabaseChanged[i] = EFalse; 
    }

void CIEImageList::ConstructL() 
    {
    User::LeaveIfError(iCritical.CreateLocal());
    }
        
EXPORT_C CIEImageList::~CIEImageList()
    {
    iCritical.Close();
    }
        
EXPORT_C void CIEImageList::SetGridMode(TGridMode aGridMode)
    {
    if (iGridMode != aGridMode) 
        {
        iGridMode = aGridMode;
        iCritical.Wait();
        Rearrange(0);
        iCritical.Signal();
        }
    }

EXPORT_C TGridMode CIEImageList::GetGridMode() const
    {
    return iGridMode; 
    }

EXPORT_C void CIEImageList::SetChanged(TDesC& aPath)
    {
    TImageListDrive drive = EImageListDriveC;
    TRAPD(err, drive = GetPathDriveL(aPath));
    if (err == KErrNone)
        {
        iDatabaseChanged[drive] = ETrue;
        }
    }

void CIEImageList::SetChanged(CImageData* aImageData)
    {
    TFileName fileName;
    aImageData->GetFileName(fileName, EFullSize);
    SetChanged(fileName);
    }

CImageData* CIEImageList::CreateImageDataL(
        const TFileName& aFileName, 
        const TTime& aTime,
        const TReal orientation)
    {
    DP0_IMAGIC(_L("CIEImageList::CreateImageDataL++"));
    
    // Create new image data instance
    CImageData* imageData = CImageData::NewL(
#ifdef LATETHUMBCHECK            
            /*EFullSize|*/ESize512x512|ESize128x128|ESize32x32
#endif            
            );
    
    imageData->SetCreatedTime(aTime);
    imageData->SetFileNameL(aFileName);
    imageData->SetOrientation(orientation);    
    
#ifdef _S60_5x_ACCELEROMETER_
    // Portrait
    if(iCallback->DeviceOrientation() == TSensrvOrientationData::EOrientationDisplayUp)
        {
        imageData->iGridData.iTargetRotationAngle = 90 + orientation;
        }
    // Landscape
    else//(iCallback->DeviceOrientation() == TSensrvOrientationData::EOrientationDisplayRightUp)
        {
        imageData->iGridData.iTargetRotationAngle = orientation;
        }
    imageData->iGridData.iRotationAngle = orientation;
#else
    imageData->iGridData.iRotationAngle = orientation;
    imageData->iGridData.iTargetRotationAngle = orientation;
#endif
    
    DP1_IMAGIC(_L("CIEImageList::AddImageL - filename: %S"), &aFileName);

#ifndef LATETHUMBCHECK    
    //Check and mark to imageData which thumbnails exists
    CheckCreatedThumbnails(*imageData);
#endif
    
    DP0_IMAGIC(_L("CIEImageList::CreateImageDataL--"));
    return imageData;
    }
    
TBool CIEImageList::IsImageBefore(CImageData* aNewImageData, TInt aIndex) const
    {
    if (aIndex >= iImageDataList.Count())
        return ETrue;
    
    // Use folder grouping
    if (iGridMode != EGridModeTime)      
        {
        TFileName newPath, path;
        aNewImageData->GetPath(newPath);
        iImageDataList[aIndex]->GetPath(path);

#ifdef GROUP_FOLDERS_BY_NAME
        // Folders are sorted by name
        if (newPath > path)      // TODO: should trim drive + base path (e.g. C:\data\)
            return ETrue;
        if (newPath < path)
            return EFalse;
#else

        if (iGridMode == EGridModePeople)
            {
            return (iImageDataList[aIndex]->iPersonId > 
                    aNewImageData->iPersonId);
            }
        else if (iGridMode == EGridModeFolder && aIndex > 0)
            {
            // Current image path is not same
            if (path != newPath)
                {
                TFileName prevPath;
                iImageDataList[aIndex - 1]->GetPath(prevPath);
                  
                // Previous image path is same, add after that
                if (newPath == prevPath)
                     return ETrue;
                
                // Compare only against the first image in the folder
                if (path == prevPath)
                     return EFalse;
                }
            }
#endif
        }
    
    // Compare times
    return (aNewImageData->GetCreatedTime() > iImageDataList[aIndex]->GetCreatedTime());
    }

TInt CIEImageList::GetNewImageIndex(CImageData* aImageData) const
    {
    TInt index = 0; 
    while(index < iImageDataList.Count())
        {
        if(IsImageBefore(aImageData, index))
            {
            break;
            }
        index++;
        }
    return index;
    }

void CIEImageList::Rearrange(TInt aStartIndex)
    {
    for (TInt i = aStartIndex;i < iImageDataList.Count();i++)
        {
        CImageData* imageData = iImageDataList[i];            
        iImageDataList.Remove(i);
        TInt newIndex = GetNewImageIndex(imageData);
        iImageDataList.Insert(imageData, newIndex);
        }
    }

EXPORT_C void CIEImageList::AddImage(CImageData* aImageData)
    {
    DP0_IMAGIC(_L("CIEImageList::AddImageL++"));
    
    iCritical.Wait();  
    
    // Insert image to list
    TInt index = GetNewImageIndex(aImageData);
    iImageDataList.Insert(aImageData, index);
        
    // Need to resort all items if use time based folder sort
#ifndef GROUP_FOLDERS_BY_NAME
    if (iGridMode != EGridModeTime)      
        {
        Rearrange(index + 1);
        }
#endif        

    // Image is not added as last image
    if (index < iImageDataList.Count() - 1)
        {
        // Mark database as changed
        SetChanged(aImageData);
        
        // Inform UI
        //iCallback->ImageListChanged(index, ETrue);
        }
    
    iCallback->ImageListChanged(index, ETrue);
    
    iCritical.Signal();
    
    DP0_IMAGIC(_L("CIEImageList::AddImageL-- tmpImageData"));
    }

#ifdef IMAGIC_DATABASE

void CIEImageList::GetDatabaseFileName(TFileName& aFileName, TImageListDrive aDrive)
    {
    switch (aDrive)
        {
        case EImageListDriveC:
            aFileName.Copy(PathInfo::PhoneMemoryRootPath());
            break;
            
        case EImageListDriveE:
            aFileName.Copy(PathInfo::MemoryCardRootPath());
            break;
            
        case EImageListDriveF:           
            aFileName.Copy(KRootPathFDrive);
            break;
           
        default:
            return;
        }
    
    aFileName.Append(KDatabaseFileName);
    }

EXPORT_C void CIEImageList::ReadDatabaseL()
    {  
    DP0_IMAGIC(_L("CIEImageList::ReadDatabaseL++"));

    RFileReadStream readStreams[KNumOfDrives];
    TBool openStreams[KNumOfDrives];
    CImageData* imageDatas[KNumOfDrives];
    RFs fs;
    
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    
    // Open databases
    for (TInt i = 0;i < KNumOfDrives;i++)
        {
        openStreams[i] = EFalse;
        imageDatas[i] = NULL;        
        
        TFileName databaseFileName;
        GetDatabaseFileName(databaseFileName, TImageListDrive(i));
        if (readStreams[i].Open(fs, databaseFileName, EFileShareAny) == KErrNone)
            {
            // Check file validity and version
            TUint8 buf[8];
            TPtr8 ptr(buf, sizeof(buf));
            readStreams[i].ReadL(ptr, KDatabaseId.iTypeLength);
            if (ptr.Compare(KDatabaseId) != 0)
                readStreams[i].Close();
            else
                openStreams[i] = ETrue;
            }
        }
    
    // Read databases
    while(iCallback->ImageFinderState() == CIEFileLoader::EImageFinderRunning)
        {
        // Read image datas from each database
        TBool endOfData = ETrue;
        for(TInt i = 0;i < KNumOfDrives;i++)
            {
            // Database is open and no image data is left
            if (imageDatas[i] == NULL && openStreams[i])
                {
                TRAPD(err, imageDatas[i] = ReadImageDataL(readStreams[i], fs));
                if (err != KErrNone || imageDatas[i] == NULL) 
                    {
                    openStreams[i] = EFalse;
                    readStreams[i].Close();
                    }
                }
            
            if (imageDatas[i])
                endOfData = EFalse;
            }
        
        if (endOfData)
            break;
        
        // Pick the most leftmost image 
        TInt index = -1;
        for (TInt i = 0;i < KNumOfDrives;i++)
            {
            if (imageDatas[i] && 
                    (index < 0 || 
                     IsImageBefore(imageDatas[i], index)))
                index = i;
            }
            
        // Add image to list
        if (index >= 0) 
            {
            AddImage(imageDatas[index]);
            imageDatas[index] = NULL;
            }
        }
    
    CleanupStack::Pop();
    fs.Close();

    DP0_IMAGIC(_L("CIEImageList::ReadDatabaseL--"));
    }

CImageData* CIEImageList::ReadImageDataL(RFileReadStream& readStream, RFs& aFs)
    {
    TUint8 buf[KMaxFileName * 2];
    TPtr8 ptr(buf, sizeof(buf));
    TFileName fileName;
    TTime fileTime, createdTime;
    TSize size;
    TInt faces;
    TUint16 orientation;
    CImageData* imageData = NULL;
           
    // Read until get valid image data
    while (imageData == NULL) {
    
        // Read file name (1 byte length, unicode name)
        TInt len = readStream.ReadUint8L();
    
        // End of list
        if (len == 0)
            return NULL;

        readStream.ReadL(ptr, len * 2);
        TPtrC16 ptr16((const TUint16*)buf, len);
        fileName.Copy(ptr16);
            
        // Read file time
        readStream.ReadL(ptr, sizeof(TTime));
        fileTime = *(TTime*)ptr.Ptr();

        // Read created time
        readStream.ReadL(ptr, sizeof(TTime));
        createdTime = *(TTime*)ptr.Ptr();                    

        // Read orientation (in 90 degrees angles)
        orientation = readStream.ReadUint8L() * 90L;
            
        // Read resolution
        size.iWidth = readStream.ReadUint32L();
        size.iHeight = readStream.ReadUint32L();

        // Read number of faces
        faces = readStream.ReadInt8L();
        
        TInt personId = readStream.ReadInt32L();

        // Check that no multiple entries
        if ((imageData = GetImageData(fileName)) != NULL) 
            {
            imageData = NULL;
            continue;
            }
        
        // Check if image exist and not be hidden
        TInt error = KErrNone;
        TBool visible = ETrue;
#ifdef CHECK_IF_IMAGE_IS_VISIBLE        

        TRAP(error, visible = IsImageViewableL(fileName, aFs));
#endif        
        if (error == KErrNone && visible)
            {
            // Create image data object
            imageData = CreateImageDataL(
                    fileName, 
                    createdTime, 
                    orientation);

            if (imageData) 
                {
                imageData->SetFileTime(fileTime);
                imageData->SetSize(size);
                imageData->SetNumberOfFaces(faces);
                imageData->iPersonId = personId;
                //imageData->SetImageReady(EFullSize, ETrue);
                }        
            }
        else
            {
            // Delete thumbnails if file could not be read
            if (error != KErrNone)
                CIEEngineUtils::DeleteThumbnails(fileName, aFs);
            SetChanged(fileName);
            }
        }
    
    return imageData;
    }
    
EXPORT_C void CIEImageList::WriteDatabaseL() 
    {
    DP0_IMAGIC(_L("CIEImageList::WriteDatabaseL++"));

    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    
    for (TInt i = 0;i < KNumOfDrives;i++) 
        {
        if (iDatabaseChanged[i])
            {
            TRAP_IGNORE(WriteDatabaseL(TImageListDrive(i), fs)); 
            iDatabaseChanged[i] = EFalse;
            }
        }
    
    CleanupStack::Pop(); // fs
    fs.Close();    
    
    DP0_IMAGIC(_L("CIEImageList::WriteDatabaseL--"));    
    }
        
void CIEImageList::WriteDatabaseL(TImageListDrive aDrive, RFs& aFs)
    {
    TUint8 buf[sizeof(TUint32)];
    TPtr8 ptr(buf, sizeof(buf));
    RFile f;

    TFileName path, fileName;
    GetDatabaseFileName(fileName, aDrive);
    
    TParse parser;
    parser.Set(fileName, NULL, NULL);
    path = parser.DriveAndPath();
    TRAP_IGNORE(BaflUtils::EnsurePathExistsL(aFs, path));    
    
    if (f.Replace(
            aFs, 
            fileName, 
            EFileWrite) != KErrNone) 
        return;
        
    CleanupClosePushL(f);
    
    f.SetAtt(KEntryAttHidden, 0);
    
    RFileWriteStream writeStream(f); 
    CleanupClosePushL(writeStream); 
    
    writeStream.WriteL(KDatabaseId);

    for (TInt32 i = 0;i < iImageDataList.Count();i++)
        {
        TFileName fileName;
        TImageListDrive drive = EImageListDriveC;
        iImageDataList[i]->GetFileName(fileName, EFullSize);
        
        // Write only files that belong to this drive
        TRAPD(err, drive = GetPathDriveL(fileName));
        if (err != KErrNone || drive != aDrive)
            continue;

        // Write file name
        writeStream.WriteUint8L(fileName.Length());
        writeStream.WriteL(fileName, fileName.Length());
          
        // Write file time
        TTime fileTime = iImageDataList[i]->GetFileTime();
        TPtrC8 fileTimeptr((const TUint8 *)&fileTime, sizeof(TTime));
        writeStream.WriteL(fileTimeptr);

        // Write created time
        TTime createdTime = iImageDataList[i]->GetCreatedTime();
        TPtrC8 createdTimeptr((const TUint8 *)&createdTime, sizeof(TTime));
        writeStream.WriteL(createdTimeptr);

        // Write orientation (in 90 degrees)
        writeStream.WriteUint8L(iImageDataList[i]->GetOrientation() / 90);
                
        // Write size
        writeStream.WriteUint32L(iImageDataList[i]->GetSize().iWidth);
        writeStream.WriteUint32L(iImageDataList[i]->GetSize().iHeight);

        // Write number of faces
        writeStream.WriteInt8L(iImageDataList[i]->GetNumberOfFaces());
        writeStream.WriteInt32L(iImageDataList[i]->iPersonId);
        }

    // End of stream notification
    writeStream.WriteUint8L(0);
    
    writeStream.Close();
    
    CleanupStack::PopAndDestroy(); // write stream
    CleanupStack::PopAndDestroy(); // f
    }
#endif

EXPORT_C TBool CIEImageList::IsImageViewableL(TDesC& aFileName, RFs& aFs) const 
    {
    TUint att;
    //if(!IsFileExist(fileName))
    TInt error = aFs.Att(aFileName, att);
    if (error != KErrNone)
        User::Leave(error);
    
    return ((att & KEntryAttHidden) == KEntryAttHidden) ? EFalse : ETrue;  
    }

EXPORT_C TInt CIEImageList::GetImageIndex(CImageData* aImageData)
    {
    for (TInt i = 0;i < iImageDataList.Count();i++) 
        {
        if (aImageData == iImageDataList[i])
            return i;
        }
    return -1;
    }

EXPORT_C CImageData* CIEImageList::GetImageData(const TFileName& aFileName)
    {
    CImageData* imageData = NULL;
    iCritical.Wait();
    
    for (TInt i = 0;i < iImageDataList.Count();i++) 
        {
        TFileName fileName;
        iImageDataList[i]->GetFileName(fileName, EFullSize);
        if (fileName.Compare(aFileName) == 0)
            {
            imageData = iImageDataList[i];
            break;
            }
        }
    
    iCritical.Signal();
        
    return imageData;
    }

EXPORT_C void CIEImageList::RemoveNonExistImagesL(TDesC* aPath, RFs& aFs)
    {
    DP0_IMAGIC(_L("CIEImageList::RemoveNonExistImagesL++"));
    
    TInt i = 0;
    while (i < iImageDataList.Count()) 
        {
        iCritical.Wait();
        
        // File may not exist
        TBool bRemove = !iImageDataList[i]->IsImageReady(EFullSize); 
        
        // Start of path must be same
        if (bRemove && aPath) {
            TFileName path;
            iImageDataList[i]->GetPath(path);
            bRemove = (aPath->Compare(path) == 0);
        }

        iCritical.Signal();
        
        // Remove from list
        if (bRemove)
            {
            Remove(i, aFs);
            if (aPath)
                SetChanged(*aPath);
            }
        else
            {
            i++;
            }
        }
    
    DP0_IMAGIC(_L("CIEImageList::RemoveNonExistImagesL--"));
    }

CIEImageList::TImageListDrive CIEImageList::GetPathDriveL(TDesC& aPath)
    {
    TParse parser;
    parser.Set(aPath, NULL, NULL);
    TPtrC drive = parser.Drive();
    const TPtrC drives[] = { _L("C:"), _L("E:"), _L("F:") };
    
    for (TInt i = 0;i < sizeof(drives) / sizeof(TPtrC);i++)
        {
        if (drive.Compare(drives[i]) == 0)
            {
            return TImageListDrive(i);
            }
        }
    
    User::Leave(KErrArgument);
    return EImageListDriveC;
    }

EXPORT_C void CIEImageList::Remove(TInt aIndex, RFs& aFs)
    {
    TFileName fileName;
    
    if (aIndex < 0 || aIndex >= iImageDataList.Count())
        return;
    
    // Delete thumbnails if original file doesn't exist anymore
    iImageDataList[aIndex]->GetFileName(fileName, EFullSize);
    if(!BaflUtils::FileExists(aFs, fileName))
        CIEEngineUtils::DeleteThumbnails(fileName, aFs);
    
    iCritical.Wait();
    
    // Remove from the list
    CImageData* pRemovedImageData = iImageDataList[aIndex];
    iImageDataList.Remove(aIndex);
    delete pRemovedImageData;
    
    iCritical.Signal();
    
    SetChanged(fileName);
    
    iCallback->ImageListChanged(aIndex, EFalse);
    }
