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

#ifndef RRSENSORAPI_H
#define RRSENSORAPI_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

const TInt KMaxSensorName = 128;

// CLASS DECLARATION

/**
*  TRRSensorInfo.
*  Contains info of sensor
*
*  iSensorId identifies individual sensors
*
*  iSensorCategory can have following values:
*  0x10010FFF for sensor server internal sensors and
*  0x10010321 for external sensors.
*
*  iSensorName contains string name of sensor. This
*  can be used e.g. to show name of sensor to user.
*
*  @lib rrsensorapi.lib
*/
class TRRSensorInfo
    {
    public:
    TInt iSensorCategory;
    TInt iSensorId;
    TBuf<KMaxSensorName> iSensorName;
    };

/**
*  TRRSensorEvent
*  Data obtained from sensor 
*  --------------------------------------------------------------------------
*  E.g. to sensor server internal Accelerator sensor id: 0x10273024
*  these fields contain following information:
*  iSensorData1 = acceleration in axis X
*  iSensorData2 = acceleration in axis Y
*  iSensorData3 = acceleration in axis Z
*  --------------------------------------------------------------------------
*  Data from external sensors may vary.
*
*  @lib rrsensorapi.lib
*/
class TRRSensorEvent
    {
    public:
    TInt iSensorData1;
    TInt iSensorData2;
    TInt iSensorData3;
    };


/**
*  MRRSensorDataListener
*  Callback function for receiving sensor
*  data events
*
*  TRRSensorInfo identifies sensor that created the event.
*
*  TTRRSensorEvent contains data about created event.
*
*  @lib rrsensorapi.lib
*/
class MRRSensorDataListener
    {
    public:
        virtual void HandleDataEventL( TRRSensorInfo aSensor, 
                                       TRRSensorEvent aEvent ) = 0;
    };

/**
*  CRRSensorApi
*  User access to sensor server
*  data events
*  @lib rrsensorapi.lib
*/
class CRRSensorApi : public CBase
{
public:
    /**
    * Create new sensor access
    * @param TRRSensorInfo identifing desired sensor.
    * @return CRRSensorApi*
    */
	IMPORT_C static CRRSensorApi* NewL( TRRSensorInfo aSensor );
	
    /**
    * Retrieve list of available sensors
    * @param RArray<TRRSensorInfo>& upon completion
    *        contains list of available sensors.
    * @return void
    */
    IMPORT_C static void FindSensorsL( RArray<TRRSensorInfo>& aSensorInfoArray );
	
    /**
    * Register data listener
    * @param MRRSensorDataListener* register this pointer as sensor
    *        event listener.
    * @return void
    */
    virtual void AddDataListener( MRRSensorDataListener* aListener ) = 0;
	
    /**
    * Remove data listener
    * @param void
    * @return void
    */
    virtual void RemoveDataListener() = 0;

    /**
    * Send sensor specific command.
    * This feature is intended for future use and
    * is not currently supported.
    *
    * @param TInt& aCommand identify of command.
    *        TInt& aValue desired value for command.
    * @return TInt error code
    */
	virtual TInt SensorCommand( TInt& aCommand, TInt& aValue ) = 0;
    
};

#endif  //RRSENSORAPI_H

//  End of File
