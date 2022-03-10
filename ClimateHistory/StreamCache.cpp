/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "StreamCache.h"
#include <float.h>

/////////////////////////////////////////////////////////////////////////////
// default constructor
CStreamCache::CStreamCache(void)
{
	Null = -9999;
	Type = VT_EMPTY;
	LevelElements = 1;

	Multiplier = 1;
	Additive = 0;

	FirstLive = Null;
	LastLive = Null;
	Minimum = Null;
	Maximum = Null;
	Average = Null;

	LevelFirstLive = -1;
	LevelLastLive = -1;
	LevelMinimum = -1;
	LevelMaximum = -1;
}

/////////////////////////////////////////////////////////////////////////////
// assignment constructor
CStreamCache::CStreamCache
(
	double dNull, 
	VARENUM eType, 
	ULONG ulLevelElements/* = 1*/,
	ULONG ulLevels/* = 0*/,
	LPCTSTR pcszUnits/* = _T( "" )*/
)
{
	Create( dNull, eType, ulLevelElements, ulLevels, pcszUnits );
}

/////////////////////////////////////////////////////////////////////////////
CStreamCache::~CStreamCache(void)
{
}

/////////////////////////////////////////////////////////////////////////////
// create a variant vector
void CStreamCache::Create
(
	double dNull, // the empty value
	VARENUM eType, // variant type
	ULONG ulLevelElements/* = 1*/, // elements per column
	ULONG ulLevels/* = 0*/, // number of levels (if Set will fill with nulls)
	LPCTSTR pcszUnits/* = _T( "" )*/ // units
)
{
	clear();
	Null = dNull;
	Type = eType;
	LevelElements = ulLevelElements;
	Units = pcszUnits;

	Multiplier = 1;
	Additive = 0;

	// if the number of levels are given, preallocate with 
	// null values
	if ( ulLevels != 0 )
	{
		const ULONG ulLevelElements = LevelElements;
		const ULONG ulElements = ulLevels * ulLevelElements;
		resize( ulElements );
	}
} // Create

/////////////////////////////////////////////////////////////////////////////
// number of elements of data
ULONG CStreamCache::GetVectorElements()
{
	const VARENUM eVt = Type;
	switch ( eVt )
	{
		case VT_I1: return (ULONG)m_StreamChars.size();
		case VT_UI1: return (ULONG)m_StreamBytes.size();
		case VT_I2: return (ULONG)m_StreamShorts.size();
		case VT_UI2: return (ULONG)m_StreamUShorts.size();
		case VT_I4: return (ULONG)m_StreamLongs.size();
		case VT_UI4: return (ULONG)m_StreamULongs.size();
		case VT_I8: return (ULONG)m_StreamLongLongs.size();
		case VT_UI8: return (ULONG)m_StreamULongLongs.size();
		case VT_R4: return (ULONG)m_StreamFloats.size();
		case VT_R8: return (ULONG)m_StreamDoubles.size();
		default: return 0;
	}
} // GetVectorElements

/////////////////////////////////////////////////////////////////////////////
// assignment operator
CStreamCache& CStreamCache::operator=( CStreamCache& src )
{
	m_dNull = src.m_dNull;
	m_vtType = src.m_vtType;
	m_ulLevelElements = src.m_ulLevelElements;
	m_csUnits = src.m_csUnits;
	m_StreamChars = src.m_StreamChars;
	m_StreamBytes = src.m_StreamBytes;
	m_StreamShorts = src.m_StreamShorts;
	m_StreamUShorts = src.m_StreamUShorts;
	m_StreamLongs = src.m_StreamLongs;
	m_StreamULongs = src.m_StreamULongs;
	m_StreamLongLongs = src.m_StreamLongLongs;
	m_StreamULongLongs = src.m_StreamULongLongs;
	m_StreamFloats = src.m_StreamFloats;
	m_StreamDoubles = src.m_StreamDoubles;
	return *this;
} // operator=

/////////////////////////////////////////////////////////////////////////////
// clear the data
void CStreamCache::clear()
{
	const VARENUM eVt = Type;
	switch ( eVt )
	{
		case VT_I1: m_StreamChars.clear(); break;
		case VT_UI1 : m_StreamBytes.clear(); break;
		case VT_I2 : m_StreamShorts.clear(); break;
		case VT_UI2 : m_StreamUShorts.clear(); break;
		case VT_I4 : m_StreamLongs.clear(); break;
		case VT_UI4 : m_StreamULongs.clear(); break;
		case VT_I8 : m_StreamLongLongs.clear(); break;
		case VT_UI8 : m_StreamULongLongs.clear(); break;
		case VT_R4 : m_StreamFloats.clear(); break;
		case VT_R8 : m_StreamDoubles.clear(); break;
	}
} // clear
	
/////////////////////////////////////////////////////////////////////////////
// Set data from a char* pointer
bool CStreamCache::SetData( VARENUM nType, ULONG ulValues, char* pData )
{
	Type = nType;
	resize( ulValues );

	// if the resize above failed, return a false
	if ( VectorElements == 0 )
	{
		SetType();
		return false;
	}

	const ULONG ulBytes = ElementSizeInBytes * ulValues;
	void* pDest = GetData( 0 );
	::CopyMemory( pDest, pData, ulBytes );

	return true;
} // SetData
	
/////////////////////////////////////////////////////////////////////////////
// Get a void pointer to the data and optionally to 
// a specific index of the data
void* CStreamCache::GetData( ULONG ulIndex/* = 0*/ )
{
	void* pData = 0;
	const ULONG ulValues = VectorElements;
	if ( ulValues == 0 )
	{
		return pData;
	}

	const VARENUM eVt = Type;
	switch ( eVt )
	{
		case VT_I1: pData = &m_StreamChars[ ulIndex ]; break;
		case VT_UI1 : pData = &m_StreamBytes[ ulIndex ]; break;
		case VT_I2 : pData = &m_StreamShorts[ ulIndex ]; break;
		case VT_UI2 : pData = &m_StreamUShorts[ ulIndex ]; break;
		case VT_I4 : pData = &m_StreamLongs[ ulIndex ]; break;
		case VT_UI4 : pData = &m_StreamULongs[ ulIndex ]; break;
		case VT_I8 : pData = &m_StreamLongLongs[ ulIndex ]; break;
		case VT_UI8 : pData = &m_StreamULongLongs[ ulIndex ]; break;
		case VT_R4 : pData = &m_StreamFloats[ ulIndex ]; break;
		case VT_R8 : pData = &m_StreamDoubles[ ulIndex ]; break;
	}
	return pData;
} // GetData

/////////////////////////////////////////////////////////////////////////////
// Get a void pointer to the data at a given row, column, and element
// where the data is potentially formatted as a matrix which can 
// be an array of arrays. for a simple two dimensional array, the
// column will be zero and the element is the column.
void* CStreamCache::GetData
(	ULONG ulLevel, // row of the table
	ULONG ulElement // element of the level
)
{
	void* pData = 0;
	const ULONG ulLevels = Levels;
	if ( ulLevel >= ulLevels )
	{
		return pData;
	}
	const ULONG ulIndex = GetIndex( ulLevel, ulElement );
	pData = GetData( ulIndex );
	return pData;
} // GetData
	
/////////////////////////////////////////////////////////////////////////////
// resize the data to the given number of elements. if the size
// is extended, the new levels are filled with null values.
// if the size is reduced, data will be deleted at the end.
// if bProtectContents is true, the existing data will remain
// unchanged except when the size is reduced.
// if bProtectContents is false, the entire content will be
// filled with null values.
void CStreamCache::resize
(	// new size of the of the vector in values
	ULONG values, 
	// protect existing content when true or null the values when false
	bool bProtectContents/* = false*/ 
)
{
	const VARENUM eVt = Type;
	const double dNull = Null;
	
	// if not protecting existing content, clear the content out
	if ( !bProtectContents )
	{
		clear();
	}
	switch ( eVt )
	{
		case VT_I1: m_StreamChars.resize( values, (CHAR)dNull ); break;
		case VT_UI1 : m_StreamBytes.resize( values, (UCHAR)dNull ); break;
		case VT_I2 : m_StreamShorts.resize( values, (SHORT)dNull ); break;
		case VT_UI2 : m_StreamUShorts.resize( values, (USHORT)dNull ); break;
		case VT_I4 : m_StreamLongs.resize( values, (LONG)dNull ); break;
		case VT_UI4 : m_StreamULongs.resize( values, (ULONG)dNull ); break;
		case VT_I8 : m_StreamLongLongs.resize( values, (LONGLONG)dNull ); break;
		case VT_UI8 : m_StreamULongLongs.resize( values, (ULONGLONG)dNull ); break;
		case VT_R4 : m_StreamFloats.resize( values, (FLOAT)dNull ); break;
		case VT_R8 : m_StreamDoubles.resize( values, (DOUBLE)dNull ); break;
	}
} // resize
	
/////////////////////////////////////////////////////////////////////////////
// data element size in bytes (0 for unsupported type)
short CStreamCache::GetElementSizeInBytes( VARENUM eVt )
{
	switch ( eVt )
	{
		case VT_I1: return sizeof( CHAR );
		case VT_UI1 : return sizeof( BYTE );
		case VT_I2 : return sizeof( SHORT );
		case VT_UI2 : return sizeof( WORD );
		case VT_I4 : return sizeof( LONG );
		case VT_UI4 : return sizeof( ULONG );
		case VT_I8 : return sizeof( LONGLONG );
		case VT_UI8 : return sizeof( ULONGLONG );
		case VT_R4 : return sizeof( FLOAT );
		case VT_R8 : return sizeof( DOUBLE );
		default : return 0;
	}
} // GetElementSizeInBytes

/////////////////////////////////////////////////////////////////////////////
// convert values
bool CStreamCache::Convert( DOUBLE dMult, DOUBLE dAdd )
{	// if the multiplier is 1 and the add is zero, do nothing
	if ( CHelper::NearlyEqual( dMult, 1.0, 0.0000001 ) )
	{
		if ( CHelper::NearlyEqual( dAdd, 0.0, 0.0000001 ) )
		{
			return true;
		}
	}

	const ULONG ulValues = VectorElements;
	const VARENUM varEnum = Type;
	if ( varEnum == VT_EMPTY || ulValues == 0 )
	{
		return false;
	}

	const double dNull = Null;
	for ( ULONG ulValue = 0; ulValue < ulValues; ulValue++ )
	{
		const double dInput = GetDouble( ulValue );
		if ( CHelper::NearlyEqual( dInput, dNull ) )
		{
			continue;
		}

		const double dValue = dInput * dMult + dAdd;
		if ( !::_finite( dValue ) )
		{
			SetDouble( ulValue, dNull );

		} else
		{
			SetDouble( ulValue, dValue );
		}
	}
	return true;
} // Convert

/////////////////////////////////////////////////////////////////////////////
// find the minimum and maximum values
bool CStreamCache::FindMinMax( DOUBLE& dMinimum, DOUBLE& dMaximum )
{
	dMinimum = DBL_MAX;
	dMaximum = DBL_MIN;
	const ULONG ulValues = VectorElements;
	const VARENUM varEnum = Type;
	if ( varEnum == VT_EMPTY || ulValues == 0 )
	{
		return false;
	}

	const double dNull = Null;
	ULONG ulCount = 0;
	for ( ULONG ulValue = 0; ulValue < ulValues; ulValue++ )
	{
		COleVariant var = GetValue( ulValue );
		var.ChangeType( VT_R8 );
		const double dValue = var.dblVal;
		if ( !::_finite( dValue ) )
		{
			continue;
		}
		if ( CHelper::NearlyEqual( dValue, dNull ) )
		{
			continue;
		}
		ulCount++;
		dMinimum = min( dMinimum, dValue );
		dMaximum = max( dMaximum, dValue );
	}

	return ulCount != 0;
} // FindMinMax

/////////////////////////////////////////////////////////////////////////////
// calculate statistics--rather than making a pass through the data
// separately for each statistic, calculate them all in one pass
bool CStreamCache::CalculateStatistics()
{
	bool value = false;

	// do a quick sanity check before Getting started
	const ULONG ulValues = VectorElements;
	const VARENUM varEnum = Type;
	if ( varEnum == VT_EMPTY || ulValues == 0 )
	{
		return value;
	}

	// Setup the statistical values
	const double dNull = Null;
	double dMinimum = DBL_MAX;
	ULONG ulMinimum = -9999;
	double dMaximum = DBL_MIN;
	ULONG ulMaximum = -9999;
	double dTotal = dNull;
	double dFirstLive = dNull;
	ULONG ulFirstLive = -9999;
	double dLastLive = dNull;
	ULONG ulLastLive = -9999;
	bool bFirst = false;

	ULONG ulCount = 0;
	for ( ULONG ulValue = 0; ulValue < ulValues; ulValue++ )
	{
		// Get the value as a double
		const double dValue = GetDouble( ulValue );

		// check for not a number
		if ( !::_finite( dValue ) )
		{
			continue;
		}

		// check for a null value
		if ( CHelper::NearlyEqual( dValue, dNull, 0.001 ) )
		{
			continue;
		}

		// check for a -9999 value
		if ( CHelper::NearlyEqual( dValue, -9999.0, 0.001 ) )
		{
			continue;
		}

		// increment the value counter
		ulCount++;

		// if first live has not been found, assign it now
		if ( bFirst == false )
		{
			dFirstLive = dValue;
			ulFirstLive = ulValue;
			bFirst = true;
		}

		// do the other calculations
		if ( dValue < dMinimum )
		{
			dMinimum = dValue;
			ulMinimum = ulValue;
		}

		if ( dValue > dMaximum )
		{
			dMaximum = dValue;
			ulMaximum = ulValue;
		}

		dTotal += dValue;
		dLastLive = dValue;
		ulLastLive = ulValue;
	}

	// we are good if there are any values
	value = ulCount != 0;

	// if we were successful, record the statistics
	if ( value == true )
	{
		FirstLive = dFirstLive;
		LastLive = dLastLive;
		Average = dTotal / ulCount;
		Minimum = dMinimum;
		Maximum = dMaximum;

		LevelFirstLive = ulFirstLive;
		LevelLastLive = ulLastLive;
		LevelMinimum = ulMinimum;
		LevelMaximum = ulMaximum;
	}

	return value;
} // CalculateStatistics

/////////////////////////////////////////////////////////////////////////////
// change the data type of an existing buffer by copying from one 
// buffer to another as a byte array...it is up to the application
// to insure this makes sense.  For example, if the vector is sized
// for 10 doubles (80 bytes), data is copied into the vector as
// bytes, and ChangeType is used to put the data back to doubles, then
// the byte array should be 80 bytes long.
bool CStreamCache::ChangeType( VARENUM eNew )
{
	bool value = true;
	const VARTYPE eType = Type;
	if ( eType == eNew )
	{
		return value;
	}

	// number of bytes in the variant vector currently
	const ULONG ulBytesOld = NumberOfBytes;

	void* pOld = GetData( 0 );

	// change to the new type
	Type = eNew;

	// number of bytes in the variant vector after type change
	const ULONG ulBytesNew = NumberOfBytes;

	// the buffers need to be the same size
	value = ulBytesOld == ulBytesNew;

	// copy the old buffer to the new buffer if the buffers are the same 
	// size
	if ( value == true )
	{
		void* pNew = GetData( 0 );
		CopyMemory( pNew, pOld, ulBytesNew );
	}

	return value;
} // ChangeType
	
/////////////////////////////////////////////////////////////////////////////
// Set a value at a given index
void CStreamCache::SetValue( ULONG ulIndex, COleVariant var )
{	// range check?
	const ULONG ulValues = VectorElements;
	if ( ulValues <= ulIndex )
	{
		return; // out of range
	}
	// type compatibility?
	const VARENUM eVt = Type;
	if ( var.vt != eVt )
	{
		const short nSize = GetElementSizeInBytes( (VARENUM)var.vt );
		if ( nSize == 0 )
		{
			return; // not supported
		}
		var.ChangeType( eVt );
	}
	// assign the variant based on type
	switch ( eVt )
	{
		case VT_I1: m_StreamChars[ ulIndex ] = var.cVal; break;
		case VT_UI1 : m_StreamBytes[ ulIndex ] = var.bVal; break;
		case VT_I2 : m_StreamShorts[ ulIndex ] = var.iVal; break;
		case VT_UI2 : m_StreamUShorts[ ulIndex ] = var.uiVal; break; 
		case VT_I4 : m_StreamLongs[ ulIndex ] = var.lVal; break;
		case VT_UI4 : m_StreamULongs[ ulIndex ] = var.ulVal; break; 
		case VT_I8 : m_StreamLongLongs[ ulIndex ] = var.llVal; break;
		case VT_UI8 : m_StreamULongLongs[ ulIndex ] = var.ullVal; break;
		case VT_R4 : m_StreamFloats[ ulIndex ] = var.fltVal; break;
		case VT_R8 : m_StreamDoubles[ ulIndex ] = var.dblVal; break;
	}
} // SetValue

/////////////////////////////////////////////////////////////////////////////
// Get a value at a given index
COleVariant CStreamCache::GetValue( ULONG ulIndex )
{
	COleVariant var;
	VARIANT variant;
	::VariantInit( &variant );
	const ULONG ulValues = VectorElements;
	if ( ulValues <= ulIndex )
	{
		return var;
	}

	const VARENUM eVt = Type;
	variant.vt = eVt;
	switch ( eVt )
	{
		case VT_I1: // not handled by COleVariant directly
		{
			variant.ulVal = m_StreamChars[ ulIndex ];
			var = variant;
			break;
		}
		case VT_UI1 : var = m_StreamBytes[ ulIndex ]; break;
		case VT_I2 : var = m_StreamShorts[ ulIndex ]; break;
		case VT_UI2 : // not handled by COleVariant directly
		{
			variant.ulVal = m_StreamUShorts[ ulIndex ]; 
			var = variant;
			break;
		}
		case VT_I4 : var = m_StreamLongs[ ulIndex ]; break;
		case VT_UI4 : // not handled by COleVariant directly
		{
			variant.ulVal = m_StreamULongs[ ulIndex ]; 
			var = variant;
			break;
		}
		case VT_I8 : var = m_StreamLongLongs[ ulIndex ]; break;
		case VT_UI8 : var = m_StreamULongLongs[ ulIndex ]; break;
		case VT_R4 : 
		{
			const float value = m_StreamFloats[ ulIndex ]; 
			if ( !::_finite( value ))
			{
				var = (float)Null;
				SetValue( ulIndex, var );
			
			} else
			{
				var = value;
			}
			break;
		}
		case VT_R8 : 
		{
			const double value = m_StreamDoubles[ ulIndex ]; 
			if ( !::_finite( value ))
			{
				var = Null;
				SetValue( ulIndex, var );
			
			} else
			{
				var = value;
			}
			break;
		}
	}
	return var;
} // GetValue 

/////////////////////////////////////////////////////////////////////////////
// Get a double value at a given index and return null value if unsuccessful
double CStreamCache::GetDouble( ULONG ulIndex )
{
	double dValue = Null;
	const ULONG ulValues = VectorElements;
	if ( ulValues <= ulIndex )
	{
		return dValue;
	}

	const VARENUM eVt = Type;
	switch ( eVt )
	{
		case VT_I1: dValue = m_StreamChars[ ulIndex ]; break;
		case VT_UI1: dValue = m_StreamBytes[ ulIndex ]; break;
		case VT_I2: dValue = m_StreamShorts[ ulIndex ]; break;
		case VT_UI2: dValue = m_StreamUShorts[ ulIndex ]; break;
		case VT_I4: dValue = m_StreamLongs[ ulIndex ]; break;
		case VT_UI4: dValue = m_StreamULongs[ ulIndex ]; break;
		case VT_I8: dValue = (double)m_StreamLongLongs[ ulIndex ]; break;
		case VT_UI8: dValue = (double)m_StreamULongLongs[ ulIndex ]; break;
		case VT_R4: dValue = m_StreamFloats[ ulIndex ]; break;
		case VT_R8: dValue = m_StreamDoubles[ ulIndex ]; break;
	}
	
	// validate the value
	if ( !::_finite( dValue ) )
	{
		dValue = Null;
	}

	return dValue;
} // GetDouble 

/////////////////////////////////////////////////////////////////////////////
// Set a double value at a given index and return false if unsuccessful
bool CStreamCache::SetDouble( ULONG ulIndex, DOUBLE dValue )
{
	const ULONG ulValues = VectorElements;
	if ( ulValues <= ulIndex )
	{
		return false;
	}
	const VARENUM eVt = Type;
	switch ( eVt )
	{
		case VT_I1: m_StreamChars[ ulIndex ] = (CHAR)dValue; return true;
		case VT_UI1: m_StreamBytes[ ulIndex ] = (BYTE)dValue; return true;
		case VT_I2: m_StreamShorts[ ulIndex ] = (SHORT)dValue; return true;
		case VT_UI2: m_StreamUShorts[ ulIndex ] = (USHORT)dValue; return true;
		case VT_I4: m_StreamLongs[ ulIndex ] = (LONG)dValue; return true;
		case VT_UI4: m_StreamULongs[ ulIndex ] = (ULONG)dValue; return true;
		case VT_I8: m_StreamLongLongs[ ulIndex ] = (LONGLONG)dValue; return true;
		case VT_UI8: m_StreamULongLongs[ ulIndex ] = (ULONGLONG)dValue; return true;
		case VT_R4: m_StreamFloats[ ulIndex ] = (FLOAT)dValue; return true;
		case VT_R8: m_StreamDoubles[ ulIndex ] = dValue; return true;
	}
	return false;
} // SetDouble 

/////////////////////////////////////////////////////////////////////////////
// append a double value and return the count
ULONG CStreamCache::AppendDouble( DOUBLE dValue )
{
	const VARENUM eVt = Type;
	switch ( eVt )
	{
		case VT_I1: m_StreamChars.push_back( (CHAR)dValue ); break;
		case VT_UI1: m_StreamBytes.push_back( (BYTE)dValue ); break;
		case VT_I2: m_StreamShorts.push_back( (SHORT)dValue ); break;
		case VT_UI2: m_StreamUShorts.push_back( (USHORT)dValue ); break;
		case VT_I4: m_StreamLongs.push_back( (LONG)dValue ); break;
		case VT_UI4: m_StreamULongs.push_back( (ULONG)dValue ); break;
		case VT_I8: m_StreamLongLongs.push_back( (LONGLONG)dValue ); break;
		case VT_UI8: m_StreamULongLongs.push_back( (ULONGLONG)dValue ); break;
		case VT_R4: m_StreamFloats.push_back( (FLOAT)dValue ); break;
		case VT_R8: m_StreamDoubles.push_back( dValue ); break;
	}
	const ULONG ulValues = VectorElements;
	return ulValues;
} // AppendDouble 

/////////////////////////////////////////////////////////////////////////////
// Set a double array into the vector and return false if unsuccessful
bool CStreamCache::SetDoubles( vector<DOUBLE>& arrValues )
{
	ULONG ulValues = VectorElements;
	const ULONG ulDoubles = (ULONG)arrValues.size();
	if ( ulValues != ulDoubles )
	{
		resize( ulDoubles );
		ulValues = VectorElements;
	}
	const VARENUM eVt = Type;
	switch ( eVt )
	{
		case VT_I1:
			for ( ULONG ulIndex = 0; ulIndex < ulValues; ulIndex++ )
			{
				m_StreamChars[ ulIndex ] = (CHAR)arrValues[ ulIndex ];
			}
			return true;
		case VT_UI1:
			for ( ULONG ulIndex = 0; ulIndex < ulValues; ulIndex++ )
			{
				m_StreamBytes[ ulIndex ] = (BYTE)arrValues[ ulIndex ];
			}
			return true;
		case VT_I2:
			for ( ULONG ulIndex = 0; ulIndex < ulValues; ulIndex++ )
			{
				m_StreamShorts[ ulIndex ] = (SHORT)arrValues[ ulIndex ];
			}
			return true;
		case VT_UI2:
			for ( ULONG ulIndex = 0; ulIndex < ulValues; ulIndex++ )
			{
				m_StreamUShorts[ ulIndex ] = (USHORT)arrValues[ ulIndex ];
			}
			return true;
		case VT_I4:
			for ( ULONG ulIndex = 0; ulIndex < ulValues; ulIndex++ )
			{
				m_StreamLongs[ ulIndex ] = (LONG)arrValues[ ulIndex ];
			}
			return true;
		case VT_UI4:
			for ( ULONG ulIndex = 0; ulIndex < ulValues; ulIndex++ )
			{
				m_StreamULongs[ ulIndex ] = (ULONG)arrValues[ ulIndex ];
			}
			return true;
		case VT_I8:
			for ( ULONG ulIndex = 0; ulIndex < ulValues; ulIndex++ )
			{
				m_StreamLongLongs[ ulIndex ] = (LONGLONG)arrValues[ ulIndex ];
			}
			return true;
		case VT_UI8:
			for ( ULONG ulIndex = 0; ulIndex < ulValues; ulIndex++ )
			{
				m_StreamULongLongs[ ulIndex ] = (ULONGLONG)arrValues[ ulIndex ];
			}
			return true;
		case VT_R4:
			for ( ULONG ulIndex = 0; ulIndex < ulValues; ulIndex++ )
			{
				m_StreamFloats[ ulIndex ] = (FLOAT)arrValues[ ulIndex ];
			}
			return true;
		case VT_R8: m_StreamDoubles = arrValues; return true;
	}
	return false;
} // SetDoubles 

/////////////////////////////////////////////////////////////////////////////
// Set a double array into the given a level 
// returns false if the level is out of range
bool CStreamCache::SetDoubles
(
	ULONG ulLevel, vector<DOUBLE>& arrValues 
)
{
	const ULONG ulLevels = Levels;
	if ( ulLevel >= ulLevels )
	{
		return false;
	}
	const ULONG ulLevelElements = LevelElements;
	const ULONG ulElements = (ULONG)arrValues.size();
	ULONG ulIndex = GetIndex( ulLevel );
	for each ( DOUBLE dValue in arrValues )
	{
		SetDouble( ulIndex++, dValue );
	}
	return true;
} // SetDoubles

/////////////////////////////////////////////////////////////////////////////
// Get a double array from the given a level 
// returns false if the level is out of range
bool CStreamCache::GetDoubles
(
	ULONG ulLevel, vector<DOUBLE>& arrValues 
)
{
	arrValues.clear();
	const ULONG ulLevels = Levels;
	if ( ulLevel >= ulLevels )
	{
		return false;
	}

	const ULONG ulElements = LevelElements;

	arrValues.resize( ulElements );
	ULONG ulIndex = GetIndex( ulLevel );
	for ( ULONG ulElement = 0; ulElement < ulElements; ulElement++ )
	{
		arrValues[ ulElement ] = GetDouble( ulIndex++ );
	}

	return true;
} // GetDoubles

/////////////////////////////////////////////////////////////////////////////
// copy data into safe array
void CStreamCache::GetData( COleSafeArray& sa )
{
	const VARENUM vt = Type;
	const ULONG ulElements = VectorElements;
	if ( ulElements == 0 )
	{
		return;
	}
	void* pData = GetData( 0 );
	sa.CreateOneDim( vt, ulElements, (const void*)pData );
} // GetData

/////////////////////////////////////////////////////////////////////////////
// assigns the data from a safe array
bool CStreamCache::SetData( VARIANT& data )
{
	bool bOK = false;
	if ( !( data.vt & VT_ARRAY ) )
	{
		TRACE( _T( "CStreamCache::SetData error: expected an array.\n" ) );
		return bOK;
	}
	const VARENUM nType = (VARENUM)( data.vt & VT_TYPEMASK );
	const ULONG ulValues =
		(ULONG)CHelper::GetOneDimensionalElementCount( data );

	char* pData = 0;
	::SafeArrayAccessData( data.parray, (void**)&pData );
	bOK = SetData( nType, ulValues, pData );
	::SafeArrayUnaccessData( data.parray );
	bOK = ulValues == VectorElements;

	return bOK;
} // SetData

/////////////////////////////////////////////////////////////////////////////
