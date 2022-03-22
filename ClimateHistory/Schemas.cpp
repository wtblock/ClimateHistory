/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Schemas.h"
#include "CHelper.h"

/////////////////////////////////////////////////////////////////////////////
CSchemas::CSchemas()
{
	StateXML = stateInit;
}

/////////////////////////////////////////////////////////////////////////////
CSchemas::~CSchemas()
{
}

/////////////////////////////////////////////////////////////////////////////
// collect XML attributes into a map of key/value pairs
HRESULT CSchemas::ReadXmlAttributes
(	IXmlReader* pReader, MAP_KEY_VALUE_PAIR& mapPairs 
)
{
	USES_CONVERSION;
	
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
			if (!pReader->IsDefault())
			{
				UINT cwchPrefix;
				if (FAILED( hr = pReader->GetPrefix( &pwszPrefix, &cwchPrefix )))
				{
					return -1;
				}
				if (FAILED( hr = pReader->GetLocalName( &pwszLocalName, NULL )))
				{
					return -1;
				}
				if (FAILED( hr = pReader->GetValue( &pwszValue, NULL )))
				{
					return -1;
				}
				const CString csName = CW2CT( pwszLocalName );
				const CString csValue = CW2CT( pwszValue );
				mapPairs.insert( KEY_VALUE_PAIR( csName, csValue ));
			}
			
			if ( S_OK != pReader->MoveToNextAttribute())
			{
				break;
			}
		}
	}
	return hr;
} // ReadXmlAttributes


/////////////////////////////////////////////////////////////////////////////
// open the schema path
bool CSchemas::OpenSchemas( LPCTSTR path )
{	
	USES_CONVERSION;
	
	SchemaPath = path;

	const WCHAR* pwszPrefix;
	const WCHAR* pwszLocalName;
	const WCHAR* pwszValue;
	UINT cwchPrefix;
	HRESULT hr;
	
	// create a file stream from the given pathname
	const CString csPath( path );
	CComPtr<IStream> pFileStream;
	if 
	(
		FAILED
		(
			hr = SHCreateStreamOnFile( csPath, STGM_READ, &pFileStream )
		)
	)
	{
		return false;
	}
	
	// create an instance of the XML Reader
	CComPtr<IXmlReader> pReader;
	if 
	(
		FAILED
		(
			hr = CreateXmlReader
			(
				__uuidof(IXmlReader), (void**) &pReader, NULL 
			)
		)
	)
	{	return false;
	}
	
	if 
	(
		FAILED
		(
			hr = pReader->SetProperty
			(
				XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit 
			)
		)
	)
	{
		return false;
	}
	
	// pass the file stream to the reader as our input
	if ( FAILED( hr = pReader->SetInput( pFileStream )))
	{
		return false;
	}
	
	// read the entire XML file and build a collection of objects
	StateXML = stateInit;

	CString csCurrentCollection;
	MAP_KEY_VALUE_PAIR mapAttributes;
	XmlNodeType nodeType;

	while ( S_OK == ( hr = pReader->Read( &nodeType )))
	{
		switch ( nodeType )
		{
			case XmlNodeType_XmlDeclaration:
			{
				if 
				(
					FAILED( hr = ReadXmlAttributes( pReader, mapAttributes ))
				)
				{
					return false;
				}
				break;
			}
			case XmlNodeType_Element:
			{
				if 
				(
					FAILED
					(
						hr = pReader->GetPrefix( &pwszPrefix, &cwchPrefix )
					)
				)
				{
					return false;
				}
				if 
				(
					FAILED
					(
						hr = pReader->GetLocalName( &pwszLocalName, NULL )
					)
				)
				{
					return false;
				}
				if 
				(
					FAILED
					(
						hr = ReadXmlAttributes( pReader, mapAttributes )
					)
				)
				{
					return false;
				}
				
				const XML_PARSE_STATES eState = StateXML;
				const CString csKey = CW2CT( pwszLocalName );

				// collecting schema tables
				if ( csKey == _T( "DataSchema" )) 
				{	
					StateXML = stateCollections;

				// collection schema definition
				} else if ( csKey == _T( "Collection" )) 
				{
					csCurrentCollection = 
						GetXmlAttribute( _T( "Name" ), mapAttributes );
					if ( !csCurrentCollection.IsEmpty())
					{
						StateXML = stateStreams;

						// since this method is used to parse override files 
						// as well as the primary file, the collection may already 
						// exist
						const bool bExists = nameExists( csCurrentCollection );
						
						// if the collection exists, find it, otherwise create it
						long lIndex = -1;
						if ( bExists )
						{
							lIndex = nameFind( csCurrentCollection );

						} else
						{
							lIndex = Schemas.add();
							
							// index the schema collection name
							nameAdd( csCurrentCollection, lIndex );
						}

						// lookup the schema by index
						shared_ptr<CSchema>& pSchema = Schemas.get( lIndex );
						const long lCount1 = pSchema.use_count();

						// get the schema collection's other attributes
						const CString csDescription = GetXmlAttribute
							(
								_T( "Description" ), mapAttributes 
							);
						const CString csSchema = 
							GetXmlAttribute( _T( "Schema" ), mapAttributes );
						const CString csUniqueKeys = 
							GetXmlAttribute( _T( "IndexKeys" ), mapAttributes );
						
						// write the attributes to the class instance
						pSchema->Name = csCurrentCollection;

						// only record non-empty tokens because we do not 
						// want the override files to "blank" out the 
						// primary file
						if ( !csDescription.IsEmpty() )
						{
							pSchema->Description = csDescription;
						}
						if ( !csSchema.IsEmpty() )
						{
							pSchema->Schema = csSchema;
						}
						if ( !csUniqueKeys.IsEmpty() )
						{
							pSchema->UniqueKeys = csUniqueKeys;
						}
						
						pSchema->Host = this;
						StateXML = stateStreams;
					}

				// schema collection stream definition
				} else if ( csKey == _T( "Stream" )) 
				{
					if ( eState == stateStreams )
					{
						const long lIndex = nameFind( csCurrentCollection );
						if ( lIndex != -1 )
						{
							shared_ptr<CSchema>& pSchema = Schemas.get( lIndex );
							const CString csStreamName = 
								GetXmlAttribute( _T( "Name" ), mapAttributes );
							if ( !csStreamName.IsEmpty())
							{
								long lStream = -1;

								// since this method is used to parse 
								// override files as well as the primary 
								// file, the stream  may already exist
								const bool bExists = 
									pSchema->NameExists[ csStreamName ];
								
								// if the stream exists, find it, 
								// otherwise create it
								if ( bExists )
								{
									lStream = pSchema->NameFind( csStreamName );

								} else // create a new one
								{
									lStream = pSchema->SchemaStreams.add();
								}

								// add the stream to the name index
								pSchema->NameAdd( csStreamName, lStream );

								// get the schema column by index
								shared_ptr<CSchemaStream> pStream = 
									pSchema->SchemaStreams.get( lStream );

								// get the attribute tokens
								const CString csType = GetXmlAttribute
								(
									_T( "Type" ), mapAttributes 
								);
								const CString csNull = GetXmlAttribute
								(
									_T( "Null" ), mapAttributes 
								);
								const CString csSize = GetXmlAttribute
								(
									_T( "Size" ), mapAttributes 
								);
								const CString csUnits = GetXmlAttribute
								(
									_T( "UnitCategory" ), mapAttributes 
								);
								const CString csTitle = GetXmlAttribute
								(
									_T( "Title" ), mapAttributes 
								);
								const CString csDescription = GetXmlAttribute
								(
									_T( "Description" ), mapAttributes 
								);
								const CString csEntry = GetXmlAttribute
								(
									_T( "Entry" ), mapAttributes 
								);
								const CString csEnumeration = GetXmlAttribute
								(
									_T( "Enumeration" ), mapAttributes 
								);

								// these mnemonics can be deleted in an 
								// override by setting  them to empty
								const CString csPropertyGroup = GetXmlAttribute
								(
									_T( "PropertyGroup" ), mapAttributes 
								);
								
								// to distinguish the empty value from and 
								// undefined value, we are checking to see 
								// if the attribute is present in the map
								const bool bGroup = AttributeDefined
								(
									_T( "PropertyGroup" ), mapAttributes 
								);

								// convert the tokens into their native types
								const USHORT usSize = (USHORT)_ttol( csSize );
								
								CSchemaStream::ENUM_ENTRY eEntry = 
									CSchemaStream::eeFree;
								if ( csEntry == _T( "enumeration" ) )
								{
									eEntry = CSchemaStream::eeEnum1;

								} else if ( csEntry == _T( "numbered enumeration" ) )
								{
									eEntry = CSchemaStream::eeEnum2;

								} else if ( csEntry == _T( "Boolean" ) )
								{
									eEntry = CSchemaStream::eeBoolean;

								} else if ( csEntry == _T( "Versions" ) )
								{
									eEntry = CSchemaStream::eeVersion;

								} else if ( csEntry == _T( "stream names" ) )
								{
									eEntry = CSchemaStream::eeStream;

								} else if ( csEntry == _T( "Date and time" ) )
								{
									eEntry = CSchemaStream::eeDate;

								} else if ( csEntry == _T( "Unit Category" ) )
								{
									eEntry = CSchemaStream::eeUnitCategory;

								} else if ( csEntry == _T( "Unit Name" ) )
								{
									eEntry = CSchemaStream::eeUnitName;
								}

								VARENUM eEnum = VT_EMPTY;
								if ( csType == _T( "VT_I2" ) )
								{
									eEnum = VT_I2;
								}
								else if ( csType == _T( "VT_I4" ) )
								{
									eEnum = VT_I4;
								}
								else if ( csType == _T( "VT_R4" ) )
								{
									eEnum = VT_R4;
								}
								else if ( csType == _T( "VT_R8" ) )
								{
									eEnum = VT_R8;
								}
								else if ( csType == _T( "VT_DATE" ) )
								{
									eEnum = VT_DATE;
								}
								else if ( csType == _T( "VT_BSTR" ) )
								{
									eEnum = VT_BSTR;
								}
								else if ( csType == _T( "VT_I1" ) )
								{
									eEnum = VT_I1;
								}
								else if ( csType == _T( "VT_UI1" ) )
								{
									eEnum = VT_UI1;
								}
								else if ( csType == _T( "VT_UI2" ) )
								{
									eEnum = VT_UI2;
								}
								else if ( csType == _T( "VT_UI4" ) )
								{
									eEnum = VT_UI4;
								}
								else if ( csType == _T( "VT_I8" ) )
								{
									eEnum = VT_I8;
								}
								else if ( csType == _T( "VT_UI8" ) )
								{
									eEnum = VT_UI8;
								}

								//pStream->Host = pSchema;
								pStream->Name = csStreamName;

								if ( bGroup )
								{
									pStream->PropertyGroup = csPropertyGroup;
								}

								// only record non-empty tokens because 
								// we do not want the override files to "blank"
								// out the primary file
								if ( !csType.IsEmpty() )
								{
									pStream->Type = eEnum;
								}
								if ( !csNull.IsEmpty() )
								{
									const double dNull = _tstof( csNull );
									pStream->Null = dNull;
								}
								if ( !csSize.IsEmpty() )
								{
									pStream->Size = usSize;
								}
								if ( !csUnits.IsEmpty() )
								{
									pStream->UnitCategory = csUnits;
								}
								if ( !csTitle.IsEmpty() )
								{
									pStream->Title = csTitle;
								}
								if ( !csDescription.IsEmpty() )
								{
									pStream->Description = csDescription;
								}
								if ( !csEntry.IsEmpty() )
								{
									pStream->Entry = eEntry;
								}
								if ( !csEnumeration.IsEmpty() )
								{
									pStream->Enumeration = csEnumeration;
								}
							}
						}
					}
				}
				break;
			}
			case XmlNodeType_EndElement:
			{	if 
				(	FAILED
					(
						hr = pReader->GetPrefix( &pwszPrefix, &cwchPrefix )
					)
				)
				{	return false;
				}
				if 
				(	FAILED
					(
						hr = pReader->GetLocalName( &pwszLocalName, NULL )
					)
				)
				{	return false;
				}
				const CString csKey = CW2CT( pwszLocalName );
				break;
			}
			case XmlNodeType_Text:
			case XmlNodeType_Whitespace:
			{	if ( FAILED( hr = pReader->GetValue( &pwszValue, NULL )))
				{
					return false;
				}
				const CString csValue = CW2CT( pwszValue );
				break;
			}
			case XmlNodeType_CDATA:
			{	if ( FAILED( hr = pReader->GetValue( &pwszValue, NULL )))
				{
				return false;
				}
				break;
			}
			case XmlNodeType_ProcessingInstruction:
			{	if 
				(	FAILED
					(
						hr = pReader->GetLocalName( &pwszLocalName, NULL )
					)
				)
				{	return false;
				}
				if ( FAILED( hr = pReader->GetValue( &pwszValue, NULL )))
				{
					return false;
				}
				break;
			}
			case XmlNodeType_Comment:
			{	if ( FAILED( hr = pReader->GetValue( &pwszValue, NULL )))
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
} // OpenSchemas

/////////////////////////////////////////////////////////////////////////////

