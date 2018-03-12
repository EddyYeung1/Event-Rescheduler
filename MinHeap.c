#include	"MinHeap.h"
#include	<stdlib.h>
#include	<string.h>
#include	<stdio.h>

int
initializeMinHeap( MinHeap * self, Comparator comparator, unsigned int elemsize )
{
	if ( self == 0 )
	{
		fprintf( stderr, "\x1b[2;31mzero arg passed to initializeMinHeap() file %s line %d\x1b[0m\n",
			__FILE__, __LINE__ );
		return ~0;
	}
	else if ( (self->temp = malloc( elemsize )) == 0 )
	{
		fprintf( stderr, "\x1b[2;31mmalloc() failed in initializeMinHeap() file %s line %d\x1b[0m\n",
			__FILE__, __LINE__ );
		return ~0;
	}
	else
	{
		self->comparator = comparator;
		self->block = 0;
		self->elemsize = elemsize;
		self->elemcount = 0;
		return 0;
	}
}

static void
swap( MinHeap * self, unsigned int i, unsigned int j )
{
	unsigned int		elemsize = self->elemsize;
	unsigned char *		block = self->block;
	unsigned char *		temp = self->temp;

	memcpy( temp, block + i * elemsize, elemsize );
	memcpy( block + i * elemsize, block + j * elemsize, elemsize );
	memcpy( block + j * elemsize, temp, elemsize );
}

static void
siftUp( MinHeap * self, int pos )
{
	int			i, parent;
	unsigned int		elemsize = self->elemsize;
	Comparator		comparator = self->comparator;
	unsigned char *		block = self->block;

	i = pos;
	while ( i > 0 )
	{
		parent = ( i - 1 )/2;
		if ( comparator( block + parent * elemsize, block + i * elemsize ) > 0 )
		{
			swap( self, parent, i );
		}
		else
		{
			break;
		}
		i = parent;
	}
}

static void
siftDown( MinHeap * self, int lo, int hi )
{
	int			i, child, rchild;
	unsigned int		elemsize = self->elemsize;
	Comparator		comparator = self->comparator;
	unsigned char *		block = self->block;

	i = lo;
	child = 2*i + 1;
	while ( child <= hi )
	{
		rchild = child + 1;
		if ( rchild <= hi && comparator( block + child * elemsize, block + rchild * elemsize ) > 0 )
		{
			child = rchild;
		}
		if ( comparator( block + i * elemsize, block + child * elemsize ) > 0 )
		{
			swap( self, i, child );
		}
		else
		{
			break;
		}
		i = child;
		child = 2*i + 1;
	}
}

int
addElement( MinHeap * self, void * element )
{
	unsigned int		elemsize = self->elemsize;

	self->elemcount += 1;
	self->block = realloc( self->block, self->elemcount * elemsize );
	memcpy( self->block + (self->elemcount - 1) * elemsize, element, elemsize );
	siftUp( self, self->elemcount - 1 );
	return 0;
}

void *
removeMin( MinHeap * self, void * buffer )
{
	unsigned char *		block = self->block;
	unsigned int		elemcount = self->elemcount;
	unsigned int		elemsize = self->elemsize;

	if ( block == 0 )
	{
		return 0;
	}
	else
	{
		swap( self, 0, elemcount - 1 );
		siftDown( self, 0, elemcount - 2 );
		memcpy( buffer, block + (elemcount - 1) * elemsize, elemsize );
		self->elemcount -= 1;
		self->block = realloc( block, self->elemcount * elemsize );
		return buffer;
	}
}

void *
getMin( MinHeap * self )
{
	if ( self == 0 )
		return 0;
	else
		return self->block;
}

void
destroyMinHeap( MinHeap * self )
{
	if ( self == 0 )
	{
		fprintf( stderr, "\x1b[2;31mzero arg passed to destroyMinHeap() file %s line %d\x1b[0m\n",
			__FILE__, __LINE__ );
		return;
	}
	else
	{
		if ( self->block != 0 ) free( self->block );
		self->block = 0;
		self->elemcount = 0;
		if ( self->temp != 0 ) free( self->temp );
		self->temp = 0;
	}
}
