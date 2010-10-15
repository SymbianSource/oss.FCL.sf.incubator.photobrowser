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

#ifndef __IESensorMonitor_H__
#define __IESensorMonitor_H__

// INCLUDES

#include <e32base.h>
#include <e32uid.h>
#include "ImagicConsts.h"

#ifdef _S60_3x_ACCELEROMETER_
#define SENSOR_API_LOAD_DYNAMICALLY

#include "RRSensorApi.h"
#include "IESensorDataFilter.h"
#endif

#ifdef _S60_5x_ACCELEROMETER_
    #include <sensrvproperty.h>
    #include <sensrvtypes.h> 
    #include <sensrvchannelfinder.h>
    #include <sensrvchannel.h>
    #include <sensrvgeneralproperties.h>
    //#include <sensrvmagnetometersensor.h>
    #include <sensrvaccelerometersensor.h>
    #include <sensrvorientationsensor.h>
    #include <sensrvdatalistener.h> 
//#include <sensrvtappingsensor.h> 
#endif 

//#ifdef _S60_3x_ACCELEROMETER_
#ifdef _ACCELEROMETER_SUPPORTED_
enum TImagicDeviceOrientation
	{
	EOrientationDisplayUp = 1,// Portrait Up
	EOrientationDisplayDown,// Portrait Down
	EOrientationDisplayLeftUp,// Landscape Down
	EOrientationDisplayRigthUp// Landscape Up
	};
#endif

#ifdef _ACCELEROMETER_SUPPORTED_

class MIESensorMonitorObserver
{
public:
    virtual void SensorDataAvailable(TImagicDeviceOrientation aOrientation, TBool aValue) = 0;
    virtual void SetImageRotation(TInt aIndex) = 0;
};

#ifdef _S60_3x_ACCELEROMETER_
    class CIESensorMonitor : public MRRSensorDataListener
#endif
#ifdef _S60_5x_ACCELEROMETER_
    class CIESensorMonitor : public MSensrvDataListener
#endif
    {
    public:
        static CIESensorMonitor* NewL(MIESensorMonitorObserver& aSensorObserver);
        ~CIESensorMonitor();
 
    private:         
        void ConstructL();
        CIESensorMonitor(MIESensorMonitorObserver& aSensorObserver);
    public:    
        void StartMonitoring();
        void StopMonitoring();
    protected:
        
#ifdef _S60_3x_ACCELEROMETER_
        
        void HandleDataEventL(TRRSensorInfo aSensor, TRRSensorEvent aEvent);
#endif  
#ifdef _S60_5x_ACCELEROMETER_
        
   		void DataReceived( CSensrvChannel& aChannel, TInt aCount, TInt aDataLost );
    	void DataError( CSensrvChannel& aChannel, TSensrvErrorSeverity aError );
    	void GetDataListenerInterfaceL( TUid /*aInterfaceUid*/, TAny*& /*aInterface*/ ){};
    	
#endif
    	
    private: 
        
        MIESensorMonitorObserver& iSensorObserver;
        
#ifdef _S60_3x_ACCELEROMETER_
        
		// S60 3x Code
		RArray <TRRSensorInfo> iSensorList;

#ifdef SENSOR_API_LOAD_DYNAMICALLY
        RLibrary iSensorApi;
#endif //SENSOR_API_LOAD_DYNAMICALLY
    
        CRRSensorApi* iAccelerometerSensor;
		TInt	iAccelerometerSensorIndex;
        
        TInt iAccSensorDataX;
        TInt iAccSensorDataY;
        TInt iAccSensorDataZ;
        
        CIESensorDataFilter* iSensorDataFilterX;
        CIESensorDataFilter* iSensorDataFilterY;
        CIESensorDataFilter* iSensorDataFilterZ;
#endif

            
#ifdef _S60_5x_ACCELEROMETER_

        CSensrvChannelFinder*	iSensrvChannelFinder;
        RSensrvChannelInfoList  iChannelInfoList;
        CSensrvChannel*			iSensrvSensorChannel;
    	TInt					iUpdateInterval;
    	//TUint32					iDataCount,iDataLostCount;
#endif
    };
#endif//_ACCELEROMETER_SUPPORTED_
  
#endif // __IESensorMonitor_H__

