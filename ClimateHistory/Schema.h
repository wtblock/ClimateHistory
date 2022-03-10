/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "SmartArray.h"
#include "SchemaStream.h"
#include "SmartArray.h"
#include <map>
#include <memory>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
class CSchemas;

/////////////////////////////////////////////////////////////////////////////
class CSchema
{
// public index definitions
public:
	// map of ids/names to collection records
	typedef pair<CString, LONG> PAIR_NAMES;
	typedef map<CString, LONG> MAP_NAMES;
	typedef MAP_NAMES::iterator POS_NAMES;

/////////////////////////////////////////////////////////////////////////////
// protected Name index data
protected:
// index of schema Names
	MAP_NAMES m_indexNames;

// public Name index properties
public:
	// Name index reference
	inline MAP_NAMES& GetNameIndex()
	{
		return m_indexNames;
	}
	// Name index reference
	__declspec( property( get = GetNameIndex ) )
		MAP_NAMES& NameIndex;

	// Name index count
	inline LONG GetNameCount()
	{
		return (LONG)NameIndex.size();
	}
	// Name index count
	__declspec( property( get = GetNameCount ) )
		LONG NameCount;

// public index Name methods
public:
	// clear index
	inline void NameClear()
	{
		NameIndex.clear();
	}
	// find schema by its Name and return its index into the collection
	inline LONG NameFind( LPCTSTR pcszName )
	{
		LONG lIndex = -1;
		CString csName( pcszName );
		const POS_NAMES posEnd = NameIndex.end();
		const POS_NAMES pos = NameIndex.find( csName );
		if ( pos != posEnd )
		{
			lIndex = pos->second;
		}
		return lIndex;
	}
	// does a Name exist in the index?
	inline bool GetNameExists( LPCTSTR pcszName )
	{
		const LONG lIndex = NameFind( pcszName );
		return lIndex != -1;
	}
	// does an Name exist in the index?
	__declspec( property( get = GetNameExists ) )
		bool NameExists[];

	// add a schema Name to the index
	inline void NameAdd( LPCTSTR pcszName, LONG lIndex )
	{
		CString csName( pcszName );
		if ( !NameExists[ csName ] )
		{
			NameIndex.insert( PAIR_NAMES( csName, lIndex ) );
		}
	}

/////////////////////////////////////////////////////////////////////////////
// protected data
protected:
	// a collection of schema streams
	CSmartArray<CSchemaStream> m_SchemaStreams;
	// the hosting collection points back to the CSchemaColletion
	CSchemas* m_pHost;
	// name of the collection
	CString m_csName;
	// description of the collection
	CString m_csDescription;
	// name of the schema defining the collection
	CString m_csSchema;
	// comma separated string of unique stream names, where the combination of 
	// all of the streams define a unique record (GUID does not need to be 
	// defined here, because all records are unique by GUID)
	CString m_csUniqueKeys;

// public properties
public:
	// a collection of schema streams
	CSmartArray<CSchemaStream>& GetSchemaStreams()
	{
		return m_SchemaStreams;
	}
	// a collection of schema streams
	__declspec( property( get = GetSchemaStreams ) )
		CSmartArray<CSchemaStream>& SchemaStreams;

	// the hosting collection points back to the CSchemaColletion
	// and is untyped to avoid circular references
	CSchemas* GetHost()
	{
		return m_pHost;
	}
	// the hosting collection points back to the CSchemaColletion
	// and is untyped to avoid circular references
	void SetHost( CSchemas* value )
	{
		m_pHost = value;
	}
	// the hosting collection points back to the CSchemaColletion
	// and is untyped to avoid circular references
	__declspec( property( get = GetHost, put = SetHost ) )
		CSchemas* Host;

	// name of the collection
	inline CString GetName()
	{
		return m_csName;
	}
	// name of the collection
	inline void SetName( LPCTSTR value )
	{
		m_csName = value;
	}
	// name of the collection
	__declspec( property( get = GetName, put = SetName ) )
		CString Name;

	// description
	inline CString GetDescription(){
		return m_csDescription;
	}
	// description
	inline void SetDescription( LPCTSTR value )
	{
		m_csDescription = value;
	}
	// description
	__declspec( property( get = GetDescription, put = SetDescription ) )
		CString Description;

	// name of the schema defining the collection
	inline CString GetSchema()
	{
		return m_csSchema;
	}
	// name of the schema defining the collection
	inline void SetSchema( LPCTSTR value )
	{
		m_csSchema = value;
	}
	// name of the schema defining the collection
	__declspec( property( get = GetSchema, put = SetSchema ) )
		CString Schema;

	// comma separated string of unique stream names, where the combination of 
	// all of the streams define a unique record (GUID does not need to be 
	// defined here, because all records are unique by GUID)
	inline CString GetUniqueKeys()
	{
		return m_csUniqueKeys;
	}
	// comma separated string of unique stream names, where the combination of 
	// all of the streams define a unique record (GUID does not need to be 
	// defined here, because all records are unique by GUID)
	inline void SetUniqueKeys( LPCTSTR value )
	{
		m_csUniqueKeys = value;
	}
	// comma separated string of unique stream names, where the combination of 
	// all of the streams define a unique record (GUID does not need to be 
	// defined here, because all records are unique by GUID)
	__declspec( property( get = GetUniqueKeys, put = SetUniqueKeys ) )
		CString UniqueKeys;

// public methods
public:

// protected methods
protected:

	// constructor / destructor
public:
	CSchema();
	virtual ~CSchema();
};


