/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "KeyedCollection.h"
#include "StationYear.h"

/////////////////////////////////////////////////////////////////////////////
// climate statistics for a single year
class CClimateYear
{
// public definitions
public:

// protected data
protected:
	// year
	CString m_csYear;

	// maximum temperature
	float m_fMaximum;

	// minimum temperature
	float m_fMinimum;

	// average temperature
	float m_fAverage;

	// number of maximum stations
	int m_nMaxStations;

	// number of minimum stations
	int m_nMinStations;

	// number of average stations
	int m_nAvgStations;

	// rapid station lookup of maximum temperatures
	CKeyedCollection<CString, CStationYear> m_Maximums;

	// rapid station lookup of minimum temperatures
	CKeyedCollection<CString, CStationYear> m_Minimums;

	// rapid station lookup of average temperatures
	CKeyedCollection<CString, CStationYear> m_Averages;

	// array of Greater Than pairs
	// first number is the Fahrenheit temperature 
	// the second number is the number of temperatures greater than the first number
	vector<CStationYear::GREATER_COUNT> m_GreaterCounts;

	// number of valid maximum readings
	int m_nMaxReadings;

	// number of valid minimum readings
	int m_nMinReadings;

	// number of valid average readings
	int m_nAvgReadings;

// public properties
public:
	// year of readings
	inline CString GetYear()
	{
		const CString value = m_csYear;

		return value;
	}
	// year of readings
	inline void SetYear( CString value )
	{
		m_csYear = value;
	}
	// year of readings
	__declspec( property( get = GetYear, put = SetYear ) )
		CString Year;

	// number of maximum stations
	inline int GetMaxStations()
	{
		const int value = m_Maximums.Count;
		MaxStations = value;

		return value;
	}
	// number of maximum stations
	inline void SetMaxStations( int value )
	{
		m_nMaxStations = value;
	}
	// number of maximum stations
	__declspec( property( get = GetMaxStations, put = SetMaxStations ) )
		int MaxStations;

	// number of minimum stations
	inline int GetMinStations()
	{
		const int value = m_Minimums.Count;
		MinStations = value;

		return value;
	}
	// number of minimum stations
	inline void SetMinStations( int value )
	{
		m_nMinStations = value;
	}
	// number of minimum stations
	__declspec( property( get = GetMinStations, put = SetMinStations ) )
		int MinStations;

	// number of average stations
	inline int GetAvgStations()
	{
		const int value = m_Averages.Count;
		AvgStations = value;

		return value;
	}
	// number of average stations
	inline void SetAvgStations( int value )
	{
		m_nAvgStations = value;
	}
	// number of average stations
	__declspec( property( get = GetAvgStations, put = SetAvgStations ) )
		int AvgStations;

	// maximum temperature
	inline float GetMaximum()
	{
		// value which indicates missing data
		const float fMissing = CClimateTemperature::GetMissingValue();

		bool bFirst = true;

		// begin with the persisted value
		float value = m_fMaximum;

		// if the maximum has already been calculated, use the 
		// persisted value;
		if ( !CHelper::NearlyEqual( value, fMissing ) )
		{
			return value;
		}

		// loop through the maximums looking for non-missing values
		for ( auto& node : m_Maximums.Items )
		{
			// read the value for the year
			const float fValue = node.second->Maximum;

			// we are only interested measured values
			if ( !CHelper::NearlyEqual( fValue, fMissing ))
			{
				// initialize the return value if this is the first 
				// measured value
				if ( bFirst )
				{
					value = fValue;
					bFirst = false;

				} else
				{
					// maximize against the existing value and the measurement
					value = max( value, fValue );
				}
			}
		}

		// persist the value
		Maximum = value;

		return value;
	}
	// maximum temperature
	inline void SetMaximum( float value )
	{
		m_fMaximum = value;
	}
	// maximum temperature
	__declspec( property( get = GetMaximum, put = SetMaximum ) )
		float Maximum;

	// minimum temperature
	inline float GetMinimum()
	{
		// value which indicates missing data
		const float fMissing = CClimateTemperature::GetMissingValue();

		bool bFirst = true;

		// begin with the persisted value
		float value = m_fMinimum;

		// if the minimum has already been calculated, use the 
		// persisted value;
		if ( !CHelper::NearlyEqual( value, fMissing ) )
		{
			return value;
		}

		// loop through the minimums looking for non-missing values
		for ( auto& node : m_Minimums.Items )
		{
			// read the value for the current year
			const float fValue = node.second->Value;

			// we are only interested measured values
			if ( !CHelper::NearlyEqual( fValue, fMissing ) )
			{
				// initialize the return value if this is the first 
				// measured value
				if ( bFirst )
				{
					value = fValue;
					bFirst = false;

				} else
				{
					// maximize against the existing value and the measurement
					value = max( value, fValue );
				}
			}
		}

		// persist the value
		Minimum = value;

		return value;
	}
	// minimum temperature
	inline void SetMinimum( float value )
	{
		m_fMinimum = value;
	}
	// minimum temperature
	__declspec( property( get = GetMinimum, put = SetMinimum ) )
		float Minimum;

	// average temperature
	inline float GetAverage()
	{
		// value which indicates missing data
		const float fMissing = CClimateTemperature::GetMissingValue();

		// begin with the persisted value
		float value = m_fAverage;

		// if the average has already been calculated, use the 
		// persisted value;
		if ( !CHelper::NearlyEqual( value, fMissing ) )
		{
			return value;
		}

		// average all of the valid months
		int nCount = 0; // the number of valid months
		float fSum = 0; // sum of all valid months

		// loop through the years looking for valid values
		for ( auto& node : m_Averages.Items )
		{
			// value for the year
			const float fValue = node.second->Value;

			// we are only interested measured values
			if ( !CHelper::NearlyEqual( fValue, fMissing ))
			{
				// count to be used for averaging
				nCount++;

				// initialize the sum if this is the first 
				// measured value
				if ( nCount == 1 )
				{
					fSum = fValue;

				} else
				{
					// add the value to the running sum
					fSum += fValue;
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
	// average temperature
	inline void SetAverage( float value )
	{
		m_fAverage = value;
	}
	// average temperature
	__declspec( property( get = GetAverage, put = SetAverage ) )
		float Average;

	// number of valid maximum readings
	inline int GetMaxReadings()
	{
		int value = m_nMaxReadings;

		// if the value is zero, calculate it
		if ( value == 0 )
		{
			// sum up the valid readings from each station
			for ( auto& node : m_Maximums.Items )
			{
				value += node.second->ValidReadings;
			}
		}

		// persist the reading
		MaxReadings = value;

		return value;
	}
	// number of valid maximum readings
	inline void SetMaxReadings( int value )
	{
		m_nMaxReadings = value;
	}
	// number of valid maximum readings
	__declspec( property( get = GetMaxReadings, put = SetMaxReadings ))
		int MaxReadings;

	// number of valid minimum readings
	inline int GetMinReadings()
	{
		int value = m_nMinReadings;

		// if the value is zero, calculate it
		if ( value == 0 )
		{
			// sum up the valid readings from each station
			for ( auto& node : m_Minimums.Items )
			{
				value += node.second->ValidReadings;
			}
		}

		// persist the reading
		MinReadings = value;

		return value;
	}
	// number of valid minimum readings
	inline void SetMinReadings( int value )
	{
		m_nMinReadings = value;
	}
	// number of valid minimum readings
	__declspec( property( get = GetMinReadings, put = SetMinReadings ))
		int MinReadings;

	// number of valid average readings
	inline int GetAvgReadings()
	{
		int value = m_nAvgReadings;

		// if the value is zero, calculate it
		if ( value == 0 )
		{
			// sum up the valid readings from each station
			for ( auto& node : m_Averages.Items )
			{
				value += node.second->ValidReadings;
			}
		}

		// persist the reading
		AvgReadings = value;

		return value;
	}
	// number of valid average readings
	inline void SetAvgReadings( int value )
	{
		m_nAvgReadings = value;
	}
	// number of valid average readings
	__declspec( property( get = GetAvgReadings, put = SetAvgReadings ))
		int AvgReadings;

	// array of Greater Than pairs
	// first number is the Fahrenheit temperature 
	// the second number is the number of temperatures greater than the first number
	inline vector<CStationYear::GREATER_COUNT> GetGreaterCounts()
	{
		return m_GreaterCounts;
	}
	// array of Greater Than pairs
	// first number is the Fahrenheit temperature 
	// the second number is the number of temperatures greater than the first number
	inline void SetGreaterCounts( vector<CStationYear::GREATER_COUNT> value )
	{
		m_GreaterCounts = value;
	}
	// array of Greater Than pairs
	// first number is the Fahrenheit temperature 
	// the second number is the number of temperatures greater than the first number
	__declspec( property( get = GetGreaterCounts, put = SetGreaterCounts ) )
		vector<CStationYear::GREATER_COUNT> GreaterCounts;

// protected methods
protected:

// public methods
public:
	// store climate year data
	bool WriteStationYear( shared_ptr< CStationYear >& Year )
	{
		bool value = false;
		CClimateTemperature::MEASURE_TYPE eType = Year->MeasurementType;
		const CString csStation = Year->Station;

		switch ( eType )
		{
			case CClimateTemperature::mtMaximum:
			{
				value = m_Maximums.add( csStation, Year );
				break;
			}
			case CClimateTemperature::mtMinimum:
			{
				value = m_Minimums.add( csStation, Year );
				break;
			}
			case CClimateTemperature::mtAverage:
			{
				value = m_Averages.add( csStation, Year );
				break;
			}
			default:
			{
				value = false;
			}
		}

		return value;
	}

	// count the number values greater than several temperatures
	void CountGreaterValues()
	{
		bool bFirst = true;
		vector< CStationYear::GREATER_COUNT > value;

		// loop through all of the maximum values
		for ( auto& node : m_Maximums.Items )
		{
			if ( bFirst )
			{
				// initialize the array with the first year's values
				value = node.second->GreaterCounts;
				bFirst = false;

			} else
			{
				// sum the values from the remainder of the years
				int index = 0;

				// get the year's values
				vector< CStationYear::GREATER_COUNT > counts =
					node.second->GreaterCounts;

				// loop through the year's values and add them to
				// the total values
				for ( auto& node2 : counts )
				{
					value[ index++ ].second += node2.second;
				}
			}
		}

		GreaterCounts = value;
	}

// protected overrides
protected:

// public overrides
public:

// public constructor
public:
	// default constructor
	CClimateYear()
	{
		// value which indicates missing data
		const float fMissing = CClimateTemperature::GetMissingValue();

		// initialize the values to missing to force a calculation
		Maximum = fMissing;
		Minimum = fMissing;
		Average = fMissing;

		// station count for each measurement type
		MaxStations = 0;
		MinStations = 0;
		AvgStations = 0;

		// the number of valid readings for each measurement type
		MaxReadings = 0;
		MinReadings = 0;
		AvgReadings = 0;
	}

	// destructor
	~CClimateYear()
	{
	}
};

