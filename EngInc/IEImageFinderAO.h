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
#include <ExifUtility.h> 
#include <ICLExif.h> 


// FORWARD DECLARATIONS

class CIEImageFinder : public CBase
    {
    public:
        static CIEImageFinder* NewL(CIEFileLoader* aCallback, RArray<TImageData*>& aFileNameData,TBool& aAll128x128TNsDone, 
                                     TBool& aAll640x480TNsDone, TBool& aAll320x320TNsDone, RCriticalSection* aCritical);
        
        static CIEImageFinder* NewLC(CIEFileLoader* aCallback, RArray<TImageData*>& aFileNameData,TBool& aAll128x128TNsDone, 
                                      TBool& aAll640x480TNsDone, TBool& aAll320x320TNsDone, RCriticalSection* aCritical);
        
        CIEImageFinder(CIEFileLoader* callback, RArray<TImageData*>& aFileNameData, TBool& aAll128x128TNsDone,
                        TBool& aAll640x480TNsDone, TBool& aAll320x320TNsDone, RCriticalSection* aCritical);
        void ConstructL();
        ~CIEImageFinder();
        void ScanDirL(CDir* dir, const TDesC& aDir, const TDesC& aWild);
        //void ScanDirL(const TDesC& aDir, const TDesC& aWild);
        void SearchFilesL(const TDesC& aRootPath, const TDesC& aSearchName);
        void IEImageFinderStartL(const TDesC& aRootPath, const TDesC& aSearchName);
        TInt CheckIfFileExist(TFileName& aFileName);
        float ReadAspectRatioL(TFileName& aFileName);
  
    private:
        TBool IsFileExist(const TDesC &aFileName);
        void CheckForTNFiles(TImageData &aImageData);
        float ReadExifDataL(const TDes &aFileName);        
     
        
    private:
        RFs                     iFileServer;
        CIEFileLoader*          iCallback;
        RArray<TImageData*>&    iFileNameData;
        TBool&                  iAll640x480TNsDone; 
        TBool&                  iAll128x128TNsDone;
        TBool&                  iAll320x320TNsDone;
        TBufC<KMaxFileName>     iRootPath;
        TBufC<KMaxFileName>     iSearchName;
        RCriticalSection*       iCritical;
        TExifReaderUtility*     iExifReader;
//        CImageDecoder*          iImageDecoder;
        TBool                   iTnFoldersCreated;
    };

#endif //__IEIMAGEFINDER_H_

// End of File
