#pragma once

#include <array>

template<class T>
struct array1D
{
	array1D<T>( const std::array<unsigned, 1> &auDim );
	~array1D<T>( void );
	array1D<T>( const array1D<T> &other );

	inline T &operator[]( int iIndex ) const { return data[ iIndex ]; }

	T *data;
	std::array<unsigned, 1> auDim;
};

template<class T>
struct array2D
{
	array2D<T>( const std::array<unsigned, 2> &auDim );
	~array2D<T>( void );
	array2D<T>( const array2D<T> &other ) = delete;

	inline T &*operator[]( int iIndex ) { return data[ iIndex ]; }

	T **data;
	std::array<unsigned, 2> auDim;
};

template<class T>
struct array3D
{
	array3D<T>( const std::array<unsigned, 3> &auDim );
	~array3D<T>( void );
	array3D<T>( const array3D<T> &other ) = delete;

	inline T &**operator[]( int iIndex ) { return data[ iIndex ]; }

	T ***data;
	std::array<unsigned, 3> auDim;
};

template<class T>
struct array4D
{
	array4D<T>( const std::array<int, 4> &auDim );
	~array4D<T>( void );
	array4D<T>( const array4D<T> &other ) = delete;

	inline T &***operator[]( int iIndex ) { return data[ iIndex ]; }

	T ****data;
	std::array<int, 4> auDim;
};

template<class T>
array1D<T>::array1D( const std::array<int, 1> &auDim ) :
	auDim( auDim )
{
	data = new T[ auDim[ 0 ] ];

	//std::cout << "Array start address: 0x" << arr << std::endl; // Array address for debug
	for( int i = 0; i < auDim[ 0 ]; i++ )
		data[ i ] = 0;
}

template<class T>
array1D<T>::~array1D( void )
{
	delete[ ] data;
}

template<class T>
array1D<T>::array1D( const array1D<T> &other ) :
	auDim( other.auDim )
{
	data = new T[ auDim[ 0 ] ];

	for( int i = 0; i < auDim[ 0 ]; i++ )
		data[ i ] = other[ i ];
}

template<class T>
array2D<T>::array2D( const std::array<int, 2> &auDim ) :
	auDim( auDim )
{
	data = new T *[ auDim[ 0 ] ];
	for( int i = 0; i < auDim[ 0 ]; i++ )
		data[ i ] = new T[ auDim[ 1 ] ];

	//std::cout << "Array start address: 0x" << arr << std::endl; // Array address for debug
	for( int i = 0; i < auDim[ 0 ]; i++ )
		for( int j = 0; j < auDim[ 1 ]; j++ )
			data[ i ][ j ] = 0;
}

template<class T>
array2D<T>::~array2D( void )
{
	for( int i = 0; i < auDim[ 0 ]; i++ )
	{
		delete[ ] data[ i ];
	}
	delete[ ] data;
}

template<class T>
array2D<T>::array2D( const array2D<T> &other ) :
	auDim( other.auDim )
{
	data = new T *[ auDim[ 0 ] ];
	for( int i = 0; i < auDim[ 0 ]; i++ )
		data[ i ] = new T[ auDim[ 1 ] ];

	for( int i = 0; i < auDim[ 0 ]; i++ )
		for( int j = 0; j < auDim[ 1 ]; j++ )
			data[ i ][ j ] = other[ i ][ j ];
}

template<class T>
array3D<T>::array3D( const std::array<int, 3> &auDim ) :
	auDim( auDim )
{
	data = new T **[ auDim[ 0 ] ];
	for( int i = 0; i < auDim[ 0 ]; i++ )
	{
		data[ i ] = new T *[ auDim[ 1 ] ];
		for( int j = 0; j < auDim[ 1 ]; j++ )
			data[ i ][ j ] = new T[ auDim[ 2 ] ];
	}

	//std::cout << "Array start address: 0x" << arr << std::endl; // Array address for debug
	for( int i = 0; i < auDim[ 0 ]; i++ )
		for( int j = 0; j < auDim[ 1 ]; j++ )
			for( int k = 0; k < auDim[ 2 ]; k++ )
				data[ i ][ j ][ k ] = 0;
}

template<class T>
array3D<T>::~array3D( void )
{
	for( int i = 0; i < auDim[ 0 ]; i++ )
	{
		for( int j = 0; j < auDim[ 1 ]; j++ )
		{
			delete[ ] data[ i ][ j ];
		}
		delete[ ] data[ i ];
	}
	delete[ ] data;
}

template<class T>
array3D<T>::array3D( const array3D<T> &other ) :
	auDim( other.auDim )
{
	data = new T **[ auDim[ 0 ] ];
	for( int i = 0; i < auDim[ 0 ]; i++ )
	{
		data[ i ] = new T *[ auDim[ 1 ] ];
		for( int j = 0; j < auDim[ 1 ]; j++ )
			data[ i ][ j ] = new T[ auDim[ 2 ] ];
	}

	for( int i = 0; i < auDim[ 0 ]; i++ )
		for( int j = 0; j < auDim[ 1 ]; j++ )
			for( int k = 0; k < auDim[ 2 ]; k++ )
				data[ i ][ j ][ k ] = other[ i ][ j ][ k ];
}

template<class T>
array4D<T>::array4D( const std::array<int, 4> &auDim ) :
	auDim( auDim )
{
	data = new T ***[ auDim[ 0 ] ];
	for( int i = 0; i < auDim[ 0 ]; i++ )
	{
		data[ i ] = new T**[ auDim[ 1 ] ];
		for( int j = 0; j < auDim[ 1 ]; j++ )
		{
			data[ i ][ j ] = new T*[ auDim[ 2 ] ];
			for( int k = 0; k < auDim[ 2 ]; k++ )
				data[ i ][ j ][ k ] = new T[ auDim[ 3 ] ];
		}
	}

	//std::cout << "Array start address: 0x" << arr << std::endl; // Array address for debug
	for( int i = 0; i < auDim[ 0 ]; i++ )
		for( int j = 0; j < auDim[ 1 ]; j++ )
			for( int k = 0; k < auDim[ 2 ]; k++ )
				for( int l = 0; l < auDim[ 3 ]; l++ )
					data[ i ][ j ][ k ][ l ] = 0;
}

template<class T>
array4D<T>::~array4D( void )
{
	for( int i = 0; i < auDim[ 0 ]; i++ )
	{
		for( int j = 0; j < auDim[ 1 ]; j++ )
		{
			for( int k = 0; k < auDim[ 2 ]; k++ )
			{
				delete[ ] data[ i ][ j ][ k ];

			}
			delete[ ] data[ i ][ j ];
		}
		delete[ ] data[ i ];
	}
	delete[ ] data;
}

template<class T>
array3D<T>::array3D( const array3D<T> &other ) :
	auDim( other.auDim )
{
	data = new T ***[ auDim[ 0 ] ];
	for( int i = 0; i < auDim[ 0 ]; i++ )
	{
		data[ i ] = new T**[ auDim[ 1 ] ];
		for( int j = 0; j < auDim[ 1 ]; j++ )
		{
			data[ i ][ j ] = new T*[ auDim[ 2 ] ];
			for( int k = 0; k < auDim[ 2 ]; k++ )
				data[ i ][ j ][ k ] = new T[ auDim[ 3 ] ];
		}
	}

	for( int i = 0; i < auDim[ 0 ]; i++ )
		for( int j = 0; j < auDim[ 1 ]; j++ )
			for( int k = 0; k < auDim[ 2 ]; k++ )
				for( int l = 0; l < auDim[ 3 ]; l++ )
					data[ i ][ j ][ k ][ l ] = other[ i ][ j ][ k ][ l ];
}