/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"
#include "StationYear.h"
#include "ClimateYear.h"
#include <memory>

using namespace std;

typedef pair<CString,float> YEAR_VALUE;
typedef pair<CString, vector<CStationYear::GREATER_COUNT> > CLIMATE_COUNT;

// rapid climate year lookup
CKeyedCollection<CString, CClimateYear> m_ClimateYears;

vector<YEAR_VALUE> m_arrMaximums;
vector<YEAR_VALUE> m_arrMinimums;
vector<YEAR_VALUE> m_arrAverages;

vector< CLIMATE_COUNT > m_ClimaterCounts;

















