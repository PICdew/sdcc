/*-------------------------------------------------------------------------
   realloc.c - allocate memory.

   Copyright (C) 2015, Philipp Klaus Krause, pkk@spth.de

   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License 
   along with this library; see the file COPYING. If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.

   As a special exception, if you link this library with other files,
   some of which are compiled with SDCC, to produce an executable,
   this library does not by itself cause the resulting executable to
   be covered by the GNU General Public License. This exception does
   not however invalidate any other reasons why the executable file
   might be covered by the GNU General Public License.
-------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#if defined(__SDCC_mcs51) || defined(__SDCC_ds390) || defined(__SDCC_ds400)
#define XDATA __xdata
#else
#define XDATA
#endif

struct header
{
	struct header XDATA *next;
	struct header XDATA *next_free; // Next free block. Used in free blocks only.
};

extern struct header XDATA *__sdcc_heap_free;

void _sdcc_heap_init(void);

void XDATA *realloc(void *ptr, size_t size)
{
	void XDATA *ret;
	struct header XDATA *h, *next_free, *prev_free;
	struct header XDATA **f, **pf;
	size_t blocksize, oldblocksize, maxblocksize;

	if(!__sdcc_heap_free)
		_sdcc_heap_init();

	if(!ptr)
		return(malloc(size));

	if(!size)
	{
		free(ptr);
		return(0);
	}

	prev_free = 0, pf = 0;
	for(h = __sdcc_heap_free, f = &__sdcc_heap_free; h < ptr; prev_free = *f, pf = f, f = &(h->next_free), h = h->next_free);
	next_free = *f;

	blocksize = size + offsetof(struct header, next_free);
	if(size < sizeof(struct header)) // Requiring a minimum size makes it easier to implement free(), and avoid memory leaks.
		size = sizeof(struct header);

	h = (void XDATA *)((char XDATA *)(ptr) - offsetof(struct header, next_free));
	oldblocksize = (char XDATA *)(h->next) - (char XDATA *)h;

	maxblocksize = oldblocksize;
	if(prev_free && prev_free->next == h) // Can merge with previous block
		maxblocksize += (char XDATA *)h - (char XDATA *)prev_free;
	if(next_free == h->next) // Can merge with next block
		maxblocksize += (char XDATA *)(next_free->next) - (char XDATA *)next_free;

	if(blocksize <= maxblocksize) // Can resize in place.
	{
		if(prev_free && prev_free->next == h) // Always move into previous block to defragment
		{
			memmove(prev_free, h, blocksize <= oldblocksize ? blocksize : oldblocksize);
			h = prev_free;
			*pf = next_free;
			f = pf;
		}

		if(next_free == h->next) // Merge with following block
		{
			h->next = next_free->next;
			*f = next_free->next_free;
		}

		if(maxblocksize >= blocksize + sizeof(struct header)) // Create new block from free space
		{
			struct header XDATA *const newheader = (struct header XDATA *const)((char XDATA *)h + blocksize);
			newheader->next = h->next;
			newheader->next_free = (*f)->next_free;
			*f = newheader;
			h->next = newheader;
		}

		return(&(h->next_free));
	}

	if(ret = malloc(size))
	{
		size_t oldsize = oldblocksize - offsetof(struct header, next_free);
		memcpy(ret, ptr, size <= oldsize ? size : oldsize);
		free(ptr);
		return(ret);
	}

	return(0);
}

