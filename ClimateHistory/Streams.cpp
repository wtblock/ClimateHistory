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
	CString csSchema, CString csRoot, 
	CString csVersion, CString csGroup
)
{
	USES_CONVERSION;

	bool value = true;
	DataSchema = pDataSchema;
	Schema = csSchema;
	Root = csRoot;
	Version = csVersion;
	Group = csGroup;

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
	const CString csCollectionName = pSchema->Name;
	const CString csCollectionFolder = 
		csFolder + _T( "\\" ) + csCollectionName + _T( "\\" );
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
		pStream->Host = this;

		Streams.add( csStream, pStream );
	}

	return value;
} // CreateStreams

/////////////////////////////////////////////////////////////////////////////
// collection string property which is based on an array of VT_I1 values
// and will fail if the data type is not VT_I1
void CStreams::SetString( ULONG record, LPCTSTR stream, LPCTSTR value )
{
	shared_ptr<CStream>& pStream = Streams.find( stream );
	if ( pStream != nullptr )
	{
		CStreamCache* pCache = pStream->Cache;
		const ULONG ulLevels = pCache->Levels;
		VARENUM vt = pCache->Type;
		if ( vt == VT_I1 && record < ulLevels )
		{
			const ULONG ulSize = pCache->LevelSizeInBytes;
			CString csSource( value );
			USHORT usLength = csSource.GetLength();
			if ( usLength > ulSize )
			{
				usLength = (USHORT)ulSize;
			}
			void* pData = pCache->GetData( record, 0 );
			LPSTR pSource = csSource.GetBuffer( usLength );
			memcpy( pData, (void*)pSource, (size_t)usLength );
		}
	}
} // SetString

/////////////////////////////////////////////////////////////////////////////
// collection string property which is based on an array of VT_I1 values
// and will fail if the data type is not VT_I1
CString CStreams::GetString( ULONG record, LPCTSTR stream )
{
	CString value;
	shared_ptr<CStream>& pStream = Streams.find( stream );
	if ( pStream != nullptr )
	{
		CStreamCache* pCache = pStream->Cache;
		const ULONG ulLevels = pCache->Levels;
		VARENUM vt = pStream->Type;
		if ( vt == VT_I1 && record < ulLevels )
		{
			void* pData = pCache->GetData( record, 0 );
			const USHORT usSize = (USHORT)pCache->LevelSizeInBytes;
			vector<char> buffer = vector<char>( usSize, 0 );
			value = (LPSTR)pData;
		}
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
