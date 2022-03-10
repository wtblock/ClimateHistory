/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UnitCollection.h"

/////////////////////////////////////////////////////////////////////////////
CUnitCollection::CUnitCollection()
{
	CacheTicks = 0;
	CrossReferenceTicks = 0;
	Caching = false;

} // CUnitCollection

/////////////////////////////////////////////////////////////////////////////
CUnitCollection::~CUnitCollection()
{
} // ~CUnitCollection

/////////////////////////////////////////////////////////////////////////////
// create the cache from the table
bool CUnitCollection::CacheCollection()
{
	bool value = false;

	// if the cache was created since the table was last modified
	// there is nothing to do
	const ULONGLONG ullCache = CacheTicks;
	const ULONGLONG ullTable = LastModifyTimeTicks;
	if ( ullCache >= ullTable )
	{
		return value;
	}

	// start fresh
	ClearCache();

	value = true;

	// create the cache
	vector<long> arrRows = AllRecordNumbers;
	for ( long sRow : arrRows )
	{
		shared_ptr<CCache> pCache = shared_ptr<CCache>( new CCache );

		pCache->Name = Name[ sRow ];
		pCache->UnitCategory = UnitCategory[ sRow ];

		pCache->Multiplier = Multiplier[ sRow ];
		pCache->Additive = Additive[ sRow ];

		// record the unit class properties in the cache
		AddCache( pCache );
	}

	// time stamp the cache
	CacheTicks = ::GetTickCount64();

	return value;
} // CacheTable

/////////////////////////////////////////////////////////////////////////////
// create a cross reference of the cache
bool CUnitCollection::CrossReferenceCache()
{
	bool value = false;

	// if the cache was created since the table was last modified
	// there is nothing to do
	const ULONGLONG ullCrossReference = CrossReferenceTicks;
	const ULONGLONG ullTable = LastModifyTimeTicks;
	if ( ullCrossReference >= ullTable )
	{
		return value;
	}

	// start fresh
	ClearCrossReference();

	value = true;

	// make sure the cache is up-to-date
	CacheCollection();

	// loop through the entire cache and build the cross reference
	for ( auto& node : m_mapCache.Items )
	{
		CCache* pCache = node.second;
		const CString csUnit = pCache->Name;
		const CString csCategory = pCache->UnitCategory;
		vector<CString>* pCategories = nullptr;

		// if the unit is already in the map
		if ( m_mapCrossReference.Exists[ csUnit ] )
		{
			// get the existing array of classes associated with the unit
			pCategories = m_mapCrossReference.find( csUnit );

		} else // create new unit entry
		{
			// add a new unit class vector
			pCategories = new vector<CString>;

			// add the unit to the cross reference
			m_mapCrossReference.add( csUnit, pCategories );
		}

		// add the class that is associated with the unit to the unit's 
		// collection
		pCategories->push_back( csCategory );
	}

	// time stamp the cache
	CrossReferenceTicks = ::GetTickCount64();

	return value;
} // CrossReferenceTable

/////////////////////////////////////////////////////////////////////////////
// find the cache for the unit class and unit name combination
CUnitCollection::CCache* CUnitCollection::FindUnit
(
	CString csCategory, CString csUnit
)
{
	CUnitCollection::CCache* value = nullptr;

	// create a key from the input parameters
	CString csKey = CacheKey[ csCategory ][ csUnit ];

	// test to see if the key exists in the cache and make sure the
	// cache is up-to-date
	if ( KeyExists[ csKey ] )
	{
		value = m_mapCache.find( csKey );

	} else // try the lower case unit
	{
		// create a key from the using the lower case unit
		csKey = CacheKey[ csCategory ][ csUnit.MakeLower() ];

		// test to see if the key exists in the cache and make sure the
		// cache is up-to-date
		if ( KeyExists[ csKey ] )
		{
			value = m_mapCache.find( csKey );
		}
	}

	return value;
} // FindUnit

/////////////////////////////////////////////////////////////////////////////
// get a map of all unit classes the given unit belongs to
bool CUnitCollection::FindUnitCategories
( 
	CString csUnit, CKeyedCollection<CString,long>& mapCategories
)
{
	bool value = false;

	// make sure the cache is up-to-date
	CrossReferenceCache();

	// start fresh 
	mapCategories.clear();

	// see if the unit is cross referenced
	shared_ptr<vector<CString> > pCategories = m_mapCrossReference.find( csUnit );
	value = pCategories != nullptr;

	if ( value == true )
	{
		for ( CString csCategory : *pCategories )
		{
			mapCategories.add( csCategory, new long( 0 ));
		}
	} else // try the lower case version of the unit
	{
		pCategories = m_mapCrossReference.find( csUnit.MakeLower() );
		value = pCategories != nullptr;

		if ( value == true )
		{
			for ( CString csCategory : *pCategories )
			{
				mapCategories.add( csCategory, new long( 0 ) );
			}
		}
	}

	return value;
} // FindUnitCategories

/////////////////////////////////////////////////////////////////////////////
// given two units, find a common unit class
CString CUnitCollection::FindCommonUnitCategory( CString csUnit1, CString csUnit2 )
{
	CString value;

	CKeyedCollection<CString,long> mapUnit1, mapUnit2;

	// map the classes for unit 1
	if ( !FindUnitCategories( csUnit1, mapUnit1 ))
	{
		return value;
	}

	// map the classes for unit 2
	if ( !FindUnitCategories( csUnit2, mapUnit2 ))
	{
		return value;
	}

	// loop through the unit 1 classes
	for ( auto& node : mapUnit1.Items )
	{
		const CString csCategory1 = node.first;

		// test to see if the unit 1 class is in the unit 2 map
		if ( mapUnit2.Exists[ csCategory1 ] )
		{
			// we found a matching class
			value = csCategory1;
			break;
		}
	}

	return value;
} // FindCommonUnitCategory

/////////////////////////////////////////////////////////////////////////////
// get the unit conversion factors to convert between the given units
bool CUnitCollection::ConversionFactors
(
	CCache* pFrom,
	CCache* pTo,
	DOUBLE& Add,
	DOUBLE& Multiplier
)
{
	Add = 0;
	Multiplier = 0;

	bool value = false;

	if ( pFrom == nullptr || pTo == nullptr )
	{
		return value;
	}

	// lookup the conversions for each unit
	const double dFromAdd = pFrom->Additive;
	const double dFromMult = pFrom->Multiplier;
	const double dToAdd = pTo->Additive;
	const double dToMult = pTo->Multiplier;
	
	value = dFromMult != 0;

	if ( value )
	{
		// additive values are stored in the XML in internal units
		// so they need to be subtracted prior to multiplication by the 
		// dToMult (which performs the unit conversion)
		Add = ( dToAdd - dFromAdd ) * dToMult;
		Multiplier = dToMult / dFromMult;
	}
	
	return value;
} // ConversionFactors

/////////////////////////////////////////////////////////////////////////////
// add a record to the table from XML
bool CUnitCollection::AddRecordFromXML
(
	CString csTableKey,
	CString csRowKey,
	CKeyedCollection<CString,CString>& mapAttributes
)
{
	bool value = false;

	// this is the XML node name of our table
	const CString csAliasTableKey = TableKey;
	if ( csAliasTableKey != csTableKey )
	{
		return value;
	}

	// this is the XML node name of our table's row
	const CString csUnitRowKey = RowKey;

	// reading table row properties
	if ( csRowKey == csUnitRowKey )
	{
		// create a new cache for this record
		CCache* pCache = new CCache();

		const CString csName =
			GetXmlAttribute( _T( "Name" ), mapAttributes );
		const CString csRP66 =
			GetXmlAttribute( _T( "rp66Unit" ), mapAttributes );
		const CString csWitsml =
			GetXmlAttribute( _T( "witsmlUnit" ), mapAttributes );
		const CString csPow =
			GetXmlAttribute( _T( "Power" ), mapAttributes );
		const CString csMult =
			GetXmlAttribute( _T( "Multiplier" ), mapAttributes );
		const CString csAdd =
			GetXmlAttribute( _T( "Additive" ), mapAttributes );
		const CString csDec = GetXmlAttribute
		(
			_T( "DefaultDecimals" ), mapAttributes
		);
		const CString csWid = GetXmlAttribute
		(
			_T( "DefaultDisplayWidth" ), mapAttributes
		);
		const CString csLeft = GetXmlAttribute
		(
			_T( "LeftScale" ), mapAttributes
		);
		const CString csRight = GetXmlAttribute
		(
			_T( "RightScale" ), mapAttributes
		);

		const SHORT sDecimals = (SHORT)_ttol( csDec );
		const SHORT sWidth = (SHORT)_ttol( csWid );

		const DOUBLE dPower = _ttof( csPow );
		const DOUBLE dMultiplier = _ttof( csMult );
		const DOUBLE dAdditive = _ttof( csAdd );

		const FLOAT fLeft = (FLOAT)_ttof( csLeft );
		const FLOAT fRight = (FLOAT)_ttof( csRight );

		pCache->UnitCategory = HostRef;
		pCache->Name = csName;
		pCache->RP66 = csRP66;
		pCache->WITSML = csWitsml;

		pCache->Power = dPower;
		pCache->Multiplier = dMultiplier;
		pCache->Additive = dAdditive;
		pCache->Decimals = sDecimals;
		pCache->Width = sWidth;

		pCache->LeftScale = fLeft;
		pCache->RightScale = fRight;

		AddCache( pCache );

		value = true;
	}

	return value;
} // AddRecordFromXML

/////////////////////////////////////////////////////////////////////////////
// read the given XML file
bool CUnitCollection::ReadXML
(
	CComPtr<IStream>& pFileStream,
	CComPtr<IXmlReader>& pReader,
	LPCTSTR path
)
{
	const bool value = CXmlTable::ReadXML( pFileStream, pReader, path );

	return value;
} // ReadXML

/////////////////////////////////////////////////////////////////////////////
// write the given XML file
bool CUnitCollection::WriteXML
(
	CComPtr<IStream>& pFileStream,
	CComPtr<IXmlWriter>& pWriter,
	LPCTSTR path
)
{
	bool value = CXmlTable::WriteXML( pFileStream, pWriter, path );
	if ( value == false )
	{
		return value;
	}

	return value;
} // WriteXML

/////////////////////////////////////////////////////////////////////////////
// import an XML cache into the table
bool CUnitCollection::ImportXML()
{
	bool value = false;

	// delete any existing rows
	DeleteAllRecords();

	// read the cache map
	CString csRow;
	for ( auto& node : m_mapCache.Items )
	{
		// get a pointer to the entry for this row
		CUnitCollection::CCache* pCache = node.second;

		// the unit class and name make up the key to the table records
		const CString csUnitCategory = pCache->UnitCategory;
		const CString csName = pCache->Name;

		// put the key in the correct format
		vector<CString> keys;
		keys.push_back( csUnitCategory );
		keys.push_back( csName );

		// append key will fail if the key is not unique
		if ( AppendKey( keys ) )
		{
			const long sRow = FindKey( keys );
			Power[ sRow ] = pCache->Power;
			Multiplier[ sRow ] = pCache->Multiplier;
			Additive[ sRow ] = pCache->Additive;
			Decimals[ sRow ] = pCache->Decimals;
			Width[ sRow ] = pCache->Width;
			RP66[ sRow ] = pCache->RP66;
			WITSML[ sRow ] = pCache->WITSML;
			LeftScale[ sRow ] = pCache->LeftScale;
			RightScale[ sRow ] = pCache->RightScale;

			value = true;
		}
	}

	return value;
} // ImportXML

/////////////////////////////////////////////////////////////////////////////
