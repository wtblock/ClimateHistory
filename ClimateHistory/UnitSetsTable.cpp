//
// Copyright 2007-2019 Baker Hughes Incorporated.  All rights reserved.
// This document contains confidential and proprietary information owned 
// by Baker Hughes Incorporated.
// Do not use, copy or distribute without permission.
//
#include "stdafx.h"
#include "UnitSetsTable.h"

/////////////////////////////////////////////////////////////////////////////
CUnitSetsTable::CUnitSetsTable
(
	IUnknownPtr spUnk,
	// embedded (links to row of this table via unit set field NAME) 
	// table's interface
	IUnknownPtr spEmb
)
{
	// time when cache was populated
	CacheTicks = 0;
	Caching = false;
	Customization = _T( "none" );

	// query for the table interface and increment the interface's
	// reference counter
	if ( FAILED( spUnk->QueryInterface( &Control ) ) )
	{
		AfxThrowInvalidArgException();
	}

	// create an instance of the embedded unit set table
	m_spUnitSetTable = 
		unique_ptr<CUnitSetTable>( new CUnitSetTable( spEmb ) );

	// the embedded table needs to know about us to determine
	// if its records are read only
	m_spUnitSetTable->Host = this;

} // CUnitSetsTable

/////////////////////////////////////////////////////////////////////////////
CUnitSetsTable::~CUnitSetsTable()
{
} // ~CUnitSetsTable


/////////////////////////////////////////////////////////////////////////////
// create the cache from the table
bool CUnitSetsTable::CacheTable( bool bForce /*= false*/ )
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

		pCache->Name = Name[ sRow ];
		pCache->Description = Description[ sRow ];
		pCache->System = System[ sRow ];
		pCache->Editable = Editable[ sRow ];

		// record the unit set properties in the cache
		AddCache( pCache );
	}

	// time stamp the cache
	CacheTicks = ::GetTickCount64();

	return value;
} // CacheTable

/////////////////////////////////////////////////////////////////////////////
// add a record to the table from XML
bool CUnitSetsTable::AddRecordFromXML
(
	CString csTableKey,
	CString csRowKey,
	CKeyCollectionBase<CString>& mapAttributes
)
{
	bool value = false;

	// the current customization string can be one of the following :
	// o "none" is for Cadence
	// o "advantage" 
	// o "atlas"
	const CString csCustom = Customization;

	// a flag indicating the project is opened with Cadence customization
	const bool bCadence = csCustom == _T( "none" );

	// this is the XML node name of the table
	const CString csSetsTableKey = TableKey;

	// this is the XML node name of our table's row
	const CString csSetsRowKey = RowKey;

	// reading table row properties
	if ( csSetsRowKey == csRowKey )
	{
		CCache* pCache = new CCache();

		const CString csName =
			GetXmlAttribute( _T( "Name" ), mapAttributes );
		const CString csDesc =
			GetXmlAttribute( _T( "Description" ), mapAttributes );
		const CString csSystem =
			GetXmlAttribute( _T( "System" ), mapAttributes ).MakeUpper();
		const CString csEditable =
			GetXmlAttribute( _T( "Editable" ), mapAttributes ).MakeUpper();

		bool bSystem = csSystem != _T( "FALSE" );

		// for Cadence the system flag is controlled by the source XML 
		// being the "SystemUnitSet.XML" file
		if ( bCadence )
		{
			bSystem = SystemPath;
		}

		// even if the XML property indicates the unit set is editable,
		// system unit sets are read-only and XML files read by Cadence
		// are read-only. Read-only unit sets have to be cloned by the
		// user before they can be edited
		const bool bEditable =
			csEditable != _T( "FALSE" ) && !bSystem && !bCadence;

		pCache->Name = csName;
		pCache->Description = csDesc;
		pCache->System = bSystem;
		pCache->Editable = bEditable;

		// record the unit class properties in the cache
		AddCache( pCache );

		// let our embedded table know what unit class is being read
		EmbeddedTable->HostRef = csName;

		value = true;

	} else
	{
		value = EmbeddedTable->AddRecordFromXML
		( 
			csTableKey, csRowKey, mapAttributes 
		);
	}
	
	return value;
} // AddRecordFromXML

/////////////////////////////////////////////////////////////////////////////
// read the given XML file
bool CUnitSetsTable::ReadXML
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
bool CUnitSetsTable::WriteXML
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
bool CUnitSetsTable::ImportXML()
{
	bool value = false;

	// read the cache map
	CString csRow;
	for ( auto& node : m_mapCache.Items )
	{
		// get a pointer to the entry for this row
		CUnitSetsTable::CCache* pCache = node.second;

		// the name is the key of the table
		const CString csName = pCache->Key;

		// put the key in the correct format
		vector<CString> keys;
		keys.push_back( csName );

		// append key will fail if the key is not unique
		if ( AppendKey( keys ) )
		{
			const short sRow = FindKey( keys );
			Description[ sRow ] = pCache->Description;
			System[ sRow ] = pCache->System;
			Editable[ sRow ] = pCache->Editable;

			value = true;
		}
	}

	// import the unit table's cache into the table
	EmbeddedTable->ImportXML();

	return value;
} // ImportXML

/////////////////////////////////////////////////////////////////////////////
