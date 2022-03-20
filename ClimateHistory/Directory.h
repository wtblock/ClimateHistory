/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Streams.h"
#include "Schemas.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
class CProject;

/////////////////////////////////////////////////////////////////////////////
// the directory itemizes the properties of all the streams in the project 
class CDirectory : public CStreams
{
// public definitions
public:

// protected data
protected:
	// pointer to the hosting project
	CProject* m_pHost;

// public properties
public:
	// pointer to the hosting project
	CProject* GetHost();
	// pointer to the hosting project
	void SetHost( CProject* value );
	// pointer to the hosting project
	__declspec( property( get = GetHost, put = SetHost ) )
		CProject* Host;

// protected methods
protected:

// public methods
public:
	// create / open the directory
	ULONG CreateOpenDirctory();

// protected overrides
protected:

// public overrides
public:

// public construction / destruction
public:
	// constructor
	CDirectory( CProject* pHost );

};

