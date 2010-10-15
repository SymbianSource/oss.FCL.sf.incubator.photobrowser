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

#ifndef GESTURE_H
#define GESTURE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <w32std.h>

#undef CURSOR_SIMULATION

const TInt KDefaultThresholdOfTapPixels      = 25;
const TInt KDefaultThresholdOfCursorPixels   = 0;      // Any movement is cursor movement 
const TInt KDefaultStationaryTime            = 100000; // 100ms
const TInt KDefaultLongTapTime               = 400000; // 300ms = 200ms + stationary (100ms)  
const TInt KDefaultMonitoringTime            = 200000; // 200ms
const TInt KDefaultSafetyTime                = 200000; // 200ms
const TInt KDefaultValidityTimeOfFlick       = 300000; // recent 300ms drag to be checked 

enum {
    EGestureNone        = 0x0000,
    EGestureStationary  = 0x0001,  // Send on moved event when user hasn't moved finger
    EGestureDrag        = 0x0002,
    EGestureTap         = 0x0004,
    EGestureLongTapping = 0x0008,
    EGestureLongTap     = 0x0010,
    EGestureCursor      = 0x0100,
    // sub-type for cursor simuation
    EGestureUp          = 0x1000,
    EGestureDown        = 0x2000,
    EGestureLeft        = 0x4000,
    EGestureRight       = 0x8000,
    // 1st gesture type is stored in high 16 bits
    EGestureDragged     = EGestureDrag    << 16,
    EGestureTapped      = EGestureTap     << 16,
    EGestureLongTapped  = EGestureLongTap << 16
};
// Gestures can be combination
typedef TUint32 TGestureType;

#define IS_GESTURE_NONE(t)             ((t) == (EGestureNone))
#define IS_GESTURE_DRAG(t)             ((t) & (EGestureDrag))
#define IS_GESTURE_STATIONARY(t)       ((t) & (EGestureStationary))
#define IS_GESTURE_CURSORSIMULATION(t) ((t) & ((EGestureUp | EGestureDown | EGestureLeft | EGestureRight)))
#define IS_GESTURE_TAP(t)              ((t) & (EGestureTap))
#define IS_GESTURE_TAPPED(t)           ((t) & (EGestureTapped))
#define IS_GESTURE_LONGTAPPING(t)      ((t) & (EGestureLongTapping))
#define IS_GESTURE_LONGTAP(t)          ((t) & (EGestureLongTap))
#define IS_GESTURE_DRAGGED(t)          ((t) & (EGestureDragged))
#define IS_GESTURE_SINGLETAP(t)        (IS_GESTURE_TAP(t) && !IS_GESTURE_TAPPED(t))
#define IS_GESTURE_DOUBLETAP(t)        (IS_GESTURE_TAP(t) && IS_GESTURE_TAPPED(t))
#define IS_GESTURE_TAPnDRAG(t)         (IS_GESTURE_CURSORSIMULATION(t) && IS_GESTURE_TAPPED(t))

enum {
    EGestureModeNone                  = 0x00,
    EGestureModeCursorEmulation       = 0x01, // gives cursor movements when beyond threshold
    EGestureModeSingleCursorEmulation = 0x02, // gives singles cursor movement on each touch
    EGestureModeDrag                  = 0x04, // 
    EGestureModeTap                   = 0x08,
    EGestureModeTapAndDrag            = 0x10,
    EGestureModeDrawCircle            = 0x20
};

// CLASS DECLARATION

class MGestureCallBack
    {
public:
    /**
    * Callback method. Get's called when touch gesture started (when user touches).
    * @param aPos  Point where touch happens
    * @param aType Gesture type bit flag (move_right/left/up/down or tap)
    */
    
    // Called when user touches screen
    virtual void HandleGestureBeganL(const TPoint&      aPos ) = 0;

    // Called when user is dragging
    // Drag event comes with movement delta from previous event (EGestureDrag)
    // Drag event comes with touched position (EGestureUp/Down/Left/Right)
    //   when it exceeds defined threshold
    virtual void HandleGestureMovedL(const TPoint&      aPos,
                                     const TGestureType aType) = 0;

    // Called when user releases screen
    // Movement within defined last moment is given in aPos. (pixels/250ms by default)
    virtual void HandleGestureEndedL(const TPoint&      aPos,
                                     const TGestureType aType) = 0;
    };

/**
 *  CGesture
 * 
 */
class CGesture : public CTimer
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CGesture();

    /**
     * Two-phased constructor.
     */
    static CGesture* NewL(MGestureCallBack* aOwner);

    /**
     * Two-phased constructor.
     */
    static CGesture* NewLC(MGestureCallBack* aOwner);

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CGesture(MGestureCallBack* aOwner);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

public: // New functions

    IMPORT_C void PointerEventL(const TPointerEvent& aEvent);
    IMPORT_C void SetThresholdOfTap(const TInt aPixels);
    IMPORT_C void SetThresholdOfCursor(const TInt aPixels);
    IMPORT_C void SetStationaryTime(const TInt aMicroseconds);
    IMPORT_C void SetLongTapTime(const TInt aMicroSeconds);
    IMPORT_C void SetMonitoringTime(const TInt aMicroseconds);
    IMPORT_C void SetSafetyTime(const TInt aMicroseconds);

private:

    enum TGestureState
        {
        EWaiting,          // Waiting for first touch
        EBegan,            // Touch down. Gesture started (or second gesture)
        EStationary,       // Gesture is stationary
        ETravelling,       // Dragging beyond threshold
        EMonitoring,       // Monitoring second gesture
        EEnded             // Touch operation ended. Wait to avoid mis touch
        };

    /**
     * Just check if the movement exceeds thoreshold
     */
    TBool IsMovementWithinThreshold(const TPoint aDelta, const TInt aThreshold);

    /**
     * Map touch movement to 8-directions if volume of movement exceeds threshold
     * Also check if movement is faster than threshold
     */
    TGestureType CheckMovement(const TPoint aPointPrevious, const TPoint aPointCurrent, const TBool aSkipThresholdCheck=EFalse);

    /**
     * Maps touch movements in last defines time, aValidityTime, to 8-directions
     * Movements are recorded in iDragPoints and iDragTicks (RArray of TPoint and TInt)
     */
    TGestureType CheckFlick(const TInt aValidityTime, TPoint& aVector);

    /**
     * Active object callback. Used for timer
     */
    void RunL();

private: // Data

    /**
     * Current state of Gesture
     */
    TGestureState iState;
    
    /**
     * Pointer to owner
     */
    MGestureCallBack* iOwner;

    /**
     * Pointer event received from owner.
     */
    TPointerEvent iPointerEvent;

    /**
     * Point records
     */
    TPoint iPointBegan;         // point where user touched down
    TPoint iPointFirstBegan;    // point where user touched down in 1st gesture
    TPoint iPointLastCursor;    // point where last cursor simulation event occured
    TPoint iPointPreviousEvent; // last point where avkon informed on drag

    /**
     * Remembers gesture in 1st gesture
     */
    TGestureType iFirstGesture;

    /**
     * Thresholds in Pixels
     */
    TInt iThresholdOfTap;       // movement stays within this pixels
    TInt iThresholdOfCursor;    // cursor simulation occurs when exceeding threshold
    
    /**
     * Thresholds in Micro seconds
     */
    TInt iStationaryTime;       // UP should occur within the time for Tap 
    TInt iLongTapTime;          // for long tap
    TInt iMonitoringTime;       // for 2nd gesture. Notify gesture end if expires
    TInt iSafetyTime;           // Ignores gesture after Ended to avoid mis-touch
     
    /**
     * Drag points and the tick time for flick
     */
    RArray<TPoint> iDragPoints;
    RArray<TInt>   iDragTicks;
    };

#endif // GESTURE_H
