/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "CHelper.h"
#include <memory>

using namespace std;

class CSchema;

/////////////////////////////////////////////////////////////////////////////
// the interface to the column of a table
class CSchemaStream
{
// public definitions
public:
	// type of collection
	typedef enum ENUM_COLLECTION
	{
		ecDirectory, // directory of all data streams
		ecTabular, // tabular data is a collection of general steams
		ecTelemetry, // telemetry data is a collection numerical data
		ecImage, // image collection
		ecText, // text collection
		ecBLOB, // binary large object collection

	} ENUM_COLLECTION;

	// type of data entry
	typedef enum ENUM_ENTRY
	{
		eeFree, // free form entry
		eeEnum1, // single enumeration drop down strings
		eeEnum2, // numbered enumeration pair drop down (number, string)
		eeBoolean, // Boolean drop down (false, true)
		eeVersion, // version list drop down
		eeStream, // stream names in a collection drop down
		eeDate, // calendar entry
		eeUnitCategory, // unit category drop down
		eeUnitName, // unit name drop down

	} ENUM_ENTRY;

// protected data
protected:
	// name
	CString m_csName;
	// Variant type
	VARENUM m_eType;
	// size in bytes
	USHORT m_usSize;
	// unit category name
	CString m_csUnitCategory;
	// small title
	CString m_csTitle;
	// description
	CString m_csDescription;
	// entry type
	ENUM_ENTRY m_eEntry;
	// Enumeration string used by the "eeEnum1" and "eeEnum2" entry types
	CString m_csEnumeration;
	// name used to collect properties in a property sheet
	CString m_csPropertyGroup;
	// empty values are referred to as null
	double m_dNull;

// public properties
public:
	// Name
	inline CString GetName()
	{
		return m_csName;
	}
	// Name
	inline void SetName( LPCTSTR value )
	{
		m_csName = value;
	}
	// Name
	__declspec( property( get = GetName, put = SetName ) )
		CString Name;

	// type
	inline VARENUM GetType()
	{
		return m_eType;
	}
	// type
	inline void SetType( VARENUM value )
	{
		m_eType = value;
	}
	// type
	__declspec( property( get = GetType, put = SetType ) )
		VARENUM Type;

	// size in bytes
	inline USHORT GetSize()
	{
		return m_usSize;
	}
	// size in bytes
	inline void SetSize( USHORT value )
	{
		m_usSize = value;
	}
	// size in bytes
	__declspec( property( get = GetSize, put = SetSize ) )
		USHORT Size;

	// unit category name
	inline CString GetUnitCategory()
	{
		return m_csUnitCategory;
	}
	// unit category name
	inline void SetUnitCategory( LPCTSTR value )
	{
		m_csUnitCategory = value;
	}
	// unit category name
	__declspec( property( get = GetUnitCategory, put = SetUnitCategory ) )
		CString UnitCategory;

	// title for property sheets
	inline CString GetTitle()
	{
		return m_csTitle;
	}
	// title for property sheets
	inline void SetTitle( LPCTSTR value )
	{
		m_csTitle = value;
	}
	// title for property sheets
	__declspec( property( get = GetTitle, put = SetTitle ) )
		CString Title;

	// description
	inline CString GetDescription()
	{
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

	// entry type
	inline ENUM_ENTRY GetEntry()
	{
		return m_eEntry;
	}
	// entry type
	inline void SetEntry( ENUM_ENTRY value )
	{
		m_eEntry = value;
	}
	// entry type
	__declspec( property( get = GetEntry, put = SetEntry ) )
		ENUM_ENTRY Entry;

	// Enumeration string used by the "eeEnum1" and "eeEnum2" entry types
	inline CString GetEnumeration()
	{
		return m_csEnumeration;
	}
	// Enumeration string used by the "eeEnum1" and "eeEnum2" entry types
	inline void SetEnumeration( LPCTSTR value ){
		m_csEnumeration = value;
	}
	// Enumeration string used by the "eeEnum1" and "eeEnum2" entry types
	__declspec( property( get = GetEnumeration, put = SetEnumeration ) )
		CString Enumeration;

	// name used to collect properties in a property sheet
	inline CString GetPropertyGroup()
	{
		return m_csPropertyGroup;
	}
	// name used to collect properties in a property sheet
	inline void SetPropertyGroup( LPCTSTR value )
	{
		m_csPropertyGroup = value;
	}
	// name used to collect properties in a property sheet
	__declspec( property( get = GetPropertyGroup, put = SetPropertyGroup ) )
		CString PropertyGroup;

	// empty values are referred to as null
	inline double GetNull()
	{
		return m_dNull;
	}
	// empty values are referred to as null
	inline void SetNull( double value )
	{
		m_dNull = value;
	}
	// empty values are referred to as null
	__declspec( property( get = GetNull, put = SetNull ) )
		double Null;

// public methods
public:

// public static methods
public:

// public constructor / destructor
public:
	CSchemaStream()
	{
		Type = VT_EMPTY;
		Size = 0;
		Entry = eeFree;
		Null = 0;
	}
	virtual ~CSchemaStream()
	{
	}
};


