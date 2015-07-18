/*============================================================================*/
/*                                                                            */
/*                            C O P Y R I G H T                               */
/*                                                                            */
/*                          (C) Copyright 2003 by                             */
/*                              Todd Clements                                 */
/*                           All Rights Reserved                              */
/*                                                                            */
/*      The author assumes no responsibility for the use or reliability of    */
/*      his software.                                                         */
/*                                                                            */
/*============================================================================*/

/* ############################################################################################################################## */


#include "stdafx.h"
#include "ChartData.h"

IMPLEMENT_SERIAL( CChartData, CObject, 1 )

CChartData::CChartData()
{
	m_Data = NULL;
	m_NDimensions = 0;
	memset( m_Dimensions, 0, kMaxDims * sizeof( int ) );
	m_CurDataSize = 0;
}

CChartData::~CChartData()
{
	DeleteData();
}

double		*CChartData::GetData( void )
{
	return m_Data;
}

void CChartData::Serialize(CArchive& ar)
{
	DWORD		size = 1;
	int			i;

	if (ar.IsStoring())
	{
		ar << m_NDimensions;
		ar.Write( m_Dimensions, kMaxDims * sizeof( int ) );

		for( i = 0; i < m_NDimensions; i++ )
			size *= m_Dimensions[i];

		ar << size;
		ar.Write( m_Data, size * sizeof( double ) );
	}
	else
	{
		ar << m_NDimensions;
		ar.Write( m_Dimensions, kMaxDims * sizeof( int ) );

		ar >> size;

		m_Data = (double *) VirtualAlloc( NULL, size, 
			MEM_COMMIT, PAGE_READWRITE );

		ar.Read( m_Data, size * sizeof( double ) );
	}
}

// Delete the data set
void	CChartData::DeleteData( void )
{
	// Check if valid data. If so, delete it
	if( m_Data != NULL )
	{
		VirtualFree( m_Data, 0, MEM_RELEASE );
		m_Data = NULL;
	}
}

// Set the dimensions of the data. This can be used
// to simply resize a data set. It doesn't do a lot
// of integrity checking, so buyer beware.
BOOL	CChartData::SetDimensions( int nDims, int *dimArray )
{
	int		i;

	// Verify integrity
	if( nDims < 0 || nDims > kMaxDims || dimArray == NULL ) return FALSE;

	for( i = 0; i < nDims; i++ )
		if( dimArray[i] < 0 || dimArray[i] > m_CurDataSize )
			return FALSE;

	m_NDimensions = nDims;
	memcpy( m_Dimensions, dimArray, nDims * sizeof( int ) );

	return TRUE;
}

// Returns a pointer to an int array containing
// the size of each dimension (GetNDimensions)
int		*CChartData::GetDimensions( void )
{
	return m_Dimensions;
}

// Returns the number of dimensions in the data
int		CChartData::GetNDimensions( void )
{
	return	m_NDimensions;
}

