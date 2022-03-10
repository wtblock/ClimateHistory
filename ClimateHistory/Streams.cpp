/////////////////////////////////////////////////////////////////////////////
// Copyright © 2022 by W. T. Block, all rights reserved
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Streams.h"

/////////////////////////////////////////////////////////////////////////////
// initialize constructor will create files for the given schema
CStreams::CStreams
( 
	CSchemas* pDataSchema,
	CString csSchema, CString csRoot, CString csVersion, CString csGroup
) : m_lRecords( 0 )
{
	USES_CONVERSION;

	DataSchema = pDataSchema;
	Schema = csSchema;
	Root = csRoot;
	Version = csVersion;
	Group = csGroup;

	const CString csFolder = PathName;

	const long lSchema = pDataSchema->nameFind( csSchema );
	if ( lSchema == -1 )
	{
		return;
	}

	CSmartArray<CSchema>& Schemas = pDataSchema->Schemas;
	shared_ptr<CSchema>& pSchema = Schemas.get( lSchema );
	if ( pSchema == nullptr )
	{
		return;
	}

	// build a path to the collection 
	const CString csCollectionName = pSchema->Name;
	const CString csCollectionFolder = 
		csFolder + _T( "\\" ) + csCollectionName + _T( "\\" );
	if ( !::PathFileExists( csCollectionFolder ) )
	{
		::SHCreateDirectory( NULL, A2CW( csCollectionFolder ));
	}

	// create the collection's streams
	CSchema::MAP_NAMES& mapNames = pSchema->NameIndex;

	// the number of streams
	long lStreams = pSchema->NameCount;

	// loop through the stream collection and create the streams if they do
	// not exist
	for ( auto& node : mapNames )
	{
		const CString csStream = node.first;
		const long lStream = node.second;
		shared_ptr<CSchemaStream>& pSchemaStream = 
			pSchema->SchemaStreams.get( lStream );
		if ( pSchemaStream == nullptr )
		{
			continue;
		}

		// create the file if it does not exist or open it if it does
		const CString csPath = csCollectionFolder + csStream;
		shared_ptr<CStream> pStream = shared_ptr<CStream>( new CStream );
		pStream->Pathname = csPath;
		pStream->Name = csStream;
		pStream->SchemaStream = pSchemaStream;
		pStream->Host = this;

		//// open/create the stream's file
		//const bool bOpen = pStream->Open( csPath, csStream, pSchemaStream );

		// if the file opens, add the stream to the collection
		//if ( bOpen )
		{
			Streams.add( csStream, pStream );
		}
	}

} // CStreams

/////////////////////////////////////////////////////////////////////////////
// collection string property
void CStreams::SetString( long record, LPCTSTR stream, LPCTSTR value )
{
	shared_ptr<CStream>& pStream = Streams.find( stream );
	if ( pStream != nullptr )
	{
		VARENUM vt = pStream->Type;
		COleVariant var( value );
		if ( var.vt != vt )
		{
			var.ChangeType( vt );
		}

		switch ( vt )
		{
			case VT_I1:
			{
				pStream->Value[ record ][ 0 ] = (byte*)&var.cVal;
				break;
			}
			case VT_UI1:
			{
				pStream->Value[ record ][ 0 ] = (byte*)&var.bVal;
				break;
			}
			case VT_I2:
			{
				pStream->Value[ record ][ 0 ] = (byte*)&var.iVal;
				break;
			}
			case VT_UI2:
			{
				pStream->Value[ record ][ 0 ] = (byte*)&var.uiVal;
				break;
			}
			case VT_I4:
			{
				pStream->Value[ record ][ 0 ] = (byte*)&var.lVal;
				break;
			}
			case VT_UI4:
			{
				pStream->Value[ record ][ 0 ] = (byte*)&var.ulVal;
				break;
			}
			case VT_R4:
			{
				pStream->Value[ record ][ 0 ] = (byte*)&var.fltVal;
				break;
			}
			case VT_R8:
			{
				pStream->Value[ record ][ 0 ] = (byte*)&var.dblVal;
				break;
			}
			case VT_I8:
			{
				pStream->Value[ record ][ 0 ] = (byte*)&var.llVal;
				break;
			}
			case VT_UI8:
			{
				pStream->Value[ record ][ 0 ] = (byte*)&var.ullVal;
				break;
			}
			case VT_BSTR:
			{
				const USHORT usSize = pStream->Size;
				vector<char> buffer = vector<char>( usSize, 0 );
				LPSTR pBuf = &buffer[ 0 ];
				pStream->Value[ record ][ 0 ] = (byte*)pBuf;
				break;
			}
			default :
			{
				CHelper::ErrorMessage( __FILE__, __LINE__ );
			}
		}
	}
} // SetString

/////////////////////////////////////////////////////////////////////////////
