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

#ifndef IMAGICCONTAINERBROWSER_H
#define IMAGICCONTAINERBROWSER_H

//#define ADAPTIVE_FRAMERATE   // adaptive frame rate
#define MOMENTUM_MOVE
#define DOUBLETAP_FACEBROWSING
#define DOUBLETAP_SELECT_IN_GRID
#define SINGLETAP_CLOSE_IN_ZOOM
#define FLICK_ONLY_IN_X_IN_GRID
#define HOLD_SELECTION_ONDRAG
#undef  CURSORSIMULATION_IN_GRID
#undef  CURSORSIMULATION_IN_ONEBYONE
#undef  CURSORSIMULATION_IN_FACEBROWSER
#undef  SELECT_ON_TOUCHDOWN
#undef  TAP_AND_GESTURE
//#undef  DOUBLETAP_ZOOMGRID
#undef  USE_AVKON_LONGTAP_DETECTOR

//#undef RD_FACEFRAME
#define RD_FACEFRAME
#define RD_ZOOMICON

// INCLUDES
#include <coecntrl.h>
#include <aknnotewrappers.h>
#include <aknlongtapdetector.h>
#include <GLES\egl.h>
#include "ImagicAppUi.h"
#include <HWRMLight.h>
#include "Gesture.h"
#include "ImagicUtils.h"
#include "DrawableInterface.h"
#include "DrawUtility.h"
#include "CDrawGrid.h"
#include "CDrawOneByOne.h"
#include "CDrawFaceBrowsing.h"
#include "CDrawMagGlass.h"


#ifdef USE_AVKON_TACTILE_FEEDBACK
#include <touchfeedback.h>
#include <touchlogicalfeedback.h>
#endif

//#define VERTICES_PER_LINE 32


// FORWARD DECLARATIONS
class CImagicViewBrowser;
class CImagicAppUi;
class CGLImageHandler;
class CTextureLoader;
class CDrawUtility;
class CDrawGrid;
class CDrawOneByOne;
class CDrawFaceBrowsing;
class CDrawMagGlass;



// Variables telling how many times keys has been pressed
class CKeyData
	{
public:
    //void ResetData();
    
	//these variables hold number of key presses as long they are consumed
	//value must be reset after usage
	TInt iX,iY;		// Movement keys
	TInt iRotate;	// Rotation keys
	//TInt iZoom;		// Zooming keys
	
	// Tells if button is currently pressed
	TBool iLeft,iRight;
	TBool iUp,iDown;
	//TBool iZoomIn, iZoomOut;
	TBool iZoomInKey, iZoomOutKey;
    };


// CLASS DECLARATION

/**
*  CImagicContainerBrowser  container control class.
*  
*/
class CImagicContainerBrowser : public CCoeControl, MCoeControlObserver
    , MAknLongTapDetectorCallBack
    , MGestureCallBack
    {
    public: // Constructors and destructor
        
        //Drawing functions
        enum TDrawFunction
            {
            EGrid,
            EOneByOne,
            EFaceBrowser,
            ELastDrawFunction
            };

        //Grid Drawing modes
        enum TGridMode
            {
             EListof3 = 1,
             EListof5,
             ESquare
            };
        
        /**
        * EPOC default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(CImagicAppUi* aImagicAppUi, CImagicViewBrowser* aView, const TRect& aRect);

        /**
        * Destructor.
        */
        ~CImagicContainerBrowser();
        
        //void SetBitmapArrayL(CFbsBitmap* aBitmap);
        void ImageLoadedL(TInt aError, CFbsBitmap* aBitmap, TThumbSize aResolution/*, TInt aLoadedImageIndex*/);
        
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
        // Handle any tap on the screen
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);

        // From MAknLongTapDetectorCallBack
        void HandleLongTapEventL(const TPoint& aPenEventLocation, const TPoint& aPenEventScreenLocation);
        void HandleGestureBeganL(const TPoint& aPos);
        void HandleGestureMovedL(const TPoint& aPos, const TGestureType aType);
        void HandleGestureEndedL(const TPoint& aPos, const TGestureType aType);

        void DoTapAndDrag(const TPoint& aPos, const TGestureType aType);
        void DoCursorSimulation(const TPoint& aPos, const TGestureType aType);
        void DoDrag(const TPoint& aPos, const TGestureType aType);
        void DoFlick(const TPoint& aPos, const TGestureType aType);
        void DoSingleTap(const TPoint& aPos, const TGestureType aType);
        void DoDoubleTap(const TPoint& aPos, const TGestureType aType);
        void DoLongTapping(const TPoint& aPos, const TGestureType aType);
        void DoLongTap(const TPoint& aPos, const TGestureType aType);

        
        void SetFullScreen();
        void SetBitmapFromArrayL();
        TBool IsOpenGLInit();
        void OpenGLInitL();
        void InitL();
        void DeleteTextures();
        void DisableDisplayDraw();
        void EnableDisplayDraw();
        //void SetGridMode(TGridMode aDrawMode);
        void SetDrawMode(TDrawFunction aDrawFunction);
        TDrawFunction GetDrawMode();
        void SetBGPSStatus(TBool aValue);
        //void SetTNCreationStarted(TBool aValue);
        void DeleteImageL();
        CTextureLoader* GetTextureLoader();
        void DisplayDeleteQueryDialogL(TInt aResourceId);
        float GetAspectRatio(TInt aIndex);
        void SetFaceCoords(RArray<TRect>& aCoordinates);
        void ClearFaceArray();
        //void SwapArrays();
        void SetLoadingOn(TBool aValue);
        void DynamicLoadingL();
        void SetDeleteTextures(TBool aValue);
        //void InitFaceBrowsing();
        //MRemConCoreApiTargetObserver
        //void MrccatoCommand(TRemConCoreApiOperationId aOperationId, TRemConCoreApiButtonAction aButtonAct);
        
    private: // Functions from base classes

       /**
        * From CoeControl,SizeChanged.
        */
        void SizeChanged();

       /**
        * From CoeControl,CountComponentControls.
        */
        TInt CountComponentControls() const;

       /**
        * From CCoeControl,ComponentControl.
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

       /**
        * From CCoeControl,Draw.
        */
        void Draw(const TRect& aRect) const;

       /**
        * From CCoeControl, HandleControlEventL.
        */
        // event handling section
        // e.g Listbox events
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
        
        /**
         * Callback function for the CPeriodic. Calculates the current frame, keeps the background
         * light from turning off and orders the CSlideshow to do the rendering for each frame.
         *@param aInstance Pointer to this instance of CSlideshowContainer.*/
        static TInt DrawCallBackL( TAny* aInstance );
        void DrawL();
        void BeginDrawing();
        void EndDrawing();
        
/*----------------------------------------------------------------------*/
		void CheckLimits(float &aValue, const float aMin, const float aMax);
		void SetPictureVertices(CImageData* aData, GLfixed *aVertices);
		void HandleRotationKeys(void);
		//void HandleMovingKeysOnebyOne();
		
		
		//void BubbleEffect(TInt& x, TInt& y, float& z);
        
        static TInt DisableDrawTimer( TAny* aInstance );
        static TInt PowerSaveCallBack(TAny *aAnyPtr);
        void PowerSave();
        void CheckIndexLimits(TInt &aIndex);
        void SetQvgaCoordinates(const TPoint &aPoint, const TSize &aSize);
        void CalculateImageSize2(float& width, float& height, const float aAspectRatio);
        void InitDrawMagGlass();
        void DrawMagGlass(const TSize &aScreenPhysicalSize, TReal aImageAspectRatio);
        void DrawCross(const TSize &aScreenPhysicalSize, const TReal aImageAspectRatio);
        void SetMinMagFilterLinear(TBool aValue);
        void SetMinMagFilterLinearDo(TBool aValue);
        

        void MoveIndex(TInt aMoveX, TInt aMoveY, TBool aWrap = ETrue);
        void SelectIndex(void);
        FloatCoords ConvertCoordsFromScreen2OGl(const TPoint aPos);
        TBool FindImageInGrid(const TPoint aPos, TInt& aResultIndex);
        TInt GetFreeRam();
        TBool FindImageInScreen(const TPoint aPos, TInt& aResultIndex);
        TBool FindNearestImageInOGl(const FloatCoords aPos, TInt& aResultIndex);
        TBool SetMinMagFilter(CImageData* aImageData, TBool aMagFilterLinear, TInt aIndex);
        void ShowMagGlass(const TBool aState);
        
        void LoadHQ512Image(CImageData* imageData, TInt aIndex);
        void DrawFaceBrowsingIcon();
        void DrawZoomIcon();
        void SetMinMagFiltering();
        void ResetZoomKeys();
        void ResetDirKeyData();
        void HandleDrawingModeSwitch(TDrawFunction& aDrawFunction);
        void LoadHighResImage(CImageData* imageData, TInt aIndex);
        //void CheckIndexLimits(TInt &aIndex);
        
    public:      
        void SetDrawFreqToNormal(TInt aTimerDelay);
        TInt UpdateScreenDrawFreq();
        void CalculateImageSize(float& width, float& height, const float aAspectRatio);
        void HandleRotation(float& aRotationAngle, float& aTargetRotationAngle);
        void SetCurrentFaceNro(TInt aNro);
        void GetCurrentFilenameL(TFileName& aFilename, TThumbSize aRes);
        void ConvertScreenCoords2QvgaCoords(TPoint& aPoint, TRect& aRect);
        TBool FindNearestFace(const TPoint aPos, TInt& aResultIndex);
        void SetLastTouchPoint(const TPoint& aPos);
        TPoint GetLastTouchPoint(void);
        void SetTextIndex(GLuint aIndex);
        void IconTexturesLoaded(RArray<GLuint> aIconTexIndex);
        void Interpolate(float &aValue, const float aTarget, const float aStep);
        TBool GetScreenOrientation();
        void InitAfterPowerSaveL();
        void NewImageAdded();
        void ImageListChanged(TInt aIndex, TBool bAdded);
#ifdef _ACCELEROMETER_SUPPORTED_
        TImagicDeviceOrientation CImagicContainerBrowser::GetDeviceOrientation();
        void PhoneRotated(TImagicDeviceOrientation aDeviceOrientation);
#endif
        TBool IsHwAcceleration();
        void HandleSend2BackgroundEvent();
        void InitFaceBrowsing();
        TBool IsTouchPointThresholdExeed();
        float GetDisplayRotTargetAngle();
        float GetDisplayRotAngle();
        void SetDisplayRotAngle(float aValue);
        TReal GetMaxX() const;
        
        CKeyData& GetKeyData();
        void SetKeyData(CKeyData aData);
        void ResetKeyData();
        
        CKeyData& GetTouchData();
        void SetTouchData(CKeyData aData);
        void ResetTouchData();
                
        TBool GetSlideByDragValue();
        TInt GetCurrentIndex();
        TInt GetPrevIndex();
        void SetCurrentIndex(TInt aIndex);
        void SetPrevIndex(TInt aIndex);
        TInt GetGleMaxRes();
        void ResetHighResLoading();
        void DynamicUnLoading();
        TBool IsUserInputGiven();
        TSize GetScreenSize();
        void SetScreenImmeadetaUpdate(TBool aValue);
       
/*----------------------------------------------------------------------*/
        
    private: //data
        CImagicAppUi*           iImagicAppUi;//App UI class pointer
        CIEEngine*              iIEngine;
        CImagicViewBrowser*     iView;//Browser view class pointer
        //CTextureLoader*			iTextureLoader;
        CGesture*               iGesture;
        CDrawUtility*           iDrawUtility;
        
    friend class CTextureLoader;
        CTextureLoader*         iTextureLoader;
    friend class CDrawGrid;
        CDrawGrid*              iDrawGrid;
    friend class CDrawOneByOne;
        CDrawOneByOne*          iDrawOneByOne;
    friend class CDrawFaceBrowsing;
        CDrawFaceBrowsing*      iDrawFaceBrowsing;
    friend class CDrawMagGlass;
        //CDrawMagGlass*          iDrawMagGlass;
        
        
        
        
    private: //data
/*----------------------------------------------------------------------*/
       //OpenGL valiables
	   //Flag that indicates if OpenGL ES has been initialized or not.
	    TBool iOpenGlInitialized;
	    //Display where the OpenGL ES window surface resides.
	    EGLDisplay  iEglDisplay;
	    //Window surface where the OpenGL ES rendering is blitted to.
	    EGLSurface  iEglSurface;
	    // OpenGL ES rendering context.
	    EGLContext  iEglContext;
	    //Active object that is the timing source for the animation.
	    CPeriodic*  iPeriodic;
	    CPeriodic*  iPowerSavePeriodic;
/*----------------------------------------------------------------------*/	    
	    
        //Variables used to calculate time
        GLfloat     	    iLastTime;
        GLfloat     	    iTimeNow;
        GLfloat     	    iTimeDiff;

/*----------------------------------------------------------------------*/
        //Variables for OneByOne init
        TBool               iOnTheEdge;
        TBool               iTouchPointThreshold;
        TBool               iKeyPressedDown;

/*----------------------------------------------------------------------*/
        TInt				iCurrentIndex;// Selected picture index
		TInt                iPreviousIndex;// One before selected picture index
		
/*----------------------------------------------------------------------*/		
		
				
		//Draw function enum, OneByOne and Grid implemented, add here more when new draw function are created
		enum TDrawFunction	iDrawFunction;
		
		
		//Struct for KeyData, this struct holds all data for key events handling
		CKeyData		    iTouchMoveData;
		CKeyData            iKeyData;
		
		float               iDisplayRotation;//This controls the whole display rotation, not single picture
		float               iDisplayRotationTarget;//Display target rotation angle
		TBool               iScreenRotateOngoing;
		TReal               iScreenAspectRatio;
		TSize               iScreenSize;
		
		TBool               iTNCreationComplete; // Set TRue if TNs are created
        RCriticalSection    iDrawLock;
        
        //Texture related variables
        GLuint              iCurrentBindedIndex;
        TInt                iLoadingTextureIndex; // texture to draw if no image exist
        TInt                iExitTextureIndex; // texture to draw if no image exist
        TInt                iMenuTextureIndex; // texture to draw if no image exist
#ifdef SHADOW_PHOTOS        
        TInt                iShadowTextureIndex;
#endif        
        TInt                iDisplayDrawFreq;
        GLint               iGLMaxRes;//OpenGL max texture resolution
 
//        CRemConInterfaceSelector* iSelector;
//        CRemConCoreApiTarget* iTarget;
       
       //When set on drawing is enabled
       TBool                iDrawNow;

       //Image loading related flags
       TBool                iDynamicLoadingOn;//Set on for dynamic loading
       TBool                iNewImageAdded;//New image added to array
       TInt                 iIsLoaderRunning;
       
       TBool                iMagFilterLinear;//Set on when linear filtering is wanted
       
       RArray<GLuint>       iIconTextureIndexes;//Array to hold icon textures
       TBool                iMinMagFilterSetting;
       TBool                iPreferHighResLoading;//Set this on when want to load high resolution image(stops loading low res images)
       
       
#ifdef USE_AVKON_LONGTAP_DETECTOR
       CAknLongTapDetector* iLongTapDetector;
#endif
#ifdef USE_AVKON_TACTILE_FEEDBACK
       MTouchFeedback*      iTouchFeedBack;
#endif
       
       TPoint               iLastTouchPoint;
       
#ifdef HOLD_SELECTION_ONDRAG
       TBool                iHoldSelection;
       TBool                iOneByOneSlideByDrag;
#endif
#ifdef MOMENTUM_MOVE
       TBool                iMomentumMove;
       float                iMomentumSpeedX;
       float                iMomentumSpeedY;
#endif
       
       TReal                    iDeviceOrientationAngle;
#ifdef _ACCELEROMETER_SUPPORTED_
	   TImagicDeviceOrientation iDeviceOrientation;
	   TImagicDeviceOrientation iDeviceOrientationPrev;
#else
	   TBool                    iDeviceOrientation;
#endif
	   
#ifdef ADAPTIVE_FRAMERATE
	   TInt                     iWaitDrawTicks;
#endif	   
	   
	   //Remove these
	   TBool               iIntheEndOfGrid;
       TBool               iJumpOver;
	   TInt                iDrawOnes;
	   
	   TBool               iUserInputGiven;
	   TBool               iDeleteTextures;
	   TBool               iScreenImmeadetaUpdate;
	   
	   TReal               drawZoom;
	   TReal               inPictureX;
	   TReal               inPictureY;
	   
	   TBool               iLastEventFromKeys;
	   
	   //Class consts
	   static const GLfixed iGlobalTexCoords[4*2];
       static const float KMinOneByOneZoom;
       static const float KMaxOneByOneZoom;
	   static const TInt  KDoubleTapZoomOneByOne1;
       static const TInt  KDoubleTapZoomOneByOne2;
       static const TReal KAngle2Start128Loading;
       static const TReal KAngle2Start128LoadingHwAcc;
       static const float KSpacingX;// Picture spacing in the grid
       static const float KSpacingY;// Picture spacing in the grid
       static const float KSpacingYTarget;
       static const float KSpacingXTarget;
       // Space between pictures in one by one
       static const float KOneByOneSpacing;
       static const TInt  KGridSizeY;
       
       static /*const*/ TInt  K512TNImageBuffer;
       static const TInt  K128TNImageBuffer;
       static /*const*/ TInt  K32TNImageBuffer;
       static /*const*/ TInt  K32TNImageUnLoadBuffer;
       
    };

#endif

// End of File
