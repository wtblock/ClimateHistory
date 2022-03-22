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
//						representing columns of the streams
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

	// optional version name the collection belongs to
	CString m_csVersion;

	// optional group name the collection belongs to
	CString m_csGroup;

	// name of the collection the stream belongs to
	CString m_csCollection;

	// pathname of the collection
	CString m_csPathName;

	// number of records in each stream of the collection
	ULONG m_ulRecords;

	// globally unique identifier of a single record
	CString m_csUniqueID;

	// key stream names used to identify a specific record
	// primary, secondary, ... keys
	vector<CString> m_KeyStreams;

	// the keys associated with a level of data
	vector<CString> m_KeyValues;

	// an index key built from the key values above
	CString m_csKey;

	// comma separated labels and values for a record
	pair<CString, CString> m_csCSV;

	// flag to indicate the collection preexisted in order
	// to prevent re-parsing the source files
	bool m_bPreexist;

	// index of the streams based on key streams
	CKeyedCollection<CString, ULONG> m_KeyIndex;

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

	// optional version name the collection belongs to
	inline CString GetVersion()
	{
		return m_csVersion;
	}
	// optional version name the collection belongs to
	inline void SetVersion( CString value )
	{
		m_csVersion = value;
	}
	// optional version name the collection belongs to
	__declspec( property( get = GetVersion, put = SetVersion ) )
		CString Version;

	// optional group name the collection belongs to
	inline CString GetGroup()
	{
		return m_csGroup;
	}
	// optional group name the collection belongs to
	inline void SetGroup( CString value )
	{
		m_csGroup = value;
	}
	// optional group name the collection belongs to
	__declspec( property( get = GetGroup, put = SetGroup ) )
		CString Group;

	// name of the collection the streams belong to
	inline CString GetCollection()
	{
		return m_csCollection;
	}
	// name of the collection the streams belong to
	inline void SetCollection( CString value )
	{
		m_csCollection = value;
	}
	// name of the collection the streams belong to
	__declspec( property( get = GetCollection, put = SetCollection ) )
		CString Collection;

	// comma separated labels and values for a record
	pair<CString,CString> GetCSV( ULONG ulRecord );
	// comma separated labels and values for a record
	inline void SetCSV( ULONG /*ulRecord*/, pair<CString, CString> value )
	{
		m_csCSV = value;
	}
	// comma separated labels and values for a record
	__declspec( property( get = GetCSV, put = SetCSV ) )
		pair<CString, CString> CSV[];

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
		ULONG value = m_ulRecords;
		if ( value == 0 )
		{
			// every collection has a GUID stream
			shared_ptr<CStream>& pStream = Streams.find( _T( "GUID" ) );
			if ( pStream != nullptr )
			{
				// size of a record in bytes
				const ULONG ulSize = pStream->Size;

				// file size in bytes
				const ULONG ulFilesize = pStream->FileSize;

				// number of records is the file size divided by record size
				if ( ulSize != 0 )
				{
					value = ulFilesize / ulSize;

					// persist the value
					Records = value;
				}
			}
		}

		return value;
	}
	// number of records in a the streams
	inline void SetRecords( ULONG value )
	{
		m_ulRecords = value;
	}
	// number of records in a the streams
	__declspec( property( get = GetRecords, put = SetRecords ) )
		ULONG Records;

	// flag to indicate the collection preexisted
	inline bool GetPreexist()
	{
		return m_bPreexist;
	}
	// flag to indicate the collection preexisted
	inline void SetPreexist( bool value )
	{
		m_bPreexist = value;
	}
	// flag to indicate the collection preexisted
	__declspec( property( get = GetPreexist, put = SetPreexist ) )
		bool Preexist;

	// is the value a valid record
	inline bool GetValidRecord( ULONG value )
	{
		return value < Records;
	}
	// is the value a valid record
	__declspec( property( get = GetValidRecord ) )
		bool ValidRecord[];

	// all record numbers
	inline vector<ULONG> GetAllRecordNumbers()
	{
		const ULONG records = Records;
		ULONG record = 0;
		vector<ULONG> value( records, 0 );
		for ( auto& node : value )
		{
			node = record++;
		}
		return value;
	}
	// all record numbers
	__declspec( property( get = GetAllRecordNumbers ) )
		vector<ULONG> AllRecordNumbers;

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
	inline long GetAllRecordGUIDs( CKeyedCollection<CString,ULONG>& mapGUIDs )
	{
		vector<ULONG> arrRecords = AllRecordNumbers;
		const ULONG ulRecords = (ULONG)arrRecords.size();
		for ( ULONG ulRecord : arrRecords )
		{
			const CString csGUID = UniqueID[ ulRecord ];
			if ( mapGUIDs.exists( csGUID ) )
			{
				continue;
			}
			shared_ptr<ULONG> pGUIDS = shared_ptr<ULONG>
			( 
				new ULONG( ulRecord )
			);
			mapGUIDs.add( csGUID, pGUIDS );
		}

		// this will not match if there are duplicate GUIDs which is 
		// an error condition
		const ULONG value = (ULONG)mapGUIDs.Count;
		ASSERT( ulRecords == value );

		return value;
	}

	// collection string property which is based on an array of VT_I1 values
	// and will fail if the data type is not VT_I1, the steam does not exist
	// or the record is out-of-range
	CString GetString( ULONG record, LPCTSTR stream );
	// collection string property which is based on an array of VT_I1 values
	// and will fail if the data type is not VT_I1, the steam does not exist
	// or the record is out-of-range
	void SetString( ULONG record, LPCTSTR stream, LPCTSTR value );
	// collection string property which is based on an array of VT_I1 values
	// and will fail if the data type is not VT_I1, the steam does not exist
	// or the record is out-of-range
	__declspec( property( get = GetString, put = SetString ) )
		CString String[][];

// protected index properties
protected:
	// globally unique identifier of a single record
	inline CString GetUniqueID( ULONG record )
	{
		const CString value = String[ record ][ _T( "GUID" ) ];

		return value;
	}
	// globally unique identifier of a single record
	inline void SetUniqueID( ULONG record, CString value )
	{
		String[ record ][ _T( "GUID" ) ] = value;
	}
	// globally unique identifier of a single record
	__declspec( property( get = GetUniqueID, put = SetUniqueID ) )
		CString UniqueID[];

	// index of the streams based on key streams
	inline CKeyedCollection<CString,ULONG>& GetKeyIndex()
	{
		return m_KeyIndex;
	}
	// index of the streams based on key streams
	__declspec( property( get = GetKeyIndex ) )
		CKeyedCollection<CString,ULONG> KeyIndex;

	// key stream index count
	inline int GetIndexCount()
	{
		return KeyIndex.Count;
	}
	// key stream index count
	__declspec( property( get = GetIndexCount ) )
		int IndexCount;

	// streams is indexed by key streams
	inline bool GetIndexed()
	{
		const int nValues = IndexCount;
		return nValues > 0;
	}
	// streams is indexed by key streams
	__declspec( property( get = GetIndexed ) )
		bool Indexed;

	// key stream names used to identify a specific record
	// primary, secondary, ... keys
	vector<CString>& GetKeyStreams();
	// key stream names used to identify a specific record
	// primary, secondary, ... keys
	inline void SetKeyStreams( vector<CString> value )
	{
		m_KeyStreams = value;
	}
	// key stream names used to identify a specific record
	// primary, secondary, ... keys
	__declspec( property( get = GetKeyStreams, put = SetKeyStreams ) )
		vector<CString> KeyStreams;

	// get the key values for a given record number, return empty
	// array on failure
	inline vector<CString> GetKeyValues( ULONG record )
	{
		vector<CString> values;
		bool bOkay = true;

		// get the key streams, and if empty add the GUID stream
		vector<CString> arrStreams = KeyStreams;
		if ( arrStreams.empty() )
		{
			arrStreams.push_back( _T( "GUID" ) );
		}

		// read and append values for each key stream
		for ( CString csStream : arrStreams )
		{
			CString csValue;
			
			shared_ptr<CStream>& pStream = Streams.find( csStream );
			if ( pStream != nullptr )
			{
				CStreamCache* pCache = pStream->Cache;
				const ULONG ulLevels = pCache->Levels;
				if ( record < ulLevels )
				{
					csValue = pCache->String[ record ];
					values.push_back( csValue );
				} 
				else
				{
					values.empty();
					break;
				}
			}
		}

		// persist the value
		KeyValues[ record ] = values;

		return values;
	}
	// get the key value for a given record number, return empty
	// array on failure
	inline void SetKeyValues( ULONG /*record*/, vector<CString> value )
	{
		m_KeyValues = value;
	}
	// get the key value for a given record number, return empty
	// array on failure
	__declspec( property( get = GetKeyValues, put = SetKeyValues ) )
		vector<CString> KeyValues[];

	// get the key string for a given record and return 
	// empty string on failure
	inline CString GetKey( ULONG record )
	{
		vector<CString> values = KeyValues[ record ];
		const CString value = MakeKey( values );

		// persist the value
		Key[ record ] = value;

		return value;
	}
	// get the key string for a given record and return 
	// empty string on failure
	inline void SetKey( ULONG record, CString value )
	{
		m_csKey = value;
	}
	// get the key string for a given record and return 
	// empty string on failure
	__declspec( property( get = GetKey, put = SetKey ) )
		CString Key[];

// protected methods
protected:

// public methods
public:
	// index by key fields
	inline void Index()
	{
		KeyIndex.clear();
		const size_t nKeys = KeyStreams.size();
		if ( nKeys == 0 )
		{
			return;
		}
		
		for ( ULONG record = 0; record < Records; record++ )
		{
			const CString csKey = Key[ record ];
			if ( !KeyIndex.exists( csKey ) )
			{
				shared_ptr<ULONG> pRecord = shared_ptr<ULONG>
				( 
					new ULONG( record )
				);
				KeyIndex.add( csKey, pRecord );
			}
		}
	}

	// create the collection's steams and return true if they 
	// already exist
	bool CreateStreams
	(
		CSchemas* pDataSchema,
		CString csSchema, // schema the collection is modeled after
		CString csRoot, // root folder of the collection
		CString csCollection, // the collection name
		CString csGroup = _T( "" ), // optional group
		CString csVersion = _T( "" ) // optional version
	);

	// make a key from an array of key values
	static CString MakeKey( vector<CString> values )
	{
		CString csKey;
		for ( CString csValue : values )
		{
			if ( !csKey.IsEmpty() )
			{
				csKey += _T( "\n" ); // line feed separator
			}
			csKey += csValue;
		}
		return csKey;
	}

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
		DataSchema = nullptr;
	}
	// default destructor
	virtual ~CStreams()
	{
	}
};

/////////////////////////////////////////////////////////////////////////////
