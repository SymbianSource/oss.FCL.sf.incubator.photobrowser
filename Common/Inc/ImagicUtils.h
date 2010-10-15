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

#ifndef IMAGICUTILS_
#define IMAGICUTILS_

#include <e32base.h>
#include <W32STD.H> 
#include <aknwaitdialog.h> 
#include "Imagic.hrh"
#include <AknInfoPopupNoteController.h>

class CAknInfoPopupNoteController;

class CImagicUtils: public CBase
{
public:
	/*
	* Symbian First phase constructor
	* 
	* @aParam aFs - FileServer
	*/
	static CImagicUtils* NewL(RFs &aFs);
	
	/*
	* Destructor.
	* 
	*/
 	~CImagicUtils();
public:
    void DisplayYearAndMonth(TInt aIndex, TDateTime dateTime);
    void ExecutePopUpNote(TDes& aFilename, TInt aTime, TBool aAligment);
    void ExecutePopUpNote(TInt aResourceId, TDes& aFilename, TInt aTime);
    void ExecutePopUpNote(TInt aResourceId, TInt aTime);
    void ExecuteFileScanPopUpNote(TInt aResourceId, TInt aTime);
    
	/*
	* Show Text on Display
	*
	* @aParam aText - Text to Display on screen
	* @aParam  gc - Windows GC
	* @aParam aRect - Window rectangle.
	* @aParam aFont - DisplayFont 
	* @aParam  aTransparentBlack - Black Transparent value 
	* @aParam  aTransparentWhite - White Transparent value 
	*/
    void ShowText(const TDesC16& aText, CWindowGc& gc, TRect aRect,const CFont*aFont,
    			 TRgb aTransparentBlack, TRgb aTransparentWhite) const;
    			 
    /*
    * Executing Error Dialog
    * 
    * @aParam aError - Error code
    * @aParam aResourceId - dialog Resource ID
    */
   	void ExecuteQueryDialog(TInt aError,TInt aResourceId);
   	
   	/*
    * Executing Query Dialog
    * 
    * @aParam aResourceId - dialog Resource ID
    * @returns - Returns Dialog error ID
    */
   	TInt ExecuteQueryDialog(TInt aResourceId);
   	
   	
   /*
    * Display Wait Dialog
    * 
    * @aParam aResourceId - dialog Resource ID
    * @aParam aSingular - Text to execute 
    */
    void DisplayWaitDialog(TInt aResourceId, TBool aSingular);
   
    /*
    * Cancels Wait Dialog
    * 
    */
    void CancelWaitDialog();
    
    /*
    * Display Wait Dialog
    * 
    * @aParam aResourceId - dialog Resource ID
    */
    void ShowWaitDialog(TInt aResouceId);
    
    /*
    * Cancels Wait Dialog
    * 
    */
    void ProcessFinishedL();
     
     
    /*
    * Display Infonote 
    */
    void DisplayTNInfoNoteStarted();
    
    /*
    * Display End Info note.
    */
    void DisplayTNInfoNoteCompleted();
    
    /*
    * Show Info note.
    *
    * @aParam aResourceId - Resource Id.
    */
    void ShowInfoNote(TInt aResourceId);
    
    /*
    * Display Saving Dailog
    *
    * @aParam aResourceId - Resource Id.
    */  
    void DisplaySavingDialog(TInt aResourceId);
     
    /*
    * Show Info note.
    *
    * @aParam aResourceId - Resource Id.
    * @aParam FileName - File Name.
    */
    void ShowInfoNote(TInt aResourceId, TDes& aFilename);
    
     /*
    * Get New File Name
    *
    * @aParam aFileName - Filename
    */
    
    void GetNewFileName( TDes&  aFileName );
    
    /*
    * Get Original file name
    *
    * @aParam aFileName - Original file name.
    */
    void GetOriginalFileName( TDes&  aFileName );
private:
	void ConstructL();
	CImagicUtils(RFs &aFs);
	
	void ExecuteInternalWaitNote(TInt aResourceId,TBool aTextPlurality);
 
private:


	RFs iFs;
	CAknWaitDialog*                iWaitDialog;
	CAknInfoPopupNoteController*   iPopUpNote;
	CAknInfoPopupNoteController*   iFileScanPopUpNote;
	HBufC*                         iTextResource;
};
#endif /*IMAGICUTILS_*/
