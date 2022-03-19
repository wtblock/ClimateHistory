/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Project.h"
#include "ClimateStations.h"

/////////////////////////////////////////////////////////////////////////////
// constructor
CClimateStations::CClimateStations( CProject* pHost )
{
	Host = pHost;

	const ULONG ulStations = CreateStationList();
}

/////////////////////////////////////////////////////////////////////////////
// pointer to the hosting project
CProject* CClimateStations::GetHost()
{
	return m_pHost;

} // GetHost

/////////////////////////////////////////////////////////////////////////////
// pointer to the hosting project
void CClimateStations::SetHost( CProject* value )
{
	m_pHost = value;

} // SetHost

/////////////////////////////////////////////////////////////////////////////
// the path to the station data text file
CString CClimateStations::GetStationPath()
{
	const CString value = Host->StationPath;
	StationPath = value;
	return value;

} // GetStationPath

/////////////////////////////////////////////////////////////////////////////
// the path to the station data text file
void CClimateStations::SetStationPath( CString value )
{
	m_csStationPath = value;

} // SetStationPath

/////////////////////////////////////////////////////////////////////////////
// read the station data into the stations collection from the original
// USHCN text file: "ushcn-v2.5-stations.txt"
bool CClimateStations::ReadStationData()
{
	// the stations text file pathname
	const CString csStationData = StationPath;

	// open the stations text file
	CStdioFile file;
	const bool value =
		file.Open
		( 
			csStationData, CFile::modeRead | CFile::shareDenyNone 
		);

	// if the open was successful, read each line of the file and 
	// collect the station data properties
	if ( value == true )
	{
		CString csLine;
		while ( file.ReadString( csLine ) )
		{
			shared_ptr<CClimateStation> pStation =
				shared_ptr<CClimateStation>( new CClimateStation( csLine ) );
			const CString csKey = pStation->Station;
			m_Stations.add( csKey, pStation );
		}
	}

	return value;
} // ReadStationData

/////////////////////////////////////////////////////////////////////////////
// create the station list
ULONG CClimateStations::CreateStationList()
{
	ULONG value = 0;

	// schema definitions
	CSchemas* pSchemas = Host->Schemas;

	// find the record for the station list schema
	const long lSchema = pSchemas->nameFind( _T( "StationList" ));

	// the array of schema definitions
	CSmartArray<CSchema>& arrDefs = pSchemas->Schemas;

	// the schema for the station list
	shared_ptr<CSchema>& refSchema = arrDefs.get( lSchema );

	// collection name from the schema
	const CString csCollectionName = refSchema->Name;

	// array schema streams
	CSmartArray<CSchemaStream>& refStreams = refSchema->SchemaStreams;

	// ask the collection to create its folder and streams
	bool bPreexist = CreateStreams
	( 
		Host->Schemas, csCollectionName, Host->WorkingFolder 
	);

	// if the collection did not preexist, we need to read the source
	// text "ushcn-v2.5-stations.txt" so we can populate the streams
	bool bStations = false;
	if ( !bPreexist )
	{
		bStations = ReadStationData();
	}

	// number of stream in station list collection
	value = Streams.Count;

	// collection of data streams
	CKeyedCollection<CString, CStream>& streams = Streams;
	
	// number of records in the files
	ULONG ulRecords = 0;
	bool bFirst = true;
	bool bConsistent = true;
	
	// open and/or create the file in the streams 
	for ( auto& stream : streams.Items )
	{
		stream.second->Open();

		// do a consistency test to verify all streams have
		// the same number of levels
		const ULONG ulLevels = stream.second->Levels;
		if ( bPreexist )
		{
			// all of the levels should be the same
			if ( ulLevels != ulRecords )
			{
				// initialize on the first file
				if ( bFirst )
				{
					ulRecords = ulLevels;
					bFirst = false;
				}
				else // error
				{
					CHelper::ErrorMessage( __FILE__, __LINE__ );
					bConsistent = false;
				}
			}
		}
	}

	// if the collection did not preexist, we need to read the source
	// text files so we can populate the streams
	if ( !bConsistent )
	{
		const bool bStations = ReadStationData();
		bPreexist = false;
	}

	// start with record 0
	ULONG ulRecord = bPreexist ? ulRecords : 0;
	
	// flag to indicate the data is already cached
	bool bCached = bPreexist ? true : false;

	// loop through all of the stations
	for ( auto& station : m_Stations.Items )
	{
		shared_ptr<CClimateStation>& pStation = station.second;
		if ( pStation == nullptr )
		{
			// flag the problem if it happens
			CHelper::ErrorMessage( __FILE__, __LINE__ );
			ASSERT( FALSE );
			continue;
		}

		// loop through all of the schema streams in the schema
		for ( auto& schemaStream : refStreams.Items )
		{
			const CString csStream = schemaStream->Name;
			const VARENUM eType = schemaStream->Type;
			const size_t usRecordSize = (size_t)schemaStream->Size;

			// find the stream based on the schema's stream name
			shared_ptr<CStream>& pStream = streams.find( csStream );
			if ( pStream == nullptr )
			{
				// flag the problem if it happens
				CHelper::ErrorMessage( __FILE__, __LINE__ );
				ASSERT( FALSE );
				continue;
			}

			// test the cache to see if the data has already been read
			CStreamCache* pCache = pStream->Cache;
			const ULONG ulLevels = pCache->Levels;

			// write the value to the correct file
			const bool bOpen = pStream->IsOpen;
			if ( !bOpen )
			{
				// flag the problem if it happens
				CHelper::ErrorMessage( __FILE__, __LINE__ );
				ASSERT( FALSE );
				continue;
			}

			// get a pointer to the stream's file
			CFile* pFile = pStream->File;

			vector<byte> buffer( usRecordSize, 0 );
			byte* pBuf = &buffer[ 0 ];
			CString csSource;
			float fSource = 0;
			short sSource = 0;
			LPSTR pSource = nullptr;

			if ( csStream == _T( "GUID" ) )
			{
				csSource = CHelper::MakeGUID();
			}
			else if ( csStream == _T( "Station" ) )
			{
				csSource = pStation->Station;
			}
			else if ( csStream == _T( "Latitude" ) )
			{
				fSource = pStation->Latitude;
				::memcpy( pBuf, &fSource, usRecordSize );
			}
			else if ( csStream == _T( "Longitude" ) )
			{
				fSource = pStation->Longitude;;
				::memcpy( pBuf, &fSource, usRecordSize );
			}
			else if ( csStream == _T( "Elevation" ) )
			{
				fSource = pStation->Elevation;
				::memcpy( pBuf, &fSource, usRecordSize );
			}
			else if ( csStream == _T( "OffsetUTC" ) )
			{
				sSource = pStation->OffsetUTC;
				::memcpy( pBuf, &sSource, usRecordSize );
			}
			else if ( csStream == _T( "State" ) )
			{
				csSource = pStation->State;
			}
			else if ( csStream == _T( "Location" ) )
			{
				csSource = pStation->Location;
			}
			else if ( csStream == _T( "Component1" ) )
			{
				csSource = pStation->Component1;
			}
			else if ( csStream == _T( "Component2" ) )
			{
				csSource = pStation->Component2;
			}
			else if ( csStream == _T( "Component3" ) )
			{
				csSource = pStation->Component3;
			}

			// string case
			if ( !csSource.IsEmpty() )
			{
				pSource = csSource.GetBuffer();
				::memcpy( pBuf, pSource, usRecordSize );
			}

			pStream->Value[ ulRecord ][ 0 ] = pBuf;
		}

		ulRecord++;
	}

	// the number of records of station data
	Records = ulRecord;

	// the data will be cached if the streams preexisted
	// but that means the properties of each stream have
	// not been populated and need to be populated now
	if ( bCached )
	{
		// loop through all of the levels of the streams and
		// create a station populated from the stream data
		CKeyedCollection<CString, CStream>& streams = Streams;
		shared_ptr<CStream>& keyStream = streams.find( _T( "Station" ));
		const ULONG ulLevels = keyStream->Levels;
		for ( ULONG ulLevel = 0; ulLevel < ulLevels; ulLevel++ )
		{
			// create a new station
			shared_ptr<CClimateStation> pStation =
				shared_ptr<CClimateStation>( new CClimateStation );

			// loop through each of the streams
			for ( auto& refStream : streams.Items )
			{
				// name of the stream
				const CString csName = refStream.second->Name;
				
				// station data is only strings or floats
				CString csValue;
				short sValue = 0;
				float fValue = -999.9f;

				// use the stream's cache for fast access
				CStreamCache* pCache = refStream.second->Cache;

				// read the data as a variant
				COleVariant var = pCache->GetVariant( ulLevel, 0 );

				// type of data
				const VARENUM eVt = pCache->Type;

				switch ( eVt )
				{
					case VT_I1:
					{
						csValue = CString( var.bstrVal );
						break;
					}
					case VT_I2:
					{
						sValue = var.iVal;
						break;
					}
					case VT_R4:
					{
						fValue = var.fltVal;
						break;
					}
					default:
					{
						value = false;
					}
				}

				// populate the property based on the stream's name
				if ( csName == _T( "GUID" ) )
				{
					pStation->GUID = csValue;
				}
				else if ( csName == _T( "Station" ) )
				{
					pStation->Station = csValue;
				}
				else if ( csName == _T( "Latitude" ) )
				{
					pStation->Latitude = fValue;
				}
				else if ( csName == _T( "Longitude" ) )
				{
					pStation->Longitude = fValue;
				}
				else if ( csName == _T( "Elevation" ) )
				{
					pStation->Elevation = fValue;
				}
				else if ( csName == _T( "State" ) )
				{
					pStation->State = csValue;
				}
				else if ( csName == _T( "Location" ) )
				{
					pStation->Location = csValue;
				}
				else if ( csName == _T( "Component1" ) )
				{
					pStation->Component1 = csValue;
				}
				else if ( csName == _T( "Component2" ) )
				{
					pStation->Component2 = csValue;
				}
				else if ( csName == _T( "Component3" ) )
				{
					pStation->Component3 = csValue;
				}
				else if ( csName == _T( "OffsetUTC" ) )
				{
					pStation->OffsetUTC = sValue;
				}
			}
			
			// read the key from the station stream
			const CString csKey = pStation->Station;

			// add the populated station to the collection
			m_Stations.add( csKey, pStation );
		}
	}
	// the data is not cached if the streams had to be created
	// from the original "ushcn-v2.5-stations.txt" file so we
	// need to populate the streams now
	else
	{
		// cache the streams with the preexisting file data
		for ( auto& stream : streams.Items )
		{
			const bool bOkay =
				stream.second->ReadIntoCache( 0, ulRecord - 1 );
			if ( !bOkay )
			{
				// flag the problem if it happens
				CHelper::ErrorMessage( __FILE__, __LINE__ );
				ASSERT( FALSE );
			}
		}
	}

	return value;
} // CreateStationList

