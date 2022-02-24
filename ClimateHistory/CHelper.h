/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "stdafx.h"
#include <vector>

using namespace std;

class CHelper
{
public:
	/////////////////////////////////////////////////////////////////////////////
	// parsing command lines has some rules that user's may find difficult to
	// follow, for example:
	
	//		SetDateTaken "c:\temp\camera roll\" 1980 9 6 
	
	// will fail because the double quote (") preceded by a backslash will
	// generate a literal double quote. This will cause the rest of the
	// arguments to be included in the second argument as a single string.
	// The double quotes around the pathname are required because the path
	// has white space (spaces or tabs) in it which would ordinarily break
	// the string up into multiple arguments.
	
	// The correct way to enter these arguments is:
	
	//		SetDateTaken "c:\temp\camera roll\\" 1980 9 6 
	
	// where the double backslash will generate a single backslash to 
	// prevent the generation of a literal double quote.

	// This method addresses that specific scenario because in my mind
	// the rules seem to be too obtuse and likely to be overlooked by
	// the average user (I have made that mistake myself).
	static vector<CString> CorrectedCommandLine( int argc, TCHAR* argv[] )
	{
		// create a vector of command line arguments, which is done
		// to allow for handling of common command line argument
		// errors
		vector<CString> value;

		// copy the original arguments
		for ( int arg = 0; arg < argc; arg++ )
		{
			value.push_back( argv[ arg ] );
		}

		// a common error when passing in pathnames is to end
		// the pathname with a single backslash and a double quote
		// which will result in all of the arguments being lumped
		// into a single string
		size_t nArgs = value.size();

		// the first argument is the executable path and the 
		// second argument will be the parameters passed
		// to the executable
		if ( nArgs == 2 )
		{
			CString arg1 = value[ 1 ];
			const int nPos = arg1.Find( _T( "\"" ) );
			if ( nPos != -1 )
			{
				// replace the double quote with a backslash
				// because of a common escape sequence error of
				// placing a double quote after a single 
				// backslash in a pathname
				arg1.SetAt( nPos, '\\' );

				// parse arg1 to create the new arguments
				value.resize( 1 );
				value.push_back( arg1.Left( nPos + 1 ) );

				// start parsing after the first argument
				int nStart = nPos + 1;

				// parse the remainder of the command line with whitespace
				// delimiters
				do
				{
					const CString csToken = arg1.Tokenize( _T( " \t" ), nStart );
					if ( csToken.IsEmpty() )
					{
						break;
					}

					value.push_back( csToken );

				} while ( true );

				// recalculate the number of arguments
				nArgs = value.size();
			}
		}

		// return the arguments as a vector of strings which may be the 
		// original values or a corrected set of values.
		return value;
	}

	/////////////////////////////////////////////////////////////////////////////
	// parse the filename from a pathname
	static inline CString GetFileName( LPCTSTR pcszPath )
	{
		CString csPath( pcszPath );
		TCHAR* pBuf = csPath.GetBuffer( csPath.GetLength() + 1 );
		TCHAR szDrive[ _MAX_DRIVE ];
		TCHAR szDir[ _MAX_DIR ];
		TCHAR szFile[ _MAX_FNAME ];
		TCHAR szExt[ _MAX_EXT ];

		_tsplitpath( pBuf, szDrive, szDir, szFile, szExt );
		csPath.ReleaseBuffer();
		return szFile;
	}

	/////////////////////////////////////////////////////////////////////////////
	// parse the extension from a pathname
	static inline CString GetExtension( LPCTSTR pcszPath )
	{
		CString csPath( pcszPath );
		TCHAR* pBuf = csPath.GetBuffer( csPath.GetLength() + 1 );
		TCHAR szDrive[ _MAX_DRIVE ];
		TCHAR szDir[ _MAX_DIR ];
		TCHAR szFile[ _MAX_FNAME ];
		TCHAR szExt[ _MAX_EXT ];

		_tsplitpath( pBuf, szDrive, szDir, szFile, szExt );
		csPath.ReleaseBuffer();
		return szExt;
	}

	/////////////////////////////////////////////////////////////////////////////
	// parse the directory from a pathname
	static inline CString GetDirectory( LPCTSTR pcszPath )
	{
		CString csPath( pcszPath );
		TCHAR* pBuf = csPath.GetBuffer( csPath.GetLength() + 1 );
		TCHAR szDrive[ _MAX_DRIVE ];
		TCHAR szDir[ _MAX_DIR ];
		TCHAR szFile[ _MAX_FNAME ];
		TCHAR szExt[ _MAX_EXT ];

		_tsplitpath( pBuf, szDrive, szDir, szFile, szExt );
		csPath.ReleaseBuffer();
		return szDir;
	}

	/////////////////////////////////////////////////////////////////////////////
	// parse the drive from a pathname
	static inline CString GetDrive( LPCTSTR pcszPath )
	{
		CString csPath( pcszPath );
		TCHAR* pBuf = csPath.GetBuffer( csPath.GetLength() + 1 );
		TCHAR szDrive[ _MAX_DRIVE ];
		TCHAR szDir[ _MAX_DIR ];
		TCHAR szFile[ _MAX_FNAME ];
		TCHAR szExt[ _MAX_EXT ];

		_tsplitpath( pBuf, szDrive, szDir, szFile, szExt );
		csPath.ReleaseBuffer();
		return szDrive;
	}

	/////////////////////////////////////////////////////////////////////////////
	// parse folder from a pathname (drive and directory)
	static inline CString GetFolder( LPCTSTR pcszPath )
	{
		CString csDrive = GetDrive( pcszPath );
		CString csDir = GetDirectory( pcszPath );
		return csDrive + csDir;
	}

	/////////////////////////////////////////////////////////////////////////////
	// parse data name from a pathname (filename and extension)
	static inline CString GetDataName( LPCTSTR pcszPath )
	{
		CString csFile = GetFileName( pcszPath );
		CString csExt = GetExtension( pcszPath );
		return csFile + csExt;
	}

	/////////////////////////////////////////////////////////////////////////////
	// compare two reals and determine if they are nearly equal 
	// (within the given error range)
	template <class T> static inline bool NearlyEqual
	( 
		T value1, T value2, T error = T( 0.0001 )
	)
	{
		const T diff = fabs( value1 - value2 );
		return diff < error;
	}

	/////////////////////////////////////////////////////////////////////////////
	// floating point less than will return true if value1 is 
	// less than value2
	template <class T> static inline bool LessThan
	( 
		T value1, T value2, T error = T( 0.00001 )
	)
	{	// check for nearly equal first
		const bool bEqual = CStaticHelper::NearlyEqual( value1, value2, error );
		if ( bEqual )
		{
			return false;
		}
		// if not nearly equal do the normal compare
		if ( value1 < value2 )
		{
			return true;
		}
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////
	// floating point less than or equal will return true if value1 is 
	// less than or equal to value2
	template <class T> static inline bool LessThanOrEqual
	( 
		T value1, T value2, T error = T( 0.00001 )
	)
	{	// check for nearly equal first
		const bool bEqual = CStaticHelper::NearlyEqual( value1, value2, error );
		if ( bEqual )
		{
			return true;
		}
		// if not nearly equal do the normal compare
		if ( value1 <= value2 )
		{
			return true;
		}
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////
	// floating point greater than or equal will return true if value1 is 
	// greater than or equal to value2
	template <class T> static inline bool GreaterThanOrEqual
	( 
		T value1, T value2, T error = T( 0.00001 )
	)
	{	// check for nearly equal first
		const bool bEqual = CStaticHelper::NearlyEqual( value1, value2, error );
		if ( bEqual )
		{
			return true;
		}
		// if not nearly equal do the normal compare
		if ( value1 >= value2 )
		{
			return true;
		}
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////
	// floating point greater than will return true if value1 is greater than 
	// value2
	template <class T> static inline bool GreaterThan
	( 
		T value1, T value2, T error = T( 0.00001 )
	)
	{	// check for nearly equal first
		const bool bEqual = CStaticHelper::NearlyEqual( value1, value2, error );
		if ( bEqual )
		{
			return false;
		}
		// if not nearly equal do the normal compare
		if ( value1 > value2 )
		{
			return true;
		}
		return false;
	}

	/////////////////////////////////////////////////////////////////////////////
	// Round value to nearest multiple of nearest (ie. if nearest is 0.1
	// round to the nearest tenth or if nearest is 5, round to the nearest
	// multiple of five).  If second parameter is not supplied, value is 
	// rounded to the nearest whole number 
	template <class T> static inline T RoundToNearest
	( 
		T value, T nearest = T( 1 )
	)
	{
		if ( nearest == 0 )
		{
			nearest = 1;
		}
		nearest = fabs( nearest ); // positive number
		value /= nearest;
		value = value < 0 ? ceil( value - T( 0.5 ) ) : floor( value + T( 0.5 ) );
		value *= nearest;
		return value;
	}

	/////////////////////////////////////////////////////////////////////////////
	// method to calculate the maximum value of the array
	// taking into account the given null value
	template <class T> static T Maximum
	( 
		T null, // value's empty or missing value
		T defaultValue, // default to return if there are no non-null values
		const vector<T>& arrValues // values to operate on
	)
	{
		T result = null;
		bool bSet = false;

		for each ( T value in arrValues )
		{
			if ( NearlyEqual( value, null ) )
			{
				continue;
			}
			// update the result
			if ( bSet )
			{
				result = max( result, value );
			} else // never assigned a value
			{
				result = value;
				bSet = true;
			}
		}
		if ( !bSet )
		{
			result = defaultValue;
		}
		return result;
	}

	/////////////////////////////////////////////////////////////////////////////
	// method to calculate the minimum value of the array
	// taking into account the given null value
	template <class T> static T Minimum
	( 
		T null, // value's empty or missing value
		T defaultValue, // default to return if there are no non-null values
		const vector<T>& arrValues // values to operate on
	)
	{
		T result = null;
		bool bSet = false;

		for each ( T value in arrValues )
		{
			if ( NearlyEqual( value, null ) )
			{
				continue;
			}
			// update the result
			if ( bSet )
			{
				result = min( result, value );
			} else // never assigned a value
			{
				result = value;
				bSet = true;
			}
		}
		if ( !bSet )
		{
			result = defaultValue;
		}
		return result;
	}

	/////////////////////////////////////////////////////////////////////////////
	// method to calculate the average value of the array
	// taking into account the given null value
	template <class T> static T Average
	( 
		T null, // values's empty or missing value
		T defaultValue, // default to return if there are no non-null values
		const vector<T>& arrValues // values to operate on
	)
	{
		T result = null;
		bool bSet = false;
		T count = 0;
		const int nValues = arrValues.size();

		for each ( T value in arrValues )
		{
			if ( CStaticHelper::NearlyEqual( value, null ) )
			{
				continue;
			}
			// update the result
			if ( bSet )
			{
				result += value;
			} else // never assigned a value
			{
				result = value;
				bSet = true;
			}
			count++;
		}
		if ( !bSet )
		{
			result = defaultValue;
		} else
		{
			result /= count;
		}
		return result;
	}

	/////////////////////////////////////////////////////////////////////////////
	// get the number of days in a month given the year and month number (1 to 12)
	static inline int GetNumberOfDays( int nYear, int nMonth )
	{
		int value = 31;
		switch ( nMonth )
		{
			case 2: // feb
			{
				value = 28;
				const int nMod = nYear % 4;
				if ( nMod == 0 )
				{
					value++;
				}
				break;
			}
			case 4: // apr
			case 6: // jun
			case 9: // sep
			case 11: // nov
			{
				value = 30;
				break;
			}
		}
		return value;
	}

	/////////////////////////////////////////////////////////////////////////////
	// get temperature in degrees fahrenheit from Celsius value and an empty value
	inline static float GetFahrenheit( float fCelsius, float fEmpty)
	{
		// return value
		float value = fCelsius;

		if ( !NearlyEqual( value, fEmpty ) )
		{
			value *= 1.8f;
			value += 32.0f;
		}

		return value;
	}


	/////////////////////////////////////////////////////////////////////////////
	CHelper()
	{
	}

	/////////////////////////////////////////////////////////////////////////////
	~CHelper()
	{
	}
};

