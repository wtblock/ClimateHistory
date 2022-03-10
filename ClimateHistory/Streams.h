/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "CHelper.h"
#include "Schemas.h"
#include "Stream.h"
#include "KeyedCollection.h"
#include "SmartArray.h"
#include <memory>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// a collection of streams with the same number of records in each.
// The project folder is referred to as the root folder 
//	and the project is stored in a hierarchy of folders under the root:
//
//	Version - folder formatted as "version_name.ver"
//		if the streams are not versioned, the version is blank and the data is 
//			located in the root
//		Group - folder formatted as "group_name.grp" 
//			if the streams are not grouped, the group is blank and the data is
//				located in either the version folder (if versioned) or the root
//			Collection Name - folder name under the root (if unversioned and 
//				ungrouped), version folder (if versioned and ungrouped), or 
//				group folder (if versioned and grouped)
//					Tabular data is stored in folders containing streams 
//						representing columns of the table
//					Telemetry data is stored like tables with the only limitation 
//						being the streams are all numerical
class CStreams
{
// public definitions
public:

// protected data
protected:
	// stream collection
	CKeyedCollection<CString, CStream> m_Streams;

	// data schema
	CSchemas* m_pDataSchema;

	// collection schema
	CString m_csSchema;

	// folder containing the root of the project
	CString m_csRoot;

	// collection version
	CString m_csVersion;

	// collection group
	CString m_csGroup;

	// pathname of the collection
	CString m_csPathName;

	// number of records in each stream of the collection
	ULONG m_ulRecords;

// public properties
public:
	// data schema
	inline CKeyedCollection<CString, CStream>& GetStreams()
	{
		return m_Streams;
	}
	// data schema
	__declspec( property( get = GetStreams ) )
		CKeyedCollection<CString, CStream>& Streams;

	// data schema
	inline CSchemas* GetDataSchema()
	{
		return m_pDataSchema;
	}
	// data schema
	inline void SetDataSchema( CSchemas* value )
	{
		m_pDataSchema = value;
	}
	// data schema
	__declspec( property( get = GetDataSchema, put = SetDataSchema ) )
		CSchemas* DataSchema;

	// collection schema
	inline CString GetSchema()
	{
		return m_csSchema;
	}
	// collection schema
	inline void SetSchema( CString value )
	{
		m_csSchema = value;
	}
	// collection schema
	__declspec( property( get = GetSchema, put = SetSchema ) )
		CString Schema;

	// folder containing the root of the project
	inline CString GetRoot()
	{
		return m_csRoot;
	}
	// folder containing the root of the project
	inline void SetRoot( CString value )
	{
		m_csRoot = value;
	}
	// folder containing the root of the project
	__declspec( property( get = GetRoot, put = SetRoot ) )
		CString Root;

	// collection version
	inline CString GetVersion()
	{
		return m_csVersion;
	}
	// collection version
	inline void SetVersion( CString value )
	{
		m_csVersion = value;
	}
	// collection version
	__declspec( property( get = GetVersion, put = SetVersion ) )
		CString Version;

	// collection group
	inline CString GetGroup()
	{
		return m_csGroup;
	}
	// collection group
	inline void SetGroup( CString value )
	{
		m_csGroup = value;
	}
	// collection group
	__declspec( property( get = GetGroup, put = SetGroup ) )
		CString Group;

	// pathname of the collection
	inline CString GetPathName()
	{
		USES_CONVERSION;

		CString value = Root;
		const CString csVersion = Version;
		const CString csGroup = Group;

		value.TrimRight( _T( "\\" ));

		// append the version folder if defined
		if ( !csVersion.IsEmpty() )
		{
			value += _T( "\\" );
			value += csVersion;
			value += _T( ".ver" );
		}

		// append the group folder if defined
		if ( !csGroup.IsEmpty() )
		{
			value += _T( "\\" );
			value += csGroup;
			value += _T( ".grp" );
		}

		// persist the name
		PathName = value;

		// test for the existence of the folder and create it if it does
		// not exist
		if ( !::PathFileExists( value ) )
		{
			::SHCreateDirectory( NULL, A2CW( value ));
		}

		return value;
	}
	// pathname of the collection
	inline void SetPathName( CString value )
	{
		m_csPathName = value;
	}
	// pathname of the collection
	__declspec( property( get = GetPathName, put = SetPathName ) )
		CString PathName;

	// number of records in a the streams
	inline ULONG GetRecords()
	{
		return m_ulRecords;
	}
	// number of records in a the streams
	inline void SetRecords( ULONG value )
	{
		m_ulRecords = value;
	}
	// number of records in a the streams
	__declspec( property( get = GetRecords, put = SetRecords ) )
		ULONG Records;

	// is the value a valid record
	inline bool GetValidRecord( ULONG value )
	{
		return value < Records;
	}
	// is the value a valid record
	__declspec( property( get = GetValidRecord ) )
		bool ValidRecord[];

	// all record numbers
	inline vector<long> GetAllRecordNumbers()
	{
		vector<long> value;
		
		return value;
	}
	// all record numbers
	__declspec( property( get = GetAllRecordNumbers ) )
		vector<long> AllRecordNumbers;

	// get queried records
	inline vector<long> GetRecordNumbers( LPCTSTR query )
	{
		vector<long> value;

		return value;
	}
	// get queried records
	__declspec( property( get = GetRecordNumbers ) )
		vector<long> RecordNumbers[];

	// all record GUIDS
	inline long GetAllRecordGUIDs( CKeyedCollection<CString,long>& mapGUIDs )
	{
		vector<long> arrRecords = AllRecordNumbers;
		const long lRecords = (long)arrRecords.size();
		//for ( long lRecord : arrRecords )
		//{
		//	const CString csGUID = UniqueID[ lRecord ];
		//	if ( mapGUIDs.exists( csGUID ) )
		//	{
		//		continue;
		//	}
		//	shared_ptr<long> pGUIDS = shared_ptr<long>( new long( lRecord ));
		//	mapGUIDs.add( csGUID, pGUIDS );
		//}

		// this will not match if there are duplicate GUIDs which is 
		// an error condition
		const long value = mapGUIDs.Count;
		ASSERT( lRecords == value );

		return value;
	}

	// collection string property
	inline CString GetString( long record, LPCTSTR stream )
	{
		CString value;
		shared_ptr<CStream>& pStream = Streams.find( stream );
		if ( pStream != nullptr )
		{
			VARENUM vt = pStream->Type;
			if ( vt == VT_BSTR )
			{
				const USHORT usSize = pStream->Size;
				vector<char> buffer = vector<char>( usSize, 0 );
				LPSTR pBuf = &buffer[ 0 ];
				pBuf = (LPSTR)pStream->Value[ record ][ 0 ];
				value = pBuf;
			}
		}

		return value;
	}
	// collection string property
	void SetString( long record, LPCTSTR stream, LPCTSTR value );
	// table string property
	__declspec( property( get = GetString, put = SetString ) )
		CString String[][];

// protected methods
protected:

// public methods
public:

// protected overrides
protected:

// public overrides
public:

// public construction / destruction
public:
	// default constructor
	CStreams()
	{
		Records = 0;
	}
	// initialize constructor
	CStreams
	( 
		CSchemas* pDataSchema,
		CString csSchema, CString csRoot, 
		CString csVersion = _T( "" ), CString csGroup = _T( "" )
	);

	// default destructor
	virtual ~CStreams()
	{
		
	}
};

/////////////////////////////////////////////////////////////////////////////
