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
			const CString csStream = stream.first;
			if ( csStream == _T( "GUID" ) )
			{
				const CString csGUID = CHelper::MakeGUID();
				stream.second->String[ ulRecord ] = csGUID;
			}
			else if ( csStream == _T( "Version" ) )
			{
				const CString csVersion = Version;
				stream.second->String[ ulRecord ] = csVersion;
			}
			else if ( csStream == _T( "Group" ) )
			{
				const CString csGroup = Group;
				stream.second->String[ ulRecord ] = csGroup;
			}
			else if ( csStream == _T( "Name" ) )
			{
				stream.second->String[ ulRecord ] = csStream;
			}
			else if ( csStream == _T( "Schema" ) )
			{
				stream.second->String[ ulRecord ] = refSchema->Schema;
			}
			else if ( csStream == _T( "Description" ) )
			{
				stream.second->String[ ulRecord ] = refSchema->Description;
			}
			else if ( csStream == _T( "Classification" ) )
			{
				CSchemaStream::ENUM_COLLECTION eType = CSchemaStream::ecDirectory;
				const BYTE bType = (BYTE)eType;
				stream.second->Value[ ulRecord ][ 0 ] = (byte*)&bType;
			}
			else if ( csStream == _T( "CreateionDate" ) )
			{
				const double dCD = stream.second->CreationDate.m_dt;
				stream.second->Value[ ulRecord ][ 0 ] = (byte*)&dCD;
			}
			else if ( csStream == _T( "ModificationDate" ) )
			{
				const double dMD = stream.second->ModificationDate.m_dt;
				stream.second->Value[ ulRecord ][ 0 ] = (byte*)&dMD;
			}
		}
	}

	vector<CString>& keyStreams = KeyStreams;

	return value;
} // CreateOpenDirctory

/////////////////////////////////////////////////////////////////////////////

