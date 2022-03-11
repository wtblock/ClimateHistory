/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "CHelper.h"
#include <vector>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// this class is used to buffer telemetry data in a type safe representation

// level elements refer to the number of values in a level
// and also defaults to one.  the default constructor will therefore create
// a simple vector.
// element refers to a single value of an intrinsic type (long, float, etc.)
// all indices are zero based (i.e. level, element)

class CStreamCache
{
// protected data
protected:
	// empty value is used to initialize data vector during a resize
	double m_dNull;
	
	// data type of the curve
	//  VT_EMPTY		undefined
	//  VT_R4			4 byte real
	//  VT_R8			8 byte real
	//  VT_I1			signed char
	//  VT_UI1			unsigned char
	//  VT_I2			2 byte signed int
	//  VT_UI2			unsigned short
	//  VT_I4			4 byte signed int
	//  VT_UI4			unsigned long
	//  VT_I8			signed 64-bit int
	//  VT_UI8			unsigned 64-bit int
	VARENUM m_vtType;

	// number of elements per level
	ULONG m_ulLevelElements; // defaults to one

	// units are stored here as a reference but
	// are not required internally
	CString m_csUnits;
	
	// unit class is stored here as a reference but
	// are not required internally
	CString m_csUnitClass;

	// multiplier value used for conversions
	DOUBLE m_dMultiplier;

	// additive value used for conversions
	DOUBLE m_dAdditive;
	
	// data dependent buffers
	vector<CHAR> m_StreamChars;
	vector<BYTE> m_StreamBytes;
	vector<SHORT> m_StreamShorts;
	vector<USHORT> m_StreamUShorts;
	vector<LONG> m_StreamLongs;
	vector<ULONG> m_StreamULongs;
	vector<LONGLONG> m_StreamLongLongs;
	vector<ULONGLONG> m_StreamULongLongs;
	vector<FLOAT> m_StreamFloats;
	vector<DOUBLE> m_StreamDoubles;

	// first live value is the first non-null value
	DOUBLE m_dFirstLive;

	// level of first live value 
	ULONG m_ulLevelFirstLive;

	// first live value is the first non-null value
	DOUBLE m_dLastLive;

	// level of last live value 
	ULONG m_ulLevelLastLive;

	// minimum value
	DOUBLE m_dMinimum;

	// level of minimum value 
	ULONG m_ulLevelMinimum;

	// Maximum value
	DOUBLE m_dMaximum;

	// level of maximum value 
	ULONG m_ulLevelMaximum;

	// Average value
	DOUBLE m_dAverage;

// public properties
public:
	// number of elements of data in the vector
	ULONG GetVectorElements();
	// number of elements of data in the vector
	__declspec( property( get = GetVectorElements ))
		ULONG VectorElements;
	
	// units are stored here as a reference
	inline CString GetUnits()
	{ 
		return m_csUnits; 
	}
	// units are stored here as a reference
	inline void SetUnits( LPCTSTR value = _T( "" ))
	{ 
		m_csUnits = value; 
	}
	// units are stored here as a reference
	__declspec( property( get =GetUnits, put=SetUnits ))
		CString Units;

	// unit class is stored here as a reference
	inline CString GetUnitClass()
	{ 
		return m_csUnitClass; 
	}
	// unit class is stored here as a reference
	inline void SetUnitClass( LPCTSTR value = _T( "" ))
	{ 
		m_csUnitClass = value; 
	}
	// unit class is stored here as a reference
	__declspec( property( get =GetUnitClass, put=SetUnitClass ))
		CString UnitClass;

	// empty value is used to initialize data vectors during a resize
	inline double GetNull()
	{ 
		return m_dNull; 
	}
	// empty value is used to initialize data vectors during a resize
	inline void SetNull( double value = -9999.0 )
	{ 
		m_dNull = value; 
	}
	// empty value is used to initialize data vectors during a resize
	__declspec( property( get =GetNull, put=SetNull ))
		double Null;
	
	// multiplier value used for conversions
	inline double GetMultiplier()
	{ 
		return m_dMultiplier; 
	}
	// multiplier value used for conversions
	inline void SetMultiplier( double value = -9999.0 )
	{ 
		m_dMultiplier = value; 
	}
	// multiplier value used for conversions
	__declspec( property( get =GetMultiplier, put=SetMultiplier ))
		double Multiplier;
	
	// additive value used for conversions
	inline double GetAdditive()
	{ 
		return m_dAdditive; 
	}
	// additive value used for conversions
	inline void SetAdditive( double value = -9999.0 )
	{ 
		m_dAdditive = value; 
	}
	// additive value used for conversions
	__declspec( property( get =GetAdditive, put=SetAdditive ))
		double Additive;
	
	// number of elements per level
	inline ULONG GetLevelElements()
	{ 
		return m_ulLevelElements; 
	}
	// number of elements per level
	inline void SetLevelElements( ULONG value = 1 )
	{ 
		m_ulLevelElements = value; 
	}
	// number of elements per level
	__declspec( property( get = GetLevelElements, put=SetLevelElements ))
		ULONG LevelElements;

	// Get the number of levels (defaults to number of elements)
	inline ULONG GetLevels()
	{
		ULONG ulLevels = 0;
		const ULONG ulCount = VectorElements;
		if ( ulCount == 0 )
		{
			return ulLevels;
		}
		const ULONG ulLevelSize = LevelElements;
		ulLevels = ulCount / ulLevelSize;
		return ulLevels;
	}
	// Get the number of levels (defaults to number of elements)
	__declspec( property( get = GetLevels ))
		ULONG Levels;

	// data type of the curve
	inline VARENUM GetType()
	{ 
		return m_vtType; 
	}
	// data type of the curve
	inline void SetType( VARENUM value = VT_EMPTY )
	{ 
		m_vtType = value; 
	}
	// data type of the curve
	__declspec( property( get =GetType, put=SetType ))
		VARENUM Type;

	// data element size in bytes (0 for unsupported type)
	inline short GetElementSizeInBytes()
	{
		return GetElementSizeInBytes( GetType() );
	}
	// data element size in bytes (0 for unsupported type)
	__declspec( property( get = GetElementSizeInBytes ) )
		short ElementSizeInBytes;

	// Get the level size in bytes
	inline ULONG GetLevelSizeInBytes()
	{
		const ULONG ulElementSize = ElementSizeInBytes;
		const ULONG ulElements = GetLevelElements();
		const ULONG ulBytes = ulElementSize * ulElements;
		return ulBytes;
	}
	// Get the level size in bytes
	__declspec( property( get = GetLevelSizeInBytes ) )
		ULONG LevelSizeInBytes;

	// total number of bytes in the array
	inline ULONG GetNumberOfBytes()
	{
		const ULONG ulElementSize = ElementSizeInBytes;
		const ULONG ulElements = VectorElements;
		const ULONG ulBytes = ulElementSize * ulElements;
		return ulBytes;
	}
	// total number of bytes in the array
	__declspec( property( get =GetNumberOfBytes ))
		ULONG NumberOfBytes;
	
	// first live value is the first non-null value
	inline double GetFirstLive()
	{
		return m_dFirstLive;
	}
	// first live value is the first non-null value
	inline void SetFirstLive( double value = -9999.0 )
	{
		m_dFirstLive = value;
	}
	// first live value is the first non-null value
	__declspec( property( get = GetFirstLive, put = SetFirstLive ) )
		double FirstLive;

	// level of first live value 
	inline ULONG GetLevelFirstLive()
	{
		return m_ulLevelFirstLive;
	}
	// level of first live value 
	inline void SetLevelFirstLive( ULONG value = -9999.0 )
	{
		m_ulLevelFirstLive = value;
	}
	// level of first live value 
	__declspec( property( get = GetLevelFirstLive, put = SetLevelFirstLive ) )
		ULONG LevelFirstLive;

	// last live value is the last non-null value
	inline double GetLastLive()
	{
		return m_dLastLive;
	}
	// last live value is the last non-null value
	inline void SetLastLive( double value = -9999.0 )
	{
		m_dLastLive = value;
	}
	// last live value is the last non-null value
	__declspec( property( get = GetLastLive, put = SetLastLive ) )
		double LastLive;

	// level of last live value 
	inline ULONG GetLevelLastLive()
	{
		return m_ulLevelLastLive;
	}
	// level of last live value 
	inline void SetLevelLastLive( ULONG value = -9999.0 )
	{
		m_ulLevelLastLive = value;
	}
	// level of last live value 
	__declspec( property( get = GetLevelLastLive, put = SetLevelLastLive ) )
		ULONG LevelLastLive;

	// minimum value
	inline double GetMinimum()
	{
		return m_dMinimum;
	}
	// minimum value
	inline void SetMinimum( double value = -9999.0 )
	{
		m_dMinimum = value;
	}
	// minimum value
	__declspec( property( get = GetMinimum, put = SetMinimum ) )
		double Minimum;

	// level of minimum value 
	inline ULONG GetLevelMinimum()
	{
		return m_ulLevelMinimum;
	}
	// level of minimum value 
	inline void SetLevelMinimum( ULONG value = -9999.0 )
	{
		m_ulLevelMinimum = value;
	}
	// level of minimum value 
	__declspec( property( get = GetLevelMinimum, put = SetLevelMinimum ) )
		ULONG LevelMinimum;

	// Maximum value
	inline double GetMaximum()
	{
		return m_dMaximum;
	}
	// Maximum value
	inline void SetMaximum( double value = -9999.0 )
	{
		m_dMaximum = value;
	}
	// Maximum value
	__declspec( property( get = GetMaximum, put = SetMaximum ) )
		double Maximum;

	// level of maximum value 
	inline ULONG GetLevelMaximum()
	{
		return m_ulLevelMaximum;
	}
	// level of maximum value 
	inline void SetLevelMaximum( ULONG value = -9999.0 )
	{
		m_ulLevelMaximum = value;
	}
	// level of maximum value 
	__declspec( property( get = GetLevelMaximum, put = SetLevelMaximum ) )
		ULONG LevelMaximum;

	// Average value
	inline double GetAverage()
	{
		return m_dAverage;
	}
	// Average value
	inline void SetAverage( double value = -9999.0 )
	{
		m_dAverage = value;
	}
	// Average value
	__declspec( property( get = GetAverage, put = SetAverage ) )
		double Average;

// public methods
public:
	// calculate an index based on level and element
	inline ULONG GetIndex
	(
		ULONG ulLevel = 0,
		ULONG ulElement = 0 
	)
	{
		// special handling for strings which are defined
		// as an array of characters (VT_I1)
		const VARENUM eVt = Type;
		if ( eVt == VT_I1 )
		{
			// for element to zero because the entire array
			// is considered as a single value
			ulElement = 0;
		}
		const ULONG ulLevelElements = LevelElements;
		const ULONG ulIndex = 
			ulLevelElements * ulLevel + ulElement;
		return ulIndex;
	}
	
	// change the data type of an existing buffer by copying from one 
	// buffer to another as a byte array...it is up to the application
	// to insure this makes sense.  For example, if the vector is sized
	// for 10 doubles (80 bytes), data is copied into the vector as
	// bytes, and ChangeType is used to put the data back to doubles, then
	// the byte array should be 80 bytes long.
	bool ChangeType( VARENUM value );
	
	// Get a value at a given index
	COleVariant GetValue( ULONG ulIndex );

	// Get a value at a level and element
	inline COleVariant GetValue
	( 
		ULONG ulLevel, ULONG ulElement 
	)
	{
		const ULONG ulIndex = GetIndex( ulLevel, ulElement );
		return GetValue( ulIndex );
	}

	// Set a value at a given index
	void SetValue( ULONG ulIndex, COleVariant var );

	// Set a value at a given index
	inline void SetValue
	( 
		COleVariant var, ULONG ulLevel, ULONG ulElement = 0 
	)
	{
		const ULONG ulIndex = GetIndex( ulLevel, ulElement );
		SetValue( ulIndex, var );
	}
	
	// Get a double value at a given index and return null value if unsuccessful
	DOUBLE GetDouble( ULONG ulIndex );

	// Get a double value at a given level, and element.
	// return null value if unsuccessful
	inline DOUBLE GetDouble( ULONG ulLevel, ULONG ulColumn, ULONG ulElement = 0 )
	{
		const ULONG ulIndex = GetIndex( ulLevel, ulElement );
		return GetDouble( ulIndex );
	}

	// Set a double value at a given index and return false if unsuccessful
	bool SetDouble( ULONG ulIndex, DOUBLE dValue );

	// Set a double value at the given level, and element
	// and return false if unsuccessful
	inline bool SetDouble
	(
		DOUBLE dValue, ULONG ulLevel, ULONG ulColumn = 0, ULONG ulElement = 0 
	)
	{
		const ULONG ulIndex = GetIndex( ulLevel, ulElement );
		return SetDouble( ulIndex, dValue );
	}

	// append a double value and return the count
	ULONG AppendDouble( DOUBLE dValue );

	// Set a double array into the vector and return false if unsuccessful
	bool SetDoubles( vector<DOUBLE>& arrValues );

	// Set a double array into the given a level 
	// returns false if the level is out of range
	bool SetDoubles( ULONG ulLevel, vector<DOUBLE>& arrValues );

	// Get a double array from the given level and level 
	// returns false if the level is out of range
	bool GetDoubles( ULONG ulLevel, vector<DOUBLE>& arrValues );
	
	// create a variant vector
	void Create
	(	double dNull, // the empty value
		VARENUM eType, // variant type
		ULONG ulLevelElements = 1, // elements per level
		ULONG ulLevels = 0, // number of levels (if Set will fill with nulls)
		LPCTSTR pcszUnits = _T( "" ) // units
	);

	// assignment operator
	CStreamCache& operator=( CStreamCache& src );

	// convert values
	bool Convert( DOUBLE dMult, DOUBLE dAdd );

	// find the minimum and maximum values
	bool FindMinMax( DOUBLE& dMinimum, DOUBLE& dMaximum );	

	// calculate statistics--rather than making a pass through the data
	// separately for each statistic, calculate them all in one pass
	bool CalculateStatistics();

	// copy data into safe array
	void GetData( COleSafeArray& sa );

	// assigns the data	from a safe array
	bool SetData( VARIANT& data );
	
	// clear the data
	void clear();
	
	// Set data from a char* pointer
	bool SetData( VARENUM nType, ULONG ulValues, char* pData );
	
	// Get a void pointer to the data and optionally to 
	// a specific index of the data
	void* GetData( ULONG ulIndex = 0 );

	// Get a void pointer to the data at a given level and element
	// where the data at a level can be an array of elements
	void* GetData
	(	ULONG ulLevel, // level of the vector
		ULONG ulElement // element of the level
	);
	
	// resize the data to the given number of elements. if the size
	// is extended, the new levels are filled with null values.
	// if the size is reduced, data will be deleted at the end.
	// if bProtectContents is true, the existing data will remain
	//		unchanged except when the size is reduced.
	// if bProtectContents is false, the entire content will be
	//		filled with null values.
	void resize
	(	// new size of the of the vector in values
		ULONG values, 
		// protect existing content when true or null the values when false
		bool bProtectContents = false 
	);
	
	// data element size in bytes (0 for unsupported type)
	static short GetElementSizeInBytes( VARENUM nType );

	// copy a variant array to a vector
	template <class T>
	static inline bool CopyVariant
	(	VARIANT& Source, // source safe array
		VARENUM eVar, // type of variant expected
		vector<T>& Dest // vector is returned
	)
	{
		Dest.clear();
		bool bOK = Source.vt == ( VT_ARRAY | eVar );
		if ( !bOK )
		{
			return false;
		}

		// number of source values
		const long lValues =
			CHelper::GetOneDimensionalElementCount( Source );
		if ( lValues == 0 )
		{
			return false;
		}

		COleSafeArray sa( Source );
		T* pData = 0;
		sa.AccessData( (void**)&pData );
		for ( long lValue = 0; lValue < lValues; lValue++ )
		{
			const T value( pData[ lValue ] );
			Dest.push_back( value );
		}
		sa.UnaccessData();
		return true;
	}

	// copy vector into a safe array
	template <class T>
	static bool CopyVector
	(	vector<T>& Source, // source vector
		VARENUM eVar, // destination type corresponding to source type
		VARIANT* Dest // returned safe array
	)
	{
		::VariantInit( Dest );
		const long lValues = Source.size();
		if ( lValues == 0 )
		{
			return false;
		}

		// create a 1 dimensional safe array of given type and initialize
		COleSafeArray sa;
		T* pData = &Source[ 0 ];
		sa.CreateOneDim( eVar, lValues, pData );

		// copy out the result
		::VariantCopy( Dest, &sa );
		return true;
	}

// construction / destruction
public:
	// default constructor
	CStreamCache(void);

	// assignment constructor
	CStreamCache
	(
		double dNull, // the empty value
		VARENUM eType, // variant type
		ULONG ulLevelElements = 1, // elements per level
		ULONG ulLevels = 0, // number of levels (if Set will fill with nulls)
		LPCTSTR pcszUnits = _T( "" ) // units
	);

	// destructor
	virtual ~CStreamCache(void);
};
