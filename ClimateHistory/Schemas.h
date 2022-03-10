/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "KeyedCollection.h"
#include "SmartArray.h"
#include "Schema.h"
#include <map>
#include <atlbase.h>
#include "xmllite.h"
#include <strsafe.h>
#include <memory>

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CSchemas 
class CSchemas
{
/////////////////////////////////////////////////////////////////////////////
// protected Schema Name index data
protected:
	// index of schema Names
	CKeyedCollection<CString,long> m_indexNames;

// public Schema Name index attributes
public:
	// Schema Name index count
	inline LONG GetNameCount()
	{ 
		return (LONG)m_indexNames.Count; 
	}
	// Schema Name index count
	__declspec( property( get = GetNameCount ) )
		LONG NameCount;

// public index Schema Name methods
public:
	// clear index
	inline void nameClear()
	{
		m_indexNames.clear();
	}

	// find schema by its Name and return its index into the collection
	inline LONG nameFind( LPCTSTR pcszName )
	{
		long value = -1;
		CString csName( pcszName );
		if ( m_indexNames.Exists[ csName ] )
		{
			value = *m_indexNames.find( csName );
		}
		return value;
	}

	// does an Name exist in the index?
	inline bool nameExists( LPCTSTR pcszName )
	{
		CString csName( pcszName );
		const bool value = m_indexNames.Exists[ csName ];
		return value;
	}

	// add a schema Name to the index
	inline void nameAdd( LPCTSTR pcszName, LONG lIndex )
	{
		CString csName( pcszName );
		if ( !nameExists( csName ) )
		{
			shared_ptr<LONG> pValue = shared_ptr<LONG>( new LONG( lIndex ));
			m_indexNames.add( csName, pValue );
		}
	}
	// get an array of schema names as CStrings
	inline LONG nameSchemas( vector<CString>& arrSchemas )
	{
		arrSchemas.clear();
		for ( auto& node : m_indexNames.Items )
		{
			const CString csName = node.first;
			arrSchemas.push_back( csName );
		}
		return (LONG)arrSchemas.size();
	}

/////////////////////////////////////////////////////////////////////////////
// protected data
protected:
	// location of the schema file
	CString m_csSchemaPath;

	// array of CSchema objects
	CSmartArray<CSchema> m_arrSchema;

// public properties
public:
	// location of the schema file
	CString GetSchemaPath()
	{ 
		return m_csSchemaPath; 
	}
	// location of the schema file
	void SetSchemaPath( LPCTSTR value )
	{
		m_csSchemaPath = value; 
	}
	// location of the schema file
	__declspec( property( get = GetSchemaPath, put = SetSchemaPath ) )
		CString SchemaPath;
	
	// array of CSchema objects
	CSmartArray<CSchema>& GetSchemas()
	{ 
		return m_arrSchema;
	}
	// array of CSchema objects
	__declspec( property( get = GetSchemas ) )
		CSmartArray<CSchema>& Schemas;
	
/////////////////////////////////////////////////////////////////////////////
// XML Parsing protected types
protected:
	// key / value pairs
	typedef pair<CString,CString> KEY_VALUE_PAIR;
	// map key / value pairs
	typedef map<CString,CString> MAP_KEY_VALUE_PAIR;
	// iterator key / value pairs
	typedef MAP_KEY_VALUE_PAIR::iterator POS_KEY_VALUE_PAIR;
	// XML parsing states
	typedef enum 
	{	stateInit, // starting state
		stateCollections, // collecting collection schemas
		stateStreams, // collecting streams for a collection schema
	} XML_PARSE_STATES;
	
// XML Parsing protected data
protected:
	// XML parsing state
	XML_PARSE_STATES m_stateXML;
	
// XML Parsing public attributes
public:
	// XML parsing state
	XML_PARSE_STATES GetStateXML()
	{ 
		return m_stateXML; 
	}
	// XML parsing state
	void SetStateXML( XML_PARSE_STATES value )
	{ 
		m_stateXML = value; 
	}
	__declspec( property( get = GetStateXML, put = SetStateXML ) )
		XML_PARSE_STATES StateXML;
	
// XML Parsing protected methods
protected:
	// collect XML attributes into a vector of strings organized as key/value pairs
	HRESULT ReadXmlAttributes( IXmlReader* pReader, MAP_KEY_VALUE_PAIR& mapPairs );
	// get attribute by name
	inline static CString GetXmlAttribute( LPCTSTR pcszName, MAP_KEY_VALUE_PAIR& mapPairs )
	{	CString csValue;
		const POS_KEY_VALUE_PAIR posEnd = mapPairs.end();
		const POS_KEY_VALUE_PAIR pos = mapPairs.find( pcszName );
		if ( pos != posEnd )
		{	csValue = pos->second;
		}
		return csValue;
	}
	// is the given attribute name defined
	inline static bool AttributeDefined( LPCTSTR pcszName, MAP_KEY_VALUE_PAIR& mapPairs )
	{	CString csValue;
		const POS_KEY_VALUE_PAIR posEnd = mapPairs.end();
		const POS_KEY_VALUE_PAIR pos = mapPairs.find( pcszName );
		const bool bDefined = pos != posEnd;
		return bDefined;
	}
	
// public methods
public:
	// open the schema path
	bool OpenSchemas( LPCTSTR path );
	
// constructor / destructor
public:
	CSchemas();
	virtual ~CSchemas();
};
