/////////////////////////////////////////////////////////////////////////////
// Copyright � 2022 by W. T. Block, all rights reserved
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
	File = nullptr;
}

/////////////////////////////////////////////////////////////////////////////
CStream::~CStream()
{
	CFile* pFile = File;
	if ( pFile != nullptr )
	{
		delete pFile;
	}
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
	bool bCreate = false;
	ULONG ulLevels = 0;
	Pathname = csPath;
	SchemaStream = pSchemaStream;
	Name = csName;
	CFile* pFile = File;
	if ( pFile == nullptr )
	{
		pFile = new CFile;
	}

	// we are either opening an existing file or creating it
	if ( ::PathFileExists( csPath ) )
	{
		// open an existing file
		value = pFile->Open
		( 
			csPath, 
			CFile::modeReadWrite | CFile::shareDenyNone
		);
	}
	else
	{
		// create the file
		bCreate = true;
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
		const ULONG ulLevelValues = LevelValues;
		File = pFile;
		COleDateTime cd = CreationDate;
		COleDateTime md = ModificationDate;

		const ULONG ulLevels = Levels;
		Cache->Create( Null, Type, ulLevelValues, ulLevels );
		if ( !bCreate && ulLevels > 0 )
		{
			ReadIntoCache( 0, ulLevels - 1 );
		}
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

	// type of data in the file
	VARENUM eType = Type;
	if ( eType == VT_I1 ) // special string handling
	{
		item = 0;
	}

	// size of a single value within the record
	const long lValueSize = eType == VT_I1 ? Size : ValueSize;

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

	// special handling for strings which are stored as VT_I1
	// (character) so the value size is not 1 but the record
	// size instead
	VARENUM eType = Type;
	const USHORT usRecordSize = Size;
	USHORT usValueSize = ValueSize;
	if ( VT_I1 == eType )
	{
		item = 0;
		usValueSize = usRecordSize;
	}

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
// string property for a given record
CString CStream::GetString( ULONG record )
{
	CString value;
	const ULONG ulLevels = Levels;
	if ( record >= ulLevels )
	{
		return value;
	}

	// get the data at the beginning of the record
	void* pData = GetValue( record, 0 );

	// entry enumeration implies a text value associated with a number
	ENUM_ENTRY eEnum = Entry;
	const bool bEnum = eEnum == eeEnum1 || eEnum == eeEnum2;

	// size of a single value within the record
	const size_t lValueSize = (size_t)ValueSize;

	// size of the records
	const size_t usSize = (size_t)Size;

	// create a buffer large enough to hold the record plus a null character
	vector<char> buffer = vector<char>( usSize + 1, 0 );
	void* pBuf = &buffer[ 0 ];

	// copy the data to the buffer
	::memcpy( pBuf, pData, usSize );

	VARENUM vt = Type;
	switch ( vt )
	{
		case VT_I1:  
		{
			// read the value out as a string
			value = (LPSTR)pBuf;
			break;
		}
		case VT_UI1: 
		{
			byte temp = 0;
			::memcpy( &temp, pBuf, lValueSize );

			// enumerations are always VT_UI1 type
			if ( bEnum )
			{
				value = *m_mapEnum2Text.find( temp );
			}
			else
			{
				value.Format( _T( "%d" ), temp );
			}
			break;
		}
		case VT_I2:  
		{
			short temp = 0;
			::memcpy( &temp, pBuf, lValueSize );
			value.Format( _T( "%d" ), temp );
			break;
		}
		case VT_UI2: 
		{
			USHORT temp = 0;
			::memcpy( &temp, pBuf, lValueSize );
			value.Format( _T( "%d" ), temp );
			break;
		}
		case VT_I4:  
		{
			long temp = 0;
			::memcpy( &temp, pBuf, lValueSize );
			value.Format( _T( "%d" ), temp );
			break;
		}
		case VT_UI4: 
		{
			ULONG temp = 0;
			::memcpy( &temp, pBuf, lValueSize );
			value.Format( _T( "%d" ), temp );
			break;
		}
		case VT_I8:  
		{
			LONGLONG temp = 0;
			::memcpy( &temp, pBuf, lValueSize );
			value.Format( _T( "%d" ), temp );
			break;
		}
		case VT_UI8: 
		{
			ULONGLONG temp = 0;
			::memcpy( &temp, pBuf, lValueSize );
			value.Format( _T( "%d" ), temp );
			break;
		}
		case VT_R4:  
		{
			float temp = 0;
			::memcpy( &temp, pBuf, lValueSize );
			value.Format( _T( "%g" ), temp );
			break;
		}
		case VT_DATE:
		{
			double temp = 0;
			::memcpy( &temp, pBuf, lValueSize );
			COleDateTime oDT( temp );
			value = oDT.Format( _T( "%Y-%m-%d %H:%M:%S" ));
			break;
		}
		case VT_R8:  
		{
			double temp = 0;
			::memcpy( &temp, pBuf, lValueSize );
			value.Format( _T( "%g" ), temp );
			break;
		}
		default: return value;
	}

	return value;
} // GetString

/////////////////////////////////////////////////////////////////////////////
// a single value of a record can return VT_EMPTY on failure
COleVariant CStream::GetVariant( ULONG record, USHORT item )
{
	// return value
	COleVariant var;

	const VARENUM eVt = Type;

	// VT_I1 is treated like an array of characters with
	// a single value
	if ( eVt == VT_I1 )
	{
		item = 0;
	}

	const ULONG ulFileSize = FileSize;
	const ULONG ulOffset = Offset[ record ][ item ];
	if ( ulFileSize <= ulOffset )
	{
		return var;
	}

	const USHORT usSizeValue = ValueSize;
	byte* pBuf = Value[ record ][ item ];

	switch ( eVt )
	{
		case VT_I1: 
		{
			const CString csValue = String[ ulOffset ];
			VARIANT variant;
			::VariantInit( &variant );
			variant.vt = VT_BSTR;
			variant.bstrVal = csValue.AllocSysString();
			::VariantCopy( &var, &variant );
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
		case VT_DATE:
		{
			const double value = (double)*pBuf;
			if ( !::_finite( value ) )
			{
				var = Null;
				Variant[ record ][ item ] = var;
			}
			else
			{
				COleDateTime oDT( value );
				var = oDT;
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
	USHORT usValueSize = ValueSize;

	if ( usValueSize == 0 )
	{
		// flag the problem if it happens
		CHelper::ErrorMessage( __FILE__, __LINE__ );

		return; // not supported
	}

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

	// a string can be written into an array of characters
	const bool bStringCase = eVt == VT_I1 && value.vt == VT_BSTR;

	// handle string case
	if ( bStringCase )
	{
		item = 0;
		usValueSize = usRecordSize;
	}
	else if ( value.vt != eVt )
	{
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
		case VT_I1:
		{
			// an array of characters is treated as a single
			// string value
			if ( bStringCase )
			{
				CString csValue( value.bstrVal );
				::memcpy( pBuf, csValue.GetBuffer(), usRecordSize );
			}
			else // single character
			{
				::memcpy( pBuf, &value.cVal, usValueSize );
			}
			break;
		}
		case VT_UI1: ::memcpy( pBuf, &value.bVal, usValueSize ); break;
		case VT_I2: ::memcpy( pBuf, &value.iVal, usValueSize ); break;
		case VT_UI2: ::memcpy( pBuf, &value.uiVal, usValueSize ); break;
		case VT_I4: ::memcpy( pBuf, &value.lVal, usValueSize ); break;
		case VT_UI4: ::memcpy( pBuf, &value.ulVal, usValueSize ); break;
		case VT_I8: ::memcpy( pBuf, &value.llVal, usValueSize ); break;
		case VT_UI8: ::memcpy( pBuf, &value.ullVal, usValueSize ); break;
		case VT_R4: ::memcpy( pBuf, &value.fltVal, usValueSize ); break;
		case VT_R8: ::memcpy( pBuf, &value.dblVal, usValueSize ); break;
		case VT_DATE: ::memcpy( pBuf, &value.date, usValueSize ); break;
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
	const ULONG ulValues = LevelValues;
	//const CString csUnits = Units;

	// create the stream cache to contain our data as it is defined
	// and allocate the space
	Cache->Create( dNull, eType, ulValues, ulRecords );

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
