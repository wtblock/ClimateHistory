//
// Copyright 2007-2019 Baker Hughes Incorporated.  All rights reserved.
// This document contains confidential and proprietary information owned 
// by Baker Hughes Incorporated.
// Do not use, copy or distribute without permission.
//
#pragma once
#include "XmlTable.h"
#include "KeyCollectionBase.h"
#include "UnitSetTable.h"
#include <memory>

/////////////////////////////////////////////////////////////////////////////
// a wrapper class for the unit sets XML definitions
class CUnitSetsTable : public CXmlTable
{
// public definitions
public:
	// intermediate cache that is used while reading XML
	class CCache
	{
	// protected data
	protected:
		// name associated with the record
		CString m_csName;

		// description associated with this name
		CString m_csDescription;

		// the unit set is supplied by the system unit set XML
		bool m_bSystem;

		// the unit set cannot be modified
		bool m_bEditable;

	// public properties
	public:
		// unique key for this record
		inline CString GetKey()
		{
			const CString value = Name;
			return value;
		}
		// unique key for this record
		__declspec( property( get = GetKey ) )
			CString Key;

		// name associated with the record
		inline CString GetName()
		{
			return m_csName;
		}
		// name associated with the record
		inline void SetName( LPCTSTR value )
		{
			m_csName = value;
		}
		// name associated with the record
		__declspec( property( get = GetName, put = SetName ) )
			CString Name;

		// description associated with this name
		inline CString GetDescription()
		{
			return m_csDescription;
		}
		// description associated with this name
		inline void SetDescription( CString value )
		{
			m_csDescription = value;
		}
		// description associated with this name
		__declspec( property( get = GetDescription, put = SetDescription ) )
			CString Description;

		// the unit set is supplied by the system unit set XML
		inline bool GetSystem()
		{
			return m_bSystem;
		}
		// the unit set is supplied by the system unit set XML
		inline void SetSystem( bool value )
		{
			m_bSystem = value;
		}
		// the unit set is supplied by the system unit set XML
		__declspec( property( get = GetSystem, put = SetSystem ) )
			bool System;

		// the unit set cannot be modified
		inline bool GetEditable()
		{
			return m_bEditable;
		}
		// the unit set cannot be modified
		inline void SetEditable( bool value )
		{
			m_bEditable = value;
		}
		// the unit set cannot be modified
		__declspec( property( get = GetEditable, put = SetEditable ) )
			bool Editable;

	// public methods
	public:
		// constructor
		CCache()
		{
			System = false;
			Editable = false;
		}
		// constructor
		CCache( CCache* pSrc )
		{
			Name = pSrc->Name;
			Description = pSrc->Description;
			System = pSrc->System;
			Editable = pSrc->Editable;
		}
	};

// protected data
protected:
	// cache of table rows
	CKeyCollectionBase<CCache> m_mapCache;

	// the units for each unit class
	unique_ptr<CUnitSetTable> m_spUnitSetTable;

	// time when cache was populated
	ULONGLONG m_ullCacheTicks;

	// the current customization string can be one of the following :
	// o "none" is for Cadence
	// o "advantage" 
	// o "atlas"
	CString m_csCustomization;

// public properties
public:
	// time when cache was populated
	inline ULONGLONG GetCacheTicks()
	{
		return m_ullCacheTicks;
	}
	// time when cache was populated
	inline void SetCacheTicks( ULONGLONG value )
	{
		m_ullCacheTicks = value;
	}
	// time when cache was populated
	__declspec( property( get = GetCacheTicks, put = SetCacheTicks ) )
		ULONGLONG CacheTicks;

	// the current customization string can be one of the following :
	// o "none" is for Cadence
	// o "advantage" 
	// o "atlas"
	inline CString GetCustomization()
	{
		return m_csCustomization;
	}
	// the current customization string can be one of the following :
	// o "none" is for Cadence
	// o "advantage" 
	// o "atlas"
	inline void SetCustomization( LPCTSTR value )
	{
		m_csCustomization = value;
	}
	// the current customization string can be one of the following :
	// o "none" is for Cadence
	// o "advantage" 
	// o "atlas"
	__declspec( property( get = GetCustomization, put = SetCustomization ) )
		CString Customization;

	// embedded unit table rows are linked to this table via UNTCLASS field
	inline CUnitSetTable* GetEmbeddedTable()
	{
		return m_spUnitSetTable.get();
	}
	// embedded unit table rows are linked to this table via UNTCLASS field
	__declspec( property( get = GetEmbeddedTable ) )
		CUnitSetTable* EmbeddedTable;

	// the table key of the XML
	const CString GetTableKey()
	{
		return _T( "UnitSets" );
	}
	// the table key of the XML
	__declspec( property( get = GetTableKey ) )
		CString TableKey;

	// the row key of the XML
	const CString GetRowKey()
	{
		return _T( "UnitSet" );
	}
	// the row key of the XML
	__declspec( property( get = GetRowKey ) )
		CString RowKey;

	// the unit set is supplied by the system unit set XML
	inline bool GetSystem( short record )
	{
		ASSERT( ValidRecord( record ) );
		const short sValue = Short[ record ][ _T( "SYSTEM" ) ];
		return sValue != 0;
	}
	// the unit set is supplied by the system unit set XML
	inline void SetSystem( short record, bool value )
	{
		ASSERT( ValidRecord( record ) );
		const short sValue = value ? 1 : 0;
		Short[ record ][ _T( "SYSTEM" ) ] = sValue;
	}
	// the unit set is supplied by the system unit set XML
	__declspec( property( get = GetSystem, put = SetSystem ) )
		bool System[];

	// the unit set cannot be modified
	inline bool GetEditable( short record )
	{
		ASSERT( ValidRecord( record ) );
		const short sValue = Short[ record ][ _T( "EDITABLE" ) ];
		return sValue != 0;
	}
	// the unit set cannot be modified
	inline void SetEditable( short record, bool value )
	{
		ASSERT( ValidRecord( record ) );
		const short sValue = value ? 1 : 0;
		Short[ record ][ _T( "EDITABLE" ) ] = sValue;
	}
	// the unit set cannot be modified
	__declspec( property( get = GetEditable, put = SetEditable ) )
		bool Editable[];

	// return a vector of cache keys
	inline vector<CString> GetCacheKeys()
	{
		// make sure the cache is up-to-date
		CacheTable();

		vector<CString> value;
		for ( auto& node : m_mapCache.Items )
		{
			value.push_back( node.first );
		}

		return value;
	}
	// return a vector of cache keys
	__declspec( property( get = GetCacheKeys ) )
		vector<CString> CacheKeys;

// protected methods
protected:
	// test to see if the key exists
	inline bool GetKeyExists( LPCTSTR key )
	{
		bool value = false;
		const bool bCaching = Caching;

		// make sure the cache is up-to-date if we are not currently caching
		if ( !bCaching )
		{
			CacheTable();
		}

		if ( m_mapCache.Exists[ key ] )
		{
			value = true;
		}

		return value;
	}
	// test to see if the key exists
	__declspec( property( get = GetKeyExists ) )
		bool KeyExists[];

	// add a cache instance to the collection
	inline bool AddCache( CCache* pCache )
	{
		bool value = false;

		// this flag is set while the cache is being populated from the 
		// table or from the XML
		Caching = true;

		// if the key exists, remove it
		const CString csKey = pCache->Key;
		if ( KeyExists[ csKey ] )
		{
			m_mapCache.remove( csKey );
		}

		// add the new item
		value = m_mapCache.add( csKey, pCache );

		// indicate that the cache is not being created
		Caching = false;

		return value;
	}

// public methods
public:
	// import an XML cache into the table
	bool ImportXML();

	// create the cache from the table
	bool CacheTable( bool bForce = false );

// protected overrides
protected:
	// add a record to the table from XML
	virtual bool AddRecordFromXML
	(
		CString csTableKey,
		CString csRowKey,
		CKeyCollectionBase<CString>& mapAttributes
	);

	// read the given XML file into the cache
	virtual bool ReadXML
	(
		CComPtr<IStream>& pFileStream,
		CComPtr<IXmlReader>& pReader,
		LPCTSTR path
	);

	// write the given XML file
	virtual bool WriteXML
	(
		CComPtr<IStream>& pFileStream,
		CComPtr<IXmlWriter>& pWriter,
		LPCTSTR path
	);

// public overrides
public:
	// the entire record is read only
	virtual bool GetReadOnly( int record )
	{
		bool value = false;
		const bool bEditable = Editable[ record ];
		if ( bEditable == false )
		{
			value = true;
		}
		const bool bSystem = System[ record ];
		if ( bSystem == true )
		{
			value = true;
		}

		return value;
	}
	// the entire record is read only
	virtual void SetReadOnly( int record, bool value = true )
	{
		Editable[ record ] = value;
	}
	// the entire record is read only
	__declspec( property( get = GetReadOnly, put = SetReadOnly ) )
		bool ReadOnly[];

	// clear the XML cache
	virtual void ClearCache()
	{
		m_mapCache.clear();
	}

// public construction / destruction
public:
	CUnitSetsTable()
	{
	}
	CUnitSetsTable
	( 
		IUnknownPtr spUnk, 
		// embedded (links to row of this table via unit class) 
		// table's interface
		IUnknownPtr spEmb
	);
	virtual ~CUnitSetsTable();
};

