#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
  v->elemSize = elemSize;
  v->freeFn = freeFn;
  v->allocLength = initialAllocation;
  v->curLength = 0;

  v->elemMemory = malloc(initialAllocation*elemSize);
  assert(v-> elemMemory != NULL);
}

void VectorDispose(vector *v)
{
  if(v->freeFn)
  {
    for(int i = 0; i < v->curLength; i++)
    {
      v-> freeFn(VectorNth(v,i));
    }
  }
}

int VectorLength(const vector *v)
{ return v->curLength; }

void *VectorNth(const vector *v, int position)
{ 
  assert(position >= 0 && position <= v->curLength);
  char* charPtr = (char*) v->elemMemory + position * v-> elemSize;
  return (void*) charPtr; 
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
  assert( position >= 0 && position < v->curLength);
  void* MemLocation = VectorNth(v, position);
  if (v-> freeFn)
  {
    v-> freeFn(MemLocation);
  }
  memcpy(MemLocation, elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
  assert(position >= 0 && position <= v->curLength);

  if( v->curLength + 1 > v->allocLength)
  {
    v->elemMemory = realloc(v->elemMemory, 2 * v->allocLength * v->elemSize);
    assert(v->elemMemory != NULL);
    v->allocLength *=2;
  }

  void* MemLocation = VectorNth(v, position);
  char* NewLocation = (char*) MemLocation +  v-> elemSize;
  memmove( (void*) NewLocation, MemLocation, (v->curLength - position) *v->elemSize);
  
  v->curLength++;
  memcpy(MemLocation, elemAddr, v->elemSize);
}

void VectorAppend(vector *v, const void *elemAddr)
{
  if( v->curLength + 1 > v->allocLength)
  {
    v->elemMemory = realloc(v->elemMemory, (v->allocLength) * (v->elemSize) * 2);
    assert(v->elemMemory != NULL);
    v->allocLength *=2;
  }

  v->curLength++;
  void* MemLocation = VectorNth(v, v->curLength -1);
  memcpy(MemLocation, elemAddr, v->elemSize);

}

void VectorDelete(vector *v, int position)
{
  assert(position >= 0 && position <= v->curLength);
  void* MemLocation = VectorNth(v, position);
  if(v->freeFn){
    v-> freeFn(MemLocation);
  }

  void* ShiftLocation = VectorNth(v, position + 1);
  memmove(MemLocation, ShiftLocation, (v->curLength - (position + 1)) * v->elemSize);
  v->curLength--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
  qsort(v->elemMemory, v->curLength, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
  for(int i= 0; i < v-> curLength; i++)
    mapFn(VectorNth(v,i), auxData);
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{
  assert(startIndex >= 0 && startIndex <= v->curLength);
  if(isSorted){
    void* res = bsearch(key, VectorNth(v, startIndex), v->curLength - startIndex, v->elemSize, searchFn);
    if (res != NULL){
      return ((char*) res - (char*)v->elemMemory)/ v->elemSize;
    }
  }
  else{
    for(int i=0; i< v->curLength; i++){
      if( searchFn(VectorNth(v,i), key) == 0){
        return i;
      }
    }
  }
  return kNotFound;
} 
