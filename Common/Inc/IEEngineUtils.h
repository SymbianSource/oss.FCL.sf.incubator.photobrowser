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

#ifndef  __IEENGINEUTILS__
#define __IEENGINEUTILS__

#include <f32file.h>
#include <f32file.h>
#include <e32base.h>
//#include "IEImageData.h"
#include "IEImage.h"

class CIEEngineUtils: public CBase
{
public:
	/*
	* Symbian First phase constructor
	* 
	* @aParam aFs - FileServer
	*/
	IMPORT_C CIEEngineUtils(RFs &aFs);
	
	/*
	* Destructor.
	* 
	*/
	IMPORT_C ~CIEEngineUtils();
public:
	/*
	 * GenerateIEThumbNailPath depending on the resolution
	 * 
	 * @aParam  aTNResolution - Thumbnail resolution
	 * @aParam  aIETNFileName - IEFilename
	 */ 
    IMPORT_C static void GenerateThumbnailFileName(TThumbSize aResolution, const TDesC& aFileName, TDes& aThumbnailFileName);
    IMPORT_C static void DeleteThumbnails(TDesC& aFileName, RFs& aFs);
    
    /*
     * Create TN folder
     * 
     * @aParam  aTNPath -  Thumbnail path
     * @return - systemwide error code.
     */ 
     IMPORT_C static TInt CreateTNFolder(RFs aFs, const TDesC& aTNPath);
     IMPORT_C static void PrivatePath(TFileName& aFileName);
     
     IMPORT_C TInt AddFaceCoordinate(const TFileName a128x128TNFileName, RArray<TRect>& aCordArray);
     IMPORT_C TInt RemoveFaceCoordinate(const TFileName a128x128TNFileName, RArray<TRect>& aCordArray);
     IMPORT_C HBufC8* ReadExifMakerNoteL(const TDes &aFileName);
     IMPORT_C void ReadFaceCoordinatesL(const TFileName a128x128TNFileName, RArray<TRect>& aCordArray);
     IMPORT_C void WriteFaceCoordinatesL(const TFileName a128x128TNFileName, RArray<TRect>& aCordArray);
     IMPORT_C void GetModifiedTimeL(const TDes &aFileName, TTime& aTime);
     IMPORT_C void GetImageSizeL(const TDes &aFileName, TSize& aSize);
     IMPORT_C static HBufC8* ReadExifThumbnailL(RFs& aFs, const TDesC& aFileName);     
     //void ReadFile2BuffL(const TDes &aFileName, HBufC8** buffer);
     //TPtr8 LoadImageIntoMemoryLC(const TDesC& aFileName);
     IMPORT_C void GetExifDateTimeAndOrientationL(const TDesC& aFilename, TTime& aExifDateTime, TUint16& aOrientation);
     IMPORT_C static TUid GetImageDecoderUid();
     
private:
      IMPORT_C static HBufC8* ReadExifHeaderL(RFs& aFs, const TDesC& aFileName);
      
      RFs& iFs;
};
#endif /*__IEENGINEUTILS__*/
