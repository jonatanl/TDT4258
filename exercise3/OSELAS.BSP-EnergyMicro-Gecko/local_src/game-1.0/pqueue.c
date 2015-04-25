/* The MIT License (MIT)
   
   Copyright (c) 2014-2015 Audun Sutterud
   
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE. */
#include <stdlib.h>

#define INITIAL_SIZE 15


struct pqueue {
	int size;
	int sizelimit;
	void** tree;
	int (*compare)(void*, void*);
};


struct pqueue* pqueue_new(int (*compare)(void*, void*))
{
	struct pqueue* pqueue = (struct pqueue*) malloc(sizeof(struct pqueue));
	pqueue->size = 0;
	pqueue->sizelimit = INITIAL_SIZE;
	pqueue->tree = (void**) malloc(sizeof(void*) * (pqueue->sizelimit + 1));
	pqueue->compare = compare;
	return pqueue;
}


void pqueue_destroy(struct pqueue* pqueue)
{
	free(pqueue->tree);
	free(pqueue);
}


void pqueue_insert(struct pqueue* pqueue, void* element)
{
	// increase the max queue size
	if(pqueue->size == pqueue->sizelimit){
		pqueue->sizelimit *= 2;
		pqueue->sizelimit += 1;
		pqueue->tree = (void**) realloc(pqueue->tree, sizeof(void*) * (pqueue->sizelimit + 1));
	}
	
	// increment the queue size
	pqueue->size += 1;

	// insert the new element
	pqueue->tree[pqueue->size] = element;

	// swap it upwards
	int i = pqueue->size;
	void* temp;
	void** tree = pqueue->tree;
	while(i > 1 && pqueue->compare(tree[i/2], tree[i]) > 0){
		temp = tree[i];
		tree[i] = tree[i/2];
		tree[i/2] = temp;
		i /= 2;
	}
}


void* pqueue_poll(struct pqueue* pqueue)
{
	// ASSERT(pqueue->size > 0)
	
	// poll best node
	void* result = pqueue->tree[1];

	// poll last node
	void* last = pqueue->tree[pqueue->size];
	pqueue->size -= 1;

	// move hole downwards, comparing agains last
	int i = 1, done = 0;
	void** tree = pqueue->tree;
	while((i*2 + 1) <= pqueue->size && !done){

		// pick the smallest child
		int smallest;
	   	if(pqueue->compare(tree[i*2], tree[i*2 + 1]) < 0){
			smallest = i*2;
		}else{
			smallest = i*2 + 1;
		}
		// swap if larger than smallest child
		if(pqueue->compare(last, tree[smallest]) > 0){
			tree[i] = tree[smallest];
			i = smallest;
		}else{
			done = 1;
		}
	}

	// special check for nodes with one branch
	if(!done && i*2 < pqueue->size && pqueue->compare(last, tree[i*2]) > 0){
		tree[i]	= tree[i*2];
		i *= 2;
	}

	tree[i] = last;
	return result;
}


int pqueue_size(struct pqueue* pqueue)
{
	return pqueue->size;
}


int pqueue_empty(struct pqueue* pqueue)
{
	return pqueue->size == 0;
}
