//
// Copyright 2007-2019 Baker Hughes Incorporated.  All rights reserved.
// This document contains confidential and proprietary information owned 
// by Baker Hughes Incorporated.
// Do not use, copy or distribute without permission.
//
#pragma once
#include "XmlTable.h"
#include "UnitTable.h"
#include "UnitAliasTable.h"
#include "UnitLongTable.h"
#include "KeyCollectionBase.h"

/////////////////////////////////////////////////////////////////////////////
// a wrapper class for the unit class XML definition
class CUnitClassTable : public CXmlTable
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

		// enumeration associated with the record
		short m_sEnum;

		// description associated with this name
		CString m_csDescription;

		// the units that are used to persist the data
		CString m_csUnits;

		// type associated with the name
		CString m_csType;

		// hidden from the UI
		bool m_bHide;

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

		// type associated with the name
		inline CString GetType()
		{
			return m_csType;
		}
		// type associated with the name
		inline void SetType( LPCTSTR value )
		{
			m_csType = value;
		}
		// type associated with the name
		__declspec( property( get = GetType, put = SetType ) )
			CString Type;

		// the units that are used to persist the data
		inline CString GetUnits()
		{
			return m_csUnits;
		}
		// the units that are used to persist the data
		inline void SetUnits( LPCTSTR value )
		{
			m_csUnits = value;
		}
		// the units that are used to persist the data
		__declspec( property( get = GetUnits, put = SetUnits ) )
			CString Units;

		// hidden from the UI
		inline bool GetHide()
		{
			return m_bHide;
		}
		// hidden from the UI
		inline void SetHide( bool value )
		{
			m_bHide = value;
		}
		// hidden from the UI
		__declspec( property( get = GetHide, put = SetHide ) )
			bool Hide;

		// enumeration associated with the record
		inline short GetEnum()
		{
			return m_sEnum;
		}
		// enumeration associated with the record
		inline void SetEnum( short value )
		{
			m_sEnum = value;
		}
		// enumeration associated with the record
		__declspec( property( get = GetEnum, put = SetEnum ) )
			short Enum;

	// public methods
	public:
		// constructor
		CCache()
		{
			Enum = -1;
			Hide = false;
		}
		// constructor
		CCache( CCache* pSrc )
		{
			Name = pSrc->Name;
			Description = pSrc->Description;
			Type = pSrc->Type;
			Hide = pSrc->Hide;
			Enum = pSrc->Enum;
		}
	};

// protected data
protected:
	// cache of table rows
	CKeyCollectionBase<CCache> m_mapCache;

	// time when cache was populated
	ULONGLONG m_ullCacheTicks;

	// cross reference enumerations to class name
	CKeyCollectionBase<CString> m_mapEnum;

	// time when enumeration cross reference was populated
	ULONGLONG m_ullEnumerationTicks;

	// the units for each unit class
	unique_ptr<CUnitTable> m_spUnitTable;

	// the unit alias table
	unique_ptr<CUnitAliasTable> m_spUnitAliasTable;

	// the long unit name table
	unique_ptr<CUnitLongTable> m_spUnitLongTable;
	
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

	// time when enumeration cross reference was populated
	inline ULONGLONG GetEnumerationTicks()
	{
		return m_ullEnumerationTicks;
	}
	// time when enumeration cross reference was populated
	inline void SetEnumerationTicks( ULONGLONG value )
	{
		m_ullEnumerationTicks = value;
	}
	// time when enumeration cross reference was populated
	__declspec( property( get = GetEnumerationTicks, put = SetEnumerationTicks ) )
		ULONGLONG EnumerationTicks;

	// embedded unit table rows are linked to this table via UNTCLASS field
	inline CUnitTable* GetEmbeddedTable()
	{
		return m_spUnitTable.get();
	}
	// embedded unit table rows are linked to this table via UNTCLASS field
	__declspec( property( get = GetEmbeddedTable ) )
		CUnitTable* EmbeddedTable;

	// the unit alias table
	inline CUnitAliasTable* GetUnitAliasTable()
	{
		return m_spUnitAliasTable.get();
	}
	// the unit alias table
	__declspec( property( get = GetUnitAliasTable ) )
		CUnitAliasTable* UnitAliasTable;

	// the long unit name table
	inline CUnitLongTable* GetUnitLongTable()
	{
		return m_spUnitLongTable.get();
	}
	// the long unit name table
	__declspec( property( get = GetUnitLongTable ) )
		CUnitLongTable* UnitLongTable;

	// the XML table key
	const CString GetTableKey()
	{
		return _T( "UnitClasses" );
	}
	// the XML table key
	__declspec( property( get = GetTableKey ) )
		CString TableKey;

	// the row key
	const CString GetRowKey()
	{
		return _T( "UnitClass" );
	}
	// the row key
	__declspec( property( get = GetRowKey ) )
		CString RowKey;

	// the units that are used to persist the data
	inline CString GetUnits( short record )
	{
		ASSERT( ValidRecord( record ) );
		const CString value = String[ record ][ _T( "UNITS" ) ];
		return value;
	}
	// the units that are used to persist the data
	inline void SetUnits( short record, LPCTSTR value )
	{
		ASSERT( ValidRecord( record ) );
		String[ record ][ _T( "UNITS" ) ] = CString( value );
	}
	// the units that are used to persist the data
	__declspec( property( get = GetUnits, put = SetUnits ) )
		CString Units[];

	// type associated with the name
	inline CString GetType( short record )
	{
		ASSERT( ValidRecord( record ) );
		const CString value = String[ record ][ _T( "TYPE" ) ];
		return value;
	}
	// type associated with the name
	inline void SetType( short record, LPCTSTR value )
	{
		ASSERT( ValidRecord( record ) );
		String[ record ][ _T( "TYPE" ) ] = CString( value );
	}
	// type associated with the name
	__declspec( property( get = GetType, put = SetType ) )
		CString Type[];

	// hidden from the UI
	inline bool GetHide( short record )
	{
		ASSERT( ValidRecord( record ) );
		const short sValue = Short[ record ][ _T( "HIDE" ) ];
		return sValue != 0;
	}
	// hidden from the UI
	inline void SetHide( short record, bool value )
	{
		ASSERT( ValidRecord( record ) );
		const short sValue = value ? 1 : 0;
		Short[ record ][ _T( "HIDE" ) ] = sValue;
	}
	// hidden from the UI
	__declspec( property( get = GetHide, put = SetHide ) )
		bool Hide[];

	// enumeration associated with the record
	inline short GetEnum( short record )
	{
		ASSERT( ValidRecord( record ) );
		const short value = Short[ record ][ _T( "ENUM" ) ];
		return value;
	}
	// enumeration associated with the record
	inline void SetEnum( short record, short value )
	{
		ASSERT( ValidRecord( record ) );
		Short[ record ][ _T( "ENUM" ) ] = value;
	}
	// enumeration associated with the record
	__declspec( property( get = GetEnum, put = SetEnum ) )
		short Enum[];

	// return a vector of cache keys
	inline vector<CString> GetCacheKeys()
	{
		// insure the cache is up-to-date
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

	// return a class name for the given enumeration
	inline CString GetEnumClass( short sEnum )
	{
		CString value;

		// make sure the cross reference is up-to-date
		CrossReferenceEnumerations();

		// get a key for the enumeration
		const CString csKey = EnumKey[ sEnum ];

		// if the enumeration is in the cross reference
		if ( m_mapEnum.Exists[ csKey ] )
		{
			// lookup the class name for the enumeration
			value = *m_mapEnum.find( csKey );
		}

		return value;
	}
	// return a class name for the given enumeration
	__declspec( property( get = GetEnumClass ) )
		CString EnumClass[];

// protected methods
protected:
	// return a key for the given enumeration
	inline CString GetEnumKey( short sEnum )
	{
		CString value;
		value.Format( _T( "%05d" ), sEnum );
		return value;
	}
	// return a key for the given enumeration
	__declspec( property( get = GetEnumKey ) )
		CString EnumKey[];

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
	bool CacheTable();

	// cross reference the enumerations to class names
	bool CrossReferenceEnumerations();

	// find the unit class
	CCache* FindUnitClass( VARIANT& uc );

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
	// clear the XML cache
	virtual void ClearCache()
	{
		m_mapCache.clear();
		EmbeddedTable->ClearCache();
		UnitAliasTable->ClearCache();
		UnitLongTable->ClearCache();
	}

// public construction / destruction
public:
	CUnitClassTable
	( 
		IUnknownPtr spUnk, // this table's interface
		// embedded (links to row of this table via unit class) 
		// table's interface
		IUnknownPtr spEmb,
		IUnknownPtr spAlias, // alias table's interface
		IUnknownPtr spLong // long name table's interface
	);
	virtual ~CUnitClassTable();
};

