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

#ifndef __IEIMAGEDATA_H__
#define __IEIMAGEDATA_H__

// Include files
#include <e32base.h> 
#include <IEImage.h>

class HGridData
    {
public:
    TBool       iCorrupted;
    float       iRotationAngle; // rotation angle of one image
    float       iTargetRotationAngle; // target rotation angle of one image
    float       iX, iY, iZ; // OpenGL coordinates
    float       iScale; // OpenGL iScale
    TUint       iGlLQ32TextIndex; // OpenGL 128x128 texture index
    TUint       iGlLQ128TextIndex; // OpenGL 128x128 texture index
    TUint       iGlHQ512TextIndex; // OpenGL 512x512 texture index
    TUint       iGlSuperHQTextIndex; // OpenGL 2048x2048 texture index
    
    // Returns opengl index for best quality image that has been loaded
    inline TUint BestImage(void)
        {
        if (iGlSuperHQTextIndex!=0)
            return iGlSuperHQTextIndex;
        if (iGlHQ512TextIndex!=0)
            return iGlHQ512TextIndex;
        if (iGlLQ128TextIndex!=0)
            return iGlLQ128TextIndex;
        if (iGlLQ32TextIndex!=0)
            return iGlLQ32TextIndex;
        return 0;
        }
    };


class CImageData
    {
public:
    IMPORT_C static CImageData* NewL(TUint32 aImagesReady = 0);
    IMPORT_C ~CImageData();
    
protected:    
    TReal       iAspectRatio; 
    TSize       iSize;
    TTime       iFileTime, iCreatedTime;
    HBufC*      iPath;
    HBufC*      iFileName;
    TUint16     iImagesReady;
    TUint16     iOrientation;
    TInt        iNumberOfFaces;

    void ConstructL(TUint32 aImagesReady);
    TBool IsCreatedTimeSet() const;
    CImageData();
    
public:    
    HGridData   iGridData;
    TInt        iPersonId;  // TODO temp solution, -1 = no person
    
    IMPORT_C void GetFileName(TFileName & aFullFileName, TThumbSize aSize) const;
    IMPORT_C void GetPath(TFileName & aPath) const;
    IMPORT_C void GetFileName(TFileName & aFileName) const;
    IMPORT_C void SetFileNameL(const TFileName & aFullFileName);
    IMPORT_C TBool IsImageReady(TThumbSize aSize) const;
    IMPORT_C void SetImageReady(TThumbSize aSize, TBool bReady);
    IMPORT_C const TReal GetAspectRatio() const;
    IMPORT_C void SetSize(const TSize aSize);
    IMPORT_C TSize GetSize() const;
    IMPORT_C const TTime & GetFileTime() const;
    IMPORT_C void SetFileTime(const TTime & aTime);
    IMPORT_C const TTime & GetCreatedTime() const;
    IMPORT_C void SetCreatedTime(const TTime & aTime);
    IMPORT_C TUint16 GetOrientation() const;
    IMPORT_C void SetOrientation(TUint16 aOrientation);
    IMPORT_C TInt GetNumberOfFaces() const;
    IMPORT_C void SetNumberOfFaces(TInt aValue);
    IMPORT_C TBool IsSamePath(CImageData& aImageData) const;
    };  

#endif // __IEIMAGEDATA_H__
