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

#include "SendImageFile.h"

#include <sendui.h>
#include <eikenv.h>

#ifdef SEND_FILE_DIALOGUE
#include <BTObjectExchange.rsg>
#endif

#include <caknfileselectiondialog.h>
#include <caknmemoryselectiondialog.h> 

#include <cmessagedata.h> 


CSendImageFile::CSendImageFile() {

}

CSendImageFile* CSendImageFile::NewL() {
    CSendImageFile* self = new ( ELeave ) CSendImageFile();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
}

void CSendImageFile::ConstructL() {

    iSendUi = CSendUi::NewL();

}

CSendImageFile::~CSendImageFile() {
    if(iSendUi) {
        delete iSendUi;
    }
}

#ifdef SEND_FILE_DIALOGUE
TBool CSendImageFile::AskFileL(TFileName& aFileName)
    {

    // Select memory
    CAknMemorySelectionDialog* memSelectionDialog = 
        CAknMemorySelectionDialog::NewL(ECFDDialogTypeNormal, EFalse);
    CleanupStack::PushL(memSelectionDialog);
    CAknMemorySelectionDialog::TMemory mem(CAknMemorySelectionDialog::EPhoneMemory);

    TInt ret = memSelectionDialog->ExecuteL(mem);
    CleanupStack::PopAndDestroy(memSelectionDialog);
    if (!ret) 
        {        
        return EFalse;
        }
    //Select file from the chosen memory
    CAknFileSelectionDialog* fileSelectionDialog = NULL; 
    if (mem == CAknMemorySelectionDialog::EMemoryCard)
        {  
        fileSelectionDialog = CAknFileSelectionDialog::NewL(ECFDDialogTypeNormal,R_FILE_SELECTION_DIALOG_E );
        }
    else
        {  
        fileSelectionDialog= CAknFileSelectionDialog::NewL(ECFDDialogTypeNormal,R_FILE_SELECTION_DIALOG_C );
        } 

    TBool result = fileSelectionDialog->ExecuteL(aFileName);
    delete fileSelectionDialog;
    return result;

    }

void CSendImageFile::SendFileViaSendUiL()
    {

    TFileName path;

    AskFileL(path);
    SendFileViaSendUiL(path);

    }
#endif

void CSendImageFile::SendFileViaSendUiL(TFileName path)
    {

        TSendingCapabilities capabs( 0, 1024, TSendingCapabilities::ESupportsAttachments ); 

        RFs fs;
        CleanupClosePushL(fs);
        User::LeaveIfError( fs.Connect() );
        fs.ShareProtected();
        
        RFile temp;
        User::LeaveIfError( temp.Open( fs, path, EFileShareReadersOnly | EFileRead ) );
        CleanupClosePushL(temp);
                
        CMessageData* messageData = CMessageData::NewL();
        CleanupStack::PushL(messageData);
        messageData->AppendAttachmentHandleL(temp);
        
        TRAPD(err, iSendUi->ShowQueryAndSendL(messageData, capabs) );

        CleanupStack::PopAndDestroy(messageData);
        
        CleanupStack::PopAndDestroy(&temp);
        CleanupStack::PopAndDestroy(&fs);        
   
    }
