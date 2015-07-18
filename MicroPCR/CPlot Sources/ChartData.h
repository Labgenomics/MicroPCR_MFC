#pragma once

#define		kMaxDims		3

class CChartData : public CObject
{
public:
	DECLARE_SERIAL( CChartData )

	double		*m_Data;		// the data (linear storage)
	int			m_NDimensions;	// number of dimensions in data
	int			m_Dimensions[kMaxDims];	// size of each dimension

private:
	long		m_CurDataSize;

public:
	// Functions

	CChartData();
	~CChartData();
	virtual	void Serialize(CArchive& ar);

	double	*GetData( void );
	BOOL	SetDimensions( int nDims, int *dimArray );
	int		*GetDimensions( void );
	int		GetNDimensions( void );
	void	DeleteData( void );

	// We template this function so we can call it with any sort
	// of starting data type
template <class T>
BOOL	SetData( T *data, int nDims, int *dimArray )
{
	long			i;
	// Verify integrity
	if( data == NULL || nDims < 0 || nDims > kMaxDims || dimArray == NULL ) return FALSE;

	for( i = 0; i < nDims; i++ )
	{
		if( dimArray[i] < 0 ) return FALSE;
	}

	// Set up the dimensions
	m_CurDataSize = 1;
	for( i = 0; i < nDims; i++ )
	{
		m_CurDataSize *= dimArray[i];
		m_Dimensions[i] = dimArray[i];
	}

	m_NDimensions = nDims;

	// Get rid of old data
	if( m_Data != NULL )
	{
		VirtualFree( m_Data, 0, MEM_RELEASE );
		m_Data = NULL;
	}

	// Allocate memory for the data
	m_Data = (double *)VirtualAlloc( NULL, m_CurDataSize * sizeof( double ),
		MEM_COMMIT, PAGE_READWRITE );

	if( m_Data == NULL )
		goto resetData;

	// Copy the data
	for( i = 0; i < m_CurDataSize; i++ )
		m_Data[i] = (double)data[i];

	return TRUE;

resetData:
	m_Data = NULL;
	m_NDimensions = 0;
	memset( m_Dimensions, 0, kMaxDims * sizeof( int ) );

	return FALSE;
}
};