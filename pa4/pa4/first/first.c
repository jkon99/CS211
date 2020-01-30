#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/* CACHE SIMULATOR ASSIGNMENT
check pa4 pdf and TA slide from recitation
understand cache from videos and slides, write down trace to see how it all works
argv[1] = cachesize, power of 2, total size of cache in bytes
argv[2] = blocksize, power of 2 integer that specifies the size of the cache blocks in bytes
argv[3] = cache policy, valid cache is fifo and lru. EXTRA CREDIT is lru
argv[4] = associativity, either direct, assoc, or assoc:n
argv[5] = prefetch size, number of adjacent blocks that should be prefetched by the prefetcher in case of miss
argv[6] = trace file, name of input trace file
When started cache lines are empty, assume memory size is 2^48 and memory addresses are 48-bit,
number of bits in the tag, cache address, and byte address are determined by the cache size and block size.
Assume block is read from memory and then memory write for a write-through cache.
Do not need to simulate memory.
REQUIREMENTS
Simulate one level cache, cache and block size are specified in bytes, replacement algorithm is FIFO, simulate a write-through cache
// 2d array
 four counters
 reads per cache block
 memory writes per cache block
 cache hits
 cache misses
 */
 int reads;  //tjhese are for no prefetch
 int writes;  //can you initialize to values in global?
 int hits;
 int miss;
 int count;  //int or size_t
 char replacement; //fifo = f or lru = l
 //int checkhit;
 typedef struct cacheBlock {  //2d array structure? maybe fix parameters. cache line arrays. array of sets containing array of blocks. given by TA
   int valid;  //or unsigned int, size_t or int?
   size_t tag;  // or unsigned long int
   int time;  //or unsigned int
 }cacheBlock;  //typedef or just struct?? spit error without typedef because no name check if this fix es
 //use -lm flag in makefile?
 cacheBlock** cache;    //cache[1][0] means first block in set 1, cache[4][2] means the third blocks in set 4
//array of sets containing array of blocks
 //void doPrefetching(unsigned long int address, int prefetch, int blockSize);  //prefetching method
 //int checkHit();
 int checkHit(size_t tagi, int setIndex, int blocks, int p, int w);
 cacheBlock** allocate(int sets, int blocks);
 void freeCache(int sets);
 //make cache?

int main (int argc, char** argv) {

  if (argc < 7) {
    printf("error");
    return 0;
  }

  FILE *fp;
  fp = fopen(argv[6],"r");  //argv[6] should be the trace file
  if (fp==NULL) {  //check if there is a trace file and it's in right format
    printf("error");
    return 0;
  }
  int cacheSize = atoi(argv[1]);  //must be power of 2 otherwise print error, specified in bytes
  int blockSize = atoi(argv[2]);

  //int prefetchSize=0; //0 when no-prefetch
  //assume prefetch size is always >=1, so if it's not use 0 in argument for read/write
  if (((cacheSize !=0) && ((cacheSize &(cacheSize-1))!=0)) || ((blockSize !=0) && ((blockSize &(blockSize-1))!=0))) { //check if cache and block size are powers of 2
    printf("error");  //num is not power of 2 print error
    return 0;
  }
  int blocks;  //associavitiy
  int n;
  int sets;  //how many sets
  int setSize;
  size_t address; //address
  int prefetchSize;
  int setIndex;
  size_t tagi;
  size_t mask;
  size_t prefetchAddress;
  int sb;
  int bb;
  miss = 0;
  reads = 0;
  writes = 0;
  hits = 0;
  count = 0;
  int check;
  //sets and assoc are dimensions of array, sets and blocks
  if (strcmp(argv[3],"fifo") ==0) {  //check if cache policy is fifo
    //printf("not yet");
    //return 0;
    replacement = 'f';
  } else if (strcmp(argv[3],"lru") ==0) {  //only extra credit
    replacement = 'l';
  } else {
    printf("error");
    return 0;
  } //calculations given by TA
  if (strcmp(argv[4],"direct") ==0) { //check if it is direct
    blocks = 1;
    sets = cacheSize/blockSize;
  } else if (argv[4][5]==':') { //check if assoc:n
    sscanf(argv[4],"assoc:%d",&n);
    blocks =n;
    setSize = n * blockSize;
    sets = cacheSize/setSize;
  }  else {
    sets = 1;
    blocks = cacheSize/blockSize;//check if fully assoc
  }//check if above calculations are right
  //keep track of memory reads, writes, hits, misses for output
  //read file
  sb = log2(sets); //need to import math? sb is set bits. find way to do this check operations necessary
  bb = log2(blockSize);//block bits??  log2(blockSize) and log2(sets)
  mask = (1 << sb) -1; //need to declare?
  cache = allocate(sets,blocks);
    //unsigned long or unsigned long int?
  char mode; //R or W, so read or write
  int m;
  while ((fscanf(fp, "%c %zx\n", &mode,&address))==2) {  //keep going through until there is not 2 arguments or get to eof, maybe comine lines?
     //store character and address?  lx or zx for file?
    //methods
    if (mode == '#') { //if it's #eof end of file
      break;
    }
    setIndex = (address >> bb) & mask;  //long int or int?
    tagi = (address >> bb) >> sb; //long or long int? check if proper by TA
    if (mode == 'R') {
        //read method
        m=0;
        check = checkHit(tagi, setIndex, blocks, 0, m); //no prefetch so 0

    } else if (mode == 'W') {
      //write method
        m=1;
        check = checkHit(tagi, setIndex, blocks, 0, m); //no prefetch so 0
    }
  }  //put into cache array?
  //when done do this
  printf("no-prefetch\n");
  printf("Memory reads: %d\n", reads);
  printf("Memory writes: %d\n", writes);
  printf("Cache hits: %d\n", hits);
  printf("Cache misses: %d\n", miss);

  miss = 0;
  reads = 0;
  writes = 0;
  hits = 0;
  count = 0;
  freeCache(sets);
  fclose(fp);
  //do prefetch by redoing the restarting the file
  fp = fopen(argv[6],"r");  //argv[6] should be the trace file
  if (fp==NULL) {  //check if there is a trace file and it's in right format
    printf("error");
    return 0;
  }

  cacheSize = atoi(argv[1]);  //must be power of 2 otherwise print error, specified in bytes
  blockSize = atoi(argv[2]);
  prefetchSize = atoi(argv[5]); //only used for cache misses, none of the counters are updated, replacement status not updated
  //prefetchSize=prefetch;
  if (strcmp(argv[4],"direct") ==0) { //check if it is direct
    blocks = 1;
    sets = cacheSize/blockSize;
  } else if (argv[4][5]==':') { //check if assoc:n
    sscanf(argv[4],"assoc:%d",&n);
    blocks =n;
    setSize = n * blockSize;
    sets = cacheSize/setSize;
  }  else {
    sets = 1;
    blocks = cacheSize/blockSize;//check if fully assoc
  }//check if above calculations are right
  sb = log2(sets); //need to import math? sb is set bits. find way to do this check operations necessary
  bb = log2(blockSize);//block bits??  log2(blockSize) and log2(sets)
  mask = (1 << sb) -1; //need to declare?
  cache = allocate(sets,blocks); //reallocate

  while ((fscanf(fp, "%c %zx\n", &mode,&address))==2) {  //same as before, keep going through until there is not 2 arguments or get to eof, maybe comine lines?
 //need to include with prefetching
    //methods
    //doprefteching?
    if (mode == '#') { //if it's #eof end of file
      break;
    }

    check = 0;
    setIndex = (address >> bb) & mask;  //long int or int?
    tagi = (address >> bb) >> sb;
    if (mode=='R') {
      m=0;
      check = checkHit(tagi, setIndex, blocks, 0, m);
    } else if (mode=='W') {
      m=1;
      check = checkHit(tagi, setIndex, blocks, 0, m);
      //writes++;
      //check =1 inside or outside?
    }
    if (check==1) {
      continue;
    }
    //prefetchReads(tagi, setIndex, blocks, 0);
    prefetchAddress = address; //sort of given by TA, modified to fit implementation
    for (int p=1;p<=prefetchSize; p++) {  //<= or <

      prefetchAddress = prefetchAddress+blockSize;
      setIndex = (prefetchAddress >> bb) & mask;  //long int or int?
      tagi = (prefetchAddress >> bb) >> sb; //long or long int? check if proper by TA
      check = checkHit(tagi, setIndex, blocks, p, 0);
    }
  }

  printf("with-prefetch\n");
  printf("Memory reads: %d\n", reads);
  printf("Memory writes: %d\n", writes);
  printf("Cache hits: %d\n", hits);
  printf("Cache misses: %d\n", miss);

  freeCache(sets);
  fclose(fp);
  return 0;
}

cacheBlock** allocate(int sets, int blocks) { //looks good, works like part 3 in pa2

  cacheBlock** cache1 = malloc(sets*sizeof(cacheBlock*)); //declartion proper?
  for (int i=0;i<sets;i++) {
    cache1[i]=malloc(blocks*sizeof(cacheBlock));
  }
  for (int i =0;i<sets;i++) {
    for (int j=0;j<blocks;j++) {
      cache1[i][j].valid=0;
      cache1[i][j].tag=0;
      cache1[i][j].time=0;
    }
  }
  return cache1;
}

void freeCache(int sets) { //free memory
  for (int i =0; i < sets; i++) {
    free(cache[i]);
  }
  free(cache);
  return;
}

int checkHit(size_t tagi, int setIndex, int blocks, int p, int w) { // return hit status, update counters, write through cache

  int rp; //replacement
  for (int i=0;i<blocks;i++) {
   if (cache[setIndex][i].valid==0) { //pointer or dot?
      if (p<1) {
        miss++;  //not updated on prefetch
       if (w==1) {
         writes++;
       }
     }
     reads++;
     count++;
     cache[setIndex][i].valid=1; //insert
     cache[setIndex][i].tag=tagi;
     cache[setIndex][i].time=count;
     return 0;  //return 0 if not a hit
   } else {
     if (cache[setIndex][i].tag==tagi) { // have a cache hit because passed test of valid bit and tag match
         if (p<1) {
           hits++;
           if (w==1) {
             writes++;
           }
         }
         if (replacement == 'l') {
           cache[setIndex][i].time=count;
         }
         //check replacement method, lru also updatez on hits
         //count++;
         return 1; //return 1 if hit or should be a hit
       }
     if (i==(blocks-1)) { //full so must replace
       if (p<1) {
          miss++;  //not updated on condition
         if (w==1) {
           writes++;
         }
       }
       reads++;
       rp=0;
       for (int j=0;j<blocks;j++) {
         if(cache[setIndex][j].time<=cache[setIndex][rp].time) {
           rp=j;
         }
       }
       count++;
       cache[setIndex][rp].time=count;
       cache[setIndex][rp].valid=1; //insert
       cache[setIndex][rp].tag=tagi;
       return 0;
     }
   }
 }
 return 0;
}
