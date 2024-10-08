/*
Copyright (c) 2006, Michael Kazhdan and Matthew Bolitho
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution. 

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/

template< class Real > Real Random( void ){ return Real( rand() )/Real( RAND_MAX ); }

template< class Real , int Dim >
Point< Real , Dim > RandomBallPoint( void )
{
	Point< Real , Dim > p;
	while(1)
	{
		for( int d=0 ; d<Dim ; d++ ) p[d] = Real( 1.0-2.0*Random< Real >() );
		double l=SquareLength(p);
		if( SquareLength( p )<=1 ) return p;
	}
}
template< class Real , int Dim >
Point< Real , Dim > RandomSpherePoint( void )
{
	Point< Real , Dim > p = RandomBallPoint< Real , Dim >();
	return p / (Real)Length( p );
}

/////////////
// Simplex //
/////////////
template< class Real , unsigned int Dim , unsigned int K >
void Simplex< Real , Dim , K >::split( Point< Real , Dim > pNormal , Real pOffset , std::vector< Simplex >& back , std::vector< Simplex >& front ) const
{
	Real values[K+1];
	bool frontSet = false , backSet = false;

	// Evaluate the hyper-plane's function at the vertices and mark if strictly front/back vertices have been found
	for( unsigned int k=0 ; k<=K ; k++ )
	{
		values[k] = Point< Real , Dim >::Dot( p[k] , pNormal ) - pOffset;
		backSet |= ( values[k]<0 ) , frontSet |= ( values[k]>0 );
	}

	// If all the vertices are behind or on, or all the vertices are in front or on, we are done.
	if( !frontSet ){ back.push_back( *this ) ; return; }
	if( !backSet ){ front.push_back( *this ) ; return; }

	// Pick some intersection of the hyper-plane with a simplex edge
	unsigned int v1 , v2;
	Point< Real , Dim > midPoint;
	{
		for( unsigned int i=0 ; i<K ; i++ ) for( unsigned int j=i+1 ; j<=K ; j++ ) if( values[i]*values[j]<0 )
		{
			v1 = i , v2 = j;
			Real t1 = values[i] / ( values[i] - values[j] ) , t2 = (Real)( 1. - t1 );
			midPoint = p[j]*t1 + p[i]*t2;
		}
	}
	// Iterate over each face of the simplex, split it with the hyper-plane and connect the sub-simplices to the mid-point
	for( unsigned int i=0 ; i<=K ; i++ )
	{
		if( i!=v1 && i!=v2 ) continue;
		Simplex< Real , Dim , K-1 > f;		// The face
		Simplex< Real , Dim , K > s;		// The sub-simplex
		for( unsigned int j=0 , idx=0 ; j<=K ; j++ )	if( j!=i ) f[idx++] = p[j];
		std::vector< Simplex< Real , Dim , K-1 > > _back , _front;
		f.split( pNormal , pOffset , _back , _front );
		s[i] = midPoint;

		for( unsigned int j=0 ; j<_back.size() ; j++ ) 
		{
			for( unsigned int k=0 ; k<K ; k++ ) s[ k<i ? k : k+1 ] = _back[j][k];
			back.push_back( s );
		}

		for( unsigned int j=0 ; j<_front.size() ; j++ ) 
		{
			for( unsigned int k=0 ; k<K ; k++ ) s[ k<i ? k : k+1 ] = _front[j][k];
			front.push_back( s );
		}
	}
}