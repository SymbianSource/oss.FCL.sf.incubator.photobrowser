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

#ifndef __IEBGPSERVERSESSION_H__
#define __IEBGPSERVERSESSION_H__

// Include files
#include <e32base.h>
#include <e32cmn.h>

#include <IEBgpsInfo.h>
#include "IEFaceBrowser.h"


class CIETNGeneratorAO;
class MIEFaceBrowserObserver;

class MIEThumbNailObserver
{
public:

virtual void ThumbNailGenerationCompleted(TInt aError) = 0;
//virtual void ThumbNailGenerationCompleted(TInt aError) = 0;
};



class CIEBgpServerSession : public CSession2, 
                            public MIEThumbNailObserver,
                            public MIEFaceBrowserObserver
{
public:
	static CIEBgpServerSession* NewL(RFs* aFileServer);
	~CIEBgpServerSession();
	
private:
	void ConstructL(RFs* aFileServer);
	CIEBgpServerSession();	
	
public: // From CSession2
	void ServiceL(const RMessage2& aMessage);

public:

    void ThumbNailGenerationCompleted(TInt aError);
    
public: // From MIEFaceBrowserObserver
    void FaceBrowsingComplete();
    void FaceBrowsingError(TInt aError);
    void FaceCroppingError(TInt aError);
    void FaceSingleFaceBrowsingComplete();
	
private:

TBool iSingleTNGeneration;

CFaceBrowser* iFaceBrowser;

TInt iCount;
TInt iImageCount;
RArray<CImageData*> iImageArray; 
CIETNGeneratorAO *iIETNGeneratorAO;
RMessage2 iMessage;	

TFileName iJpegFileName;
TFileName iImagicThumbFileName;
TSize iResolutionSize;
CFbsBitmap* iSrcBitmap;


};

#endif // __IEBGPSERVERSESSION_H__
