//
// Copyright 2007-2019 Baker Hughes Incorporated.  All rights reserved.
// This document contains confidential and proprietary information owned 
// by Baker Hughes Incorporated.
// Do not use, copy or distribute without permission.
//
#include "stdafx.h"
#include "UnitSetTable.h"
#include "UnitSetsTable.h"

/////////////////////////////////////////////////////////////////////////////
CUnitSetTable::CUnitSetTable( IUnknownPtr spUnk )
{
	// time when cache was populated
	CacheTicks = 0;
	Caching = false;

	// query for the table interface and increment the interface's
	// reference counter
	if ( FAILED( spUnk->QueryInterface( &Control ) ) )
	{
		AfxThrowInvalidArgException();
	}

} // CUnitSetTable

/////////////////////////////////////////////////////////////////////////////
CUnitSetTable::~CUnitSetTable()
{
} // ~CUnitSetTable

/////////////////////////////////////////////////////////////////////////////
// the entire record is read only
bool CUnitSetTable::GetReadOnly( int record )
{
	bool value = false;
	CUnitSetsTable* pHost = (CUnitSetsTable*)Host;

	if ( pHost != nullptr )
	{
		// the unit set of this record
		const CString csSet = Name[ record ];

		// find our parent's record
		vector<CString> arrKeys;
		arrKeys.push_back( csSet );
		const short sHost = pHost->FindKey( arrKeys );
		if ( sHost != -1 )
		{
			// we are read only if our parent is read only
			value = pHost->ReadOnly[ sHost ];
		}
	}

	return value;
} // GetReadOnly

/////////////////////////////////////////////////////////////////////////////
// the entire record is read only
void CUnitSetTable::SetReadOnly( int record, bool value /*= true */)
{
	// this page intensionally left blank

} // SetReadOnly

/////////////////////////////////////////////////////////////////////////////
// create the cache from the table
bool CUnitSetTable::CacheTable( bool bForce /*= false*/ )
{
	bool value = false;

	// if the cache was created since the table was last modified
	// there is nothing to do
	const ULONGLONG ullCache = CacheTicks;
	const ULONGLONG ullTable = LastModifyTimeTicks;
	if ( !bForce && ullCache >= ullTable )
	{
		return value;
	}

	// start fresh
	ClearCache();

	value = true;

	// create the cache
	vector<short> arrRows = AllRecordNumbers;
	for ( short sRow : arrRows )
	{
		CCache* pCache = new CCache();

		pCache->UnitSet = Name[ sRow ];
		pCache->UnitClass = UnitClass[ sRow ];
		pCache->Unit = Unit[ sRow ];
		pCache->Decimals = Decimals[ sRow ];
		pCache->Width = Width[ sRow ];

		// record the unit set properties in the cache
		AddCache( pCache );
	}

	// time stamp the cache
	CacheTicks = ::GetTickCount64();

	return value;
} // CacheTable

/////////////////////////////////////////////////////////////////////////////
// find the cache for the unit set and unit class combination
CUnitSetTable::CCache* CUnitSetTable::FindUnitSet
( 
	CString csSet, CString csClass 
)
{
	CUnitSetTable::CCache* value = nullptr;

	const CString csKey = CacheKey[ csSet ][ csClass ];

	// test to see if the key exists in the cache and make sure the
	// cache is up-to-date
	if ( KeyExists[ csKey ] )
	{
		value = m_mapCache.find( csKey );
	}

	return value;
} // FindUnitSet

/////////////////////////////////////////////////////////////////////////////
// add a record to the table from XML
bool CUnitSetTable::AddRecordFromXML
(
	CString csTableKey,
	CString csRowKey,
	CKeyCollectionBase<CString>& mapAttributes
)
{
	bool value = false;

	// this is the XML node name of our table's row
	const CString csSetRowKey = RowKey;

	// reading table row properties
	if ( csRowKey == csSetRowKey )
	{
		// create a new cache for this record
		CCache* pCache = new CCache();

		const CString csUnitClass =
			GetXmlAttribute( _T( "UnitClass" ), mapAttributes );
		const CString csUnit =
			GetXmlAttribute( _T( "Unit" ), mapAttributes );
		const CString csDec = GetXmlAttribute
		(
			_T( "Decimals" ), mapAttributes
		);
		const CString csWid = GetXmlAttribute
		(
			_T( "DisplayWidth" ), mapAttributes
		);

		const SHORT sDecimals = (SHORT)_ttol( csDec );
		const SHORT sWidth = (SHORT)_ttol( csWid );

		pCache->UnitSet = HostRef;
		pCache->UnitClass = csUnitClass;
		pCache->Unit = csUnit;

		pCache->Decimals = sDecimals;
		pCache->Width = sWidth;

		AddCache( pCache );

		value = true;
	}

	return value;
} // AddRecordFromXML

/////////////////////////////////////////////////////////////////////////////
// read the given XML file
bool CUnitSetTable::ReadXML
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
bool CUnitSetTable::WriteXML
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
bool CUnitSetTable::ImportXML()
{
	bool value = false;

	// read the cache map
	CString csRow;
	for ( auto& node : m_mapCache.Items )
	{
		// get a pointer to the entry for this row
		CUnitSetTable::CCache* pCache = node.second;

		// the unit set and name (unit class) make up the key to the 
		// table records
		const CString csUnitSet = pCache->UnitSet;
		const CString csName = pCache->UnitClass;

		// put the key in the correct format
		vector<CString> keys;
		keys.push_back( csUnitSet );
		keys.push_back( csName );

		// append key will fail if the key is not unique
		if ( AppendKey( keys ) )
		{
			const short sRow = FindKey( keys );
			Unit[ sRow ] = pCache->Unit;
			Decimals[ sRow ] = pCache->Decimals;
			Width[ sRow ] = pCache->Width;

			value = true;
		}
	}

	return value;
} // ImportXML

/////////////////////////////////////////////////////////////////////////////
