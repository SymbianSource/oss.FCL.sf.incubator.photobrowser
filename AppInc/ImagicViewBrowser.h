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

#ifndef IMAGICVIEWBROWSER_H
#define IMAGICVIEWBROWSER_H

// INCLUDES
#include <aknview.h>
#include "TextureLoader.h"
#include "ImagicAppUi.h"


// FORWARD DECLARATIONS
class CImagicContainerBrowser;
class CImagicAppUi;
class CTextureLoader;

// CLASS DECLARATION

//Face browsing states
enum TFaceBrowsingModes
    {
     EFaceBrowserNone = 1,
     EFaceBrowsingShowRect,
     EFaceBrowsing,
     EFaceBrowsingRemoveFace,
     EFaceBrowsingAddNewFace,
     EFaceBrowsingAddThisAsNewFace
    };

enum TApplicationFeature
    {
    EAppFeatureCropping = 1,
    EAppFeatureEditing,
    EAppFeatureFaceBrowsing,
    EAppFeatureNone
    };


class CSettings
{
public :
    enum TSettingsValue {
        ESettingImageIndex = 0,
        ESettingGridMode = 1
    };
    
    CSettings();
    void ExternalizeL(RWriteStream& aStream) const;
    void InternalizeL(RReadStream& aStream);
    void SetValue(TSettingsValue aIndex, TInt aValue);
    TInt GetValue(TSettingsValue aIndex) const;
    TBool IsChanged() const;

private :
    TInt    iValues[2];
    TBool   iChanged;
};
 
/**
*  CImagicViewBrowser view class.
* 
*/
class CImagicViewBrowser : public CAknView
    {
    public: // Constructors and destructor

        /**
        * EPOC default constructor.
        */
        void ConstructL(CImagicAppUi*	aImagicAppUi);

        /**
        * Destructor.
        */
        ~CImagicViewBrowser();

    public: // Functions from base classes
        
        /**
        * Return Uid
        */
        TUid Id() const;

        /**
        * Handle Commands
        */
        void HandleCommandL(TInt aCommand);

        /**
        * Handle size change
        */
        void HandleClientRectChange();
        
        void SetActiveViewL(TUid aViewNro);
        void LoadBitmapsToBrowserL(CImageData* aImageData, TThumbSize aImageResolution);
        CImagicContainerBrowser* GetContainer();
        void BitmapLoadedByEngineL(TInt aError);
        void TNCreationComplete();
        void FaceDetectionComplete();
        void TNCreationBegin();
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
        
        void WriteSettingsFileL(const TDesC& aName);
        void ReadSettingsFileL(const TDesC& aName);
        //TBool FindFileName(const TDesC& aName);
        TApplicationFeature GetAppFeature();
        
        void EditImageL();
        void CropImageL();
        
        //void SetFaceCoords(RArray<TRect>& aCoordinates);
        void ResetFaceCoords();
        void SetFaceBrowsingMode(TFaceBrowsingModes aMode);
        TFaceBrowsingModes GetFaceBrowsingMode();
        
    private:

        /**
        * From AknView, activates view
        */
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);

        /**
        * From AknView, deactivates view
        */
        void DoDeactivate();
        void DisplayAddFacesQueryDialogL(TInt aResourceId);
        void SetGridMode(TGridMode aGridMode);
        
    private: // Data
        CImagicContainerBrowser*    iContainer;
        CImagicAppUi*	            iImagicAppUi;
        CFbsBitmap*                 iBitmap;
        TBool                       iHighRes;
        TBool                       iTNCreationComplete;//Set TRue if TNs are created
        TBool                       iFaceBrowsingComplete;//Set TRue if background Face Browsing is complete
        TBool                       iFaceCroppingComplete;//Set TRue if background Face Cropping is complete
        TBool                       iEditModeEnabledCmd1;
        RFs                         iFsSession;
        RArray<TRect>               iCoordinates;
        TFileName                   iFaceTNFilename;
        TFaceBrowsingModes          iFaceBrowsingMode;
        RArray<TRect>               iTempCordArray;
        TApplicationFeature         iApplicationFeature;
        //TFileName                   tmpFileName;
        TThumbSize                  iImageResolution;
        CSettings                   iSettings;        
    };

#endif

// End of File
