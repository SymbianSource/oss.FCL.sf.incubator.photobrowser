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

#include "IESensorMonitor.h"
#include "debug.h"

#ifdef _ACCELEROMETER_SUPPORTED_

#ifdef _S60_3x_ACCELEROMETER_
const TInt KAccelerometerSensorUID = 0x10273024;
#endif

CIESensorMonitor* CIESensorMonitor::NewL(MIESensorMonitorObserver& aSensorObserver)
{
    DP0_IMAGIC(_L("CIESensorMonitor::NewL++"));
    CIESensorMonitor* self=new (ELeave) CIESensorMonitor(aSensorObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    
    DP0_IMAGIC(_L("CIESensorMonitor::NewL--"));
    return self;
}


void CIESensorMonitor::ConstructL()
{
    DP0_IMAGIC(_L("CIESensorMonitor::ConstructL++"));

#ifdef _S60_3x_ACCELEROMETER_
    // Noise filter for the accelerometer;
    iAccSensorDataX = 0;
    iAccSensorDataY = 0;
    iAccSensorDataZ = 0;

#ifdef SENSOR_API_LOAD_DYNAMICALLY
    _LIT( KSensorApiDll, "RRSensorApi" );
    TUidType dllUid( KDynamicLibraryUid );
    TInt error = iSensorApi.Load( KSensorApiDll, dllUid );
    User::LeaveIfError(error);
#endif    
    
    iSensorDataFilterX = CIESensorDataFilter::NewL();
    iSensorDataFilterY = CIESensorDataFilter::NewL();
    iSensorDataFilterZ = CIESensorDataFilter::NewL();
    
#ifdef SENSOR_API_LOAD_DYNAMICALLY
    // If Sensor API library is dynamically linked
    typedef void ( *TFindSensorsLFunction )( RArray<TRRSensorInfo>& ); 
    TFindSensorsLFunction findSensorsLFunction = ( TFindSensorsLFunction )iSensorApi.Lookup( 1 );
    findSensorsLFunction( iSensorList );
#else
    TRAPD( error , CRRSensorApi::FindSensorsL(iSensorList));
    if (error)
    {
    // Error found in sensors  
    }
#endif    
    
    TInt sensorCount = iSensorList.Count();
    
    for (TInt i = 0; i < sensorCount; i++ )
    {
        if (iSensorList[i].iSensorId == KAccelerometerSensorUID)
        {
            iAccelerometerSensorIndex = i;       
            break;
        }
    }
#endif _S60_3x_ACCELEROMETER_
#ifdef _S60_5x_ACCELEROMETER_
    
    DP0_IMAGIC(_L("CIESensorMonitor::ConstructL - create CSensrvChannelFinder"));
    iSensrvChannelFinder = CSensrvChannelFinder::NewL();
    DP1_IMAGIC(_L("CIESensorMonitor::ConstructL - CSensrvChannelFinder created: %d"),iSensrvChannelFinder);
        
    iChannelInfoList.Reset();
    TSensrvChannelInfo mySearchConditions; // none, so matches all.
    DP0_IMAGIC(_L("CIESensorMonitor::ConstructL - iSensrvChannelFinder->FindChannelsL"));
    TRAPD(err, iSensrvChannelFinder->FindChannelsL(iChannelInfoList, mySearchConditions));
    if(err != KErrNone)
        {
        DP1_IMAGIC(_L("CIESensorMonitor::ConstructL - iSensrvChannelFinder->FindChannelsL ERROR: %d"), err);
        User::Leave(err);
        }
    DP0_IMAGIC(_L("CIESensorMonitor::ConstructL - iSensrvChannelFinder->FindChannelsL - OK"));
    
    TInt senIndex(0); // Sensor Selection 
    
    TBuf<256> text;
    text.Append(_L(" ----------------------------FOUND SENSOR=" ));
    text.AppendNum(iChannelInfoList.Count());
    DP0_IMAGIC(text);
    
    if(senIndex >= 0 && senIndex < iChannelInfoList.Count())
    {
        DP0_IMAGIC(_L("CIESensorMonitor::ConstructL++"));
        iSensrvSensorChannel = CSensrvChannel::NewL( iChannelInfoList[senIndex] );
        iSensrvSensorChannel->OpenChannelL();
    }
    
#endif //_S60_5x_ACCELEROMETER_
    
    DP0_IMAGIC(_L("CIESensorMonitor::ConstructL++"));
}


CIESensorMonitor::CIESensorMonitor(MIESensorMonitorObserver& aSensorObserver)
    :iSensorObserver(aSensorObserver)
    {
    }


CIESensorMonitor::~CIESensorMonitor()
{
    DP0_IMAGIC(_L("CIESensorMonitor::~CIESensorMonitor"));
    
    StopMonitoring();
    
#ifdef _S60_3x_ACCELEROMETER_ 

#ifdef SENSOR_API_LOAD_DYNAMICALLY
    // Close dynamically loaded library
    iSensorApi.Close();
#endif //SENSOR_API_LOAD_DYNAMICALLY    
    
    delete iAccelerometerSensor;
    iAccelerometerSensor = NULL;
#endif
#ifdef _S60_5x_ACCELEROMETER_ 

    if(iSensrvSensorChannel)
	    iSensrvSensorChannel->CloseChannel();
    
    delete iSensrvSensorChannel;
    
    iChannelInfoList.Reset();
    delete iSensrvChannelFinder;
    
#endif
}

void CIESensorMonitor::StartMonitoring()
{
    DP0_IMAGIC(_L("CIESensorMonitor::StartMonitoring+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"));

#ifdef _S60_3x_ACCELEROMETER_
    
#ifdef SENSOR_API_LOAD_DYNAMICALLY
    // If Sensor API library is dynamically linked
    typedef CRRSensorApi* ( *TNewLFunction )( TRRSensorInfo ); 
    TNewLFunction newLFunction = ( TNewLFunction )iSensorApi.Lookup( 2 );
    iAccelerometerSensor = newLFunction( iSensorList[iAccelerometerSensorIndex] );
#else    
    iAccelerometerSensor = CRRSensorApi::NewL(iSensorList[iAccelerometerSensorIndex]);
#endif

    if (iAccelerometerSensor)
        iAccelerometerSensor->AddDataListener(this);
#endif
#ifdef _S60_5x_ACCELEROMETER_
    if(iSensrvSensorChannel)
        iSensrvSensorChannel->StartDataListeningL( this, 1,1,iUpdateInterval);         
#endif
}

void CIESensorMonitor::StopMonitoring()
{
    DP0_IMAGIC(_L("CSensorMonitor::StopMonitoring++"));
    
#ifdef _S60_3x_ACCELEROMETER_
    if(iAccelerometerSensor)
        iAccelerometerSensor->RemoveDataListener();
#endif
#ifdef _S60_5x_ACCELEROMETER_
    if(iSensrvSensorChannel)
        iSensrvSensorChannel->StopDataListening();
    
#endif
}

#ifdef _S60_3x_ACCELEROMETER_

void CIESensorMonitor::HandleDataEventL(TRRSensorInfo aSensor, TRRSensorEvent aEvent)
    {
    TImagicDeviceOrientation deviceOrientation;
     // Axis Data
    switch (aSensor.iSensorId)
        {
        case KAccelerometerSensorUID:
            {
            iAccSensorDataX = iSensorDataFilterX->FilterSensorData(aEvent.iSensorData1); // X 
            iAccSensorDataY = iSensorDataFilterY->FilterSensorData(aEvent.iSensorData2); // Y
            iAccSensorDataZ = iSensorDataFilterZ->FilterSensorData(aEvent.iSensorData3); // Z
            
            TInt x = Abs(iAccSensorDataX);
            TInt y = Abs(iAccSensorDataY);
            TInt z = Abs(iAccSensorDataZ);
            
            // Calculate the orientation of the screen
            if (x>z && x>z) // Landscape
                {
                if (iAccSensorDataX > 0)
                    deviceOrientation = EOrientationDisplayRigthUp;
                else
                    deviceOrientation = EOrientationDisplayLeftUp;
                }
            if (y>x && y>z) // Portrait Mode
                {
                if (iAccSensorDataY > 0)
                    deviceOrientation = EOrientationDisplayUp;
                else
                    deviceOrientation = EOrientationDisplayDown;
                
                }
            //if (z>x && z>y)  
            //    {
            //    if (iAccSensorDataZ)
                  //Not used  deviceOrientation = EOrientationDisplayDownwards;
            //    else
                  //Not used  deviceOrientation = EOrientationDisplayUpwards;
            //    } 
            
            iSensorObserver.SensorDataAvailable(deviceOrientation, EFalse);
            
            }
            break;
        default:
            break;
        }
    }
#endif
#ifdef _S60_5x_ACCELEROMETER_

 _LIT( KTimeString, "%-B%:0%J%:1%T%:2%S%.%*C4%:3%+B" );
 
void CIESensorMonitor::DataReceived( CSensrvChannel& aChannel, TInt aCount, TInt aDataLost )
{
    DP0_IMAGIC(_L("CSensorMonitor::DataReceived"));
    
    TBuf<250> progressBuf;
	
	TInt errErr(KErrNone);
	
	//iDataLostCount = iDataLostCount + aDataLost;
	//iDataCount = iDataCount + aCount;

	if( aChannel.GetChannelInfo().iChannelType == KSensrvChannelTypeIdOrientationData )
	{
		TSensrvOrientationData data;
		
		//TRAP(errErr,
		for( TInt i = 0; i < aCount; i++ )
	    {
	    	TPckgBuf<TSensrvOrientationData> dataBuf;
	    	aChannel.GetData( dataBuf );
	    	data = dataBuf();
	    	data.iTimeStamp.FormatL(progressBuf, KTimeString );
	    }

	    if(errErr != KErrNone)
	    {
	    	progressBuf.Zero();
	    }
	
    	switch ( data.iDeviceOrientation )
        {
        case EOrientationDisplayUp:
            {
            progressBuf.Append( _L( "Display up" ) );
            DP1_IMAGIC( _L( "Display up: %d" ),data.iDeviceOrientation );
            break;
            }
        case EOrientationDisplayDown:
            {
            progressBuf.Append( _L( "Display down" ) );
            DP1_IMAGIC( _L( "Display down: %d" ),data.iDeviceOrientation );
            break;
            }
        case EOrientationDisplayLeftUp:
            {
            progressBuf.Append( _L( "Display left up" ) );
            DP1_IMAGIC( _L( "Display left up: %d" ),data.iDeviceOrientation );
            break;
            }
        case EOrientationDisplayRigthUp:
            {
            progressBuf.Append( _L( "Display right up" ) );
            DP1_IMAGIC( _L( "Display right up: %d" ),data.iDeviceOrientation );
            break;
            }
        default:
            {
            progressBuf.Append( _L( "Unknown orientation" ) );
            DP1_IMAGIC( _L( "Unknown orientation: %d" ),data.iDeviceOrientation );
            break;
            }
        }
    	iSensorObserver.SensorDataAvailable(TImagicDeviceOrientation(data.iDeviceOrientation), EFalse);
	}
	else
	{
		progressBuf.Copy(_L("Channel = " ));
	    progressBuf.AppendNum(aChannel.GetChannelInfo().iChannelType,EHex);
	}
	
	DP0_IMAGIC(progressBuf);
	
}

void CIESensorMonitor::DataError( CSensrvChannel& /*aChannel*/, TSensrvErrorSeverity /*aError*/)
{
    DP0_IMAGIC(_L("CIESensorMonitor::DataReceived"));
}

#endif //_S60_5x_ACCELEROMETER_

#endif //_ACCELEROMETER_SUPPORTED_
// End of File

