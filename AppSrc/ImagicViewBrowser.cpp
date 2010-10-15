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


// INCLUDE FILES
#include  <viewcli.h>
#include  <aknviewappui.h>
#include  <avkon.hrh>
#include  <aknquerydialog.h>
#include  <PhotoBrowser.rsg>
#include  "ImagicViewBrowser.h"
#include  "ImagicContainerBrowser.h"
#include  "Imagic.hrh"
#include  "ImagicAppUi.h"
#include  <S32FILE.H> 
#include  "ImagicUtils.h"
#include  "SendImageFile.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CImagicViewBrowser::ConstructL(const TRect& aRect)
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CImagicViewBrowser::ConstructL(CImagicAppUi* aImagicAppUi)
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::ConstructL++"));
    
    iImagicAppUi = aImagicAppUi;
    iApplicationFeature = EAppFeatureNone;
    iFaceCroppingComplete = ETrue;
    
    BaseConstructL( R_IMAGIC_VIEWBROWSER );
    
    //Create bitmap for high res image loading
    iBitmap = new (ELeave) CFbsBitmap();
#ifdef USE_RGBA
    iBitmap->Create(TSize(10,10), EColor16MU);
#else
    iBitmap->Create(TSize(10,10), EColor16M);
#endif
    iTNCreationComplete = EFalse;
    iFaceBrowsingComplete = EFalse;
    
    //For dynamic options menu
    iEditModeEnabledCmd1 = ETrue;
    
    User::LeaveIfError(iFsSession.Connect());
    
#ifdef USE_SETTINGS_FILE
    // Load user settings
    TRAP_IGNORE(ReadSettingsFileL(KSettingFileName));  
#endif
    SetGridMode((TGridMode)iSettings.GetValue(CSettings::ESettingGridMode));
    
    DP0_IMAGIC(_L("CImagicViewBrowser::ConstructL--"));
    }

// ---------------------------------------------------------
// CImagicViewBrowser::~CImagicViewBrowser()
// Default destructor
// ---------------------------------------------------------
//
CImagicViewBrowser::~CImagicViewBrowser()
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::~ConstructL++"));
    
#ifdef USE_SETTINGS_FILE
    // Save user settings
    if (iSettings.IsChanged())
        TRAP_IGNORE(WriteSettingsFileL(KSettingFileName));  
#endif    
    
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    iBitmap->Reset();
    delete iBitmap;
    delete iContainer;
    iFsSession.Close();
    
    iTempCordArray.Close();
    iCoordinates.Close();
    
    DP0_IMAGIC(_L("CImagicViewBrowser::~ConstructL--"));
    }

// ---------------------------------------------------------
// TUid CImagicViewBrowser::Id()
// This returns the view ID
// ---------------------------------------------------------
//
TUid CImagicViewBrowser::Id() const
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::id"));
    return BrowserView;
    }

// ---------------------------------------------------------
// CImagicViewBrowser::HandleCommandL(TInt aCommand)
// Here we handle commands for this view.
// Each view has their own HandleCommandL()
// ---------------------------------------------------------
//
void CImagicViewBrowser::HandleCommandL(TInt aCommand)
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::HandleCommandL++"));
    
    CIEEngine* engine = iImagicAppUi->GetEngine();
    
    switch ( aCommand )
        {
        iContainer->DrawNow();
        //case EAknSoftkeyOk:
        case EAknSoftkeyBack:
            {
            if(iContainer->GetDrawMode() == CImagicContainerBrowser::EOneByOne){
                iContainer->SetDrawMode(CImagicContainerBrowser::EGrid);
                }
            else if(iContainer->GetDrawMode() == CImagicContainerBrowser::EFaceBrowser){
                iContainer->SetDrawMode(CImagicContainerBrowser::EOneByOne);
                iFaceBrowsingMode = EFaceBrowserNone;
                }
            else if(iContainer->GetDrawMode() == CImagicContainerBrowser::EGrid){
                AppUi()->HandleCommandL(EEikCmdExit);
                }
            //iContainer->DrawNow();
            break;
            }
            
        case EAknSoftkeyExit:
            {
            AppUi()->HandleCommandL(EEikCmdExit);
            break;
            }
            
        case EAknCmdExit:
            {
            AppUi()->HandleCommandL(EEikCmdExit);
            break;
            }    
            
        case EImagicCmdViewBrowserRotateRight:
            {
            CImageData* imageData = engine->GetImageData(iImagicAppUi->GetImageIndex());
            TInt rotAngle = imageData->GetOrientation();
            imageData->SetOrientation((rotAngle + 270)%360);
            engine->SetImageRotation(iImagicAppUi->GetImageIndex());
            engine->SetDBChanged(imageData);
            break;
            }
     
        case EImagicCmdViewBrowserRotateLeft:
            {
            CImageData* imageData = engine->GetImageData(iImagicAppUi->GetImageIndex());
            TInt rotAngle = imageData->GetOrientation();
            imageData->SetOrientation((rotAngle + 90)%360);
            engine->SetImageRotation(iImagicAppUi->GetImageIndex());
            engine->SetDBChanged(imageData);
            break;
            }
     
        //Deleting the Image...
        case EImagicCmdViewBrowserDelete:
            {
            iContainer->DisplayDeleteQueryDialogL(R_DELETE_QUERY);
            break;
            }

        //Switch grid mode 
        case EImagicCmdViewBrowserGridModeFolder:
            SetGridMode(EGridModeFolder);
            break;
            
        case EImagicCmdViewBrowserGridModeTime:
            SetGridMode(EGridModeTime);
            break;

        case EImagicCmdViewBrowserGridModePeople:
            SetGridMode(EGridModePeople);
            break;          
            
        //Show Image info
        case EImagicCmdViewBrowserShowImageInfo:
            {
            //Get image Info ----------------->
            
            const TInt KMaxInfoFileNameLength = 50;
            
            TBuf<1024> buf;
            //TPtr ptr(buf.Des());
            TFileName fileName;
            CImageData* imageData = engine->GetImageData(iImagicAppUi->GetImageIndex());
            imageData->GetFileName(fileName, EFullSize);
            
            // Make file name shorter if doesn't fit to info box well
            if (fileName.Length() > KMaxInfoFileNameLength)
                {
                for(TInt i = fileName.Length() - KMaxInfoFileNameLength;i < fileName.Length();i++)
                    {
                    if (i > 3 && fileName[i] == '\\')
                        {
                        fileName.Replace(3, i - 3, _L("..."));
                        break;
                        }
                    }
                }
            
            buf.Append(fileName);
            buf.Append(_L("\n"));     
            
            TDateTime dateTime = imageData->GetCreatedTime().DateTime();
            TInt year, month, day, hour, minute, second;
            day = dateTime.Day() + 1;
            month = dateTime.Month() + 1;
            year = dateTime.Year();
            hour = dateTime.Hour();
            minute = dateTime.Minute();
            second = dateTime.Second();
            
            TLocale locale;
            TDateFormat dateFormat = locale.DateFormat();

            TBuf<20> num1, num2, num3;
            switch (dateFormat)
                {
                case EDateEuropean:
                    num1.Num(day);
                    buf.Append(num1); 
                    buf.Append(locale.DateSeparator(1));
                    num1.Num(month);
                    buf.Append(num1); 
                    buf.Append(locale.DateSeparator(2));
                    num1.Num(year);
                    buf.Append(num1);
                    break;

                case EDateAmerican:
                    num1.Num(month);
                    buf.Append(num1); 
                    buf.Append(locale.DateSeparator(1));
                    num1.Num(day);
                    buf.Append(num1); 
                    buf.Append(locale.DateSeparator(2));
                    num1.Num(year);
                    buf.Append(num1); 
                    break;
                
                case EDateJapanese:
                    num1.Num(year); 
                    buf.Append(num1); 
                    buf.Append(locale.DateSeparator(1));
                    num1.Num(day); 
                    buf.Append(num1); 
                    buf.Append(locale.DateSeparator(2));
                    num1.Num(month); 
                    buf.Append(num1); 
                    break;
                }
            buf.Append(KSpace);
            
            TTimeFormat timeFormat = locale.TimeFormat();
            num2.Num(minute); if(minute < 10) num2.Insert(0, _L("0"));
            num3.Num(second); if(second < 10) num3.Insert(0, _L("0"));
            if (timeFormat == ETime12)
                {
                num1.Num(hour > 12 ? hour - 12 : hour);
                if (locale.AmPmSymbolPosition() == ELocaleBefore)
                    {
                    buf.Append(hour < 12 ? _L("AM") : _L("PM"));
                    if (locale.AmPmSpaceBetween()) 
                        buf.Append(KSpace);
                    buf.Append(num1);
                    buf.Append(locale.TimeSeparator(1));
                    buf.Append(num2);
                    buf.Append(locale.TimeSeparator(2));
                    buf.Append(num3);                    
                    }
                else
                    {
                    buf.Append(num1);
                    buf.Append(locale.TimeSeparator(1));
                    buf.Append(num2);
                    buf.Append(locale.TimeSeparator(2));
                    buf.Append(num3);
                    if (locale.AmPmSpaceBetween()) 
                        buf.Append(KSpace);
                    buf.Append(hour < 12 ? _L("AM") : _L("PM"));                    
                    }
                }
            else
                {
                num1.Num(hour);                
                buf.Append(num1);
                buf.Append(locale.TimeSeparator(1));            
                buf.Append(num2);
                buf.Append(locale.TimeSeparator(2));            
                buf.Append(num3);                
                }
            buf.Append(KNewLine);            

            num1.Num(imageData->GetSize().iWidth); 
            num2.Num(imageData->GetSize().iHeight);
            buf.Append(num1);
            buf.Append(_L(" x "));
            buf.Append(num2);            
            
            //buf.Append(_L("\n"));
            //dateTime.TDateTime(aYear, aMonth, aDay, aHour, aMinute, aSecond, aMicroSecond);
                    
            iImagicAppUi->GetImagicUtils()->ExecutePopUpNote(buf, 10000, iContainer->GetScreenOrientation());
            break;
            }

        //FaceBrowsing          
        case EImagicCmdViewFaceBrowsing:
            {
            //Set Face Browsing Mode
            //iFaceBrowsingMode = EFaceBrowsing;
            iFaceBrowsingMode = EFaceBrowsingShowRect;
            iContainer->InitFaceBrowsing();
            
            //Clear if face browser had old data
            iContainer->ClearFaceArray();
            ResetFaceCoords();
            
            //Get the file for FB processing
            TFileName fileName;
            engine->GetFileNameL(iImagicAppUi->GetImageIndex(), ESize512x512, fileName);

            //Check if background face browsing is still going on 
            //if(iFaceBrowsingComplete)
                {
                //If face browsing is complete just get face coordinates
                engine->GetFaceCoordinates(fileName, iCoordinates);
                
                if(iCoordinates.Count() >= 1)
                    {
                    if(iContainer)
                        iContainer->SetFaceCoords(iCoordinates);
                    
                    iContainer->SetDrawMode(CImagicContainerBrowser::EFaceBrowser);
                    }
                else
                    {
                    iImagicAppUi->GetImagicUtils()->ShowInfoNote(R_IMAGE_EDITOR_NO_FACES_FOUND_TEXT);
                    }
                }
            break;
            }

// unno begin
//#ifdef DOUBLETAP_FACEBROWSING
        //FaceBrowsing with coordinates          
        case EImagicCmdViewFaceBrowsingWithCoordinates:
            {
            CImageData* imageData = engine->GetImageData(iImagicAppUi->GetImageIndex());
            if(imageData->IsImageReady(ESize512x512))
                {
                //Get the original file, and reset local coord storage
                TFileName fileName;
                engine->GetFileNameL(iImagicAppUi->GetImageIndex(), ESize512x512, fileName);
                
                ResetFaceCoords();
                
                //Check if background face browsing is still going on 
                if(/*iFaceBrowsingComplete &&*/ iContainer) // also make sure container exists
                    {
                    //ResetFaceCoords();
                    
                    //If face browsing is complete just get face coordinates
                    engine->GetFaceCoordinates(fileName, iCoordinates);
                    
                    if(iCoordinates.Count() >= 1)
                        {
                        iContainer->SetFaceCoords(iCoordinates);

                        TInt faceid;
                        if (iContainer->FindNearestFace(iContainer->GetLastTouchPoint(), faceid))
                            {
                            // start face browsing view if there are faces
                            iContainer->SetDrawMode(CImagicContainerBrowser::EFaceBrowser);
                            iContainer->SetCurrentFaceNro(faceid);
                            //iFaceBrowsingMode = EFaceBrowsing;
                            iFaceBrowsingMode = EFaceBrowsingShowRect;
                            iContainer->InitFaceBrowsing();

                            //unno
                            DP0_IMAGIC(_L(" ------------------ Face browsing"));
                            }
                        else
                            {
                            DP1_IMAGIC(_L(" ------------------ No near faces (%d)"), faceid);
                            }
                        }
                    else
                        {
                        DP0_IMAGIC(_L(" ------------------ No faces in picture"));
                        }
                    }
                else
                    {
                    //Prioritise face browsing of selected picture if background process not completed 
                    iContainer->ClearFaceArray();
                    
                    DP0_IMAGIC(_L(" ------------------ Background process ongoing"));
                    }
                }
            break;
            }
//#endif

        //Remove false face detection Coords from exif data
        case EImagicCmdViewBrowserRemoveFace:
            {
            break;
            }
            
        //Add new face detection to exif data
        case EImagicCmdViewBrowserAddNewFace:
            {
            //Set Face Browsing Mode
            iFaceBrowsingMode = EFaceBrowsingAddNewFace;
            iImagicAppUi->GetImagicUtils()->ExecutePopUpNote(R_IMAGE_ADD_NEW_FACE_HELP_TEXT, 15000);
                        
            //Just set draw mode as oneByOne here
            iContainer->SetDrawMode(CImagicContainerBrowser::EOneByOne);
            break;
            }

        //Add as new face to exif data
        case EImagicCmdViewBrowserAddAsThisNewFace:
            {
            break;
            }
        
        //Face cropping
        case EImagicCmdViewBrowserFaceCropping:
            {
            iContainer->SetBGPSStatus(EFalse);
            
            iFaceCroppingComplete = EFalse;
            //iImagicAppUi->GetEngine()->StartFaceCropping(iImagicAppUi->GetImageIndex());
            break;
            }
            
        case EImagicCmdViewBrowserHelp:
            {
            CArrayFix<TCoeHelpContext>* buf = iImagicAppUi->AppHelpContextL();
            //TBuf<10> buf;
            HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), buf);
            break;
            }
            
        case EImagicCmdViewBrowserSend:
            {
            TFileName imageFileName;
            
            iImagicAppUi->GetEngine()->GetFileNameL(iContainer->GetCurrentIndex(), EFullSize, imageFileName);
            
            CSendImageFile *sender;
            sender = CSendImageFile::NewL();
            sender->SendFileViaSendUiL(imageFileName);
            delete sender;
            break;
            }

        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    DP0_IMAGIC(_L("CImagicViewBrowser::HandleCommandL--"));
    }

void CImagicViewBrowser::SetGridMode(TGridMode aGridMode)
    {
    CIEEngine* engine = iImagicAppUi->GetEngine();
    CIEImageList& imageList = engine->GetImageList();
    //if (imageList.IsGroupingFolders() != aEnable)
        {
        iSettings.SetValue(CSettings::ESettingGridMode, aGridMode);
        
        CImageData* imageData = NULL;
        TInt index = 0;
        
        if (iContainer)
            {
            // Get index of currently selected image
            index = iContainer->GetCurrentIndex();
            if (iContainer->IsUserInputGiven())
                imageData = engine->GetImageData(iContainer->GetCurrentIndex());
            }
        
        imageList.SetGridMode(aGridMode); 
        
        if (iContainer)
            {
            // Update grid order
            iContainer->ImageListChanged(0, EFalse);
            
            // Set index of same image in new grid
            if (imageData)
                {
                index = imageList.GetImageIndex(imageData);
                iContainer->SetCurrentIndex(index);
                }
            }
        }
    }

/*
void CImagicViewBrowser::SingleFaceBrowsingComplete()
    {
    //iImagicAppUi->GetEngineL()->GetFaceCoordinates(tmpFileName, iCoordinates);
    
    if(iCoordinates.Count() >= 1)
        {
        if(iContainer)
            iContainer->SetFaceCoords(iCoordinates);
        
        iContainer->SetDrawMode(CImagicContainerBrowser::EFaceBrowser);
        }
    else
        {
        iImagicAppUi->GetImagicUtils()->ShowInfoNote(R_IMAGE_EDITOR_NO_FACES_FOUND_TEXT);
        //DisplayAddFacesQueryDialogL(R_ADD_FACE_MANUALLY_QUERY);
        }
    }
*/


void CImagicViewBrowser::DisplayAddFacesQueryDialogL(TInt aResourceId)
    {
    CAknQueryDialog* dlg;
    dlg = new ( ELeave ) CAknQueryDialog();
    TInt result = dlg->ExecuteLD( aResourceId );
    
    if(result != KErrNone)
        {
        iFaceBrowsingMode = EFaceBrowsingAddNewFace;
        //iImagicAppUi->GetImagicUtils()->ShowInfoNote(R_IMAGE_ADD_NEW_FACE_HELP_TEXT);
        iImagicAppUi->GetImagicUtils()->ExecutePopUpNote(R_IMAGE_ADD_NEW_FACE_HELP_TEXT, 15000);
        }
    else
        {
        iFaceBrowsingMode = EFaceBrowserNone;                    
        }
    }

void CImagicViewBrowser::SetFaceBrowsingMode(TFaceBrowsingModes aMode)
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::SetFaceBrowsingMode"));
    iFaceBrowsingMode = aMode;
    }

TFaceBrowsingModes CImagicViewBrowser::GetFaceBrowsingMode()
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::GetFaceBrowsingMode"));
    return iFaceBrowsingMode;
    }


// ---------------------------------------------------------
// CImagicViewBrowser::HandleClientRectChange()
// ---------------------------------------------------------
//
void CImagicViewBrowser::HandleClientRectChange()
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::HandleClientRectChange++"));
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        iContainer->SetFullScreen();
        }
    DP0_IMAGIC(_L("CImagicViewBrowser::HandleClientRectChange--"));
    }

// ---------------------------------------------------------
// CImagicViewBrowser::DoActivateL(...)
// This is called when a view needs to be activated.
// This creates container with its controls.
// ---------------------------------------------------------
//
void CImagicViewBrowser::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::DoActivateL++"));
    
    // Create Container
    if (!iContainer)
        {
        DP0_IMAGIC(_L("CImagicViewBrowser::DoActivateL - create container"));
        iContainer = new (ELeave) CImagicContainerBrowser;
        iContainer->SetMopParent(this);
        iContainer->ConstructL( iImagicAppUi, this, ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        
        if(iTNCreationComplete)
            iContainer->SetBGPSStatus(ETrue);
        else
            iContainer->SetBGPSStatus(EFalse);
        }
    
    iImagicAppUi->CImagicAppUiReady();
    iImagicAppUi->BrowserContainerInitialized();
    
    DP0_IMAGIC(_L("CImagicViewBrowser::DoActivateL--"));
   }

// ---------------------------------------------------------
// CImagicViewBrowser::HandleCommandL(TInt aCommand)
// This is called when a view needs to be deactivated.
// This destroys container with its controls.
// ---------------------------------------------------------
//
void CImagicViewBrowser::DoDeactivate()
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::DoDeactivate++"));
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    DP0_IMAGIC(_L("CImagicViewBrowser::DoDeactivate--"));
    }


// Set bitmap to draw
void CImagicViewBrowser::SetActiveViewL(TUid /*aViewNro*/)
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::SetActiveView"));
    AppUi()->ActivateLocalViewL(BrowserView);
    }

//void CImagicViewBrowser::LoadBitmapsToBrowserL(TInt aIndex, TBool aHighRes)
void CImagicViewBrowser::LoadBitmapsToBrowserL(CImageData* aImageData, TThumbSize aImageResolution)
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::LoadBitmapsToBrowserL++"));
    
    ASSERT(iContainer);
    
    if (iContainer)
        {
        iImagicAppUi->GetEngine()->GetBitmapL(aImageData, iBitmap, aImageResolution);
        iImageResolution = aImageResolution;
        }
    DP0_IMAGIC(_L("CImagicViewBrowser::LoadBitmapsToBrowserL--"));
    }


//Callback from engine that bitmap has been loaded
void CImagicViewBrowser::BitmapLoadedByEngineL(TInt aError)
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::BitmapLoadedByEngine++"));
    
    iContainer->ImageLoadedL(aError, iBitmap, iImageResolution);
	
    iContainer->SetLoadingOn(ETrue);
    
	// Request to load next thumbnail
    if (aError == KErrNone)
        iContainer->DynamicLoadingL(); 
    
    DP0_IMAGIC(_L("CImagicViewBrowser::BitmapLoadedByEngine--"));
    }

CImagicContainerBrowser* CImagicViewBrowser::GetContainer()
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::GetContainer"));
    //We return null on purpose if container does not exits
    return iContainer;
    }

void CImagicViewBrowser::TNCreationComplete()
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::TNCreationComplete++"));
    iTNCreationComplete = ETrue;
    
    if(iContainer)
        iContainer->SetBGPSStatus(ETrue);
    
    DP0_IMAGIC(_L("CImagicViewBrowser::TNCreationComplete--"));
    }

void CImagicViewBrowser::FaceDetectionComplete()
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::FaceBrowsingComplete++"));
    iFaceBrowsingComplete = ETrue;
    
    /*if(iContainer)
        iContainer->SetTNCreationComplete(ETrue);*/
    
    DP0_IMAGIC(_L("CImagicViewBrowser::FaceBrowsingComplete--"));
    }

void CImagicViewBrowser::TNCreationBegin()
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::TNCreationBegin++"));
    iTNCreationComplete = EFalse;
    
    if(iContainer)
        iContainer->SetBGPSStatus(EFalse);
    DP0_IMAGIC(_L("CImagicViewBrowser::TNCreationBegin--"));
    }

TApplicationFeature CImagicViewBrowser::GetAppFeature()
    {
    return iApplicationFeature;
    }

// ----------------------------------------------------------------------------
// CImagicViewBrowser::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
// This function is called by the EIKON framework just before it displays
// a menu pane. Sets the state of menu items dynamically according
// to the state of application data.
// ----------------------------------------------------------------------------
//
void CImagicViewBrowser::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::DynInitMenuPaneL++"));
    
    CTextureLoader* textLoader = iContainer->GetTextureLoader();
    CImagicContainerBrowser::TDrawFunction drawMode = iContainer->GetDrawMode();
    
    if(aResourceId == R_IMAGIC_VIEWBROWSER_MENU)
        {
        //Initialaly hide Edit, Crop and Delete
        aMenuPane->SetItemDimmed(EImagicCmdViewEdit, ETrue); //Edit
        aMenuPane->SetItemDimmed(EImagicCmdViewFaceBrowsing, ETrue); //FaceBrowsing
        aMenuPane->SetItemDimmed(EImagicCmdViewBrowserCropping, ETrue); //Crop
        //aMenuPane->SetItemDimmed(EImagicCmdViewBrowserDelete, EFalse); //Delete
                
        aMenuPane->SetItemDimmed(EImagicCmdViewBrowserRemoveFace, ETrue);//Remove false detection Coords from exif data
        aMenuPane->SetItemDimmed(EImagicCmdViewBrowserAddNewFace, ETrue);//Add new face detection to exif data
        aMenuPane->SetItemDimmed(EImagicCmdViewBrowserAddAsThisNewFace, ETrue);//Add as new face to exif data
        aMenuPane->SetItemDimmed(EImagicCmdViewBrowserFaceCropping, ETrue);//Face cropping
        
        aMenuPane->SetItemDimmed(ECmdRotateImage, EFalse); //Layouts
        aMenuPane->SetItemDimmed(EImagicCmdViewBrowserShowImageInfo, EFalse); //Image Info
        aMenuPane->SetItemDimmed(EImagicCmdViewBrowserHelp, ETrue); //Image Info
        
        if((drawMode == CImagicContainerBrowser::EOneByOne || drawMode == CImagicContainerBrowser::EFaceBrowser))
            {
            aMenuPane->SetItemDimmed(EImagicCmdViewBrowserGridModeFolder, ETrue);            
            aMenuPane->SetItemDimmed(EImagicCmdViewBrowserGridModeTime, ETrue); 
            
            //if(iTNCreationComplete && iFaceCroppingComplete)
            CImageData* imageData = iImagicAppUi->GetEngine()->GetImageData(iImagicAppUi->GetImageIndex());
            if(imageData->IsImageReady(ESize512x512))
                {
                aMenuPane->SetItemDimmed(EImagicCmdViewEdit, ETrue); //Edit
                if(imageData->GetNumberOfFaces() > 0)
                    aMenuPane->SetItemDimmed(EImagicCmdViewFaceBrowsing, EFalse); //FaceBrowsing
                
                aMenuPane->SetItemDimmed(EImagicCmdViewBrowserCropping, ETrue); //Crop
                aMenuPane->SetItemDimmed(EImagicCmdViewBrowserFaceCropping, ETrue);//Face cropping
                
                if(iFaceBrowsingMode == EFaceBrowsing)
                    {
                    aMenuPane->SetItemDimmed(EImagicCmdViewFaceBrowsing, ETrue); //FaceBrowsing
                    aMenuPane->SetItemDimmed(EImagicCmdViewBrowserAddAsThisNewFace, ETrue);//Add as new face to exif data
                    aMenuPane->SetItemDimmed(EImagicCmdViewBrowserCropping, ETrue); //Crop
                    aMenuPane->SetItemDimmed(EImagicCmdViewBrowserDelete, ETrue); //Delete
                    aMenuPane->SetItemDimmed(EImagicCmdViewBrowserFaceCropping, ETrue);//Face cropping
                    aMenuPane->SetItemDimmed(EImagicCmdViewBrowserShowImageInfo, EFalse); //Image Info
                    }
                }
            }
        else
            {
            TGridMode gridMode = (TGridMode)iSettings.GetValue(CSettings::ESettingGridMode);
            aMenuPane->SetItemDimmed(EImagicCmdViewBrowserGridModeFolder, gridMode != EGridModeFolder);            
            aMenuPane->SetItemDimmed(EImagicCmdViewBrowserGridModeTime, gridMode != EGridModeTime); 
            aMenuPane->SetItemDimmed(EImagicCmdViewBrowserGridModePeople, 
#ifdef PEOPLE_VIEW			
				ETrue);
#else
				gridMode != EGridModePeople);
#endif				
            }
        }
    
    DP0_IMAGIC(_L("CImagicViewBrowser::DynInitMenuPaneL--"));
    }

void CImagicViewBrowser::WriteSettingsFileL(const TDesC& aName)
{
    DP0_IMAGIC(_L("CImagicViewBrowser::WriteSettingsFileL++"));
    
    TParse filestorename;
    iFsSession.Parse(aName,filestorename);
    
    RFileWriteStream writer;
    writer.PushL();
    User::LeaveIfError(writer.Replace(iFsSession, filestorename.FullName(), EFileWrite));
    
    writer << iSettings; 
    writer.CommitL();
    
    CleanupStack::PopAndDestroy();

    DP0_IMAGIC(_L("CImagicViewBrowser::WriteSettingsFileL--"));
}


void CImagicViewBrowser::ReadSettingsFileL(const TDesC& aName)
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::ReadSettingsFileL++"));
    TParse filestorename;
    iFsSession.Parse(aName,filestorename);

    RFileReadStream reader;
    reader.PushL();
    
    User::LeaveIfError(reader.Open(iFsSession, filestorename.FullName(),EFileRead));
    reader >> iSettings;

    // Cleanup the stream object
    CleanupStack::PopAndDestroy();
    DP0_IMAGIC(_L("CImagicViewBrowser::ReadSettingsFileL--"));
    }

//Returns true if file exists
/*TBool CImagicViewBrowser::FindFileName(const TDesC& aName)
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::FindFileName++"));
    TBool result = BaflUtils::FileExists(iFsSession, aName); 
    DP0_IMAGIC(_L("CImagicViewBrowser::FindFileName--"));
    return result;
    }*/    

CSettings::CSettings()
    {
    iChanged = EFalse;
    Mem::FillZ(iValues, sizeof(iValues));
    }

//Functions from TModeSettings class
void CSettings::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteL((TUint8*)iValues, sizeof(iValues));
    }  
 
void CSettings::InternalizeL(RReadStream& aStream)
    {
    aStream.ReadL((TUint8*)iValues, sizeof(iValues));
    }

void CSettings::SetValue(TSettingsValue aIndex, TInt aValue)
    {
    if (iValues[aIndex] != aValue)
        {
        iValues[aIndex] = aValue;
        iChanged = ETrue;
        }
    }

TInt CSettings::GetValue(TSettingsValue aIndex) const
    {
    return iValues[aIndex];
    }

TBool CSettings::IsChanged() const
    {
    return iChanged;
    }

/*
void CImagicViewBrowser::SetFaceCoords(RArray<TRect>& aCoordinates)
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::SetFaceCoords++"));
    
    iCoordinates = aCoordinates;
    
    if(iContainer)
        iContainer->SetFaceCoords(aCoordinates);
    
    TInt tmp = iCoordinates.Count();
    for(TInt i = 0; i < tmp; i++)
        {
        iCoordinates.Remove(0);
        }
        
    DP0_IMAGIC(_L("CImagicViewBrowser::SetFaceCoords--"));
    }
*/
void CImagicViewBrowser::ResetFaceCoords()
    {
    DP0_IMAGIC(_L("CImagicViewBrowser::ResetFaceCoords++"));
    
    //iCoordinates.Reset();
    TInt tmp = iCoordinates.Count();
    for(TInt i = 0; i < tmp; i++)
        {
        iCoordinates.Remove(0);
        }
    }

// End of File

