/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "CHelper.h"

/////////////////////////////////////////////////////////////////////////////
// a single month's temperature data that has been parsed from a line of text
// stored in tmax, tmin, or tavg files which can be downloaded from this site:
//
//	https://www.ncei.noaa.gov/products/land-based-station/us-historical-climatology-network
//
// The format for these files is a text file where the information is encoded
// by column positions as defined in the following comments from the web site's
// readme.txt file located here:
//
//	https://www.ncei.noaa.gov/pub/data/ushcn/v2.5/readme.txt
//
// the data is divided into three categories:
//
//	1. Raw - data that has had no compensations
//	2. TOB - Time of Observation Bias
//	3. FLs.52j - Pairwise Homogenization Algorithm used
// 
// There are three different versions of the above categories
//
//	1. tavg - temperature average
//	2. tmax - temperature maximum
//	3. tmin - temperature minimum
//
// The combination of these two categories yields 9 different file types that
// can be downloaded from this page:
//
//	https://www.ncei.noaa.gov/pub/data/ushcn/v2.5/
//
// Each line of text from each of these files is formatted by column positions
// and the constructor of this class parses a line of that text into the 
// properties of this class.
//
class CClimateTemperature
{
// public definitions
public:
	// each month contains a value representing one of three possible
	// values: maximum, minimum, and average 
	typedef enum MEASURE_TYPE
	{
		 mtMissing = 0,
		 mtMaximum = 1,
		 mtMinimum = 2,
		 mtAverage = 3,

	} MEASURE_TYPE;

// protected data
protected:
	// each month contains a value representing the one of three possible
	// values: maximum, minimum, and average 
	MEASURE_TYPE m_eMeasurementType;

	// temperature in degrees centigrade
	float m_fCentigrade;

	// Fragment of readme.txt file referenced at the top of this file:
	// DMFLAG: data measurement flag, nine possible values :
	// 	blank = no measurement information applicable
	// 	a - i = number of days missing in calculation of monthly mean
	// 		temperature
	// 	E = The value is estimated using values from surrounding
	// 		stations because a monthly value could not be computed
	// 		from daily data; or ,
	// 			the pairwise homogenization algorithm removed the value
	// 		because of too many apparent inhomogeneities occurring
	// 		close together in time.
	CString m_csFlagDM;

	// Fragment of readme.txt file referenced at the top of this file:
	// QCFLAG: quality control flag, seven possibilities within
	//	quality controlled unadjusted( qcu ) dataset, and 2
	//	possibilities within the quality controlled adjusted( qca )
	//	dataset.
	//	Quality Controlled Unadjusted( QCU ) QC Flags :
	//	BLANK = no failure of quality control check or could not be
	//		evaluated.
	//	D = monthly value is part of an annual series of values that
	//		are exactly the same( e.g.duplicated ) within another
	//		year in the station's record.
	//	I = checks for internal consistency between TMAX and TMIN.
	//		Flag is set when TMIN > TMAX for a given month.
	//	L = monthly value is isolated in time within the station
	//		record, and this is defined by having no immediate non -
	//		missing values 18 months on either side of the value.
	//	M = Manually flagged as erroneous.
	//	O = monthly value that is >= 5 bi - weight standard deviations
	//		from the bi - weight mean.Bi - weight statistics are
	//		calculated from a series of all non - missing values in
	//		the station's record for that particular month.
	//	S = monthly value has failed spatial consistency check.
	//		Any value found to be between 2.5 and 5.0 bi - weight
	//		standard deviations from the bi - weight mean, is more
	//		closely scrutinized by examining the 5 closest neighbors
	//		( not to exceed 500.0 km ) and determine their associated
	//		distribution of respective z - scores.At least one of
	//		the neighbor stations must have a z score with the same
	//		sign as the target and its z - score must be greater than
	//		or equal to the z - score listed in column B( below ),
	//		where column B is expressed as a function of the target
	//		z - score ranges( column A ).
	//		----------------------------
	//		A			| B
	//		----------------------------
	//		4.0 - 5.0	| 1.9
	//		----------------------------
	//		3.0 - 4.0	| 1.8
	//		----------------------------
	//		2.75 - 3.0	| 1.7
	//		----------------------------
	//		2.50 - 2.75 | 1.6
	//	W = monthly value is duplicated from the previous month,
	//		based upon regional and spatial criteria and is only
	//		applied from the year 2000 to the present.
	//	Quality Controlled Adjusted( QCA ) QC Flags :
	//	A = alternative method of adjustment used.
	//	M = values with a non - blank quality control flag in the "qcu"
	//		dataset are set to missing the adjusted dataset and given
	//		an "M" quality control flag.
	CString m_csFlagQC;

	// Fragment of readme.txt file referenced at the top of this file:
	// DSFLAG: data source flag for monthly value :
	//	Blank = Value was computed from daily data available in GHCN - Daily
	//	Not Blank = Daily data are not available so the monthly value was
	//		obtained from the USHCN version 1 dataset.The possible
	//		Version 1 DSFLAGS are as follows :
	//	1 = NCDC Tape Deck 3220, Summary of the Month Element Digital File
	//	2 = Means Book - Smithsonian Institute, C.A.Schott( 1876, 1881 thru 1931 )
	//	3 = Manuscript - Original Records, National Climatic Data Center
	//	4 = Climatological Data( CD ), monthly NCDC publication
	//	5 = Climate Record Book, as described in History of Climatological Record
	//		Books, U.S.Department of Commerce, Weather Bureau, USGPO( 1960 )
	//	6 = Bulletin W - Summary of the Climatological Data for the United States( by
	//		section ), F.H.Bigelow, U.S.Weather Bureau( 1912 ); and, Bulletin W -
	//		Summary of the Climatological Data for the United States, 2nd Ed.
	//	7 = Local Climatological Data( LCD ), monthly NCDC publication
	//	8 = State Climatologists, various sources
	//	B = Professor Raymond Bradley - Refer to Climatic Fluctuations of the Western
	//		United States During the Period of Instrumental Records, Bradley, et.al.,
	//		Contribution No. 42, Dept.of Geography and Geology, University of
	//		Massachusetts( 1982 )
	//	D = Dr.Henry Diaz, a compilation of data from Bulletin W, LCD, and NCDC Tape
	//		Deck 3220 ( 1983 )
	//	G = Professor John Griffiths - primarily from Climatological Data
	CString m_csFlagDS;

// public properties
public:
	// value to indicate missing data
	inline static float GetMissingValue()
	{
		return -9999.0f;
	}
	// value to indicate missing data
	__declspec( property( get = GetMissingValue ) )
		float MissingValue;
	
	// flag to indicate missing data
	inline bool GetMissing()
	{
		const float fValue = Centigrade;
		const bool value = CHelper::NearlyEqual( fValue, -9999.0f );
		return value;
	}
	// flag to indicate missing data
	__declspec( property( get = GetMissing ) )
		bool Missing;

	// length of source value
	inline int GetValueLength()
	{
		return 6;
	}
	// length of source value
	__declspec( property( get = GetValueLength ) )
		int ValueLength;
	
	// length of source flag
	inline int GetFlagLength()
	{
		return 1;
	}
	// length of source flag
	__declspec( property( get = GetFlagLength ) )
		int FlagLength;
	
	// each month contains a value representing the one of three possible
	// values depending on the file extension: maximum (.tmax), minimum 
	// (.tmin), and average (.tavg)
	inline MEASURE_TYPE GetMeasurementType()
	{
		// return value
		const MEASURE_TYPE value = m_eMeasurementType;

		return value;
	}
	// each month contains a value representing the one of three possible
	// values depending on the file extension: maximum (.tmax), minimum 
	// (.tmin), and average (.tavg)
	inline void SetMeasurementType( MEASURE_TYPE value )
	{
		m_eMeasurementType = value;
	}
	// each month contains a value representing the one of three possible
	// values depending on the file extension: maximum (.tmax), minimum 
	// (.tmin), and average (.tavg)
	__declspec( property( get = GetMeasurementType, put = SetMeasurementType ) )
		MEASURE_TYPE MeasurementType;
	
	// temperature in degrees centigrade
	inline float GetCentigrade()
	{
		// return value
		const float value = m_fCentigrade;

		return value;
	}
	// temperature in degrees centigrade
	inline void SetCentigrade( float value )
	{
		m_fCentigrade = value;
	}
	// temperature in degrees fahrenheit
	__declspec( property( get = GetCentigrade, put = SetCentigrade ) )
		float Centigrade;
	
	// temperature in degrees fahrenheit
	inline float GetFahrenheit()
	{
		// value of missing data
		const float fMissing = MissingValue;

		// return value
		float value = Centigrade;
		if ( !CHelper::NearlyEqual( value, fMissing ))
		{
			value = value * 1.8f + 32.0f;
		}

		return value;
	}
	// temperature in degrees fahrenheit
	inline void SetFahrenheit( float value )
	{
		// value of missing data
		const float fMissing = MissingValue;

		if ( CHelper::NearlyEqual( value, fMissing ))
		{
			Centigrade = value;

		} else
		{
			Centigrade = ( value - 32.0f ) / 1.8f;
		}

	}
	// temperature in degrees fahrenheit
	__declspec( property( get = GetFahrenheit, put = SetFahrenheit ) )
		float Fahrenheit;
	
	// data measurement flag
	inline CString GetDataMeasurementFlag()
	{
		const CString value = m_csFlagDM;

		return value;
	}
	// data measurement flag
	inline void SetDataMeasurementFlag( CString value )
	{
		m_csFlagDM = value;
	}
	// data measurement flag
	__declspec( property( get = GetDataMeasurementFlag, put = SetDataMeasurementFlag ) )
		CString DataMeasurementFlag;

	// quality control flag
	inline CString GetQualityControlFlag()
	{
		const CString value = m_csFlagQC;

		return value;
	}
	// quality control flag
	inline void SetQualityControlFlag( CString value )
	{
		m_csFlagQC = value;
	}
	// quality control flag
	__declspec( property( get = GetQualityControlFlag, put = SetQualityControlFlag ) )
		CString QualityControlFlag;

	// data source flag
	inline CString GetDataSourceFlag()
	{
		const CString value = m_csFlagDS;

		return value;
	}
	// data source flag
	inline void SetDataSourceFlag( CString value )
	{
		m_csFlagDS = value;
	}
	// data source flag
	__declspec( property( get = GetDataSourceFlag, put = SetDataSourceFlag ) )
		CString DataSourceFlag;

// protected methods
protected:

// public methods
public:

// protected overrides
protected:

// public overrides
public:

// public constructor
public:
	// default constructor
	CClimateTemperature()
	{
		Centigrade = MissingValue;
		MeasurementType = mtMissing;
	}

	// constructor given a source line of text, the starting position of
	// a temperature value in the source line, and the type of measurement:
	// maximum, minimum, or average values:
	//
	// a source line looks like this:
	//
	//		Station ID  Year Temp 1  Temp 2   ...  Temp 12
	//		USH00011084 1900 -9999    -9999   ...  2067a 3
	//
	// NOTES: 
	//	-9999 represents missing temperatures
	//	Each temperature is either a maximum, minimum, or average
	//		temperature depending on the extension of the file:
	//			tmax, tmin, or tavg
	//		and this is passed to the constructor via the eType parameter
	//	This class only stores a single temperature based on the starting
	//		position in the source line
	//	There are 3 characters following the temperatures that represent 
	//		flags for: Data Measurement, Quality Control, and Data Source
	//		as described at the beginning of this class and the most 
	//		common for all of them is to be blank.
	//	There is a class called CStationYear that collects 12 of these
	//		objects for a single station for a year.
	CClimateTemperature( CString& source, int& nStart, MEASURE_TYPE eType )
	{
		// parse the value from the source line
		const CString csValue = source.Mid( nStart, ValueLength ).Trim();
		const float fValue = (float)_tstof( csValue );

		// parse the flags from the source line
		nStart += ValueLength;
		DataMeasurementFlag = source.Mid( nStart, FlagLength );
		nStart += FlagLength;
		QualityControlFlag = source.Mid( nStart, FlagLength );
		nStart += FlagLength;
		DataSourceFlag = source.Mid( nStart, FlagLength );
		nStart += FlagLength;

		// is our data missing?
		const float fMissing = MissingValue;
		const bool bMissing = CHelper::NearlyEqual( fValue, fMissing );

		// test for missing value
		if ( bMissing )
		{
			Centigrade = fMissing;
			MeasurementType = mtMissing;

		} else // data is not missing
		{
			// the file stores the data in 100ths of a degree centigrade
			Centigrade = fValue / 100.0f;
			MeasurementType = eType;
		}
	}

	// destructor
	~CClimateTemperature()
	{
	}
};

