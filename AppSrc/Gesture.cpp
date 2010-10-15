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

#include "debug.h"
#include "Gesture.h"
#include <aknnotewrappers.h>

// TODO: Add flexibility to support more gesture. e.g. circle
// TODO: Add tap range (size of allowed moving area for tap)
// TODO: Add mode-type working. Change config by one call

CGesture::CGesture(MGestureCallBack* aOwner)
    //: CTimer(CActive::EPriorityUserInput)
: CTimer(CActive::EPriorityHigh)
    , iState(EWaiting)
    , iOwner(aOwner)
    , iFirstGesture(EGestureNone)
    , iThresholdOfTap(KDefaultThresholdOfTapPixels)
    , iThresholdOfCursor(KDefaultThresholdOfCursorPixels)
    , iStationaryTime(KDefaultStationaryTime)
    , iLongTapTime(KDefaultLongTapTime)
    , iMonitoringTime(KDefaultMonitoringTime)
    , iSafetyTime(KDefaultSafetyTime)
    {
    // No implementation required
    }

CGesture::~CGesture()
    {
    Cancel(); // CTimer
    iDragPoints.Close();
    iDragTicks.Close();
    }

CGesture* CGesture::NewLC(MGestureCallBack* aOwner)
    {
    CGesture* self = new (ELeave) CGesture(aOwner);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CGesture* CGesture::NewL(MGestureCallBack* aOwner)
    {
    CGesture* self = CGesture::NewLC(aOwner);
    CleanupStack::Pop(); // self;
    return self;
    }

void CGesture::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }

EXPORT_C void CGesture::PointerEventL( const TPointerEvent& aEvent )
    {
#define RESETPOINTS()        {iDragPoints.Reset(); iDragTicks.Reset();}
#define RECORDPOINTS(p, t)   {iDragPoints.Append(p); iDragTicks.Append(t);}
#define SETMOVEMENT(d, c, p) {d.iX=c.iX-p.iX; d.iY=c.iY-p.iY;}

    TInt tick = User::NTickCount();
    TGestureType type = EGestureNone;
    TPoint delta;
    TPoint vector;
    TInt threshold;

    switch( aEvent.iType )
        {
        case TPointerEvent::EButton1Down:
            DP2_IMAGIC(_L("CGesture::PointerEventL: TPointerEvent::EButton1Down (%d, %d)"), aEvent.iPosition.iX, aEvent.iPosition.iY);
            
            if (iState == EEnded) break; // do nothing if it's in safety time
            
            // Start timer to monitor long tap
            Cancel(); // CTimer. Stop all timers.
            DP1_IMAGIC(_L("iStationaryTime=%d"), iStationaryTime);
            After( TTimeIntervalMicroSeconds32(iStationaryTime));

            // Initialise pointer records and start new record
            iPointBegan = iPointLastCursor = iPointPreviousEvent = aEvent.iPosition;
            RESETPOINTS();
            RECORDPOINTS(aEvent.iPosition, tick);

            if (iState == EMonitoring)
                {
                DP0_IMAGIC(_L("CGesture::PointerEventL (2nd gesture starting)"));
                // store 1st gesture type in high 16 bits if this is 2nd gesture
                iFirstGesture = iFirstGesture << 16;
                }
            else // iState should be Waiting. Resets anyway even if it's not. 
                {
                DP0_IMAGIC(_L("CGesture::PointerEventL (1st gesture starting)"));
                // Set point only for 1st gesture and notify owner
                iFirstGesture    = EGestureNone;
                iPointFirstBegan = aEvent.iPosition;
                iOwner->HandleGestureBeganL(aEvent.iPosition);
                }

            iState = EBegan;

            DP1_IMAGIC(_L("####### Down:iFirstGesture=%x"), iFirstGesture);
            break;

        case TPointerEvent::EDrag:
            DP2_IMAGIC(_L("CGesture::PointerEventL: TPointerEvent::EDrag (%d, %d)"), aEvent.iPosition.iX, aEvent.iPosition.iY);

            if ((iState != EBegan) && (iState != EStationary) && (iState != ETravelling))
                break; // do nothing if it's not in the state above
            
            // record drag points and thier tick counts
            RECORDPOINTS(aEvent.iPosition, tick);

            SETMOVEMENT(delta, aEvent.iPosition, iPointPreviousEvent);

            threshold = (iState == EBegan)? iThresholdOfTap * 3: iThresholdOfTap;
            
            if (IsMovementWithinThreshold(delta, threshold))
                {
                DP0_IMAGIC(_L("CGesture::PointerEventL (staying within threshold)"));
                }
            else
                {
                DP0_IMAGIC(_L("CGesture::PointerEventL (going beyond threshold)"));
                Cancel(); // CTimer. Stop all timers.
                iState = ETravelling;
                }

            if (iState == ETravelling)
                {
                DP0_IMAGIC(_L("CGesture::PointerEventL (notify drag event)"));
                iOwner->HandleGestureMovedL(delta, EGestureDrag);
                iPointPreviousEvent = aEvent.iPosition;
                }

#ifdef CURSOR_SIMULATION
            type = CheckMovement(iPointLastCursor, aEvent.iPosition);

            if (type != EGestureNone)
                {
                iOwner->HandleGestureMovedL(aEvent.iPosition, EGestureCursor|type);
                iPointLastCursor = aEvent.iPosition;
                }
            DP5_IMAGIC(_L("iPointLastCursor(%d)"), type);
#endif
            break;

        case TPointerEvent::EButton1Up:
            DP2_IMAGIC(_L("CGesture::PointerEventL: TPointerEvent::EButton1Up (%d, %d)"), aEvent.iPosition.iX, aEvent.iPosition.iY);

            if ((iState != EBegan) && (iState != EStationary) && (iState != ETravelling))
                break; // do nothing if it's not in the state above
            
            Cancel(); // Stop timers
            
            iPointPreviousEvent = aEvent.iPosition;

            // record drag points and thier tick counts
            RECORDPOINTS(aEvent.iPosition, tick);

            if ((iState == EBegan) ||
                ((iState == EStationary) && !IS_GESTURE_LONGTAPPING(iFirstGesture)))
                {
                DP0_IMAGIC(_L("CGesture::PointerEventL (Tap!)"));
                type = EGestureTap;
                
                // TODO: check distance of each tap if it's double tap
                iFirstGesture |= type;

                TInt t = (IS_GESTURE_TAPPED(iFirstGesture))? 0: iMonitoringTime;
                After(TTimeIntervalMicroSeconds32(t)); // call immediately if double tap
                iState = EMonitoring;
                }
            else if ((iState == EStationary) && IS_GESTURE_LONGTAPPING(iFirstGesture))
                {
                DP0_IMAGIC(_L("CGesture::PointerEventL (Long tap!)"));
                type = EGestureLongTap;

                iFirstGesture |= type;
                After(TTimeIntervalMicroSeconds32(iMonitoringTime));
                iState = EMonitoring;
                }
            else if (iState == ETravelling)
                {
                DP0_IMAGIC(_L("CGesture::PointerEventL (Was drag. Flick!)"));
                type = CheckFlick(KDefaultValidityTimeOfFlick, vector);
                iPointPreviousEvent = vector;

                iFirstGesture |= type;
                After(TTimeIntervalMicroSeconds32(0)); // call immediately 
                iState = EMonitoring;
                }

            DP1_IMAGIC(_L("####### Up:iFirstGesture=%x"), iFirstGesture);
            break;

        default:
            break;
        }
    }

EXPORT_C void CGesture::SetThresholdOfTap( const TInt aPixels)
    {
    iThresholdOfTap = aPixels;
    }

EXPORT_C void CGesture::SetThresholdOfCursor( const TInt aPixels)
    {
    iThresholdOfCursor = aPixels;
    }

EXPORT_C void CGesture::SetStationaryTime(const TInt aMicroseconds)
    {
    iStationaryTime = aMicroseconds;
    }

EXPORT_C void CGesture::SetLongTapTime(const TInt aMicroSeconds)
    {
    iLongTapTime = aMicroSeconds;
    }

EXPORT_C void CGesture::SetMonitoringTime(const TInt aMicroseconds)
    {
    iMonitoringTime = aMicroseconds;
    }

EXPORT_C void CGesture::SetSafetyTime(const TInt aMicroseconds)
    {
    iSafetyTime = aMicroseconds;
    }

TBool CGesture::IsMovementWithinThreshold(const TPoint aDelta, const TInt aThreshold)
    {
    TBool ret = ETrue;

    TInt diff_x      = aDelta.iX;
    TInt diff_y      = aDelta.iY;
    TInt abs_diff_2  = diff_x * diff_x + diff_y * diff_y;
    TInt threshold_2 = aThreshold * aThreshold;

    if (abs_diff_2 > threshold_2) ret = EFalse;
    
    return ret;
    }

TGestureType CGesture::CheckMovement(const TPoint aPointPrevious, const TPoint aPointCurrent, const TBool aSkipThresholdCheck)
    {
    TGestureType ret = EGestureNone;

    TInt diff_x  = aPointCurrent.iX - aPointPrevious.iX;
    TInt diff_y  = aPointCurrent.iY - aPointPrevious.iY;

    TInt abs_diff_x = Abs(diff_x);
    TInt abs_diff_y = Abs(diff_y);
    TInt abs_diff_2 = abs_diff_x*abs_diff_x + abs_diff_y*abs_diff_y;  

    if (abs_diff_2 > iThresholdOfCursor*iThresholdOfCursor)
        {
        // Movement is mapped to one of 8 directions
        TBool valid_x = (abs_diff_x && (abs_diff_x * 5 > abs_diff_y * 4))? ETrue: EFalse;
        TBool valid_y = (abs_diff_y && (abs_diff_y * 5 > abs_diff_x * 4))? ETrue: EFalse;

        if (valid_y)
            {
            if (diff_y < 0) ret |= EGestureUp;
            else            ret |= EGestureDown;
            }
        if (valid_x)
            {
            if (diff_x < 0) ret |= EGestureLeft;
            else            ret |= EGestureRight;
            }
        }
    
    return ret;
    }

TGestureType CGesture::CheckFlick(const TInt aValidityTime, TPoint& aVector)
    {
    DP0_IMAGIC(_L("CGesture::CheckFlick++"));

    // TODO: need to check if counts are same in iDragPoints and iDragTicks
    TInt first, last;
    TInt validtick = aValidityTime / 1000; // convert micro sec to milli sec.
    TGestureType ret = EGestureNone;

    first = last = iDragPoints.Count() - 1;
    aVector.iX = aVector.iY = 0;
    
    for (TInt i=last-1;i>=0;--i)
        {
        TInt tickdiff = iDragTicks[last] - iDragTicks[i];

        DP5_IMAGIC(_L("i=%d, tick=%d, tickdiff=%d (x,y)=(%d,%d)"),
                i, iDragTicks[i], tickdiff, iDragPoints[i].iX, iDragPoints[i].iY);

        if (tickdiff < validtick)
            {
            first = i;
            }
        else
            {
            break;
            }
        }

    if (first != last)
        {
        TInt tickdiff = iDragTicks[last] - iDragTicks[first];
        ret = CheckMovement(iDragPoints[first], iDragPoints[last], ETrue);

        // Tick diff is 100 at minimum to avoid returning extreamly big vector.
        if (tickdiff < 100) tickdiff = 100;

        // Calculate the movement speed = pixels/sec
        aVector.iX = (iDragPoints[last].iX - iDragPoints[first].iX)*1000/tickdiff;
        aVector.iY = (iDragPoints[last].iY - iDragPoints[first].iY)*1000/tickdiff;
        }

    DP5_IMAGIC(_L("%d~%d, %d=>%d =%d"),
            first, last, iDragPoints[first].iX, iDragPoints[last].iX,
            CheckMovement(iDragPoints[first], iDragPoints[last]));
    DP2_IMAGIC(_L("vector (%d,%d)"), aVector.iX, aVector.iY);
    DP0_IMAGIC(_L("CGesture::CheckFlick--"));

    return ret;
    }

void CGesture::RunL()
    {
    DP0_IMAGIC(_L("CGesture::RunL++"));

    switch (iState)
        {
        case EBegan:
            DP1_IMAGIC(_L("CGesture::RunL (EBegan -> EStationary) iLongTapTime=%d"), iLongTapTime);
            iOwner->HandleGestureMovedL(iPointPreviousEvent, EGestureStationary);
            After(TTimeIntervalMicroSeconds32(iLongTapTime));
            iState = EStationary;

            // Update the position so that movement check occurs against 
            // last drag event, not against initial touch position
            if (iDragPoints.Count() > 1)
                iPointPreviousEvent = iDragPoints[iDragPoints.Count()-1];
            break;
        case EStationary:
            DP0_IMAGIC(_L("CGesture::RunL (EStationary -> EStationary)"));
            iOwner->HandleGestureMovedL(iPointPreviousEvent, EGestureLongTapping);
            // record it's possible long tap if movement stays within threshold
            iFirstGesture |= EGestureLongTapping;
            // no state change
            break;
        case EMonitoring:
            DP1_IMAGIC(_L("CGesture::RunL (EMonitoring -> EEnded) iSafetyTime=%d"), iSafetyTime);
            iOwner->HandleGestureEndedL(iPointPreviousEvent, iFirstGesture);
            After(TTimeIntervalMicroSeconds32(iSafetyTime));
            iState = EEnded;
            break;
        case EEnded:
            DP0_IMAGIC(_L("CGesture::RunL (EEnded -> EWaiting)"));
            iState = EWaiting;
            break;
        default:
            DP0_IMAGIC(_L("CGesture::RunL (default)"));
            // do nothing
            break;
        }

    DP0_IMAGIC(_L("CGesture::RunL--"));
    }
