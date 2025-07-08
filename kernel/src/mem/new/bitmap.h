#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#ifndef BITMAP_H
#define BITMAP_H

typedef struct DS_Bitmap {
  uint8_t *Bitmap;
  size_t   BitmapSizeInBlocks; // CEIL(x / BLOCK_SIZE)
  size_t   BitmapSizeInBytes;  // CEIL(blockSize / 8)

  size_t allocatedSizeInBlocks;

  // optimization(1): For (de/)allocations know to what extend our
  // bitmap is fully fragmented so it doesn't scan over it over and over and
  // over again.. This is especially useful for single-block allocations (such
  // as pageframes), typically done in a loop.
  size_t lastDeepFragmented;

  size_t mem_start;
  bool   ready; // has been initiated
} DS_Bitmap;

#define BLOCKS_PER_BYTE 8 // using uint8_t
#define BLOCK_SIZE 4096
#define INVALID_BLOCK ((size_t)-1)

void  *ToPtr(DS_Bitmap *bitmap, size_t block);
size_t ToBlock(DS_Bitmap *bitmap, void *ptr);
size_t ToBlockRoundUp(DS_Bitmap *bitmap, void *ptr);

size_t BitmapCalculateSize(size_t totalSize);
int    BitmapGet(DS_Bitmap *bitmap, size_t block);
void   BitmapSet(DS_Bitmap *bitmap, size_t block, bool value);

void BitmapDump(DS_Bitmap *bitmap);
void BitmapDumpBlocks(DS_Bitmap *bitmap);

void MarkBlocks(DS_Bitmap *bitmap, size_t start, size_t size, bool val);
void MarkRegion(DS_Bitmap *bitmap, void *basePtr, size_t sizeBytes, int isUsed);
size_t FindFreeRegion(DS_Bitmap *bitmap, size_t blocks);
void  *BitmapAllocate(DS_Bitmap *bitmap, size_t blocks);

size_t BitmapAllocatePageframe(DS_Bitmap *bitmap);
void BitmapFree(DS_Bitmap *bitmap, void *base, size_t blocks);
void   BitmapFreePageframe(DS_Bitmap *bitmap, void *addr);

#endif