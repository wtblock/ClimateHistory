/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 by W. T. Block, All Rights Reserved
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "KeyedCollection.h"
#include "Streams.h"
#include <atlbase.h>
#include <atlcomcli.h>
#include <xmllite.h>
#pragma comment(lib, "xmllite.lib")

using namespace ATL;

/////////////////////////////////////////////////////////////////////////////
// a base class wrapper class for XML collections 
class CXmlCollection : public CStreams
{
// public definitions
public:

// protected data
protected:
	// host collection record's field name if this collection is hosted by 
	// another collection
	CString m_csHostRef;

	// time when cache was populated
	ULONGLONG m_ullCacheTicks;

	// building cache
	bool m_bCaching;

	// the path of the XML file
	CString m_csPath;

// public properties
public:
	// host collection record's field name if this collection is hosted by 
	// another collection
	inline CString GetHostRef()
	{
		return m_csHostRef;
	}
	// host collection record's field name if this collection is hosted by 
	// another collection
	inline void SetHostRef( LPCTSTR value )
	{
		m_csHostRef = value;
	}
	// host collection record's field name if this collection is hosted by 
	// another collection
	__declspec( property( get = GetHostRef, put = SetHostRef ) )
		CString HostRef;

	// time when cache was populated
	inline ULONGLONG GetCacheTicks()
	{
		return m_ullCacheTicks;
	}
	// time when cache was populated
	inline void SetCacheTicks( ULONGLONG value )
	{
		m_ullCacheTicks = value;
	}
	// time when cache was populated
	__declspec( property( get = GetCacheTicks, put = SetCacheTicks ) )
		ULONGLONG CacheTicks;

	// this flag is set while the cache is being populated from the 
	// collection or from the XML
	inline bool GetCaching()
	{
		return m_bCaching;
	}
	// this flag is set while the cache is being populated from the 
	// collection or from the XML
	inline void SetCaching( bool value )
	{
		m_bCaching = value;
	}
	// this flag is set while the cache is being populated from the 
	// collection or from the XML
	__declspec( property( get = GetCaching, put = SetCaching ) )
		bool Caching;

	// the path of the XML file
	inline CString GetPath()
	{
		return m_csPath;
	}
	// the path of the XML file
	inline void SetPath( LPCTSTR value )
	{
		m_csPath = value;
	}
	// the path of the XML file
	__declspec( property( get = GetPath, put = SetPath ) )
		CString Path;

// protected methods
protected:
	// get attribute by name
	inline static CString GetXmlAttribute
	(
		LPCTSTR pcszName, CKeyedCollection<CString,CString>& mapPairs
	)
	{
		CString value;

		if ( mapPairs.Exists[ pcszName ] )
		{
			value = *mapPairs.find( pcszName );
		}

		return value;
	}

	// collect XML attributes into a vector of strings organized as 
	// key/value pairs
	HRESULT ReadXmlAttributes
	(
		IXmlReader* pReader, // the XML reader instance
		CKeyedCollection<CString, CString>& mapPairs // attribute/value pairs
	);

// public methods
public:

// protected overrides
protected:
	// read the given XML file
	virtual bool ReadXML
	( 
		CComPtr<IStream>& pFileStream,
		CComPtr<IXmlReader>& pReader,
		LPCTSTR path 
	);

	// write the given XML file
	virtual bool WriteXML
	( 
		CComPtr<IStream>& pFileStream,
		CComPtr<IXmlWriter>& pWriter,
		LPCTSTR path 
	);

	// add a record to the collection from XML
	virtual bool AddRecordFromXML
	(
		CString csTableKey,
		CString csRowKey,
		CKeyedCollection<CString, CString>& mapAttributes

	) = 0;

	// clear the XML cache
	virtual void ClearCache() = 0;

// public overrides
public:
	// read the given XML file
	virtual bool Read( LPCTSTR xml );

	// write the given XML file
	virtual bool Write( LPCTSTR xml );

// public construction / destruction
public:
	CXmlCollection();
	virtual ~CXmlCollection();
};

