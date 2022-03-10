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
// create the station list
ULONG CProject::CreateStationList
( 
	CKeyedCollection<CString, CClimateStation>& stations 
)
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
		new CStreams( Schemas, csCollectionName, WorkingFolder )
	);

	// number of stream in station list collection
	value = pValue->Streams.Count;

	// persist the list
	StationList = pValue;

	// collection of data streams
	CKeyedCollection<CString, CStream>& streams = pValue->Streams;
	
	// open and/or create the file in the streams 
	for ( auto& stream : streams.Items )
	{
		stream.second->Open();
	}

	// start with record 0
	ULONG ulRecord = 0;
	
	// flag to indicate the data is already cached
	bool bCache = false;

	// loop through all of the stations
	for ( auto& station : stations.Items )
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
			bCache = ulLevels > 0;

			// if the data is already cached, break out of the loop
			if ( bCache )
			{
				// the return value is the number of levels in the cache
				pValue->Records = ulLevels;
				break;
			}

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

		// if bCache, break out of this loop, we are done
		if ( bCache )
		{
			break;
		}

		ulRecord++;
		pValue->Records = ulRecord;
	}

	// if the data is not cache, cache it now
	if ( !bCache )
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
