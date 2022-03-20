/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ClimateStation.h"
#include "Streams.h"
#include "KeyedCollection.h"

/////////////////////////////////////////////////////////////////////////////
class CProject;

/////////////////////////////////////////////////////////////////////////////
// a collection of climate stations as defined by the 
// U.S. Historical Climatology Network (USHCN)
class CClimateStations : public CStreams
{
// public definitions
public:

// protected data
protected:
	// pointer to the hosting project
	CProject* m_pHost;

	// the path to the station data text file
	CString m_csStationPath;

	// rapid station lookup of climate stations
	CKeyedCollection<CString, CClimateStation> m_Stations;

	// station key
	CString m_csStationKey;

// public properties
public:
	// pointer to the hosting project
	CProject* GetHost();
	// pointer to the hosting project
	void SetHost( CProject* value );
	// pointer to the hosting project
	__declspec( property( get = GetHost, put = SetHost ) )
		CProject* Host;

	// the path to the station data text file
	CString GetStationPath();
	// the path to the station data text file
	void SetStationPath( CString value );
	// the path to the station data text file
	__declspec( property( get = GetStationPath, put = SetStationPath ) )
		CString StationPath;

	// station key
	inline CString GetStationKey()
	{
		return m_csStationKey;
	}
	// station key
	inline void SetStationKey( CString value )
	{
		m_csStationKey = value;
	}
	// station key
	__declspec( property( get = GetStationKey, put = SetStationKey  ) )
		CString StationKey;

// protected methods
protected:

// public methods
public:
	// read the station data into the stations collection from the original
	// USHCN text file "ushcn-v2.5-stations.txt" 
	bool ReadStationData();

	// create the station list and return the number of stations
	ULONG CreateStationList();

// protected overrides
protected:

// public overrides
public:

// public construction / destruction
public:
	// constructor
	CClimateStations( CProject* pHost );

	// destructor
	virtual ~CClimateStations()
	{
		//m_spStationList.reset();
	}
};

/////////////////////////////////////////////////////////////////////////////
