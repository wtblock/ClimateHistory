/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Project.h"

/////////////////////////////////////////////////////////////////////////////
// read the schemas that can be in the project from the DataSchema.XML
// file which is located in the Settings folder of the application 
// folder
bool CProject::ReadDataSchema( const CString& csExe )
{
	bool value = false;

	const CString csFolder = CHelper::GetFolder( csExe );
	const CString csSettings = csFolder + _T( "Settings\\" );
	Settings = csSettings;
	const CString csDataSchema = csSettings + "DataSchema.xml";
	const bool bExists = ::PathFileExists( csDataSchema ) != FALSE;
	if ( bExists )
	{
		value = m_Schemas.OpenSchemas( csDataSchema );
	}

	return value;
} // ReadDataSchema

/////////////////////////////////////////////////////////////////////////////
