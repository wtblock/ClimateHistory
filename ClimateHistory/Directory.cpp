/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Project.h"
#include "Directory.h"

/////////////////////////////////////////////////////////////////////////////
// constructor
CDirectory::CDirectory( CProject* pHost )
{
	Host = pHost;

	const ULONG ulStreams = CreateOpenDirctory();
}

/////////////////////////////////////////////////////////////////////////////
// pointer to the hosting project
CProject* CDirectory::GetHost()
{
	return m_pHost;

} // GetHost

/////////////////////////////////////////////////////////////////////////////
// pointer to the hosting project
void CDirectory::SetHost( CProject* value )
{
	m_pHost = value;

} // SetHost

/////////////////////////////////////////////////////////////////////////////
void CDirectory::AddDirectoryEntry
( 
	// collection of data streams
	CKeyedCollection<CString, CStream>& streams,
	shared_ptr<CStream>& pStream, CString csGUID, ULONG ulRecord,
	CSchemaStream::ENUM_COLLECTION eType
)
{
	shared_ptr<CStream>& pGUID = streams.find( _T( "GUID" ) );
	pGUID->String[ ulRecord ] = csGUID;

	shared_ptr<CStream>& pVersion = streams.find( _T( "Version" ) );
	pVersion->String[ ulRecord ] = Version;

	shared_ptr<CStream>& pGroup = streams.find( _T( "Group" ) );
	pGroup->String[ ulRecord ] = Group;

	shared_ptr<CStream>& pName = streams.find( _T( "Name" ) );
	pName->String[ ulRecord ] = pStream->Name;

	shared_ptr<CStream>& pSchema = streams.find( _T( "Schema" ) );
	pSchema->String[ ulRecord ] = Schema;

	shared_ptr<CStream>& pDescription =
		streams.find( _T( "Description" ) );
	pDescription->String[ ulRecord ] = pStream->Description;

	shared_ptr<CStream>& pClassification =
		streams.find( _T( "Classification" ) );
	const BYTE bType = (BYTE)eType;
	pClassification->Value[ ulRecord ][ 0 ] = (byte*)&bType;

	shared_ptr<CStream>& pCreateionDate =
		streams.find( _T( "CreateionDate" ) );
	const double dCD = pStream->CreationDate.m_dt;
	pCreateionDate->Value[ ulRecord ][ 0 ] = (byte*)&dCD;

	shared_ptr<CStream>& pModificationDate =
		streams.find( _T( "ModificationDate" ) );
	const double dMD = pStream->ModificationDate.m_dt;
	pModificationDate->Value[ ulRecord ][ 0 ] = (byte*)&dMD;

} // AddDirectoryStream

/////////////////////////////////////////////////////////////////////////////
ULONG CDirectory::CreateOpenDirctory()
{
	ULONG value = 0;

	// schema definitions
	CSchemas* pSchemas = Host->Schemas;

	// find the record for the directory schema
	const long lSchema = pSchemas->nameFind( _T( "Directory" ) );

	// the array of schema definitions
	CSmartArray<CSchema>& arrDefs = pSchemas->Schemas;

	// the schema for the station list
	shared_ptr<CSchema>& refSchema = arrDefs.get( lSchema );

	// collection name from the schema
	const CString csCollectionName = refSchema->Name;

	// the keys to uniquely identify a record
	const CString csKeys = refSchema->UniqueKeys;

	// array schema streams
	CSmartArray<CSchemaStream>& refStreams = refSchema->SchemaStreams;

	// ask the collection to create its folder and streams
	bool bPreexist = CreateStreams
	(
		Host->Schemas, csCollectionName, Host->WorkingFolder
	);

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
		const CString csStream = stream.first;
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

	// if there are zero records, the streams of the directory
	// will become the first entries into the directory
	if ( ulRecords == 0 )
	{
		ULONG ulRecord = 0;
		for ( auto& stream : streams.Items )
		{
			const CString csGUID = CHelper::MakeGUID();
			CSchemaStream::ENUM_COLLECTION eType =
				CSchemaStream::ecDirectory;
			AddDirectoryEntry
			( 
				streams, stream.second, csGUID, ulRecord, eType 
			);

			ulRecord++;
		}
	}

#ifdef _DEBUG
	for ( ULONG ulRecord = 0; ulRecord < Records; ulRecord++ )
	{
		pair<CString,CString> pairCSV = CSV[ ulRecord ];
		if ( ulRecord == 0 )
		{
			TRACE( pairCSV.first + _T( "\n" ));
		}
		TRACE( pairCSV.second + _T( "\n" ) );
	}
#endif

	// the stream names that comprise the unique key for each record
	vector<CString>& keyStreams = KeyStreams;

	// create the index based on key streams
	Index();

	return value;
} // CreateOpenDirctory

/////////////////////////////////////////////////////////////////////////////

