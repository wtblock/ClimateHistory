/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ClimateTemperature.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// This class contains a single year of data for a single station and is
// capable of parsing a single line of data from a tmax, tmin, or tavg file 
// which can be downloaded from the following web site:

//	https://www.ncei.noaa.gov/products/land-based-station/us-historical-climatology-network

// The format for these files is a text file where the information is encoded
// by column positions as defined in the following fragment from the web site's
// readme.txt file located here:

//	https://www.ncei.noaa.gov/pub/data/ushcn/v2.5/readme.txt

// Fragment of readme.txt file pertinent to this class:

//	2.2  DATA
//	
//	Monthly data in the USHCN v2.5 data use a "3 flag" format
//	similar to that used within the Global Historical Climatology
//	Network - Daily( GHCN - Daily ) and GHCN - Monthly( version 3 ) datasets.
//	
//	2.2.1 DATA FORMAT
//	
//		Variable  Columns  Type
//		--------  ------ - ----
//	
//		ID         1 - 11  Char
//		YEAR      13 - 16  Int
//		VALUE1    17 - 22  Int
//		DMFLAG1   23 - 23  Char
//		QCFLAG1   24 - 24  Char
//		DSFLAG1   25 - 25  Char
//		.         .        .
//		.         .        .
//		.         .        .
//		VALUE12  116 - 121 Int
//		DMFLAG12 122 - 122 Char
//		QCFLAG12 123 - 123 Char
//		DSFLAG12 124 - 124 Char
//	
//	Variable Definitions :
//	
//	ID: 11 character alphanumeric identifier :
//		characters 1 - 2 = Country Code( 'US' for all USHCN stations )
//		character 3 = Network code( 'H' for Historical Climatology Network )
//		digits 4 - 11 = '00' + 6 - digit Cooperative Observer Identification Number
//	
//	YEAR : 4 digit year of the station record.
//	
//	VALUE : monthly value( MISSING = -9999 ).Temperature values are in
//		hundredths of a degree Celsius, but are expressed as whole
//		integers( e.g.divide by 100.0 to get whole degrees Celsius ).

class CStationYear
{
// public definitions
public:
	typedef pair<int,int> GREATER_COUNT;

// protected data
protected:
	// measurement type values: maximum, minimum, and average 
	CClimateTemperature::MEASURE_TYPE m_eMeasurementType;

	// the station ID
	CString m_csStation;

	// the year of the measurement
	CString m_csYear;

	// collection of monthly temperatures for the year
	vector<shared_ptr<CClimateTemperature> > m_arrMonths;

	// maximum, minimum, or average reading of all months
	float m_fValue;

	// array of Greater Than pairs
	// first number is the Fahrenheit temperature 
	// the second number is the number of temperatures greater than the first number
	vector<GREATER_COUNT> m_GreaterCounts;

	// number of valid readings
	int m_nValidReadings;

// public properties
public:
	// length of source station ID
	inline int GetStationLength()
	{
		return 11;
	}
	// length of source station ID
	__declspec( property( get = GetStationLength ) )
		int StationLength;

	// start position of source station ID
	inline int GetStationStart()
	{
		return 0;
	}
	// start position of source station ID
	__declspec( property( get = GetStationStart ) )
		int StationStart;

	// length of source year
	inline int GetYearLength()
	{
		return 4;
	}
	// length of source year
	__declspec( property( get = GetYearLength ) )
		int YearLength;

	// start position of source year
	inline int GetYearStart()
	{
		return 12;
	}
	// start position of source year
	__declspec( property( get = GetYearStart ) )
		int YearStart;

	// start position of first source month
	inline int GetMonthStart()
	{
		return YearStart + YearLength;
	}
	// start position of first source month
	__declspec( property( get = GetMonthStart ) )
		int MonthStart;

	// key for searches
	inline CString GetKey()
	{
		CString value = Station;
		return value;
	}
	// key for searches
	__declspec( property( get = GetKey ) )
		CString Key;

	// each year contains values representing one of three possible
	// types of measurements: maximum, minimum, and average temperature
	inline CClimateTemperature::MEASURE_TYPE GetMeasurementType()
	{
		// return value
		const CClimateTemperature::MEASURE_TYPE value = m_eMeasurementType;

		return value;
	}
	// each year contains values representing one of three possible
	// types of measurements: maximum, minimum, and average temperature
	inline void SetMeasurementType( CClimateTemperature::MEASURE_TYPE value )
	{
		m_eMeasurementType = value;
	}
	// each year contains values representing one of three possible
	// types of measurements: maximum, minimum, and average temperature
	__declspec( property( get = GetMeasurementType, put = SetMeasurementType ) )
		CClimateTemperature::MEASURE_TYPE MeasurementType;

	// station name (columns 1 - 11 of source)
	inline CString GetStation()
	{
		return m_csStation;
	}
	// station name (columns 1 - 11 of source)
	// NOTE: CString values are zero based (0 - 10)
	inline void SetStation( CString value )
	{
		m_csStation = value;
	}
	// station name (columns 1 - 11 of source)
	// NOTE: CString values are zero based (0 - 10)
	__declspec( property( get = GetStation, put = SetStation ) )
		CString Station;

	// year (columns 13 - 16 of source)
	// NOTE: CString values are zero based (12 - 15)
	inline CString GetYear()
	{
		return m_csYear;
	}
	// year (columns 13 - 16 of source)
	// NOTE: CString values are zero based (12 - 15)
	inline void SetYear( CString value )
	{
		m_csYear = value;
	}
	// year (columns 13 - 16 of source)
	// NOTE: CString values are zero based (12 - 15)
	__declspec( property( get = GetYear, put = SetYear ) )
		CString Year;

	// monthly data is indexed from 0 to 11 (Jan to Dec)
	inline shared_ptr<CClimateTemperature> GetMonth( int month )
	{
		const int nMonths = (int)m_arrMonths.size();
		if ( month < nMonths )
		{
			return m_arrMonths[ month ];

		} else if ( month == nMonths )
		{
			return shared_ptr<CClimateTemperature>( new CClimateTemperature );
		}
	}
	// monthly data is indexed from 0 to 11 (Jan to Dec)
	inline void SetMonth( int month, shared_ptr<CClimateTemperature> value )
	{
		const int nMonths = (int)m_arrMonths.size();
		if ( month < nMonths )
		{
			m_arrMonths[ month ] = value;

		} else if ( month == nMonths )
		{
			m_arrMonths.push_back( value );
		}

	}
	// monthly data is indexed from 0 to 11 (Jan to Dec)
	__declspec( property( get = GetMonth, put = SetMonth ) )
		shared_ptr<CClimateTemperature> Month[];

	// maximum reading of all months
	inline float GetMaximum()
	{
		// value which indicates missing data
		const float fMissing = CClimateTemperature::GetMissingValue();

		bool bFirst = true;

		// begin with the persisted value
		float value = m_fValue;

		// if the maximum has already been calculated, use the 
		// persisted value;
		if ( !CHelper::NearlyEqual( value, fMissing ))
		{
			return value;
		}

		// loop through the months looking for non-missing values
		for ( auto& node : m_arrMonths )
		{
			// we are only interested measured values
			if ( !node->Missing )
			{
				// initialize the return value if this is the first 
				// measured value
				if ( bFirst )
				{
					value = node->Centigrade;
					bFirst = false;

				} else 
				{
					// maximize against the existing value and the measurement
					value = max( value, node->Centigrade );
				}
			}
		}

		// persist the value
		Maximum = value;

		return value;
	}
	// maximum reading of all months
	inline void SetMaximum( float value )
	{
		m_fValue = value;
	}
	// maximum reading of all months
	__declspec( property( get = GetMaximum, put = SetMaximum ) )
		float Maximum;

	// minimum reading of all months
	inline float GetMinimum()
	{
		// value which indicates missing data
		const float fMissing = CClimateTemperature::GetMissingValue();

		bool bFirst = true;

		// begin with the persisted value
		float value = m_fValue;

		// if the minimum has already been calculated, use the 
		// persisted value;
		if ( !CHelper::NearlyEqual( value, fMissing ) )
		{
			return value;
		}

		// loop through the months looking for non-missing values
		for ( auto& node : m_arrMonths )
		{
			// we are only interested measured values
			if ( !node->Missing )
			{
				// initialize the return value if this is the first 
				// measured value
				if ( bFirst )
				{
					value = node->Centigrade;
					bFirst = false;

				} else
				{
					// minimize against the existing value and the measurement
					value = min( value, node->Centigrade );
				}
			}
		}

		// persist the value
		Minimum = value;

		return value;
	}
	// minimum reading of all months
	inline void SetMinimum( float value )
	{
		m_fValue = value;
	}
	// minimum reading of all months
	__declspec( property( get = GetMinimum, put = SetMinimum ) )
		float Minimum;

	// average reading of all months
	inline float GetAverage()
	{
		// value which indicates missing data
		const float fMissing = CClimateTemperature::GetMissingValue();

		// begin with the persisted value
		float value = m_fValue;

		// if the average has already been calculated, use the 
		// persisted value;
		if ( !CHelper::NearlyEqual( value, fMissing ))
		{
			return value;
		}

		// average all of the valid months
		int nCount = 0; // the number of valid months
		float fSum = 0; // sum of all valid months

		// loop through the months looking for valid values
		for ( auto& node : m_arrMonths )
		{
			// we are only interested measured values
			if ( !node->Missing )
			{
				// count to be used for averaging
				nCount++;

				// initialize the sum if this is the first 
				// measured value
				if ( nCount == 1 )
				{
					fSum = node->Centigrade;

				} else
				{
					// add the value to the running sum
					fSum += node->Centigrade;
				}
			}
		}

		// average the values if there are any readings
		if ( nCount > 0 )
		{
			// calculate the average
			value = fSum / nCount;

			// persist the value
			Average = value;
		}

		return value;
	}
	// average reading of all months
	inline void SetAverage( float value )
	{
		m_fValue = value;
	}
	// average reading of all months
	__declspec( property( get = GetAverage, put = SetAverage ) )
		float Average;

	// maximum, minimum, or average reading of all months
	// depending on the measurement type
	inline float GetValue()
	{
		// value which indicates missing data
		const float fMissing = CClimateTemperature::GetMissingValue();

		// begin with the empty value
		float value = fMissing;

		// the value to calculate depend on the measurement type
		CClimateTemperature::MEASURE_TYPE eType = MeasurementType;

		switch ( eType )
		{
			case CClimateTemperature::mtMaximum:
			{
				value = Maximum;
				break;
			}
			case CClimateTemperature::mtMinimum:
			{
				value = Minimum;
				break;
			}
			case CClimateTemperature::mtAverage:
			{
				value = Average;
				break;
			}
			default :
			{
				value = fMissing;
			}
		}

		return value;
	}
	// maximum, minimum, or average reading of all months
	// depending on the measurement type
	inline void SetValue( float value )
	{
		m_fValue = value;
	}
	// maximum, minimum, or average reading of all months
	// depending on the measurement type
	__declspec( property( get = GetValue, put = SetValue ) )
		float Value;

	// number of valid readings
	inline int GetValidReadings()
	{
		int value = m_nValidReadings;

		// if the current value is -1, calculate it
		if ( value == -1 )
		{
			// begin with zero readings
			value = 0;

			// sum up the valid readings from each month
			for ( auto& node : m_arrMonths )
			{
				if ( !node->Missing )
				{
					value++;
				}
			}
		}

		ASSERT( value >= 0 && value <= 12 );

		// persist the reading
		ValidReadings = value;

		return value;
	}
	// number of valid readings
	inline void SetValidReadings( int value )
	{
		m_nValidReadings = value;
	}
	// number of valid readings
	__declspec( property( get = GetValidReadings, put = SetValidReadings ))
		int ValidReadings;

	// array of Greater Than pairs
	// first number is the Fahrenheit temperature 
	// the second number is the number of temperatures greater than the first number
	inline vector<GREATER_COUNT> GetGreaterCounts()
	{
		return m_GreaterCounts;
	}
	// array of Greater Than pairs
	// first number is the Fahrenheit temperature 
	// the second number is the number of temperatures greater than the first number
	inline void SetGreaterCounts( vector<GREATER_COUNT> value )
	{
		m_GreaterCounts = value;
	}
	// array of Greater Than pairs
	// first number is the Fahrenheit temperature 
	// the second number is the number of temperatures greater than the first number
	__declspec( property( get = GetGreaterCounts ))
		vector<GREATER_COUNT> GreaterCounts;

// protected methods
protected:
	// parse single line of stations text file into properties
	inline void ParseSource( CString source )
	{
		// parse the station name (11 characters starting at zero)
		Station = source.Mid( StationStart, StationLength );

		// parse the year (11 characters starting at 12)
		Year = source.Mid( YearStart, YearLength );
		
		int nStart = MonthStart;
		for ( int nMonth = 0; nMonth < 12; nMonth++ )
		{
			Month[ nMonth ] = shared_ptr<CClimateTemperature>
			(
				new CClimateTemperature( source, nStart, MeasurementType )
			);
		}
	}

	// calculate greater than value array
	void CountGreaterValues()
	{
		// generate entries for 90 to 130 degrees in 5 degree increments
		for ( int n = 90; n <= 130; n += 5 )
		{
			GREATER_COUNT value( n, 0 );
			for ( auto& node : m_arrMonths )
			{
				if ( !node->Missing )
				{
					const float fValue = node->Fahrenheit;
					if ( fValue > n )
					{
						value.second++;
					}
				}
			}

			m_GreaterCounts.push_back( value );
		}
	}

// public methods
public:

// protected overrides
protected:

// public overrides
public:

// public constructor
public:
	// default constructor
	CStationYear()
	{
		// value which indicates missing data
		const float fMissing = CClimateTemperature::GetMissingValue();

		// initialize the value to missing to force a calculation
		Value = fMissing;

		// number of valid readings
		ValidReadings = -1;

		// mark the object and undefined
		MeasurementType = CClimateTemperature::mtMissing;
	}

	// constructor using a source line of text and the measurement type
	CStationYear( CString& source, CClimateTemperature::MEASURE_TYPE eType )
	{
		// value which indicates missing data
		const float fMissing = CClimateTemperature::GetMissingValue();

		// initialize the value to missing to force a calculation
		Value = fMissing;

		// number of valid readings
		ValidReadings = -1;

		// record the measurement type
		MeasurementType = eType;

		// parse single line of stations text file into properties
		ParseSource( source );

		// maximum, minimum, or average reading of all months
		// depending on the measurement type
		const float fValue = Value;

		// number of valid readings
		const int nValues = ValidReadings;

		// calculate greater than values
		CountGreaterValues();
	}

	// destructor
	~CStationYear()
	{
	}
};

