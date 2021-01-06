#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{

  h->elemSize = elemSize;
  h->numBuckets = numBuckets;
  h->hashfn = hashfn;
  h->comparefn = comparefn;
  h->freefn = freefn;
  h->size = 0;
  h->buckets = malloc(numBuckets * sizeof(vector));


  assert(elemSize > 0);
  assert(numBuckets > 0);
  assert(hashfn != NULL);
  assert(comparefn != NULL);

  for (int i=0; i < numBuckets; i++){
    vector newVector;
    VectorNew(&newVector, h->elemSize, freefn, 1);
    vector *bucket = h->buckets + i;
    *bucket = newVector;
  }


}

void HashSetDispose(hashset *h)
{
  for(int i=0; i< h->numBuckets; i++){
    VectorDispose(h->buckets + i);
  }
}

int HashSetCount(const hashset *h)
{ return h->size; }

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
  for(int i=0; i< h->numBuckets; i++){
    VectorMap(h->buckets + i, mapfn, auxData);
  }
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
  assert(elemAddr != NULL);

  int index = h->hashfn(elemAddr, h->numBuckets);
  assert(index >= 0 && index < h->numBuckets);
  vector* bucket = h->buckets + index;

  int VIndex = VectorSearch(bucket, elemAddr, h->comparefn, 0, false);
  if( VIndex == -1){
    VectorAppend(bucket, elemAddr);
    h->size++;
  }
  else{
    VectorReplace(bucket, elemAddr, VIndex);
  }

}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{ 
  assert( elemAddr != NULL);

  int index = h->hashfn(elemAddr, h->numBuckets);
  assert(index >= 0 && index < h->numBuckets);
  vector* bucket = h->buckets + index;

  int VIndex = VectorSearch(bucket, elemAddr, h->comparefn, 0, false);
  
  if( VIndex == -1){
    return NULL; 
  }
  return VectorNth(bucket,VIndex);
  
}
