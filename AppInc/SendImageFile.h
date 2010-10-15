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

#ifndef __SENDIMAGE_H__
#define __SENDIMAGE_H__

#include <eikenv.h>

class CSendUi;

class CSendImageFile {

public:
    CSendImageFile();
    static CSendImageFile* NewL();
    void ConstructL();
    ~CSendImageFile();
#ifdef SEND_FILE_DIALOGUE
    void SendFileViaSendUiL();
#endif
    void SendFileViaSendUiL(TFileName path);
    
private:
#ifdef SEND_FILE_DIALOGUE
    TBool AskFileL(TFileName& aFileName);
#endif
    
private:
    CSendUi* iSendUi;
};

#endif
