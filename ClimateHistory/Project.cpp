/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Project.h"

/////////////////////////////////////////////////////////////////////////////
// read the schemas that can be in the project from the DataSchema.XML
// file which is located in the Settings folder of the application 
// folder
bool CProject::ReadDataSchema( const CString& csExe )
{
	bool value = false;

	const CString csFolder = CHelper::GetFolder( csExe );
	const CString csSettings = csFolder + _T( "Settings\\" );
	Settings = csSettings;
	const CString csDataSchema = csSettings + "DataSchema.xml";
	const bool bExists = ::PathFileExists( csDataSchema ) != FALSE;
	if ( bExists )
	{
		value = m_Schemas.OpenSchemas( csDataSchema );
	}

	return value;
} // ReadDataSchema

/////////////////////////////////////////////////////////////////////////////
// read the station data into the stations collection from the original
// USHCN text file: "ushcn-v2.5-stations.txt"
bool CProject::ReadStationData()
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
	// collect the station data
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
ULONG CProject::CreateStationList()
{
	ULONG value = 0;

	// schema definitions
	CSchemas* pSchemas = Schemas;

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

	// a collection of CStreams for the station list
	shared_ptr<CStreams> pValue = shared_ptr<CStreams>
	( 
		new CStreams()
	);

	// ask the collection to create its folder and streams
	bool bPreexist = 
		pValue->CreateStreams( Schemas, csCollectionName, WorkingFolder );

	// if the collection did not preexist, we need to read the source
	// text files so we can populate the streams
	bool bStations = 0;
	if ( !bPreexist )
	{
		bStations = ReadStationData();
	}

	// number of stream in station list collection
	value = pValue->Streams.Count;

	// persist the list
	StationList = pValue;

	// collection of data streams
	CKeyedCollection<CString, CStream>& streams = pValue->Streams;
	
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
			else if ( csStream == _T( "OffsetUTC" ) )
			{
				*pBuf = pStation->OffsetUTC;
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
		pValue->Records = ulRecord;
	}

	// if the data is not cached, cache it now
	if ( !bCached )
	{
		// cache the file in the streams 
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

/////////////////////////////////////////////////////////////////////////////
