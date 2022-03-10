/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "XmlCollection.h"
#include "KeyedCollection.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
// a wrapper class for the units stored in the unit category XML definition
class CUnitCollection : public CXmlCollection
{
// public definitions
public:
	// intermediate cache that is used while reading XML
	class CCache
	{
	// protected data
	protected:
		// unit category associated with the record
		CString m_csUnitCategory;

		// name associated with the record
		CString m_csName;

		// factor the value has to be multiplied by in order to convert from 
		// internal units to this unit
		double m_dMultiplier;

		// amount that has to be added to convert from internal units to 
		// this unit
		double m_dAdditive;

	// public properties
	public:
		// unique key for this record
		inline CString GetKey()
		{
			CString value;
			value.Format( _T( "%s;%s" ), UnitCategory, Name );
			return value;
		}
		// unit category associated with the record
		__declspec( property( get = GetKey ) )
			CString Key;

		// unit category associated with the record
		inline CString GetUnitCategory()
		{
			return m_csUnitCategory;
		}
		// unit category associated with the record
		inline void SetUnitCategory( LPCTSTR value )
		{
			m_csUnitCategory = value;
		}
		// unit category associated with the record
		__declspec( property( get = GetUnitCategory, put = SetUnitCategory ) )
			CString UnitCategory;

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

		// factor the value has to be multiplied by in order to convert from 
		// internal units to this unit
		inline double GetMultiplier()
		{
			return m_dMultiplier;
		}
		// factor the value has to be multiplied by in order to convert from 
		// internal units to this unit
		inline void SetMultiplier( double value )
		{
			m_dMultiplier = value;
		}
		// factor the value has to be multiplied by in order to convert from 
		// internal units to this unit
		__declspec( property( get = GetMultiplier, put = SetMultiplier ) )
			double Multiplier;

		// amount that has to be added to convert from internal units to 
		// this unit
		inline double GetAdditive()
		{
			return m_dAdditive;
		}
		// amount that has to be added to convert from internal units to 
		// this unit
		inline void SetAdditive( double value )
		{
			m_dAdditive = value;
		}
		// amount that has to be added to convert from internal units to 
		// this unit
		__declspec( property( get = GetAdditive, put = SetAdditive ) )
			double Additive;

	// public methods
	public:
		// constructor
		CCache()
		{
			Multiplier = 1;
			Additive = 0;
		}
		// constructor
		CCache( CCache* pSrc )
		{
			Name = pSrc->Name;
			UnitCategory = pSrc->UnitCategory;
			Multiplier = pSrc->Multiplier;
			Additive = pSrc->Additive;
		}
	};

// protected data
protected:
	// cache of collection rows
	CKeyedCollection<CString,CCache> m_mapCache;

	// time when cache was populated
	ULONGLONG m_ullCacheTicks;

	// given a unit, quickly find the unit categories it is a member of
	CKeyedCollection<CString, vector<CString> > m_mapCrossReference;

	// time when Cross Reference was populated
	ULONGLONG m_ullCrossReferenceTicks;

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

	// time when Cross Reference was populated
	inline ULONGLONG GetCrossReferenceTicks()
	{
		return m_ullCrossReferenceTicks;
	}
	// time when Cross Reference was populated
	inline void SetCrossReferenceTicks( ULONGLONG value )
	{
		m_ullCrossReferenceTicks = value;
	}
	// time when Cross Reference was populated
	__declspec
	( 
		property( get = GetCrossReferenceTicks, 
		put = SetCrossReferenceTicks ) 
	) ULONGLONG CrossReferenceTicks;

	// the XML collection key
	const CString GetCollectionKey()
	{
		return _T( "Units" );
	}
	// the XML collection key
	__declspec( property( get = GetCollectionKey ) )
		CString CollectionKey;

	// the record key
	const CString GetRecordKey()
	{
		return _T( "Unit" );
	}
	// the record key
	__declspec( property( get = GetRecordKey ) )
		CString RecordKey;

	// unit category name associated with this record
	inline CString GetUnitCategory( long record )
	{
		ASSERT( ValidRecord[ record ] );
		const CString value = String[ record ][ _T( "UnitCategory" ) ];
		return value;
	}
	// unit category name associated with this record
	inline void SetUnitCategory( long record, LPCTSTR value )
	{
		ASSERT( ValidRecord[ record ] );
		String[ record ][ _T( "UnitCategory" ) ] = CString( value );
	}
	// unit category name associated with this record
	__declspec( property( get = GetUnitCategory, put = SetUnitCategory ) )
		CString UnitCategory[];

	// factor the value has to be multiplied by in order to convert from 
	// internal units to this unit
	inline double GetMultiplier( long record )
	{
		ASSERT( ValidRecord[ record ] );
		const double value = Double[ record ][ _T( "Multiplier" ) ];
		return value;
	}
	// factor the value has to be multiplied by in order to convert from 
	// internal units to this unit
	inline void SetMultiplier( long record, double value )
	{
		ASSERT( ValidRecord[ record ] );
		Double[ record ][ _T( "Multiplier" ) ] = value;
	}
	// factor the value has to be multiplied by in order to convert from 
	// internal units to this unit
	__declspec( property( get = GetMultiplier, put = SetMultiplier ) )
		double Multiplier[];

	// amount that has to be added to convert from internal units to 
	// this unit
	inline double GetAdditive( long record )
	{
		ASSERT( ValidRecord[ record ] );
		const double value = Double[ record ][ _T( "Additive" ) ];
		return value;
	}
	// amount that has to be added to convert from internal units to 
	// this unit
	inline void SetAdditive( long record, double value )
	{
		ASSERT( ValidRecord[ record ] );
		Double[ record ][ _T( "Additive" ) ] = value;
	}
	// amount that has to be added to convert from internal units to 
	// this unit
	__declspec( property( get = GetAdditive, put = SetAdditive ) )
		double Additive[];

	// return a vector of cache keys
	inline vector<CString> GetCacheKeys()
	{
		// make sure the cache is up-to-date
		CacheCollection();

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
			CacheCollection();
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
	inline bool AddCache( shared_ptr<CCache> pCache )
	{
		bool value = false;

		// this flag is set while the cache is being populated from the 
		// collection or from the XML
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

	// clear the XML cross reference
	virtual void ClearCrossReference()
	{
		m_mapCrossReference.clear();
	}

// public methods
public:
	// get a cache key
	inline CString GetCacheKey( CString csClass, CString csUnit )
	{
		CString value;
		value.Format( _T( "%s;%s" ), csClass, csUnit );
		return value;
	}
	// get a cache key
	__declspec( property( get = GetCacheKey ) )
		CString CacheKey[][];

	// import an XML cache into the collection
	bool ImportXML();

	// create the cache from the collection
	bool CacheCollection();

	// create a cross reference of the cache
	bool CrossReferenceCache();

	// find the cache for the unit category and unit name combination
	CCache* FindUnit( CString csClass, CString csUnit );

	// get a map of all unit categories the given unit belongs to
	bool FindUnitCategories
	( 
		CString csUnit, CKeyedCollection<CString, long>& mapClasses
	);

	// given two units, find a common unit category
	CString FindCommonUnitCategory( CString csUnit1, CString csUnit2 );

	// get the unit conversion factors to convert between the given 
	// units
	bool ConversionFactors
	(
		CCache* FromUnit,
		CCache* ToUnit,
		DOUBLE& Add,
		DOUBLE& Multiplier
	);

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
	// add a record to the collection from XML
	virtual bool AddRecordFromXML
	(
		CString csCollectionKey,
		CString csRecordKey,
		CKeyedCollection<CString,CString>& mapAttributes
	);

	// clear the XML cache
	virtual void ClearCache()
	{
		m_mapCache.clear();
	}

// public construction / destruction
public:
	CUnitCollection()
	{
	}
	virtual ~CUnitCollection();
};

