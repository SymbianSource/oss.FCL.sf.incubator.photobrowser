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

#include <aknquerydialog.h> 
#include <avkon.hrh>
#include <BAUTILS.H>
#include <stringloader.h> 
#include <aknnotewrappers.h>
 
/*Imagic RSG file for rss file */
#include <PhotoBrowser.rsg>
#include "ImagicUtils.h"
#include  "Imagic.hrh"

CImagicUtils* CImagicUtils::NewL(RFs & aFs)
{
    
	CImagicUtils* self=new (ELeave) CImagicUtils(aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
}

CImagicUtils::CImagicUtils(RFs& aFs)
    : iFs(aFs)
    {
        
    } 

void CImagicUtils::ConstructL()
    {
    //no implementation required.
    }

CImagicUtils::~CImagicUtils()
    {
    }

void CImagicUtils::DisplayYearAndMonth(TInt aIndex, TDateTime dateTime)
    {
    if(iPopUpNote)
        {
        delete iPopUpNote;
        }
  /*  if(iTextResource)
        {
        //CleanupStack::PopAndDestroy(iTextResource);
        delete iTextResource;
        }*/

    iPopUpNote = CAknInfoPopupNoteController::NewL();
    TMonth month = dateTime.Month();
    TInt year = dateTime.Year();
    //Convert int to buf
    TBuf<10> valToNumBuf;
    valToNumBuf.Num(year);
    
    _LIT(KMonth1, "January ");
    _LIT(KMonth2, "February ");
    _LIT(KMonth3, "March ");
    _LIT(KMonth4, "April ");
    _LIT(KMonth5, "May ");
    _LIT(KMonth6, "June ");
    _LIT(KMonth7, "July ");
    _LIT(KMonth8, "August ");
    _LIT(KMonth9, "September ");
    _LIT(KMonth10, "October ");
    _LIT(KMonth11, "November ");
    _LIT(KMonth12, "December ");
    
    TBuf<512> text;
    
    if(month == EJanuary)  {text.Format(KMonth1);text.Append(valToNumBuf);}
    if(month == EFebruary) {text.Format(KMonth2);text.Append(valToNumBuf);}
    if(month == EMarch)    {text.Format(KMonth3);text.Append(valToNumBuf);}
    if(month == EApril)    {text.Format(KMonth4);text.Append(valToNumBuf);}
    if(month == EMay)      {text.Format(KMonth5);text.Append(valToNumBuf);}
    if(month == EJune)     {text.Format(KMonth6);text.Append(valToNumBuf);}
    if(month == EJuly)     {text.Format(KMonth7);text.Append(valToNumBuf);}
    if(month == EAugust)   {text.Format(KMonth8);text.Append(valToNumBuf);}
    if(month == ESeptember){text.Format(KMonth9);text.Append(valToNumBuf);}
    if(month == EOctober)  {text.Format(KMonth10);text.Append(valToNumBuf);}
    if(month == ENovember) {text.Format(KMonth11);text.Append(valToNumBuf);}
    if(month == EDecember) {text.Format(KMonth12);text.Append(valToNumBuf);}
                        
    iPopUpNote->SetTextL(text);
    iPopUpNote->SetTimeDelayBeforeShow(0);
    iPopUpNote->SetTimePopupInView( 4000 );
    iPopUpNote->SetPositionAndAlignment(TPoint(0,0), /*TGulAlignmentValue*/EHLeftVTop);
    iPopUpNote->ShowInfoPopupNote();
    }

void CImagicUtils::ExecutePopUpNote(TInt aResourceId, TDes& aFilename, TInt aTime)
    {
    if(iPopUpNote)
        {
        delete iPopUpNote;
        }
    if(iTextResource)
        {
        //CleanupStack::PopAndDestroy(iTextResource);
        delete iTextResource;
        }
    
    iPopUpNote = CAknInfoPopupNoteController::NewL();
    //iTextResource = StringLoader::LoadLC( aResourceId );
    iTextResource = StringLoader::LoadL( aResourceId );
    TBuf<512> text;
    
    //text.Format(*textResource);
    TParse parser;
    parser.Set(aFilename, NULL, NULL);
    aFilename = parser.NameAndExt();
    text.Format(*iTextResource, &aFilename);
    
    iPopUpNote->SetTextL(text);
    iPopUpNote->SetTimeDelayBeforeShow(0);
    iPopUpNote->SetTimePopupInView( aTime );
    iPopUpNote->SetPositionAndAlignment(TPoint(0,0), /*TGulAlignmentValue*/EHLeftVTop);
    iPopUpNote->ShowInfoPopupNote();
    }  

void CImagicUtils::ExecutePopUpNote(TInt aResourceId, TInt aTime)
    {
    if(iPopUpNote)
        {
        delete iPopUpNote;
        }
    if(iTextResource)
        {
        //CleanupStack::PopAndDestroy(iTextResource);
        delete iTextResource;
        }
    
    iPopUpNote = CAknInfoPopupNoteController::NewL();
    //iTextResource = StringLoader::LoadLC( aResourceId );
    iTextResource = StringLoader::LoadL( aResourceId );
    TBuf<512> text;
    
    //text.Format(*textResource);
/*    TParse parser;
    parser.Set(aFilename, NULL, NULL);
    aFilename = parser.NameAndExt();*/
    text.Format(*iTextResource);
    
    iPopUpNote->SetTextL(text);
    iPopUpNote->SetTimeDelayBeforeShow(0);
    iPopUpNote->SetTimePopupInView( aTime );
    iPopUpNote->SetPositionAndAlignment(TPoint(0,0), /*TGulAlignmentValue*/EHLeftVTop);
    iPopUpNote->ShowInfoPopupNote();
    }

void CImagicUtils::ExecuteFileScanPopUpNote(TInt aResourceId, TInt aTime)
    {
    if(iFileScanPopUpNote)
        {
        delete iFileScanPopUpNote;
        }
    if(iTextResource)
        {
        //CleanupStack::PopAndDestroy(iTextResource);
        delete iTextResource;
        }
    
    iFileScanPopUpNote = CAknInfoPopupNoteController::NewL();
    //iTextResource = StringLoader::LoadLC( aResourceId );
    iTextResource = StringLoader::LoadL( aResourceId );
    TBuf<512> text;
    
    //text.Format(*textResource);
/*    TParse parser;
    parser.Set(aFilename, NULL, NULL);
    aFilename = parser.NameAndExt();*/
    text.Format(*iTextResource);
    
    iFileScanPopUpNote->SetTextL(text);
    iFileScanPopUpNote->SetTimeDelayBeforeShow(0);
    iFileScanPopUpNote->SetTimePopupInView( aTime );
    iFileScanPopUpNote->SetPositionAndAlignment(TPoint(0,0), /*TGulAlignmentValue*/EHLeftVTop);
    iFileScanPopUpNote->ShowInfoPopupNote();
    }


void CImagicUtils::ExecutePopUpNote(TDes& aFilename, TInt aTime, TBool aAligment)
    {
    if(iPopUpNote)
        {
        delete iPopUpNote;
        }
    
    iPopUpNote = CAknInfoPopupNoteController::NewL();
    
    /*TBuf<512> text;
    
    TParse parser;
    parser.Set(aFilename, NULL, NULL);
    aFilename = parser.NameAndExt();*/
    
    iPopUpNote->SetTextL(aFilename);
    iPopUpNote->SetTimeDelayBeforeShow(0);
    iPopUpNote->SetTimePopupInView(aTime);
    iPopUpNote->SetPriority(EPriorityHigh);
    
    //if(aAligment)
        iPopUpNote->SetPositionAndAlignment(TPoint(0,0), EHLeftVTop);
    /*else
        iPopUpNote->SetPositionAndAlignment(TPoint(0,0), EHRightVCenter);*/
    
    iPopUpNote->ShowInfoPopupNote();
    }  

/* Show text on Display */
void CImagicUtils::ShowText(const TDesC16& aText, CWindowGc& gc, TRect aRect,const CFont*aFont,TRgb aTransparentBlack, TRgb aTransparentWhite) const
    {
    //RDebug::Print(_L("CImagicAppUi::ShowText"));
        
    gc.SetPenStyle(CGraphicsContext::ESolidPen);//ESolidPen, ENullPen
    gc.UseFont(aFont);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
        
    gc.SetBrushColor(aTransparentWhite);
    gc.SetPenColor(aTransparentBlack);
    
    TRect rectText(TPoint(0, 0), TPoint(aRect.Width(),aFont->HeightInPixels()+3));
    gc.DrawText(aText, rectText, rectText.Height() - aFont->DescentInPixels(), CGraphicsContext::ECenter );
    
    }

/* Executing Error Dialog */
void CImagicUtils::ExecuteQueryDialog(TInt /*aError*/,TInt aResourceId)
    {
    //if (aError != KErrNone)
       {
         CAknQueryDialog* dlg;
         dlg = new ( ELeave ) CAknQueryDialog();
         TInt result = dlg->ExecuteLD( aResourceId );
         if(result == KErrNone || result != KErrNone)
             User::Exit(-1);
       }
    }  
    
TInt CImagicUtils::ExecuteQueryDialog(TInt aResourceId)
	{
	 CAknQueryDialog* dlg;
	 dlg = new ( ELeave ) CAknQueryDialog();
	 TInt result = dlg->ExecuteLD( aResourceId );
	 return result;
	}
	
 void CImagicUtils::DisplayWaitDialog(TInt aResourceId, TBool aSingular)
    {
    //show wait dialog untill engine callback cancells it
    if(iWaitDialog)
        {
        delete iWaitDialog;
        }
    iWaitDialog = NULL;
    iWaitDialog = new(ELeave) CAknWaitDialog(NULL, ETrue);
    
    iWaitDialog->SetTextPluralityL(aSingular);
    iWaitDialog->ExecuteLD( aResourceId );
 
    }
 
void CImagicUtils::ProcessFinishedL()
    {
	//iWaitDialog->ProcessFinishedL();
	//iWaitDialog = NULL;
	CancelWaitDialog();
    }

void CImagicUtils::CancelWaitDialog()
    {
    if(iWaitDialog != NULL)
        {
        iWaitDialog->ProcessFinishedL();
        iWaitDialog = NULL;
        }
    }
  
void CImagicUtils::ShowWaitDialog( TInt  aResourceId )
    {
    TInt result = 0;
    //show wait dialog untill engine callback cancells it
    if(iWaitDialog)
        {
        delete iWaitDialog;
        }
    iWaitDialog = NULL;
    iWaitDialog = new(ELeave) CAknWaitDialog(NULL, ETrue);
    iWaitDialog->ExecuteLD( aResourceId );
    
    
    }
void CImagicUtils::DisplayTNInfoNoteStarted()
   {
   ExecuteInternalWaitNote(R_WAIT_NOTE, EFalse);
   }

void CImagicUtils::DisplayTNInfoNoteCompleted()
   {
   ExecuteInternalWaitNote(R_WAIT_NOTE_END, EFalse);
   }

void CImagicUtils::ShowInfoNote(TInt aResourceId)
    {
    HBufC* textResource = StringLoader::LoadLC( aResourceId );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote(ETrue);
    TBuf<512> text;
    text.Format(*textResource);
    note->ExecuteLD( text );
    CleanupStack::PopAndDestroy(textResource);
    }
    
void CImagicUtils::ShowInfoNote(TInt aResourceId, TDes& aFilename)
    {
    HBufC* textResource = StringLoader::LoadLC( aResourceId );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote(ETrue);
    TBuf<256+50> text;
    
    TParse parser;
    parser.Set(aFilename, NULL, NULL);
    aFilename = parser.NameAndExt();
    
    text.Format(*textResource, &aFilename);
    note->ExecuteLD( text );
    
    CleanupStack::PopAndDestroy(textResource);
    }

//Modifies given file name to new with adding counting 
//number to the end of the file name
void CImagicUtils::GetNewFileName( TDes&  aFileName )
    {
    TBuf<300> valToNumBuf;
    TInt tempCounter = 0;
    TParse parser;
    TFileName tmpName;
    TBool result = EFalse;
    
    while(1)
        {
        tempCounter++;
        
        /*Converting from Number to Buffer */
        valToNumBuf.Num(tempCounter);
        
        /*Parsing file name. */
        parser.Set(aFileName, NULL, NULL );
        tmpName = parser.DriveAndPath();
        tmpName.Append(parser.Name());
        
        /*Appending */
        tmpName.Append(_L("_"));
        tmpName.Append(valToNumBuf);
        tmpName.Append(_L(".jpg"));
        
        /*Checking the existing of file */
        result = BaflUtils::FileExists(iFs, tmpName);
        if(result)
            {
            /* File exists continue */
            continue;
            }
        else
            {
            /*File does not found then copy the file and comeout of the loop */
             aFileName.Copy(tmpName);
             break;     
            }
        
        }
    }
    
 
void CImagicUtils::GetOriginalFileName( TDes&  aFileName )
    {
  
    TParse parser;
    parser.Set(aFileName, NULL, NULL );
    TFileName tmpFileName = parser.Name();
    //tmpName.Append(_L("_01.jpg"));
    
    
    TFileName tmpPathName = parser.FullName();
    //Delete characters from TN folder after _PAlbTN folder
    TInt ret = tmpPathName.Find(_L("_PAlbTN\\"));
    tmpPathName.Delete(ret, tmpPathName.Length()-ret);
    
    aFileName = tmpPathName;
    aFileName.Append(tmpFileName);
    aFileName.Append(_L(".jpg"));
    }
 


    
 void CImagicUtils::ExecuteInternalWaitNote(TInt aResourceId,TBool aTextPlurality)
 {
   CAknNoteDialog* dlg = new ( ELeave ) CAknNoteDialog(CAknNoteDialog::ENoTone,CAknNoteDialog::ELongTimeout);
   dlg->PrepareLC( aResourceId );
   dlg->SetTextPluralityL(aTextPlurality);
   
   // Show the Dialog
   dlg->RunLD();
 }
