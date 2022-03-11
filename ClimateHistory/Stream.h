/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "StreamCache.h"
#include "comutil.h"
#include <memory>

using namespace std;

class CStreams;
class CSchemaStream;

/////////////////////////////////////////////////////////////////////////////
// an individual stream member of the CStreams collection class
class CStream
{
// public definitions
public:
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
	// the hosting collection points back to the CStreams host
	CStreams* m_pHost;
	// cache for the stream
	CStreamCache m_Cache;
	// empty value for numeric data that has not been read
	double m_dNull;
	// the schema definition of this stream
	shared_ptr<CSchemaStream> m_pSchemaStream;
	// pathname of the stream
	CString m_csPathname;
	// streams are implemented as files
	CFile* m_pFile;
	// name
	CString m_csName;
	// Variant type
	VARENUM m_eType;
	// size in bytes of a record
	USHORT m_usSize;
	// size in bytes of a value
	USHORT m_usValueSize;
	// number of values on a level
	USHORT m_usLevelValues;	
	// size in bytes of the file
	ULONG m_ulFileSize;
	// number of levels (AKA rows or records) in the file
	ULONG m_ulLevels;
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
	// buffer for reading data
	vector<byte> m_arrBuffer;
	// unit category this stream is controlled by
	CString m_csUnitCategory;
	// the units the stream is persisted in
	CString m_csPersistUnits;
	// the units the stream is displayed in
	CString m_csDisplayUnits;
	// the stream's unit conversion multiplier
	double m_dMultiplier;
	// the stream's unit conversion offset
	double m_dAdditive;

// public properties
public:
	// the hosting collection points back to the CStreams host
	inline CStreams* GetHost()
	{
		return m_pHost;
	}
	// the hosting collection points back to the CStreams host
	inline void SetHost( CStreams* value )
	{
		m_pHost = value;
	}
	// the hosting collection points back to the CStreams host
	__declspec( property( get = GetHost, put = SetHost ) )
		CStreams* Host;

	// cache for the stream
	inline CStreamCache* GetCache()
	{
		return &m_Cache;
	}
	// cache for the stream
	__declspec( property( get = GetCache ) )
		CStreamCache* Cache;

	// empty value for numeric data that has not been read
	inline double GetNull()
	{
		return m_dNull;
	}
	// empty value for numeric data that has not been read
	inline void SetNull( double value = -9999.0 )
	{
		m_dNull = value;
	}
	// empty value for numeric data that has not been read
	__declspec( property( get = GetNull, put = SetNull ) )
		double Null;

	// the schema definition of this stream
	inline shared_ptr<CSchemaStream>& GetSchemaStream()
	{
		return m_pSchemaStream;
	}
	// the schema definition of this stream
	inline void SetSchemaStream( shared_ptr<CSchemaStream> value )
	{
		m_pSchemaStream = value;
	}
	// the schema definition of this stream
	__declspec( property( get = GetSchemaStream, put = SetSchemaStream ) )
		shared_ptr<CSchemaStream> SchemaStream;

	// Pathname
	inline CString GetPathname()
	{
		return m_csPathname;
	}
	// Pathname
	inline void SetPathname( LPCTSTR value )
	{
		m_csPathname = value;
	}
	// Pathname
	__declspec( property( get = GetPathname, put = SetPathname ) )
		CString Pathname;

	// streams are implemented as files
	inline CFile* GetFile()
	{
		return m_pFile;
	}
	// streams are implemented as files
	inline void SetFile( CFile* value )
	{
		m_pFile = value;
	}
	// streams are implemented as files
	__declspec( property( get = GetFile, put = SetFile ) )
		CFile* File;

	// is the file open
	inline bool GetIsOpen()
	{
		CFile* pFile = File;
		const bool value = 
			pFile != nullptr && 
			pFile->m_hFile != CFile::hFileNull;

		return value;
	}
	// is the file open
	__declspec( property( get = GetIsOpen ) )
		bool IsOpen;

	// Name of the stream
	inline CString GetName()
	{
		return m_csName;
	}
	// Name of the stream
	inline void SetName( LPCTSTR value )
	{
		m_csName = value;
	}
	// Name of the stream
	__declspec( property( get = GetName, put = SetName ) )
		CString Name;

	// data type of the stream
	inline VARENUM GetType()
	{
		return m_eType;
	}
	// data type of the stream
	inline void SetType( VARENUM value )
	{
		m_eType = value;
	}
	// data type of the stream
	__declspec( property( get = GetType, put = SetType ) )
		VARENUM Type;

	// size in bytes of a record
	inline USHORT GetSize()
	{
		return m_usSize;
	}
	// size in bytes of a record
	inline void SetSize( USHORT value )
	{
		m_usSize = value;
	}
	// size in bytes of a record
	__declspec( property( get = GetSize, put = SetSize ) )
		USHORT Size;

	// the size of a single value in a record since a record can be an array 
	// of values
	inline USHORT GetValueSize()
	{
		// return value
		USHORT value = 0;
		VARENUM eType = Type;
		switch ( eType )
		{
			// special case handled as an array of characters 
			// where a value is the record size
			case VT_I1: 
			case VT_UI1: value = 1; break;
			case VT_I2:
			case VT_UI2: value = 2; break;
			case VT_I4:
			case VT_UI4:
			case VT_R4:  value = 4; break;
			case VT_I8:
			case VT_UI8:
			case VT_R8:  value = 8;
			default : // unsupported type
			{
				CHelper::ErrorMessage( __FILE__, __LINE__ );
				return value;
			}
		}

		// persist the value
		ValueSize = value;

		return value;
	}
	// size in bytes of a record
	inline void SetValueSize( USHORT value )
	{
		m_usValueSize = value;
	}
	// the size of a single value in a record since a record can be an array 
	// of values
	__declspec( property( get = GetValueSize, put = SetValueSize ) )
		USHORT ValueSize;

	// number of values on a level
	inline USHORT GetLevelValues()
	{
		USHORT value = 0;

		// number of bytes on a level
		const USHORT usSize = Size;

		// size of a single value in bytes
		const USHORT usValueSize = ValueSize;

		// calculate the result 
		if ( usValueSize != 0 )
		{
			value = usSize / usValueSize;
		}

		// persist the result
		LevelValues = value;

		return value;
	}
	// number of values on a level
	inline void SetLevelValues( USHORT value )
	{
		m_usLevelValues = value;
	}
	// number of values on a level
	__declspec( property( get = GetLevelValues, put = SetLevelValues ) )
		USHORT LevelValues;

	// size in bytes of the file
	inline ULONG GetFileSize()
	{
		ULONG value = 0;
		const bool bOpen = IsOpen;
		if ( bOpen )
		{
			value = (ULONG)File->GetLength();
		}

		// persist the value
		FileSize = value;

		return value;
	}
	// size in bytes of the file
	inline void SetFileSize( ULONG value )
	{
		m_ulFileSize = value;
	}
	// size in bytes of the file
	__declspec( property( get = GetFileSize, put = SetFileSize ) )
		ULONG FileSize;

	// number of levels (AKA rows or records) in the file
	inline ULONG GetLevels()
	{
		ULONG value = 0;

		// size of the file in bytes
		const ULONG ulFileSize = FileSize;

		// size of a level in bytes
		const USHORT usSize = Size;

		// calculate the value
		if ( usSize != 0 )
		{
			value = ulFileSize / usSize;
		}

		// persist the value
		Levels = value;

		return value;
	}
	// number of levels (AKA rows or records) in the file
	inline void SetLevels( ULONG value )
	{
		m_ulLevels = value;
	}
	// number of levels (AKA rows or records) in the file
	__declspec( property( get = GetLevels, put = SetLevels ) )
		ULONG Levels;

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
	
	// byte offset into the file given a record and item number
	inline ULONG GetOffset( ULONG record, USHORT item = 0 )
	{
		const ULONG ulRecordSize = Size;
		const USHORT usValueSize = ValueSize;
		VARENUM eType = Type;

		// offset into the file in bytes
		const ULONG value = record * ulRecordSize + item * usValueSize;

		return value;
	}
	// byte offset into the file given a record and item number
	__declspec( property( get = GetOffset ))
		ULONG Offset[][];

	// a single value of a record can return VT_EMPTY on failure
	byte* GetValue( ULONG record, USHORT item = 0 );
	// a single value of a record is assigned
	void SetValue( ULONG record, USHORT item, byte* value );
	// a single value of a record
	__declspec( property( get = GetValue, put = SetValue ) )
		byte* Value[][];

	// a single value of a record can return VT_EMPTY on failure
	COleVariant GetVariant( ULONG record, USHORT item = 0 );
	// a single value of a record is assigned
	void SetVariant( ULONG record, USHORT item, COleVariant value );
	// a single value of a record
	__declspec( property( get = GetVariant, put = SetVariant ) )
		COleVariant Variant[][];

	// Unit Category of the stream
	inline CString GetUnitCategory()
	{
		return m_csUnitCategory;
	}
	// Unit Category of the stream
	inline void SetUnitCategory( LPCTSTR value )
	{
		m_csUnitCategory = value;
	}
	// Unit Category of the stream
	__declspec( property( get = GetUnitCategory, put = SetUnitCategory ) )
		CString UnitCategory;

	// the units the stream is persisted in
	inline CString GetPersistUnits()
	{
		return m_csPersistUnits;
	}
	// the units the stream is persisted in
	inline void SetPersistUnits( LPCTSTR value )
	{
		m_csPersistUnits = value;
	}
	// the units the stream is persisted in
	__declspec( property( get = GetPersistUnits, put = SetPersistUnits ) )
		CString PersistUnits;

	// the units the stream is displayed in
	inline CString GetDisplayUnits()
	{
		return m_csDisplayUnits;
	}
	// the units the stream is displayed in
	inline void SetDisplayUnits( LPCTSTR value )
	{
		m_csDisplayUnits = value;
	}
	// the units the stream is displayed in
	__declspec( property( get = GetDisplayUnits, put = SetDisplayUnits ) )
		CString DisplayUnits;

	// the stream's unit conversion multiplier
	inline double GetMultiplier()
	{
		return m_dMultiplier;
	}
	// the stream's unit conversion multiplier
	inline void SetMultiplier( double value = 1 )
	{
		m_dMultiplier = value;
	}
	// the stream's unit conversion multiplier
	__declspec( property( get = GetMultiplier, put = SetMultiplier ) )
		double Multiplier;

	// the stream's unit conversion offset
	inline double GetAdditive()
	{
		return m_dAdditive;
	}
	// the stream's unit conversion offset
	inline void SetAdditive( double value = 0 )
	{
		m_dAdditive = value;
	}
	// the stream's unit conversion offset
	__declspec( property( get = GetAdditive, put = SetAdditive ) )
		double Additive;

// protected methods
protected:

// public methods
public:
	// open the stream given its pathname
	bool Open
	( 
		CString csPath, CString csName, 
		shared_ptr<CSchemaStream> pSchemaStream 
	);

	// using internal properties
	inline bool Open()
	{
		const bool value = Open( Pathname, Name, SchemaStream );
		return value;
	}

	// seek to a level of the file
	inline void SeekToLevel( ULONG ulLevel )
	{
		// convert levels into a byte offset
		const ULONG ulOffset = Offset[ ulLevel ][ 0 ];

		// do it
		File->Seek( ulOffset, 0 );
	}

// public stream cache methods
public:
	// read data into a stream cache from the given pair of zero based level
	// numbers inclusive.  These are actual level values (not indices), so data
	// that is routinely filtered out by the indexing will be included 
	bool ReadIntoCache
	(
		ULONG ulFirstLevel, // start level to read
		ULONG ulLastLevel // last level to read
	);

	// write data from a stream cache to the stream beginning at the given
	// level number.  the stream cache should be stored in the stream's
	// native units (not display units).  the stream cache should have
	// been originally populated by the ReadStreamCache to be sure that
	// missing levels which are routinely filtered out by the indexing
	// (null index, reverse transitions for drilling data, and forward 
	// transitions for back plots, etc.). are in place.
	bool WriteCacheData
	( 
		ULONG ulFirstLevel, // start level to read
		// if writing more than one block, set the flush on the 
		// last block written to insure changes are flushed from
		// the stream's cache (if set to true for every block, the
		// process will be slower)
		bool bFlush
	);

	// get a pointer to the level of a stream cache or null if invalid
	BYTE* GetLevelFromStreamCache
	(
		// stream level to read from
		ULONG ulLevel,
		// top level of stream in the stream cache if the stream cache
		// does not contain the entire stream. this value is used with
		// the stream level to offset into the stream cache's row.
		ULONG ulTopLevel = 0
	);

	// set a value in display units into a stream cache containing
	// levels of a stream in the stream's native units
	bool SetValueInStreamCache
	(	
		// value to set
		double dValue,
		// stream level to set
		ULONG ulLevel,
		// element to set (when more than one value per level)
		ULONG ulElement = 0,
		// top level of stream in the stream cache if the stream cache
		// does not contain the entire stream. this value is used with
		// the stream level to offset into the stream cache's row.
		ULONG ulTopLevel = 0
	);

	// get a value in display units from a stream cache containing
	// levels of a stream in the stream's native units
	bool GetValueFromStreamCache
	(	
		// value to read into
		double& dValue,
		// stream level to read from
		ULONG ulLevel,
		// element to read (when more than one value per level)
		ULONG ulElement = 0,
		// top level of stream in the stream cache if the stream cache
		// does not contain the entire stream. this value is used with
		// the stream level to offset into the stream cache's row.
		ULONG ulTopLevel = 0
	);

// protected overrides
protected:

// public overrides
public:

// public construction / destruction
public:
	// constructor
	CStream();
	
	// destructor
	virtual ~CStream();

};

/////////////////////////////////////////////////////////////////////////////
