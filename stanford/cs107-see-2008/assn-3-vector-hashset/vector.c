#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h>


void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
  assert(elemSize > 0);
  assert(initialAllocation >= 0);

  if (initialAllocation == 0)
  {
    initialAllocation = 4;
  }
  v->elemSize = elemSize;
  v->elems = malloc(elemSize * initialAllocation);
  assert(v->elems != NULL);

  v->allocLength = initialAllocation;
  v->logLength = 0;
  v->freeFn = freeFn;
}


void VectorDispose(vector *v)
{
  int i;


  if (v->freeFn != NULL)
  {
    for (i = 0; i < v->logLength; i++)
    {
      v->freeFn((char *)v->elems + v->elemSize * i);
    }
  }
}


int VectorLength(const vector *v)
{
  return v->logLength;
}


void *VectorNth(const vector *v, int position)
{
  assert(position >= 0);
  assert(position < v->logLength);
  return (char *)v->elems + v->elemSize * position;
}


void VectorReplace(vector *v, const void *elemAddr, int position)
{
  assert(position >= 0);
  assert(position < v->logLength);
  if (v->freeFn != NULL)
  {
    v->freeFn((char *)v->elems + v->elemSize * position);
  }
  memcpy((char *)v->elems + v->elemSize * position, elemAddr, v->elemSize);
}


void VectorInsert(vector *v, const void *elemAddr, int position)
{
  assert(position >= 0);
  assert(position <= v->logLength);
  if (v->logLength == v->allocLength)
  {
    v->elems = realloc(v->elems, v->elemSize * v->allocLength * 2);
    assert(v->elems != NULL);
    v->allocLength *= 2;
  }
  memmove((char *)v->elems + v->elemSize * (position + 1), (char *)v->elems + v->elemSize * position, v->elemSize * (v->logLength - position));
  memcpy((char *)v->elems + v->elemSize * position, elemAddr, v->elemSize);
  v->logLength++;
}


void VectorAppend(vector *v, const void *elemAddr)
{
  if (v->logLength == v->allocLength)
  {
    v->elems = realloc(v->elems, v->elemSize * v->allocLength * 2);
    assert(v->elems != NULL);
    v->allocLength *= 2;
  }
  memcpy((char *)v->elems + v->elemSize * v->logLength, elemAddr, v->elemSize);
  v->logLength++;
}

void VectorDelete(vector *v, int position)
{
  assert(position >= 0);
  assert(position < v->logLength);
  if (v->freeFn != NULL)
  {
    v->freeFn((char *)v->elems + v->elemSize * position);
  }
  memmove((char *)v->elems + v->elemSize * position, (char *)v->elems + v->elemSize * (position + 1), v->elemSize * (v->logLength - position - 1));
  v->logLength--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
  assert(compare != NULL);
  qsort(v->elems, v->logLength, v->elemSize, compare);
}


void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
  int i;


  assert(mapFn != NULL);
  for (i = 0; i < v->logLength; i++)
  {
    mapFn((char *)v->elems + v->elemSize * i, auxData);
  }
}


static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{
  void *pKeyFound = NULL;


  assert(startIndex >= 0);
  assert(startIndex <= v->logLength);
  assert(key != NULL);
  assert(searchFn != NULL);

  if (isSorted)
  {
    if ((pKeyFound = bsearch(key,
                             (char *)v->elems + v->elemSize * startIndex,
                             v->logLength - startIndex,
                             v->elemSize,
                             searchFn)) != NULL)
    {
      return ((char *)pKeyFound - (char *)v->elems) / v->elemSize;
    }
    else
    {
      return -1;
    }
  }
  else
  {
    size_t numElems = v->logLength - startIndex;
    if ((pKeyFound = lfind(key,
                           (char *)v->elems + v->elemSize * startIndex,
                           &numElems,
                           v->elemSize,
                           searchFn)) != NULL)
    {
      return ((char *)pKeyFound - (char *)v->elems) / v->elemSize;
    }
    else
    {
      return -1;
    }
  }
}

