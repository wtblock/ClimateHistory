/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Stream.h"
#include "Streams.h"

/////////////////////////////////////////////////////////////////////////////
CStream::CStream()
{
	Host = 0;
	Null = -9999;
	Type = VT_EMPTY;
	Size = 0;
	ValueSize = 0;
	FileSize = 0;
	Entry = eeFree;
	Additive = 0;
	Multiplier = 1;
}

/////////////////////////////////////////////////////////////////////////////
CStream::~CStream()
{
}

/////////////////////////////////////////////////////////////////////////////
// open the stream given its pathname
bool CStream::Open
(
	CString csPath, CString csName,
	shared_ptr<CSchemaStream> pSchemaStream
)
{
	bool value = false;
	Pathname = csPath;
	SchemaStream = pSchemaStream;
	Name = csName;
	shared_ptr<CStream> pStream = shared_ptr<CStream>( new CStream );
	pStream->Name = csName;
	shared_ptr<CFile> pFile = shared_ptr<CFile>( new CFile );

	if ( ::PathFileExists( csPath ) )
	{
		// open an existing file
		value = pFile->Open
		( 
			csPath, CFile::modeReadWrite | 
			CFile::shareDenyNone
		);
	}
	else
	{
		// create the file
		value = pFile->Open
		(
			csPath,
			CFile::modeCreate |
			CFile::modeReadWrite |
			CFile::shareDenyNone |
			CFile::osRandomAccess
		);
	}

	if ( value == true )
	{
		Type = pSchemaStream->Type;
		Size = pSchemaStream->Size;
		UnitCategory = pSchemaStream->UnitCategory;
		Title = pSchemaStream->Title;
		Description = pSchemaStream->Description;
		Entry = (ENUM_ENTRY)pSchemaStream->Entry;
		Enumeration = pSchemaStream->Enumeration;
		PropertyGroup = pSchemaStream->PropertyGroup;
		File = pFile;

	}

	return value;
} // Open

/////////////////////////////////////////////////////////////////////////////
// read a single value from the given record. the return value will be of
// nullptr on failure
byte* CStream::GetValue( ULONG record, USHORT item /*= 0*/ )
{
	// I am a pessimist 
	bool bOkay = false;

	// size of a single value within the record
	const long lValueSize = ValueSize;

	// type of data in the file
	VARENUM eType = Type;

	// buffer to read data into
	m_arrBuffer.resize( 0 );

	// fills the buffer with zero values
	m_arrBuffer.resize( lValueSize, 0 );

	// return value
	byte* value = &m_arrBuffer[ 0 ];

	// offset into the file in bytes
	const long lOffset = Offset[ record ][ item ];

	// length of the file in bytes
	const long lLength = (long)File->GetLength();

	// if we are reading within the boundaries of the file
	if ( lOffset < lLength )
	{
		// seek the file to the value
		File->Seek( lOffset, 0 );

		// read a single value from the file
		File->Read( value, lValueSize );

		// we're okay
		bOkay = true;
	}

	// flag the problem if it happens
	ASSERT( bOkay == true );

	return value;
} // GetValue

/////////////////////////////////////////////////////////////////////////////
// a single value of a record is assigned
void CStream::SetValue( ULONG record, USHORT item, byte* value )
{
	// I am a pessimist 
	bool bOkay = false;

	const USHORT usRecordSize = Size;
	const USHORT usValueSize = ValueSize;
	VARENUM eType = Type;

	// offset into the file in bytes
	const ULONG ulOffset = Offset[ record ][ item ];

	// length of the file
	ULONG ulLength = FileSize;

	// allow the write to append a single record
	if ( ulLength == ulOffset )
	{
		ulLength += usRecordSize;
		File->SetLength( ulLength );
	}

	// if we are writing within the boundaries of the file
	if ( ulOffset < ulLength )
	{
		// seek the file to the value
		File->Seek( ulOffset, 0 );

		// buffer to read data into
		vector<byte> buf( usValueSize, 0 );
		byte* pBuf = &buf[ 0 ];

		// copy the value to the write buffer
		::memcpy( pBuf, value, usValueSize );

		// write the buffer to the file
		File->Write( (void*)pBuf, usValueSize );

		bOkay = true;
	}

	// flag the problem if it happens
	ASSERT( bOkay == true );

} // SetValue

/////////////////////////////////////////////////////////////////////////////
// a single value of a record can return VT_EMPTY on failure
COleVariant CStream::GetVariant( ULONG record, USHORT item )
{
	// return value
	COleVariant var;

	const ULONG ulFileSize = FileSize;
	const ULONG ulOffset = Offset[ record ][ item ];
	if ( ulFileSize <= ulOffset )
	{
		return var;
	}

	const USHORT usSizeValue = ValueSize;
	byte* pBuf = Value[ record ][ item ];

	const VARENUM eVt = Type;
	switch ( eVt )
	{
		case VT_I1: 
		{
			var = *pBuf;
			var.ChangeType( eVt );
			break;
		}
		case VT_UI1: var = *pBuf; break;
		case VT_I2: var = (short)*pBuf; break;
		case VT_UI2: 
		{
			var = (short)*pBuf;
			var.ChangeType( eVt );
			break;
		}
		case VT_I4: var = (long)*pBuf; break;
		case VT_UI4: 
		{
			var = (long)*pBuf;
			var.ChangeType( eVt );
			break;
		}
		case VT_I8: var = (LONGLONG)*pBuf; break;
		case VT_UI8: var = (ULONGLONG)*pBuf; break;
		case VT_R4:
		{
			const float value = (float)*pBuf;
			if ( !::_finite( value ) )
			{
				var = (float)Null;
				Variant[ record ][ item ] = var;

			}
			else
			{
				var = value;
			}
			break;
		}
		case VT_R8:
		{
			const double value = (double)*pBuf;
			if ( !::_finite( value ) )
			{
				var = Null;
				Variant[ record ][ item ] = var;
			}
			else
			{
				var = value;
			}
			break;
		}
	}

	return var;
} // GetVariant

/////////////////////////////////////////////////////////////////////////////
// a single value of a record can return VT_EMPTY on failure
void CStream::SetVariant( ULONG record, USHORT item, COleVariant value )
{
	// I am a pessimist 
	bool bOkay = false;

	const USHORT usRecordSize = Size;
	const USHORT usValueSize = ValueSize;
	VARENUM eType = Type;

	// offset into the file in bytes
	const ULONG ulOffset = Offset[ record ][ item ];

	// length of the file
	ULONG ulLength = FileSize;

	// allow the write to append a single record
	if ( ulLength == ulOffset )
	{
		ulLength += usRecordSize;
		File->SetLength( ulLength );
	}

	// if we are writing within the boundaries of the file
	if ( ulOffset >= ulLength )
	{
		// flag the problem if it happens
		CHelper::ErrorMessage( __FILE__, __LINE__ );

		return;
	}
	
	// type compatibility?
	const VARENUM eVt = Type;
	if ( value.vt != eVt )
	{
		if ( usValueSize == 0 )
		{
			// flag the problem if it happens
			CHelper::ErrorMessage( __FILE__, __LINE__ );

			return; // not supported
		}

		value.ChangeType( eVt );
	}

	// seek the file to the value
	File->Seek( ulOffset, 0 );

	// buffer to read data into
	vector<byte> buf( usRecordSize, 0 );
	byte* pBuf = &buf[ 0 ];

	bOkay = true;
	
	// assign the variant based on type
	switch ( eVt )
	{
		case VT_I1: ::memcpy( pBuf, &value.cVal, usValueSize ); break;
		case VT_UI1: ::memcpy( pBuf, &value.bVal, usValueSize ); break;
		case VT_I2: ::memcpy( pBuf, &value.iVal, usValueSize ); break;
		case VT_UI2: ::memcpy( pBuf, &value.uiVal, usValueSize ); break;
		case VT_I4: ::memcpy( pBuf, &value.lVal, usValueSize ); break;
		case VT_UI4: ::memcpy( pBuf, &value.ulVal, usValueSize ); break;
		case VT_I8: ::memcpy( pBuf, &value.llVal, usValueSize ); break;
		case VT_UI8: ::memcpy( pBuf, &value.ullVal, usValueSize ); break;
		case VT_R4: ::memcpy( pBuf, &value.fltVal, usValueSize ); break;
		case VT_R8: ::memcpy( pBuf, &value.dblVal, usValueSize ); break;
		case VT_BSTR:
		{
			CString csValue( value.bstrVal );
			::memcpy( pBuf, csValue.GetBuffer(), usRecordSize );
			break;
		}
		default :
		{
			bOkay = false;

			// flag the problem if it happens
			CHelper::ErrorMessage( __FILE__, __LINE__ );
		}
	}

	// if everything worked out okay, write the data to the file
	if ( bOkay )
	{
		// write the buffer to the file
		File->Write( (void*)pBuf, usValueSize );
	}

} // SetVariant

/////////////////////////////////////////////////////////////////////////////
// read data into the stream cache from the given pair of zero based level
// numbers inclusive.  These are actual level values (not indices), so data
// that is routinely filtered out by the indexing will be included 
bool CStream::ReadIntoCache
(
	ULONG ulFirstLevel, // start level to read
	ULONG ulLastLevel // last level to read
)
{
	const bool bOpen = IsOpen;
	if ( !bOpen )
	{
		ASSERT( FALSE );
		return false;
	}

	const ULONG ulLevels = Levels;
	if ( ulLastLevel >= ulLevels )
	{
		ASSERT( FALSE );
		return false;
	}

	// the levels are inclusive
	const ULONG ulRecords = ulLastLevel - ulFirstLevel + 1;
	const double dNull = Null;
	const VARENUM eType = Type;
	const ULONG ulElements = LevelValues;
	//const CString csUnits = Units;

	// create the stream cache to contain our data as it is defined
	// and allocate the space
	Cache->Create( dNull, eType, ulElements, ulRecords );

	// ask the stream cache how many bytes to read
	const ULONG ulBytes = Cache->NumberOfBytes;

	// ask the stream cache for a void pointer to its buffer
	void* pBuffer = Cache->GetData();

	// position the stream pointer to the first level to be read
	SeekToLevel( ulFirstLevel );

	// tell the stream to read the levels into the stream cache
	const ULONG ulRead = File->Read( pBuffer, ulBytes );

	// see how we did
	const bool bOK = ulRead == ulBytes;

	return bOK;
} // ReadIntoCache

/////////////////////////////////////////////////////////////////////////////
// write data from the stream cache to the stream beginning at the given
// level number. the stream cache should be stored in the stream's
// native units (not display units).  the stream cache should have
// been originally populated by the ReadStreamCache to be sure that
// missing levels which are routinely filtered out by the indexing
// (null index, reverse transitions for drilling data, and forward 
// transitions for back plots, etc.). are in place.
bool CStream::WriteCacheData
( 
	ULONG ulFirstLevel, // start level to read
	// if writing more than one block, set the flush on the 
	// last block written to insure changes are flushed from
	// the stream's cache (if set to true for every block, the
	// process will be slower)
	bool bFlush
)
{
	const bool bOpen = IsOpen;
	if ( !bOpen )
	{
		ASSERT( FALSE );
		return false;
	}

	// number of levels in the stream
	const ULONG ulLevels = Levels;

	// number of levels in the stream cache
	const ULONG ulRecordss = Cache->Levels;

	// the last level to be written to in the stream
	const ULONG ulLastLevel = ulFirstLevel + ulRecordss;
	if ( ulLastLevel > ulLevels )
	{
		ASSERT( FALSE );
		return false;
	}

	// the levels are inclusive
	const double dNull = Cache->Null;
	const VARENUM eType = Cache->Type;
	const ULONG ulElements = Cache->LevelElements;
	const CString csUnits = Cache->Units;

	// sanity checks
	if ( !CHelper::NearlyEqual( dNull, Null ) )
	{
		ASSERT( FALSE );
		return false;
	}

	// the cache and stream need to be the same type
	if ( eType != Type )
	{
		ASSERT( FALSE );
		return false;
	}

	if ( ulElements != LevelValues )
	{
		ASSERT( FALSE );
		return false;
	}

	if ( csUnits != PersistUnits )
	{
		ASSERT( FALSE );
		return false;
	}

	// now that we have done due diligence, write the data to the stream
	// ask the stream cache how many bytes to write
	const ULONG ulBytes = Cache->NumberOfBytes;

	// ask the stream cache for a void pointer to its buffer
	void* pBuffer = Cache->GetData();

	// position the stream pointer to the first level to be written
	SeekToLevel( ulFirstLevel );

	// tell the stream to write the levels to the file system cache
	File->Write( pBuffer, ulBytes );

	// this will force the file's cache to be written to the store
	// and should only be set for the last block when making multiple
	// writes
	if ( bFlush )
	{
		File->Flush();
	}

	return true;
} // WriteCacheData

/////////////////////////////////////////////////////////////////////////////
// get a pointer to the level of a stream cache or null if invalid
BYTE* CStream::GetLevelFromStreamCache
(
	// stream level to read from
	ULONG ulLevel,
	// top level of stream in the stream cache if the stream cache
	// does not contain the entire stream. this value is used with
	// the stream level to offset into the stream cache's row.
	ULONG ulTopLevel/* = 0*/
)
{
	BYTE* value = 0;

	const ULONG ulRecord = ulLevel - ulTopLevel;
	const ULONG ulLevels = Cache->Levels;
	if ( ulLevel < ulLevels )
	{
		value = (BYTE*)Cache->GetData( ulLevel, 0 );
	}

	return value;
} // GetLevelFromStreamCache

/////////////////////////////////////////////////////////////////////////////
// set a value in display units into a stream cache containing
// levels of a curve in the curve's native units
bool CStream::SetValueInStreamCache
(	
	// value in display units to write
	double dValue,
	// curve level to set
	ULONG ulLevel,
	// element to set (when more than one value per level)
	ULONG ulElement/* = 0*/,
	// top level of curve in the stream cache if the stream cache
	// does not contain the entire curve. This value is set with
	// the curve level to offset into the stream cache's row.
	ULONG ulTopLevel/* = 0*/
)
{
	const ULONG ulRecord = ulLevel - ulTopLevel;
	const ULONG ulRecords = Cache->Levels;
	if ( ulRecord > ulRecords )
	{
		ASSERT( FALSE );
		return false;
	}

	// if there is a unit category defined, convert the data
	const CString csUnitCategory = UnitCategory;
	if ( !csUnitCategory.IsEmpty() && csUnitCategory != _T( "NONE" ) )
	{
		const double dNull = Cache->Null;
		const bool bNull = CHelper::NearlyEqual( dValue, dNull );
		if ( !bNull )
		{
			const double dAdd = Additive;
			const double dMult = Multiplier;

			// do a reverse conversion to return the value to the
			// curve's stored units
			if ( !CHelper::NearlyEqual( dMult, 0.0 ) )
			{
				dValue -= dAdd;
				dValue /= dMult;
			}
		}
	}

	// now write the value to the stream cache's level and element
	// position
	const bool bOK = Cache->SetDouble( dValue, ulRecord, 0, ulElement );

	return bOK;
} // SetValueInStreamCache

/////////////////////////////////////////////////////////////////////////////
// get a value in display units from a stream cache containing
// levels of a curve in the curve's native units
bool CStream::GetValueFromStreamCache
(	
	// value to read into
	double& dValue,
	// curve level to read from
	ULONG ulLevel,
	// element to read (when more than one value per level)
	ULONG ulElement/* = 0*/,
	// top level of curve in the stream cache if the stream cache
	// does not contain the entire curve. this value is used with
	// the curve level to offset into the stream cache's row.
	ULONG ulTopLevel/* = 0*/
)
{
	// default to null value
	const double dNull = Null;
	dValue = dNull;

	const ULONG ulRecord = ulLevel - ulTopLevel;
	const ULONG ulRecords = Cache->Levels;
	if ( ulRecord >= ulRecords )
	{
		ASSERT( FALSE );
		return false;
	}

	// now read the value from the stream cache's row and element
	// position
	dValue = Cache->GetDouble( ulRecord, 0, ulElement );
	if ( CHelper::NearlyEqual( dValue, dNull ) )
	{
		return true;
	}

	// if there are no units set, assume data already in display units
	const CString csUnits = Cache->Units;
	if ( csUnits != DisplayUnits )
	{
		return true;
	}

	// see if a conversion is needed and get out if not
	const CString csUnitCategory = UnitCategory;
	if ( csUnitCategory.IsEmpty() || csUnitCategory == _T( "NONE" ) )
	{
		return true;
	}

	// convert into display units
	const double dAdd = Additive;
	const double dMult = Multiplier;
	dValue *= dMult;
	dValue += dAdd;

	return true;
} // GetValueFromStreamCache

/////////////////////////////////////////////////////////////////////////////
