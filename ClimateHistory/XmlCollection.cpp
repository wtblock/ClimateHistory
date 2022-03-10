/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XmlCollection.h"
#include "CHelper.h"

/////////////////////////////////////////////////////////////////////////////
CXmlCollection::CXmlCollection( )
{
	CacheTicks = 0;

	// this flag is set while the cache is being populated from the 
	// collection or from the XML
	Caching = false;

} // CXmlCollection

/////////////////////////////////////////////////////////////////////////////
CXmlCollection::~CXmlCollection()
{
} // ~CXmlCollection

/////////////////////////////////////////////////////////////////////////////
// collect XML attributes into a map of key/value pairs
HRESULT CXmlCollection::ReadXmlAttributes
( 
	IXmlReader* pReader, // the XML reader instance
	CKeyedCollection<CString,CString>& mapPairs // attribute/value pairs
)
{
	const WCHAR* pwszPrefix;
	const WCHAR* pwszLocalName;
	const WCHAR* pwszValue;
	HRESULT hr = pReader->MoveToFirstAttribute();

	// be sure the output is empty to begin with
	mapPairs.clear();

	if ( S_FALSE == hr )
	{
		return hr;
	}

	if ( S_OK != hr )
	{
		return -1;

	} else
	{
		while ( TRUE )
		{
			if ( !pReader->IsDefault() )
			{
				UINT cwchPrefix;
				hr = pReader->GetPrefix( &pwszPrefix, &cwchPrefix );
				if ( FAILED( hr ) )
				{
					
					return -1;
				}

				hr = pReader->GetLocalName( &pwszLocalName, NULL );
				if ( FAILED( hr ) )
				{
					return -1;
				}

				hr = pReader->GetValue( &pwszValue, NULL );
				if ( FAILED( hr ) )
				{
					return -1;
				}

				const CString csName = CW2CT( pwszLocalName );
				const CString csValue = CW2CT( pwszValue );
				shared_ptr<CString> pValue = shared_ptr<CString>( new CString( csValue ) );
				mapPairs.add( csName, pValue );
			}

			if ( S_OK != pReader->MoveToNextAttribute() )
			{
				break;
			}
		}
	}
	return hr;
} // ReadXmlAttributes

/////////////////////////////////////////////////////////////////////////////
// read the given XML file
bool CXmlCollection::ReadXML
( 
	CComPtr<IStream>& pFileStream,
	CComPtr<IXmlReader>& pReader,
	LPCTSTR path 
)
{	
	const CString csPath( path );
	Path = csPath;
	HRESULT hr = SHCreateStreamOnFile( csPath, STGM_READ, &pFileStream );
	if ( FAILED( hr ) )
	{
		return false;
	}

	hr = CreateXmlReader( __uuidof( IXmlReader ), (void**)&pReader, NULL );
	if ( FAILED( hr ) )
	{
		return false;
	}

	hr = pReader->SetProperty
	( 
		XmlReaderProperty_DtdProcessing, 
		DtdProcessing_Prohibit
	);
	if ( FAILED( hr ) )
	{
		return false;
	}

	// pass the file stream to the reader as our input
	hr = pReader->SetInput( pFileStream );
	if ( FAILED( hr ) )
	{
		return false;
	}

	// read the entire XML file and build a collection of objects
	const WCHAR* pwszPrefix;
	const WCHAR* pwszLocalName;
	const WCHAR* pwszValue;
	UINT cwchPrefix;
	CKeyedCollection<CString,CString> mapAttributes;
	XmlNodeType nodeType;
	CString csCollection, csRecord;

	// read the XML file
	while ( S_OK == ( hr = pReader->Read( &nodeType ) ) )
	{
		switch ( nodeType )
		{
			case XmlNodeType_XmlDeclaration:
			{
				hr = ReadXmlAttributes( pReader, mapAttributes );
				if ( FAILED( hr ) )
				{
					return false;
				}
				break;
			}

			case XmlNodeType_Element:
			{
				hr = pReader->GetPrefix( &pwszPrefix, &cwchPrefix );
				if ( FAILED( hr ) )
				{
					return false;
				}

				hr = pReader->GetLocalName( &pwszLocalName, NULL );
				if ( FAILED( hr ) )
				{
					return false;
				}

				hr = ReadXmlAttributes( pReader, mapAttributes );
				if ( FAILED( hr ) )
				{
					return false;
				}

				// the key for this line
				const CString csLocal = CW2CT( pwszLocalName );

				// do not update the collection unless the record has changed
				// and the record is not empty
				if ( csLocal != csRecord )
				{
					// an empty record occurs when an XML record closes so
					// we do not want to change the collection
					if ( !csRecord.IsEmpty())
					{
						csCollection = csRecord;
					}

					// opening an XML record
					csRecord = csLocal;
				}
				AddRecordFromXML( csCollection, csRecord, mapAttributes );
				break;
			}

			case XmlNodeType_EndElement:
			{
				hr = pReader->GetPrefix( &pwszPrefix, &cwchPrefix );
				if ( FAILED( hr ) )
				{
					return false;
				}

				hr = pReader->GetLocalName( &pwszLocalName, NULL );
				if ( FAILED( hr ) )
				{
					return false;
				}

				// the key for this line
				const CString csLocal = CW2CT( pwszLocalName );

				// closing an XML collection
				if ( csLocal == csCollection )
				{
					csCollection.Empty();

				// closing an XML record
				} else if ( csLocal == csRecord )
				{
					csRecord.Empty();
				}
				
				break;
			}

			case XmlNodeType_Text:
			case XmlNodeType_Whitespace:
			{
				hr = pReader->GetValue( &pwszValue, NULL );
				if ( FAILED( hr ) )
				{
					return false;
				}

				const CString csValue = CW2CT( pwszValue );
				break;
			}

			case XmlNodeType_CDATA:
			{
				hr = pReader->GetValue( &pwszValue, NULL );
				if ( FAILED( hr ) )
				{
					return false;
				}

				break;
			}

			case XmlNodeType_ProcessingInstruction:
			{
				hr = pReader->GetLocalName( &pwszLocalName, NULL );
				if ( FAILED( hr ) )
				{
					return false;
				}

				hr = pReader->GetValue( &pwszValue, NULL );
				if ( FAILED( hr ) )
				{
					return false;
				}

				break;
			}

			case XmlNodeType_Comment:
			{
				hr = pReader->GetValue( &pwszValue, NULL );
				if ( FAILED( hr ) )
				{
					return false;
				}

				break;
			}

			case XmlNodeType_DocumentType:
			{
				break;
			}
		}
	}

	return true;
} // ReadXML

/////////////////////////////////////////////////////////////////////////////
// write the given XML file
bool CXmlCollection::WriteXML
( 
	CComPtr<IStream>& pFileStream,
	CComPtr<IXmlWriter>& pWriter,
	LPCTSTR path 
)
{
	// Open writable output stream
	HRESULT hr = SHCreateStreamOnFile
	( path, STGM_CREATE | STGM_WRITE, &pFileStream
	);
	if ( FAILED( hr ) )
	{
		TRACE( _T( "Failed to create the XML file" ) );
		return false;
	}

	hr = CreateXmlWriter( __uuidof( IXmlWriter ), (void**)&pWriter, NULL );
	if ( FAILED( hr ) )
	{
		TRACE( _T( "Failed to create the XML writer" ) );
		return false;
	}

	pWriter->SetOutput( pFileStream );
	pWriter->SetProperty( XmlWriterProperty_Indent, TRUE );
	pWriter->WriteStartDocument( XmlStandalone_Omit );
	pWriter->WriteStartElement( NULL, L"Customs", NULL );
	pWriter->WriteWhitespace( L"\r\n" );

	return true;
} // WriteXML

/////////////////////////////////////////////////////////////////////////////
// read the given XML file
bool CXmlCollection::Read( LPCTSTR xml )
{
	CComPtr<IStream> pFileStream;
	CComPtr<IXmlReader> pReader;

	const bool value = ReadXML( pFileStream, pReader, xml );

	return value;
} // Read

/////////////////////////////////////////////////////////////////////////////
// write the given XML file
bool CXmlCollection::Write( LPCTSTR xml )
{
	CComPtr<IStream> pFileStream;
	CComPtr<IXmlWriter> pWriter;
	const bool value = WriteXML( pFileStream, pWriter, xml );

	return value;
} // Write

/////////////////////////////////////////////////////////////////////////////
