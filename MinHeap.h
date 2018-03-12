#ifndef MinHeap_h
#define MinHeap_h

typedef int (*Comparator)( const void *, const void * );

typedef struct MinHeap {
	Comparator	comparator;
	void *		block;		// extensible block, moveable.
	void *		temp;		// temp space for swap()
	unsigned int	elemsize;
	unsigned int	elemcount;
} MinHeap;

int	initializeMinHeap( MinHeap *, Comparator, unsigned int elemsize );

int	addElement( MinHeap *, void * );

void *	removeMin( MinHeap *, void * buffer );

void *	getMin( MinHeap * );

void	destroyMinHeap( MinHeap * );

#endif
