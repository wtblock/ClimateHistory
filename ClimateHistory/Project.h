/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "CHelper.h"
#include "KeyedCollection.h"
#include "Schemas.h"
#include "Streams.h"
#include "Directory.h"
#include "ClimateStations.h"

/////////////////////////////////////////////////////////////////////////////
class CProject
{
// public definitions
public:

// protected data
protected:
	// the folder of this process
	CString m_csExecutableFolder;

	// the working folder of the project
	CString m_csWorkingFolder;

	// the schema definitions in the project
	CSchemas m_Schemas;

	// the schema definitions in the project
	CString m_csSettings;

	// the path to the station data text file
	CString m_csStationPath;

	// directory of all streams in the project
	shared_ptr<CDirectory> m_pDirectory;

	// collection of climate stations
	shared_ptr<CClimateStations> m_pClimateStations;

// public properties
public:
	// the folder of this process
	inline CString GetExecutableFolder()
	{
		return m_csExecutableFolder;
	}
	// the folder of this process
	inline void SetExecutableFolder( CString value )
	{
		m_csExecutableFolder = value;
	}
	// the folder of this process
	__declspec( property( get = GetExecutableFolder, put = SetExecutableFolder ) )
		CString ExecutableFolder;

	// the working folder of the project
	inline CString GetWorkingFolder()
	{
		return m_csWorkingFolder;
	}
	// the working folder of the project
	inline void SetWorkingFolder( CString value )
	{
		if ( value == _T( "." ) )
		{
			value = CHelper::GetCurrentDirectory();
		}
		if ( !::PathFileExists( value ) )
		{
			CHelper::CreatePath( value );
		}

		if ( value.Right( 1 ) != _T( "\\" ) )
		{
			value += _T( "\\" );
		}

		m_csWorkingFolder = value;
	}
	// the working folder of the project
	__declspec( property( get = GetWorkingFolder, put = SetWorkingFolder ) )
		CString WorkingFolder;

	// the path to the station data text file
	inline CString GetStationPath()
	{
		return m_csStationPath;
	}
	// the path to the station data text file
	inline void SetStationPath( CString value )
	{
		m_csStationPath = value;
	}
	// the path to the station data text file
	__declspec( property( get = GetStationPath, put = SetStationPath ) )
		CString StationPath;

	// pathname of the application settings files
	inline CString GetSettings()
	{
		return m_csSettings;
	}
	// pathname of the application settings files
	inline void SetSettings( CString value )
	{
		m_csSettings = value;
	}
	// pathname of the application settings files
	__declspec( property( get = GetSettings, put = SetSettings  ) )
		CString Settings;

	// the schema definitions in the project
	inline CSchemas* GetSchemas()
	{
		return &m_Schemas;
	}
	// the schema definitions in the project
	__declspec( property( get = GetSchemas ) )
		CSchemas* Schemas;

// protected methods
protected:

// public methods
public:
	// read the schemas that can be in the project from the DataSchema.XML
	// file which is located in the Settings folder of the application 
	// folder
	bool ReadDataSchema( const CString& csExe );

// protected overrides
protected:

// public overrides
public:

// public construction / destruction
public:
	// constructor
	CProject
	( 
		CString csExe, CString csWorkingFolder, CString csStationPath 
	)
	{
		ExecutableFolder = csExe;
		WorkingFolder = csWorkingFolder;
		StationPath = csStationPath;

		// the data schema controls how streams are created
		const bool bReadSchema = ReadDataSchema( csExe );

		// everything else depends on the schema being read
		if ( bReadSchema )
		{
			m_pDirectory = shared_ptr<CDirectory>
			(
				new CDirectory( this )
			);
			m_pClimateStations = shared_ptr<CClimateStations>
			(
				new CClimateStations( this )
			);
		}
	};

	// destructor
	virtual ~CProject()
	{
		m_Schemas.nameClear();
		m_Schemas.Schemas.clear();
	};

};

/////////////////////////////////////////////////////////////////////////////
