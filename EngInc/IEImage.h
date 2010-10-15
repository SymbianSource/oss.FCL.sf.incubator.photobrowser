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

#ifndef __IEIMAGE_H__
#define __IEIMAGE_H__

// Include files
#include <e32base.h>
#include "IEBgpsInfo.h"
#include "ImagicConsts.h"


//_LIT8(KFaceCoordsHeader, "Face Coordinates");
//_LIT8(KFaceCoordsImagicVersion, "Imagic v2.0:");

enum TThumbSize
{
    ENotDefined = 0,
    ESize32x32 = 1,
    //ESize128x96 = 2,
    ESize128x128 = 4,
    ESize512x512 = 8,
    EFullSize = 16,
    EExifThumb = 32
};

enum TIEFeature
{
	EFeatureNone = 1,
	EBrightness,
	EContrast,
	EColorAdjustment,
	EGamma,
	ECropping,
	EEdgeEnhancement,
	ERotation,
	ELocalColorCorrection,
	EFaceDetection,
	EFeatureError
};

enum TIEEditingMode
{
	EEditModeNone = 1,
	EEditModeWizard,
	EEditModeAdvanced,
	EEditModeRotate,
	EEditModeCrop,
	EEditModeError,
	EEditModeBrowsing
};

enum TImageForamt
{
	EYuv420Planar = 1,
	EYuv422,
	EYuv444
};

enum TRotationAngle
{
	ERotationClockwise90 = 1,
	ERotationClockwise180,
	ERotationClockwise270
};

class TIEColorParams
{
public:
	TInt iRedValue;
	TInt iGreenValue;
	TInt iBlueValue;
};

class TIEWizardImageParams
{
public:
	TBool iIsWizardEdit;
	TInt iBrightnessVal;
	TInt iContrastVal;
	TIEColorParams iColorVal;
	TInt iGammmaVal;
	TInt iSharpnessVal;
	TInt iLocalColorVal;
};

class TIEImageParams
{
public:
	TBool iIsNonWizardEdit;
	TIEFeature iFeature;
	TInt iValue;
	TIEColorParams iColorValue;
	TRect iCropRect;	
};

class TIEImage
{
public:	
	TFileName iFileName;
	TFileName iEditedFileName;
	TInt iFileIndex;
	TIEWizardImageParams iWizardParams;
	TIEImageParams iImageParams;
};

#endif // __IEIMAGE_H__
