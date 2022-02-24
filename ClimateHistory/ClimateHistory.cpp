/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClimateHistory.h"
#include "CHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
CWinApp theApp;

/////////////////////////////////////////////////////////////////////////////
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// output the yearly averages as comma separated values (CSV)
void OutputCSV( CStdioFile& fOut )
{
	CString csHeading
	(
		_T( "Year," )
		_T( "Max Stat," )
		_T( "Min Stat," )
		_T( "Avg Stat," )
		_T( "Max Read," )
		_T( "Min Read," )
		_T( "Avg Read," )
		_T( "Maximum," )
		_T( "Minimum," )
		_T( "Average," )
		_T( "%>90," )
		_T( "%>95," )
		_T( "%>100," )
		_T( "%>105," )
		_T( "%>110," )
		_T( "%>120," )
		_T( "%>125" )
		_T( "\n" )
	);

	fOut.WriteString( csHeading );
	const float fMissing = CClimateTemperature::GetMissingValue();

	for ( auto& node : m_ClimateYears.Items )
	{
		CString csOut;
		CString csYear = node.first;
		vector<CStationYear::GREATER_COUNT> counts = node.second->GreaterCounts;
		const int nCounts = (int)counts.size();
		const int nMaxStat = node.second->MaxStations;
		const int nMinStat = node.second->MinStations;
		const int nAvgStat = node.second->AvgStations;
		const int nMaxRead = node.second->MaxReadings;
		const int nMinRead = node.second->MinReadings;
		const int nAvgRead = node.second->AvgReadings;
		const float fMaximum = CHelper::GetFahrenheit( node.second->Maximum, fMissing );
		const float fMinimum = CHelper::GetFahrenheit( node.second->Minimum, fMissing );
		const float fAverage = CHelper::GetFahrenheit( node.second->Average, fMissing );

		// convert the greater than counts into percentage of 
		// valid readings (nMaxRead)
		vector<float> percents( 9, 0.0f );

		// handle exceptional cases
		if ( counts.size() != 0 && nMaxRead > 0 )
		{
			int index = 0;
			for ( auto& count : counts )
			{
				if ( index < 9 )
				{
					percents[ index ] = 
						float( counts[ index ].second * 100 ) / nMaxRead;
					index++;
				}
			}
		}

		csOut.Format
		(
			_T( "%s,%d,%d,%d,%d,%d,%d,%0.2f,%0.2f,%0.2f," )
			_T( "%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f,%0.2f\n" ),
			csYear, nMaxStat, nMinStat, nAvgStat, nMaxRead, nMinRead, nAvgRead, 
			fMaximum, fMinimum, fAverage,
			percents[ 0 ], percents[ 1 ], percents[ 2 ], 
			percents[ 3 ], percents[ 4 ], percents[ 5 ], percents[ 6 ]
		);

		fOut.WriteString( csOut );
	}

} // OutputCSV

/////////////////////////////////////////////////////////////////////////////
// parse a given line of source and persist it
bool ParseSource
( 
	CString& source, CClimateTemperature::MEASURE_TYPE eType
)
{
	bool value = false;

	// create a new CStationYear object based on the source and type
	shared_ptr< CStationYear > StationYear = shared_ptr< CStationYear >
		(
			new CStationYear( source, eType )
		);

	const CString csYear = StationYear->Year;
	const CString csStation = StationYear->Station;

	const bool bExists = m_ClimateYears.Exists[ csYear ];

	shared_ptr<CClimateYear> ClimateYear;

	if ( bExists )
	{
		ClimateYear = m_ClimateYears.find( csYear );

	} else
	{
		ClimateYear = shared_ptr<CClimateYear>( new CClimateYear );
		ClimateYear->Year = csYear;
		m_ClimateYears.add( csYear, ClimateYear );
	}

	value = ClimateYear->WriteStationYear( StationYear );

	return value;
} // ParseSource

/////////////////////////////////////////////////////////////////////////////
// crawl through the directory tree looking for given climate extension
void RecursePath
( 
	LPCTSTR path, // pathname to recurse
	// extension of climate files to process (tmax, tmin, or tavg)
	LPCTSTR ext, 
	CStdioFile& fOut, // standard output
	CStdioFile& fErr // error output
)
{
	USES_CONVERSION;

	int nStation = 0;

	// determine measurement type from the given extension
	CClimateTemperature::MEASURE_TYPE eType = CClimateTemperature::mtMaximum;
	const CString csExtention = CString( ext ).MakeLower();
	if ( csExtention == _T( ".tmin" ))
	{
		eType = CClimateTemperature::mtMinimum;

	} else if ( csExtention == _T( ".tavg" ))
	{
		eType = CClimateTemperature::mtAverage;
	}

	// get the folder which will trim any wild card data
	CString csPathname = CString( path ).Trim( _T( "\\" ));

	// build a pathname with wild-card extension
	CString strWildcard;
	strWildcard.Format( _T( "%s\\*.*" ), csPathname );
	
	// start trolling for files we are interested in
	CFileFind finder;
	BOOL bWorking = finder.FindFile( strWildcard );
	while ( bWorking )
	{
		bWorking = finder.FindNextFile();

		// skip "." and ".." folder names
		if ( finder.IsDots() )
		{
			continue;
		}

		// if it's a directory, recursively search it
		if ( finder.IsDirectory() )
		{
			// get the new folder name
			const CString folder =
				finder.GetFilePath().TrimRight( _T( "\\" ) );

			// recurse into the new directory with wild cards
			RecursePath( folder, ext, fOut, fErr );

		} else // write the properties for valid extension
		{
			const CString csPath = finder.GetFilePath();
			const CString csExt = CHelper::GetExtension( csPath ).MakeLower();
			if ( csExt == ext )
			{
				// open the stations text file
				CStdioFile file;
				const bool value =
					file.Open( csPath, CFile::modeRead | CFile::shareDenyNone );

				// if the open was successful, read each line of the file and 
				// collect the station data
				if ( value == true )
				{
					bool bFirst = true;

					CString csLine;
					while ( file.ReadString( csLine ) )
					{
						ParseSource( csLine, eType );
						if ( bFirst )
						{
							CString csStation;
							csStation.Format( _T( "%5d %s\n" ), ++nStation, csLine.Left( 11 ));
							fErr.WriteString( csStation );
							bFirst = false;
						}
					}
				}

				//fErr.WriteString( csPath );
				//fErr.WriteString( _T( "\n" ));
			}
		}
	}

	finder.Close();

} // RecursePath

/////////////////////////////////////////////////////////////////////////////
// read the station data into the stations collection
bool ReadStationData( LPCTSTR path )
{
	// open the stations text file
	CStdioFile file;
	const bool value = 
		file.Open( path, CFile::modeRead | CFile::shareDenyNone );

	// if the open was successful, read each line of the file and 
	// collect the station data
	if ( value == true )
	{
		CString csLine;
		while ( file.ReadString( csLine ) )
		{
			shared_ptr<CClimateStation> pStation = 
				shared_ptr<CClimateStation>( new CClimateStation( csLine ));
			const CString csKey = pStation->Station;
			m_Stations.add( csKey, pStation );
		}
	}

	return value;
} // ReadStationData

/////////////////////////////////////////////////////////////////////////////
// a console application that can crawl through the file
// system and troll for climate data
int _tmain( int argc, TCHAR* argv[], TCHAR* envp[] )
{
	HMODULE hModule = ::GetModuleHandle( NULL );
	if ( hModule == NULL )
	{
		_tprintf( _T( "Fatal Error: GetModuleHandle failed\n" ) );
		return 1;
	}

	// initialize MFC and error on failure
	if ( !AfxWinInit( hModule, NULL, ::GetCommandLine(), 0 ) )
	{
		_tprintf( _T( "Fatal Error: MFC initialization failed\n " ) );
		return 2;
	}

	// do some common command line argument corrections
	vector<CString> arrArgs = CHelper::CorrectedCommandLine( argc, argv );
	size_t nArgs = arrArgs.size();

	CStdioFile fOut( stdout );
	CStdioFile fErr( stderr );
	CString csMessage;

	// display the number of arguments if not 1 to help the user 
	// understand what went wrong if there is an error in the
	// command line syntax
	if ( nArgs != 1 )
	{
		fErr.WriteString( _T( ".\n" ) );
		csMessage.Format
		( 
			_T( "The number of parameters are %d\n.\n" ), nArgs - 1 
		);
		fErr.WriteString( csMessage );

		// display the arguments
		for ( int i = 1; i < nArgs; i++ )
		{
			csMessage.Format
			( 
				_T( "Parameter %d is %s\n.\n" ), i, arrArgs[ i ] 
			);
			fErr.WriteString( csMessage );
		}
	}

	// two arguments if a pathname to the climate data is given
	// three arguments if the station text file name is also given
	if ( nArgs != 2 && nArgs != 3 )
	{
		fErr.WriteString( _T( ".\n" ) );
		fErr.WriteString
		(
			_T( "ClimateHistory, Copyright (c) 2022, " )
			_T( "by W. T. Block.\n" )
		);

		fErr.WriteString
		(
			_T( ".\n" )
			_T( "A Windows command line program to read climate history\n" )
			_T( "  and output a comma separated value (CSV) file\n" )
			_T( "  which can be read into Excel.\n" )
			_T( ".\n" )
		);

		fErr.WriteString
		(
			_T( ".\n" )
			_T( "Usage:\n" )
			_T( ".\n" )
			_T( ".  ClimateHistory pathname [station_file_name]\n" )
			_T( ".\n" )
			_T( "Where:\n" )
			_T( ".\n" )
		);

		fErr.WriteString
		(
			_T( ".  pathname is the root of the tree to be scanned \n" )
			_T( ".  for climate data files with extensions:\n" )
			_T( ".    \"*.tavg - average temperature files\"\n" )
			_T( ".    \"*.tmax - maximum temperature files\"\n" )
			_T( ".    \"*.tmin - minimum temperature files\"\n" )
			_T( ".  station_file_name is the optional station file name: \n" )
			_T( ".    defaults to: \"ushcn-v2.5-stations.txt\"\n" )
			_T( ".\n" )
		);

		return 3;
	}

	// display the executable path
	csMessage.Format( _T( "Executable pathname: %s\n" ), arrArgs[ 0 ] );
	fErr.WriteString( _T( ".\n" ) );
	fErr.WriteString( csMessage );
	fErr.WriteString( _T( ".\n" ) );

	// retrieve the pathname which may be a single period
	CString csPath = arrArgs[ 1 ].Trim( _T( "//" ));

	// test for current folder character (a period)
	bool bExists = csPath == _T( "." );

	// if it is not a period, test for existence of the given folder name
	if ( !bExists )
	{
		if ( ::PathFileExists( csPath ) )
		{
			bExists = true;
		}
	}

	if ( !bExists )
	{
		csMessage.Format( _T( "Invalid pathname:\n\t%s\n" ), csPath );
		fErr.WriteString( _T( ".\n" ) );
		fErr.WriteString( csMessage );
		fErr.WriteString( _T( ".\n" ) );
		return 4;

	} else
	{
		csMessage.Format( _T( "Given pathname:\n\t%s\n" ), csPath );
		fErr.WriteString( _T( ".\n" ) );
		fErr.WriteString( csMessage );
	}

	// read the station data
	CString csStationFile( _T( "ushcn-v2.5-stations.txt" ) );
	if ( nArgs == 3 )
	{
		csStationFile = arrArgs[ 2 ];
	}

	// full pathname of the station file
	const CString csStationPath = csPath + _T( "\\" ) + csStationFile;

	if ( !::PathFileExists( csStationPath ))
	{
		csMessage.Format
		( 
			_T( "Invalid Station File Name:\n\t%s\n" ), csStationFile 
		);
		fErr.WriteString( _T( ".\n" ) );
		fErr.WriteString( csMessage );
		fErr.WriteString( _T( ".\n" ) );
		return 5;

	}

	// start up COM
	AfxOleInit();
	::CoInitialize( NULL );

	// if the station file parses okay, read the climate data files
	if ( ReadStationData( csStationPath ))
	{
		// crawl through directory tree defined by the command line
		// parameter trolling for given climate file extensions
		RecursePath( csPath, _T( ".tmax" ), fOut, fErr );
		RecursePath( csPath, _T( ".tmin" ), fOut, fErr );
		RecursePath( csPath, _T( ".tavg" ), fOut, fErr );

		for ( auto& node : m_ClimateYears.Items )
		{
			const CString csYear = node.second->Year;

			// measurements for this year
			const float fMaximum = node.second->Maximum;
			const float fMinimum = node.second->Minimum;
			const float fAverage = node.second->Average;

			// convert the measurements to Fahrenheit
			const float fMissing = CClimateTemperature::GetMissingValue();
			const float fMaxF = CHelper::GetFahrenheit( fMaximum, fMissing );
			const float fMinF = CHelper::GetFahrenheit( fMinimum, fMissing );
			const float fAvgF = CHelper::GetFahrenheit( fAverage, fMissing );

			// collect Fahrenheit measurements for each year
			m_arrMaximums.push_back( YEAR_VALUE( csYear, fMaxF ) );
			m_arrMinimums.push_back( YEAR_VALUE( csYear, fMinF ) );
			m_arrAverages.push_back( YEAR_VALUE( csYear, fAvgF ) );

			// the number of stations for each measurement
			const int nMaxStations = node.second->MaxStations;
			const int nMinStations = node.second->MinStations;
			const int nAvgStations = node.second->AvgStations;

			// the number of valid readings for each measurement
			const int nMaxReadings = node.second->MaxReadings;
			const int nMinReadings = node.second->MinReadings;
			const int nAvgReadings = node.second->AvgReadings;

			// count the number value greater than several temperatures
			node.second->CountGreaterValues();

			// get the yearly collection
			vector<CStationYear::GREATER_COUNT> greaterValues = 
				node.second->GreaterCounts;

			// accumulate these values in a vector for all of the years
			CLIMATE_COUNT count;
			count.first = csYear;
			count.second = greaterValues;
			m_ClimaterCounts.push_back( count );

		}

		// the actual goal is to output comma separated values (CSV)
		OutputCSV( fOut );

	} else
	{
		csMessage.Format
		( 
			_T( "Failed Reading Station File Name:\n\t%s\n" ), csStationFile 
		);
		fErr.WriteString( _T( ".\n" ) );
		fErr.WriteString( csMessage );
		fErr.WriteString( _T( ".\n" ) );
		return 6;
	}

	// all is good
	return 0;

} // _tmain

/////////////////////////////////////////////////////////////////////////////
