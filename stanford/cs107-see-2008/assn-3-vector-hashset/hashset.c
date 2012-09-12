#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
                HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
  int i;


  assert(elemSize > 0);
  assert(numBuckets > 0);
  assert(hashfn != NULL);
  assert(comparefn != NULL);
  h->elemSize = elemSize;
  h->numBuckets = numBuckets;
  h->hashfn = hashfn;
  h->comparefn = comparefn;
  h->freefn = freefn;
  h->elemNum = 0;
  h->elems = malloc(sizeof(vector) * numBuckets);
  assert(h->elems != NULL);
  for (i = 0; i < numBuckets; i++)
  {
    void *vector_address = (char *)h->elems + i * sizeof(vector);
    VectorNew(vector_address, elemSize, freefn, 4);
  }
}


void HashSetDispose(hashset *h)
{
  int i;


  if (h->freefn != NULL)
  {
    for (i = 0; i < h->numBuckets; i++)
    {
      VectorDispose((vector *)((char *)h->elems + i * sizeof(vector)));
    }
  }
  free(h->elems);
}


int HashSetCount(const hashset *h)
{
  return h->elemNum;
}


void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
  int i;


  assert(mapfn != NULL);
  for (i = 0; i < h->numBuckets; i++)
  {
    VectorMap((vector *)((char *)h->elems + i * sizeof(vector)), mapfn, auxData);
  }
}


void HashSetEnter(hashset *h, const void *elemAddr)
{
  int hash_value;
  int position;


  assert(elemAddr != NULL);
  hash_value = h->hashfn(elemAddr, h->numBuckets);
  assert((hash_value >= 0) && (hash_value < h->numBuckets));
  if ((position = VectorSearch((vector *)((char *)h->elems + hash_value * sizeof(vector)), elemAddr, h->comparefn, 0, false)) == -1)
  {
    VectorInsert((vector *)((char *)h->elems + hash_value * sizeof(vector)), elemAddr, 0);
    h->elemNum++;
  }
  else
  {
    VectorReplace((vector *)((char *)h->elems + hash_value * sizeof(vector)), elemAddr, position);
  }
}


void *HashSetLookup(const hashset *h, const void *elemAddr)
{
  int hash_value;
  int position;


  assert(elemAddr != NULL);
  hash_value = h->hashfn(elemAddr, h->numBuckets);
  assert((hash_value >= 0) && (hash_value < h->numBuckets));
  if ((position = VectorSearch((vector *)((char *)h->elems + hash_value * sizeof(vector)), elemAddr, h->comparefn, 0, false)) == -1)
  {
    return NULL;
  }
  else
  {
    return VectorNth((const vector *)((char *)h->elems + hash_value * sizeof(vector)), position);
  }
}
