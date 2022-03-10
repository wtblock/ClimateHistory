//
// Copyright 2007-2019 Baker Hughes Incorporated.  All rights reserved.
// This document contains confidential and proprietary information owned 
// by Baker Hughes Incorporated.
// Do not use, copy or distribute without permission.
//
#include "stdafx.h"
#include "UnitClassTable.h"

/////////////////////////////////////////////////////////////////////////////
CUnitClassTable::CUnitClassTable
( 
	IUnknownPtr spUnk, // this table's interface
	// embedded (links to row of this table via unit class) table's interface
	IUnknownPtr spEmb, 
	IUnknownPtr spAlias, // alias table's interface
	IUnknownPtr spLong // long name table's interface
)
{
	// time when cache was populated
	CacheTicks = 0;

	// time when enumeration cross reference was populated
	EnumerationTicks = 0;

	// this flag is set while the cache is being populated from the 
	// table or from the XML
	Caching = false;

	// query for the table interface and increment the interface's
	// reference counter
	if ( FAILED( spUnk->QueryInterface( &Control ) ) )
	{
		AfxThrowInvalidArgException();
	}

	// create an instance of the embedded unit table
	m_spUnitTable = unique_ptr<CUnitTable>( new CUnitTable( spEmb ) );
	m_spUnitTable->Host = this;

	// create an instance of the embedded unit alias table
	m_spUnitAliasTable = 
		unique_ptr<CUnitAliasTable>( new CUnitAliasTable( spAlias ) );
	m_spUnitAliasTable->Host = this;

	// create an instance of the embedded long unit name table
	m_spUnitLongTable = 
		unique_ptr<CUnitLongTable>( new CUnitLongTable( spLong ) );
	m_spUnitLongTable->Host = this;

} // CUnitClassTable

/////////////////////////////////////////////////////////////////////////////
CUnitClassTable::~CUnitClassTable()
{
} // ~CUnitClassTable

/////////////////////////////////////////////////////////////////////////////
// create the cache from the table
bool CUnitClassTable::CacheTable()
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
	vector<short> arrRows = AllRecordNumbers;
	for ( short sRow : arrRows )
	{
		CCache* pCache = new CCache();

		pCache->Name = Name[ sRow ];
		pCache->Description = Description[ sRow ];
		pCache->Type = Type[ sRow ];
		pCache->Units = Units[ sRow ];
		pCache->Hide = Hide[ sRow ];
		pCache->Enum = Enum[ sRow ];

		// record the unit class properties in the cache
		AddCache( pCache );
	}

	// time stamp the cache
	CacheTicks = ::GetTickCount64();

	return value;
} // CacheTable

/////////////////////////////////////////////////////////////////////////////
// cross reference the enumerations to class names
bool CUnitClassTable::CrossReferenceEnumerations()
{
	bool value = false;

	// if the cache was created since the table was last modified
	// there is nothing to do
	const ULONGLONG ullEnum = EnumerationTicks;
	const ULONGLONG ullTable = LastModifyTimeTicks;
	if ( ullEnum >= ullTable )
	{
		return value;
	}

	// start fresh
	m_mapEnum.clear();

	value = true;

	// create the cross reference
	vector<short> arrRows = AllRecordNumbers;
	for ( short sRow : arrRows )
	{
		const CString csClass = Name[ sRow ];
		const short sEnum = Enum[ sRow ];
		const CString csKey = EnumKey[ sEnum ];
		if ( m_mapEnum.Exists[ csKey ] )
		{
			continue;
		}
		m_mapEnum.add( csKey, new CString( csClass ));
	}

	// time stamp the cross reference
	EnumerationTicks = ::GetTickCount64();

	return value;
} // CrossReferenceEnumerations

/////////////////////////////////////////////////////////////////////////////
// find the unit class
CUnitClassTable::CCache* CUnitClassTable::FindUnitClass( VARIANT& uc )
{
	CUnitClassTable::CCache* value = nullptr;

	// lookup the class name from the variant
	CString csClass;

	// if a short was passed in
	if ( uc.vt == VT_I2 )
	{
		csClass = EnumClass[ uc.iVal ];

	} else // a string was passed in
	{
		csClass = CString( uc.bstrVal );
	}

	// test to see if the class is in the cache (and update the cache
	// if necessary)
	if ( KeyExists[ csClass ] )
	{
		// lookup the class from the cache
		value = m_mapCache.find( csClass );
	}

	return value;
} // FindUnitClass

/////////////////////////////////////////////////////////////////////////////
// add a record to the table from XML
bool CUnitClassTable::AddRecordFromXML
(
	CString csTableKey,
	CString csRowKey,
	CKeyCollectionBase<CString>& mapAttributes
)
{
	bool value = false;

	// this is the XML node name of our table's row
	const CString csClassRowKey = RowKey;

	// this is the XML node name of our embedded table's row
	const CString csUnitRowKey = EmbeddedTable->RowKey;

	// this is the XML node name of our alias table's row
	const CString csAliasRowKey = UnitAliasTable->RowKey;

	// this is the XML node name of our long table's row
	const CString csLongRowKey = UnitLongTable->RowKey;

	// reading table row properties
	if ( csRowKey == csClassRowKey )
	{
		CCache* pCache = new CCache();

		const CString csName =
			GetXmlAttribute( _T( "Name" ), mapAttributes );
		const CString csUnits =
			GetXmlAttribute( _T( "InternalUnit" ), mapAttributes );
		const CString csDesc =
			GetXmlAttribute( _T( "Description" ), mapAttributes );
		const CString csType =
			GetXmlAttribute( _T( "QuantityType" ), mapAttributes );
		const CString csEnum =
			GetXmlAttribute( _T( "Enumeration" ), mapAttributes );
		const CString csHide = GetXmlAttribute
		(
			_T( "Hidden" ),
			mapAttributes
		).MakeUpper();

		const SHORT sEnumeration = (SHORT)_ttol( csEnum );
		const bool bHidden = csHide != _T( "FALSE" );

		pCache->Name = csName;
		pCache->Description = csDesc;
		pCache->Type = csType;
		pCache->Units = csUnits;
		pCache->Hide = bHidden;
		pCache->Enum = sEnumeration;

		// record the unit class properties in the cache
		AddCache( pCache );

		// let our embedded table know what unit class is being read
		EmbeddedTable->HostRef = csName;

		value = true;

	} else // one of the other tables in the XML file
	{
		value = EmbeddedTable->AddRecordFromXML
		( 
			csTableKey, csRowKey, mapAttributes 
		);
		if ( value == false )
		{
			value = UnitAliasTable->AddRecordFromXML
			(
				csTableKey, csRowKey, mapAttributes
			);
		}
		if ( value == false )
		{
			value = UnitLongTable->AddRecordFromXML
			(
				csTableKey, csRowKey, mapAttributes
			);
		}
	}
	
	return value;
} // AddRecordFromXML

/////////////////////////////////////////////////////////////////////////////
// read the given XML file
bool CUnitClassTable::ReadXML
(
	CComPtr<IStream>& pFileStream,
	CComPtr<IXmlReader>& pReader,
	LPCTSTR path
)
{
	// read the XML into the cache
	const bool value = CXmlTable::ReadXML( pFileStream, pReader, path );

	// import the cache into the table
	ImportXML();

	return value;
} // ReadXML

/////////////////////////////////////////////////////////////////////////////
// write the given XML file
bool CUnitClassTable::WriteXML
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
bool CUnitClassTable::ImportXML()
{
	bool value = false;

	// delete any existing rows
	DeleteAllRecords();

	// read the cache map
	CString csRow;
	for ( auto& node : m_mapCache.Items )
	{
		// get a pointer to the entry for this row
		CUnitClassTable::CCache* pCache = node.second;

		// the name is the key of the table
		const CString csName = pCache->Name;

		// put the key in the correct format
		vector<CString> keys;
		keys.push_back( csName );

		// append key will fail if the key is not unique
		if ( AppendKey( keys ) )
		{
			const short sRow = FindKey( keys );
			Type[ sRow ] = pCache->Type;
			Description[ sRow ] = pCache->Description;
			Type[ sRow ] = pCache->Type;
			Units[ sRow ] = pCache->Units;
			Hide[ sRow ] = pCache->Hide;
			Enum[ sRow ] = pCache->Enum;

			value = true;
		}
	}

	// time stamp the cache
	CacheTicks = ::GetTickCount64();

	// import the unit table's cache into the table
	EmbeddedTable->ImportXML();

	// import the alias table's cache into the table
	UnitAliasTable->ImportXML();

	// import the long name table's cache into the table
	UnitLongTable->ImportXML();

	return value;
} // ImportXML

/////////////////////////////////////////////////////////////////////////////
