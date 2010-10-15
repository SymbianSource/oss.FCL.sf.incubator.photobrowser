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
#include <eikenv.h>
#include "IEImageData.h"
#include "ImagicConsts.h"

EXPORT_C CImageData* CImageData::NewL(TUint32 aImagesReady)
    {
    CImageData* self = new (ELeave) CImageData();
    CleanupStack::PushL(self);
    self->ConstructL(aImagesReady);
    CleanupStack::Pop();
    return self;
    }

EXPORT_C CImageData::~CImageData()
    {
    delete iFileName;
    delete iPath;
    }

CImageData::CImageData() : 
    iFileTime(0),
    iCreatedTime(0)
    {
    }

void CImageData::ConstructL(TUint32 aImagesReady)
    {
    iFileName = NULL;
    iPath = NULL;
    iGridData.iCorrupted = 0;
    iGridData.iRotationAngle = 0;
    iGridData.iTargetRotationAngle = 0;
    iGridData.iX = iGridData.iY = iGridData.iZ = 0;
    iGridData.iScale = 0;
    iGridData.iGlLQ32TextIndex = 0;
    iGridData.iGlLQ128TextIndex = 0;
    iGridData.iGlHQ512TextIndex = 0;
    iGridData.iGlSuperHQTextIndex = 0;
    iImagesReady = aImagesReady|EExifThumb;
    iOrientation = 0;
    iNumberOfFaces = -1;
    iAspectRatio = 1;
    iSize = TSize(0,0);
    iPersonId = -1;
    }

EXPORT_C void CImageData::SetFileNameL(const TFileName & aFileName) 
    {
    TParse parser;
    parser.Set(aFileName, NULL, NULL);
    delete iFileName; iFileName = NULL;
    delete iPath; iPath = NULL;
    iFileName = parser.NameAndExt().AllocL();
    iPath = parser.DriveAndPath().AllocL();
    }

EXPORT_C void CImageData::GetFileName(TFileName & aFullFileName, TThumbSize aSize) const 
    {
    if (iPath == NULL || iFileName == NULL)
        User::Leave(KErrNotReady);
        
    aFullFileName = *iPath;

    switch (aSize) 
        { 
        case EFullSize:
            aFullFileName.Append(*iFileName);
            break;

        case ESize32x32:
            aFullFileName.Append(K32x32TNFilePath);
            aFullFileName.Append(*iFileName);
            aFullFileName.Append(K32x32Ext);
            break;            
            
        case ESize128x128:
            aFullFileName.Append(K128x128TNFilePath);
            aFullFileName.Append(*iFileName);
            aFullFileName.Append(K128x128Ext);            
            break;  
            
        case ESize512x512:
            aFullFileName.Append(K512x512TNFilePath);
            aFullFileName.Append(*iFileName);
            aFullFileName.Append(K512x512Ext);            
            break;  

        default:
            User::Leave(KErrArgument);
        }
    }

EXPORT_C void CImageData::GetPath(TFileName & aPath) const 
    {
    aPath.Copy(iPath ? iPath->Des() : _L(""));
    }

EXPORT_C void CImageData::GetFileName(TFileName & aFileName) const
    {
    aFileName.Copy(iFileName ? iFileName->Des() : _L(""));
    }

EXPORT_C TBool CImageData::IsImageReady(TThumbSize aSize) const 
    {
    return ((iImagesReady & aSize) == aSize);
    }

EXPORT_C void CImageData::SetImageReady(TThumbSize aSize, TBool aReady)  
    {
    if (aReady) 
        iImagesReady |= aSize;
    else
        iImagesReady &= ~aSize;
    }

EXPORT_C const TReal CImageData::GetAspectRatio() const 
    { 
    return iAspectRatio;
    }

TBool CImageData::IsCreatedTimeSet() const 
    {
    // Time is defined if not zero
    return (iCreatedTime.Int64() != 0);
    }

EXPORT_C const TTime & CImageData::GetFileTime() const 
    { 
    return iFileTime; 
    }

EXPORT_C void CImageData::SetFileTime(const TTime & aTime) 
    { 
    iFileTime = aTime;
    }

EXPORT_C const TTime & CImageData::GetCreatedTime() const 
    { 
    // if no created time (usually from EXIF), use file time
    return IsCreatedTimeSet() ? iCreatedTime : iFileTime; 
    }

EXPORT_C void CImageData::SetCreatedTime(const TTime & aTime) 
    { 
    iCreatedTime = aTime; 
    }

EXPORT_C TUint16 CImageData::GetOrientation() const 
    {
    return iOrientation;
    }

EXPORT_C void CImageData::SetOrientation(TUint16 aOrientation) 
    {
    iOrientation = aOrientation;
    }

EXPORT_C void CImageData::SetSize(const TSize aSize)
    {
    iSize = aSize;
    iAspectRatio = iSize.iHeight ? (TReal(iSize.iWidth) / iSize.iHeight) : 0;
    }

EXPORT_C TSize CImageData::GetSize() const
    {
    return iSize;
    }

EXPORT_C TInt CImageData::GetNumberOfFaces() const 
    {
    return iNumberOfFaces;
    }

EXPORT_C void CImageData::SetNumberOfFaces(TInt aValue) 
    {
    iNumberOfFaces = aValue;
    }

EXPORT_C TBool CImageData::IsSamePath(CImageData& aImageData) const
    {
    TFileName path, path2;
    GetPath(path);
    aImageData.GetPath(path2);
    return (path == path2);
    }
