/////////////////////////////////////////////////////////////////////////////
// Copyright � 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Stream.h"

// FORMAT OF "ushcn-v2.5-stations.txt"
// 
// ----------------------------------------
// Variable                 Columns    Type
// ----------------------------------------
// COUNTRY CODE             1 - 2      Char
// NETWORK CODE             3          Char
// ID PLACEHOLDERS( "00" )  4 - 5      Char
// COOP ID                  6 - 11     Char
// LATITUDE                13 - 20     Real
// LONGITUDE               22 - 30     Real
// ELEVATION               33 - 37     Real
// STATE                   39 - 40     Char
// NAME                    42 - 71     Char
// COMPONENT 1 ( COOP ID ) 73 - 78     Char
// COMPONENT 2 ( COOP ID ) 80 - 85     Char
// COMPONENT 3 ( COOP ID ) 87 - 92     Char
// UTC OFFSET              94 - 95     Int

// a class representing a single climate station
class CClimateStation : public CStream
{
// public definitions
public:

// protected data
protected:
	// single line of stations text file
	CString m_csSource;

	// unique identifier for this stream
	CString m_csGUID;

	// station name (columns 1 - 11 of source)
	CString m_csStation;

	// latitude in degrees
	float m_fLatitude;

	// longitude in degrees
	float m_fLongitude;

	// elevation in meters (-999.9 is missing)
	float m_fElevation;

	// two letter state code
	CString m_csState;

	// station location name
	CString m_csLocation;

	// COMPONENT 1 is the Coop Id for the first station( in chronological order ) 
	// whose records were joined with those of the HCN site to form a longer time
	// series.  "------" indicates "not applicable".
	CString m_csComponent1;

	// COMPONENT 2 is the Coop Id for the second station( if applicable ) 
	// whose records were joined with those of the HCN site to form a longer 
	// time series.
	CString m_csComponent2;

	// COMPONENT 3 is the Coop Id for the third station( if applicable ) 
	// whose records were joined with those of the HCN site to form a longer 
	// time series.
	CString m_csComponent3;

	// UTC OFFSET is the time difference between Coordinated Universal Time( UTC )
	// and local standard time at the station( i.e., the number of hours that
	// must be added to local standard time to match UTC ).
	short m_sOffsetUTC;

// public properties
public:
	// single line of stations text file
	inline CString GetSource()
	{
		// single line of source from the stations text file
		const CString value = m_csSource;

		return value;
	}
	// single line of stations text file
	inline void SetSource( CString value )
	{
		// persist the value
		m_csSource = value;

		// parse all of the properties
		const CString csStation = Station;
		const float fLatitude = Latitude;
		const float fLongitude = Longitude;
		const float fElevation = Elevation;
		const CString csState = State;
		const CString csLocation = Location;
		const CString csComp1 = Component1;
		const CString csComp2 = Component2;
		const CString csComp3 = Component3;
		const short sOffsetUTC = OffsetUTC;
	}
	// single line of stations text file
	__declspec( property( get = GetSource, put = SetSource ) )
		CString Source;

	// unique identifier for this stream
	inline CString GetGUID()
	{
		return m_csGUID;
	}
	// unique identifier for this stream
	inline void SetGUID( CString value )
	{
		// persist the value
		m_csGUID = value;
	}
	// unique identifier for this stream
	__declspec( property( get = GetGUID, put = SetGUID ) )
		CString GUID;

	// station name (columns 1 - 11 of source)
	inline CString GetStation()
	{
		CString value = m_csStation;

		if ( !m_csSource.IsEmpty() )
		{
			// 10 characters starting at zero
			value = m_csSource.Mid( 0, 10 );
		}
			
		// persist the value
		Station = value;

		return value;
	}
	// station name (columns 1 - 11 of source)
	inline void SetStation( CString value )
	{
		m_csStation = value;
	}
	// station name (columns 1 - 11 of source)
	__declspec( property( get = GetStation, put = SetStation ) )
		CString Station;

	// latitude in degrees
	inline float GetLatitude()
	{
		// return value
		float value = m_fLatitude;

		if ( !m_csSource.IsEmpty() )
		{
			// 8 characters starting at 12
			const CString csValue = m_csSource.Mid( 12, 8 );

			// return value
			value = (float)_tstof( csValue );
		}

		// persist the value
		Latitude = value;

		return value;
	}
	// latitude in degrees
	inline void SetLatitude( float value )
	{
		m_fLatitude = value;
	}
	// latitude in degrees
	__declspec( property( get = GetLatitude, put = SetLatitude ) )
		float Latitude;
	
	// longitude in degrees
	inline float GetLongitude()
	{
		// return value
		float value = m_fLongitude;

		if ( !m_csSource.IsEmpty() )
		{
			// 9 characters starting at 21
			const CString csValue = m_csSource.Mid( 21, 9 );

			// return value
			value = (float)_tstof( csValue );
		}

		// persist the value
		Longitude = value;

		return value;
	}
	// longitude in degrees
	inline void SetLongitude( float value )
	{
		m_fLongitude = value;
	}
	// longitude in degrees
	__declspec( property( get = GetLongitude, put = SetLongitude ) )
		float Longitude;
	
	// elevation in degrees
	inline float GetElevation()
	{
		// return value
		float value = m_fElevation;

		if ( !m_csSource.IsEmpty() )
		{
			// 5 characters starting at 32
			const CString csValue = m_csSource.Mid( 32, 5 );

			// return value
			value = (float)_tstof( csValue );
		}

		// persist the value
		Elevation = value;

		return value;
	}
	// elevation in degrees
	inline void SetElevation( float value )
	{
		m_fElevation = value;
	}
	// elevation in degrees
	__declspec( property( get = GetElevation, put = SetElevation ) )
		float Elevation;
	
	// two letter state code
	inline CString GetState()
	{
		CString value = m_csState;

		if ( !m_csSource.IsEmpty() )
		{
			// 2 characters starting at 38
			value = m_csSource.Mid( 38, 2 );
		}
		// persist the value
		State = value;

		return value;
	}
	// two letter state code
	inline void SetState( CString value )
	{
		m_csState = value;
	}
	// two letter state code
	__declspec( property( get = GetState, put = SetState ) )
		CString State;

	// name of the station location
	inline CString GetLocation()
	{
		CString value = m_csLocation;

		if ( !m_csSource.IsEmpty() )
		{
			// 30 characters starting at 41
			value = m_csSource.Mid( 41, 30 );
		}

		// persist the value
		Location = value;

		return value;
	}
	// name of the station location
	inline void SetLocation( CString value )
	{
		m_csLocation = value;
	}
	// name of the station location
	__declspec( property( get = GetLocation, put = SetLocation ) )
		CString Location;

	// COMPONENT 1 is the Coop Id for the first station( in chronological order ) 
	// whose records were joined with those of the HCN site to form a longer time
	// series.  "------" indicates "not applicable".
	inline CString GetComponent1()
	{
		CString value = m_csComponent1;

		if ( !m_csSource.IsEmpty() )
		{
			// 6 characters starting at 72
			value = m_csSource.Mid( 72, 6 );
		}

		// persist the value
		Component1 = value;

		return value;
	}
	// COMPONENT 1 is the Coop Id for the first station( in chronological order ) 
	// whose records were joined with those of the HCN site to form a longer time
	// series.  "------" indicates "not applicable".
	inline void SetComponent1( CString value )
	{
		m_csComponent1 = value;
	}
	// COMPONENT 1 is the Coop Id for the first station( in chronological order ) 
	// whose records were joined with those of the HCN site to form a longer time
	// series.  "------" indicates "not applicable".
	__declspec( property( get = GetComponent1, put = SetComponent1 ) )
		CString Component1;

	// COMPONENT 2 is the Coop Id for the second station( if applicable ) 
	// whose records were joined with those of the HCN site to form a longer 
	// time series.
	inline CString GetComponent2()
	{
		CString value = m_csComponent2;

		if ( !m_csSource.IsEmpty() )
		{
			// 6 characters starting at 79
			value = m_csSource.Mid( 79, 6 );
		}

		// persist the value
		Component2 = value;

		return value;
	}
	// COMPONENT 2 is the Coop Id for the second station( if applicable ) 
	// whose records were joined with those of the HCN site to form a longer 
	// time series.
	inline void SetComponent2( CString value )
	{
		m_csComponent2 = value;
	}
	// COMPONENT 2 is the Coop Id for the second station( if applicable ) 
	// whose records were joined with those of the HCN site to form a longer 
	// time series.
	__declspec( property( get = GetComponent2, put = SetComponent2 ) )
		CString Component2;

	// COMPONENT 3 is the Coop Id for the third station( if applicable ) 
	// whose records were joined with those of the HCN site to form a longer 
	// time series.
	inline CString GetComponent3()
	{
		CString value = m_csComponent3;

		if ( !m_csSource.IsEmpty() )
		{
			// 6 characters starting at 86
			value = m_csSource.Mid( 86, 6 );
		}

		// persist the value
		Component3 = value;

		return value;
	}
	// COMPONENT 3 is the Coop Id for the third station( if applicable ) 
	// whose records were joined with those of the HCN site to form a longer 
	// time series.
	inline void SetComponent3( CString value )
	{
		m_csComponent3 = value;
	}
	// COMPONENT 3 is the Coop Id for the third station( if applicable ) 
	// whose records were joined with those of the HCN site to form a longer 
	// time series.
	__declspec( property( get = GetComponent3, put = SetComponent3 ) )
		CString Component3;

	// UTC OFFSET is the time difference between Coordinated Universal Time( UTC )
	// and local standard time at the station( i.e., the number of hours that
	// must be added to local standard time to match UTC ).
	inline short GetOffsetUTC()
	{
		// return value
		short value = m_sOffsetUTC;

		if ( !m_csSource.IsEmpty() )
		{
			// 2 characters starting at 93
			const CString csValue = m_csSource.Mid( 93, 2 );

			// convert to a short
			value = (short)_tstol( csValue );
		}

		// persist the value
		OffsetUTC = value;

		return value;
	}
	// UTC OFFSET is the time difference between Coordinated Universal Time( UTC )
	// and local standard time at the station( i.e., the number of hours that
	// must be added to local standard time to match UTC ).
	inline void SetOffsetUTC( short value )
	{
		m_sOffsetUTC = value;
	}
	// UTC OFFSET is the time difference between Coordinated Universal Time( UTC )
	// and local standard time at the station( i.e., the number of hours that
	// must be added to local standard time to match UTC ).
	__declspec( property( get = GetOffsetUTC, put = SetOffsetUTC ) )
		short OffsetUTC;
	
// protected methods
protected:

// public methods
public:

// protected overrides
protected:

// public overrides
public:

// public constructor
public:
	// default constructor
	CClimateStation()
	{
		const float fMissing = -999.9f;
		Latitude =  fMissing;
		Longitude = fMissing;
		Elevation = fMissing;
		OffsetUTC = 0;
	}

	// construct from lines of the "ushcn-v2.5-stations.txt" source
	CClimateStation( LPCTSTR source )
	{
		Source = source;
	}

	// destructor
	~CClimateStation()
	{
	}
};


