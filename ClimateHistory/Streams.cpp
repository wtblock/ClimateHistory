/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Streams.h"

/////////////////////////////////////////////////////////////////////////////
// ask the collection to create its folder and streams
bool CStreams::CreateStreams
( 
	CSchemas* pDataSchema,
	CString csSchema, // schema the collection is modeled after
	CString csRoot, // root folder of the collection
	CString csCollection, // the collection name
	CString csGroup /*= _T( "" )*/, // optional group
	CString csVersion /*= _T( "" )*/ // optional version
)
{
	USES_CONVERSION;

	bool value = true;
	DataSchema = pDataSchema;
	Schema = csSchema;
	Root = csRoot;
	Version = csVersion;
	Group = csGroup;
	Collection = csCollection;

	const CString csFolder = PathName;

	const long lSchema = pDataSchema->nameFind( csSchema );
	if ( lSchema == -1 )
	{
		// flag the problem if it happens
		CHelper::ErrorMessage( __FILE__, __LINE__ );
		ASSERT( FALSE );
		return false;
	}

	CSmartArray<CSchema>& Schemas = pDataSchema->Schemas;
	shared_ptr<CSchema>& pSchema = Schemas.get( lSchema );
	if ( pSchema == nullptr )
	{
		// flag the problem if it happens
		CHelper::ErrorMessage( __FILE__, __LINE__ );
		ASSERT( FALSE );
		return false;
	}

	// build a path to the collection 
	const CString csCollectionFolder = 
		csFolder + _T( "\\" ) + csCollection + _T( "\\" );
	if ( !::PathFileExists( csCollectionFolder ) )
	{
		::SHCreateDirectory( NULL, A2CW( csCollectionFolder ));

		// let the caller know we had to create the collection folder
		value = false;
		Preexist = value;
	}

	// create the collection's streams
	CSchema::MAP_NAMES& mapNames = pSchema->NameIndex;

	// the number of streams
	long lStreams = pSchema->NameCount;

	// loop through the stream collection and create the streams if they do
	// not exist
	for ( auto& node : mapNames )
	{
		const CString csStream = node.first;
		const long lStream = node.second;
		shared_ptr<CSchemaStream>& pSchemaStream = 
			pSchema->SchemaStreams.get( lStream );
		if ( pSchemaStream == nullptr )
		{
			continue;
		}

		// create the file if it does not exist or open it if it does
		const CString csPath = csCollectionFolder + csStream;
		shared_ptr<CStream> pStream = shared_ptr<CStream>( new CStream );
		pStream->Pathname = csPath;
		pStream->Name = csStream;
		pStream->SchemaStream = pSchemaStream;
		pStream->Null = pSchemaStream->Null;
		pStream->Title = pSchemaStream->Title;
		pStream->Description = pSchemaStream->Description;
		pStream->Enumeration = pSchemaStream->Enumeration;
		pStream->PropertyGroup = pSchemaStream->PropertyGroup;

		pStream->Entry = (CStream::ENUM_ENTRY)pSchemaStream->Entry;
		pStream->Host = this;
		pStream->Type = pSchemaStream->Type;
		const USHORT usSize = pStream->ValueSize;
		pStream->Size = pSchemaStream->Size;
		const ULONG ulFileSize = pStream->FileSize;
		const USHORT usValues = pStream->LevelValues;
		const ULONG ulLevels = pStream->Levels;
		Streams.add( csStream, pStream );
	}

	return value;
} // CreateStreams

/////////////////////////////////////////////////////////////////////////////
// collection string property which is based on an array of VT_I1 values
// and will fail if the data type is not VT_I1, the steam does not exist
// or the record is out-of-range
void CStreams::SetString( ULONG record, LPCTSTR stream, LPCTSTR value )
{
	shared_ptr<CStream>& pStream = Streams.find( stream );
	if ( pStream != nullptr )
	{
		CStreamCache* pCache = pStream->Cache;
		pCache->String[ record ] = value;
	}
} // SetString

/////////////////////////////////////////////////////////////////////////////
// collection string property which is based on an array of VT_I1 values
// and will fail if the data type is not VT_I1, the steam does not exist
// or the record is out-of-range
CString CStreams::GetString( ULONG record, LPCTSTR stream )
{
	CString value;
	shared_ptr<CStream>& pStream = Streams.find( stream );
	if ( pStream != nullptr )
	{
		CStreamCache* pCache = pStream->Cache;
		value = pCache->String[ record ];
	}

	return value;
} // GetString

/////////////////////////////////////////////////////////////////////////////
// key stream names used to identify a specific record
// primary, secondary, ... keys
vector<CString>& CStreams::GetKeyStreams()
{
	m_KeyStreams.clear();

	// get the schema definition for our data
	CSchemas* pSchemas = DataSchema;

	// the schema associated with this collection
	CString csSchema = Schema;

	// find the index into the schema array
	const LONG lSchema = pSchemas->nameFind( csSchema );
	if ( lSchema != -1 )
	{
		// access the schema array
		CSmartArray<CSchema>& Schemas = pSchemas->Schemas;

		// lookup our schema
		shared_ptr<CSchema>& pSchema = Schemas.get( lSchema );

		// ask the schema for the unique keys for this collection
		const CString csIndex = pSchema->UniqueKeys;

		// parse out the comma separated keys and add them to the 
		// array of keys for this collection
		int nStart = 0;
		do
		{
			const CString csToken =
				csIndex.Tokenize( _T( ", " ), nStart );
			if ( csToken.IsEmpty() )
			{
				break;
			}

			m_KeyStreams.push_back( csToken );
		}
		while ( true );
	}

	return m_KeyStreams;
} // GetKeyStreams

/////////////////////////////////////////////////////////////////////////////
// comma separated values for a record
pair<CString, CString> CStreams::GetCSV( ULONG ulRecord )
{
	pair<CString, CString> value;

	// name of the schema this collection is based on
	const CString csSchema = Schema;

	// the index into the schema array
	const LONG lSchema = DataSchema->nameFind( csSchema );

	// should not happen, being anal
	if ( lSchema == -1 )
	{
		// flag the problem if it happens
		CHelper::ErrorMessage( __FILE__, __LINE__ );
		ASSERT( FALSE );
		return value;
	}

	// collection of all schemas
	CSmartArray<CSchema>& Schemas = DataSchema->Schemas;

	// the schema we are based on
	shared_ptr<CSchema>& pSchema = Schemas.get( lSchema );

	// should not happen, being anal
	if ( pSchema == nullptr )
	{
		// flag the problem if it happens
		CHelper::ErrorMessage( __FILE__, __LINE__ );
		ASSERT( FALSE );
		return value;
	}

	// the array of streams in schema definition order
	CSmartArray<CSchemaStream>& pStreams = pSchema->SchemaStreams;

	// first stream needs to be initialized
	bool bFirst = true;

	// loop through the schema streams in the order they are defined
	// in the original XML file
	for ( auto& node : pStreams.Items )
	{
		// name of the stream
		const CString csName = node->Name;

		// find the stream using its name
		shared_ptr<CStream>& pStream = m_Streams.find( csName );

		// read the data from the stream at the given record number
		CString csData = pStream->String[ ulRecord ].Trim();

		// CSV data does not parse easily when there is no data in 
		// a column, so instead of seeing this ",,", this is shown
		// instead ",<empty>,"
		if ( csData.IsEmpty() )
		{
			csData = _T( "<empty>" );
		}

		// setup the output for the first column
		if ( bFirst )
		{
			bFirst = false;
			value.first = csName;
			value.second = csData;
		} 
		else // remaining columns are preceded with a comma
		{
			value.first += _T( "," );
			value.first += csName;
			value.second += _T( "," );
			value.second += csData;
		}
	}

	// persist the CSV value
	CSV[ ulRecord ] = value;

	return value;
} // GetCSV

/////////////////////////////////////////////////////////////////////////////
