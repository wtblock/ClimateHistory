//
// Copyright 2007-2019 Baker Hughes Incorporated.  All rights reserved.
// This document contains confidential and proprietary information owned 
// by Baker Hughes Incorporated.
// Do not use, copy or distribute without permission.
//
#pragma once
#include "XmlTable.h"
#include "KeyCollectionBase.h"

/////////////////////////////////////////////////////////////////////////////
// a wrapper class for the sets stored in the unit set XML files
class CUnitSetTable : public CXmlTable
{
// public definitions
public:
	// intermediate cache that is used while reading XML
	class CCache
	{
	// protected data
	protected:
		// unit set associated with the record
		CString m_csUnitSet;

		// unit class associated with the record
		CString m_csUnitClass;

		// the unit being used for this unit set and unit class combination
		CString m_csUnit;

		// number of digits displayed after the decimal point
		short m_sDecimals;

		// number of digits displayed before the decimal point
		short m_sWidth;

	// public properties
	public:
		// unique key for this record
		inline CString GetKey()
		{
			CString value;
			value.Format( _T( "%s;%s" ), UnitSet, UnitClass );
			return value;
		}
		// unit class associated with the record
		__declspec( property( get = GetKey ) )
			CString Key;

		// unit set associated with the record
		inline CString GetUnitSet()
		{
			return m_csUnitSet;
		}
		// unit set associated with the record
		inline void SetUnitSet( LPCTSTR value )
		{
			m_csUnitSet = value;
		}
		// unit set associated with the record
		__declspec( property( get = GetUnitSet, put = SetUnitSet ) )
			CString UnitSet;

		// unit class associated with the record
		inline CString GetUnitClass()
		{
			return m_csUnitClass;
		}
		// unit class associated with the record
		inline void SetUnitClass( LPCTSTR value )
		{
			m_csUnitClass = value;
		}
		// unit class associated with the record
		__declspec( property( get = GetUnitClass, put = SetUnitClass ) )
			CString UnitClass;

		// the unit being used for this unit set and unit class combination
		inline CString GetUnit()
		{
			return m_csUnit;
		}
		// the unit being used for this unit set and unit class combination
		inline void SetUnit( LPCTSTR value )
		{
			m_csUnit = value;
		}
		// the unit being used for this unit set and unit class combination
		__declspec( property( get = GetUnit, put = SetUnit ) )
			CString Unit;

		// number of digits displayed after the decimal point
		inline short GetDecimals()
		{
			return m_sDecimals;
		}
		// number of digits displayed after the decimal point
		inline void SetDecimals( short value )
		{
			m_sDecimals = value;
		}
		// number of digits displayed after the decimal point
		__declspec( property( get = GetDecimals, put = SetDecimals ) )
			short Decimals;

		// number of digits displayed before the decimal point
		inline short GetWidth()
		{
			return m_sWidth;
		}
		// number of digits displayed before the decimal point
		inline void SetWidth( short value )
		{
			m_sWidth = value;
		}
		// number of digits displayed before the decimal point
		__declspec( property( get = GetWidth, put = SetWidth ) )
			short Width;

	// public methods
	public:
		// constructor
		CCache()
		{
			Decimals = 2;
			Width = 8;
		}
		// constructor
		CCache( CCache* pSrc )
		{
			UnitSet = pSrc->UnitSet;
			UnitClass = pSrc->UnitClass;
			Unit = pSrc->Unit;
			Decimals = pSrc->Decimals;
			Width = pSrc->Width;
		}
	};

// protected data
protected:
	// cache of table rows
	CKeyCollectionBase<CCache> m_mapCache;

	// time when cache was populated
	ULONGLONG m_ullCacheTicks;

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

	// the table key of the XML
	const CString GetTableKey()
	{
		return _T( "UnitSet" );
	}
	// the table key of the XML
	__declspec( property( get = GetTableKey ) )
		CString TableKey;

	// the row key
	const CString GetRowKey()
	{
		return _T( "Unit" );
	}
	// the row key
	__declspec( property( get = GetRowKey ) )
		CString RowKey;

	// unit class name associated with this record
	inline CString GetUnitClass( short record )
	{
		ASSERT( ValidRecord( record ) );
		const CString value = String[ record ][ _T( "UNTCLASS" ) ];
		return value;
	}
	// unit class name associated with this record
	inline void SetUnitClass( short record, LPCTSTR value )
	{
		ASSERT( ValidRecord( record ) );
		String[ record ][ _T( "UNTCLASS" ) ] = CString( value );
	}
	// unit class name associated with this record
	__declspec( property( get = GetUnitClass, put = SetUnitClass ) )
		CString UnitClass[];

	// the unit being used for this unit set and unit class combination
	inline CString GetUnit( short record )
	{
		ASSERT( ValidRecord( record ) );
		const CString value = String[ record ][ _T( "UNIT" ) ];
		return value;
	}
	// the unit being used for this unit set and unit class combination
	inline void SetUnit( short record, LPCTSTR value )
	{
		ASSERT( ValidRecord( record ) );
		String[ record ][ _T( "UNIT" ) ] = CString( value );
	}
	// the unit being used for this unit set and unit class combination
	__declspec( property( get = GetUnit, put = SetUnit ) )
		CString Unit[];

	// number of digits displayed after the decimal point
	inline short GetDecimals( short record )
	{
		ASSERT( ValidRecord( record ) );
		const short value = Short[ record ][ _T( "DECIMALS" ) ];
		return value;
	}
	// number of digits displayed after the decimal point
	inline void SetDecimals( short record, short value )
	{
		ASSERT( ValidRecord( record ) );
		Short[ record ][ _T( "DECIMALS" ) ] = value;
	}
	// number of digits displayed after the decimal point
	__declspec( property( get = GetDecimals, put = SetDecimals ) )
		short Decimals[];

	// number of digits displayed before the decimal point
	inline short GetWidth( short record )
	{
		ASSERT( ValidRecord( record ) );
		const short value = Short[ record ][ _T( "WIDTH" ) ];
		return value;
	}
	// number of digits displayed before the decimal point
	inline void SetWidth( short record, short value )
	{
		ASSERT( ValidRecord( record ) );
		Short[ record ][ _T( "WIDTH" ) ] = value;
	}
	// number of digits displayed before the decimal point
	__declspec( property( get = GetWidth, put = SetWidth ) )
		short Width[];

	// return a vector of cache keys
	inline vector<CString> GetCacheKeys()
	{
		if ( m_mapCache.Count == 0 )
		{
			CacheTable();
		}

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
	// get a cache key
	inline CString GetCacheKey( CString csSet, CString csClass )
	{
		CString value;
		value.Format( _T( "%s;%s" ), csSet, csClass );
		return value;
	}
	// get a cache key
	__declspec( property( get = GetCacheKey ) )
		CString CacheKey[][];

	// import an XML cache into the table
	bool ImportXML();

	// create the cache from the table
	bool CacheTable( bool bForce = false );

	// find the cache for the unit set and unit class combination
	CCache* FindUnitSet( CString csSet, CString csClass );

// protected overrides
protected:
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
	// add a record to the table from XML
	virtual bool AddRecordFromXML
	(
		CString csTableKey,
		CString csRowKey,
		CKeyCollectionBase<CString>& mapAttributes
	);

	// the entire record is read only
	virtual bool GetReadOnly( int record );
	// the entire record is read only
	virtual void SetReadOnly( int record, bool value = true );
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
	CUnitSetTable()
	{
	}
	CUnitSetTable( IUnknownPtr spUnk );
	virtual ~CUnitSetTable();
};

