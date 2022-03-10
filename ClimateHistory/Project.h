/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "CHelper.h"
#include "KeyedCollection.h"
#include "Schemas.h"
#include "Streams.h"
#include "ClimateStation.h"

/////////////////////////////////////////////////////////////////////////////
class CProject
{
// public definitions
public:

// protected data
protected:
	// the working folder of the project
	CString m_csWorkingFolder;

	// the schema definitions in the project
	CSchemas m_Schemas;

	// the schema definitions in the project
	CString m_csSettings;

	// list of the known stations
	shared_ptr<CStreams> m_spStationList;

// public properties
public:
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

	// pathname of the application settings files
	inline shared_ptr<CStreams>& GetStationList()
	{
		return m_spStationList;
	}
	// pathname of the application settings files
	inline void SetStationList( shared_ptr<CStreams> value )
	{
		m_spStationList = value;
	}
	// pathname of the application settings files
	__declspec( property( get = GetStationList, put = SetStationList  ) )
		shared_ptr<CStreams> StationList;

// protected methods
protected:

// public methods
public:
	// read the schemas that can be in the project from the DataSchema.XML
	// file which is located in the Settings folder of the application 
	// folder
	bool ReadDataSchema( const CString& csExe );

	// create the station list
	long CreateStationList
	(
		CKeyedCollection<CString, CClimateStation>& stations
	);

// protected overrides
protected:

// public overrides
public:

// public construction / destruction
public:
	// constructor
	CProject()
	{

	};
	virtual ~CProject()
	{
		const long lCount1 = m_spStationList.use_count();
		m_spStationList.reset();
		const long lCount2 = m_spStationList.use_count();

		m_Schemas.nameClear();
		m_Schemas.Schemas.clear();
	};

};

/////////////////////////////////////////////////////////////////////////////
