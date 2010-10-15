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

#ifndef __IMAGICCONSTS_H__
#define __IMAGICCONSTS_H__

//Feature definition flags
#define USE_BITMAPS_TNS //Set on if want to use Symbian Bitmap TNs 
//#define USE_RGBA //Set on if want to use RBGA(RGB+Alpha) bitmaps

#ifdef __WINS__
    #undef _S60_5x_ACCELEROMETER_
    #define _S60_3x_ACCELEROMETER_
    #define _ACCELEROMETER_SUPPORTED_
#else
    #define _ACCELEROMETER_SUPPORTED_
#ifdef __S60_50__
    #define _S60_5x_ACCELEROMETER_
#else
    #define _S60_3x_ACCELEROMETER_
#endif
#endif


struct FloatCoords
    {
    float iX;
    float iY;
    };

//App UI Feature definition flags
//#define BUBLE_EFFECT
#define DRAW_FRAME
//#define MIPMAPPING
#define SUPERZOOM
#define IMAGIC_DATABASE
#define FACE_DETECTION
//#define SHADOW_PHOTOS
//#define EMPTY_IMAGE_AS_BMP       
#define EMPTY_IMAGE_AS_WIREFRAME
#define GAP_BETWEEN_FOLDERS
//#define PEOPLE_VIEW

/**
* EXIF data DateTimeOriginal length
*/
const TInt KPMMExifDateTimeOriginalLength = 20;


_LIT(KEmptyString, "");
_LIT(KSpace, " ");
_LIT(KHash, "#");
_LIT(KZero, "0");
_LIT(KUnderScr, "_");
_LIT(KDot, ".");
_LIT(KSlash, "/");
//_LIT(KNewLine, "\n\r");
_LIT(KNewLine, "\r");

_LIT(KPAlbTNFilePath, "_PAlbTN");

_LIT(K32x32TNFilePath, "_PAlbTN\\IEImagicTN_32x32\\");
_LIT(K128x96TNFilePath, "_PAlbTN\\");
_LIT(K128x128TNFilePath, "_PAlbTN\\IEImagicTN_128x128\\");
_LIT(K512x512TNFilePath, "_PAlbTN\\IEImagicTN_512x512\\");

_LIT( K32x32Ext, "_32x32" );
_LIT( K128x96Ext, "_128x96" );
_LIT( K128x128Ext, "_128x128" );
_LIT( K512x512Ext, "_512x512" );

_LIT(KFaces, "ImagicFaces\\");
_LIT8(KFaceCoordsHeader, "Face Coordinates");
_LIT8(KFaceCoordsImagicVersion, "Imagic v2.1:");

_LIT(KRootPathFDrive, "F:\\");
_LIT(KRootPathCDrive, "C:\\");

#ifdef __WINS__
    _LIT(KFacesPath, "ImagicFaces");
    _LIT(KRootImagePath, "C:\\Images\\");
    _LIT(ImagePath, "Images\\");
    _LIT(KRootFacesImagePath, "C:\\Images\\ImagicFaces");
#else
    _LIT(KFacesPath, "ImagicFaces");
    //_LIT(KRootImagePath, "E:\\Images\\");
    _LIT(ImagePath, "Images\\");
    _LIT(KRootFacesImagePath, "E:\\Images\\ImagicFaces");
#endif

_LIT(KCRootBgroundImages, "C:\\Images\\Data\\Backgrounds");
_LIT(KFileString, "*.JPG");

#ifdef __WINS__
_LIT(KSmileFileName, "c:\\data\\smile.mbm");
_LIT(KZoomFileName, "c:\\data\\zoom.mbm");
_LIT(KLoadingFileName, "c:\\data\\loading.mbm");
_LIT(KExitFileName, "c:\\data\\exit.mbm");
_LIT(KMenuFileName, "c:\\data\\menu.mbm");
_LIT(KShadowFileName, "c:\\data\\shadow.mbm");
#else
_LIT(KSmileFileName, "c:\\resource\\apps\\smile.mbm");
_LIT(KZoomFileName, "c:\\resource\\apps\\zoom.mbm");
_LIT(KLoadingFileName, "c:\\resource\\apps\\loading.mbm");
_LIT(KExitFileName, "c:\\resource\\apps\\exit.mbm");
_LIT(KMenuFileName, "c:\\resource\\apps\\menu.mbm");
_LIT(KShadowFileName, "c:\\resource\\apps\\shadow.mbm");
#endif

#define USE_SETTINGS_FILE
#ifdef __WINS__
_LIT(KSettingFileName,"C:\\data\\photobrowser.cfg");
#else
_LIT(KSettingFileName,"C:\\data\\photobrowser.cfg");
#endif

#endif
