#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
  dm,
  fa
} cache_map_t;
typedef enum
{
  uc,
  sc
} cache_org_t;
typedef enum
{
  instruction,
  data
} access_t;

typedef struct
{
  uint32_t address;
  access_t accesstype;
} mem_access_t;

typedef struct
{
  uint64_t accesses;
  uint64_t hits;
  // You can declare additional statistics if
  // you like, however you are now allowed to
  // remove the accesses or hits
} cache_stat_t;

// DECLARE CACHES AND COUNTERS FOR THE STATS HERE

uint32_t cache_size;
uint32_t block_size = 64;
cache_map_t cache_mapping;
cache_org_t cache_org;

// USE THIS FOR YOUR CACHE STATISTICS
cache_stat_t cache_statistics;

/* Reads a memory access from the trace file and returns
 * 1) access type (instruction or data access
 * 2) memory address
 */
mem_access_t read_transaction(FILE *ptr_file)
{
  char buf[1000];
  char *token;
  char *string = buf;
  mem_access_t access;

  if (fgets(buf, 1000, ptr_file) != NULL)
  {
    /* Get the access type */
    token = strsep(&string, " \n");
    if (strcmp(token, "I") == 0)
    {
      access.accesstype = instruction;
    }
    else if (strcmp(token, "D") == 0)
    {
      access.accesstype = data;
    }
    else
    {
      printf("Unkown access type\n");
      exit(0);
    }

    /* Get the access type */
    token = strsep(&string, " \n");
    access.address = (uint32_t)strtol(token, NULL, 16);

    return access;
  }

  /* If there are no more entries in the file,
   * return an address 0 that will terminate the infinite loop in main
   */
  access.address = 0;
  return access;
}

// Calculate the integer log2 of a given number.
// Gotten from https://stackoverflow.com/questions/994593/how-to-do-an-integer-log2-in-c
static inline uint32_t ilog2(const uint32_t x)
{
  uint32_t y;
  asm("\tbsr %1, %0\n"
      : "=r"(y)
      : "r"(x));
  return y;
}

void print_cache(uint32_t *cache, uint32_t length)
{
  printf("\n\n");
  for (uint32_t i = 0; i < length; i++)
  {
    printf("%d\t%x\n", i, cache[i]);
  }
  printf("\n\n");
}

void main(int argc, char **argv)
{
  // Reset statistics:
  memset(&cache_statistics, 0, sizeof(cache_stat_t));

  /* Read command-line parameters and initialize:
   * cache_size, cache_mapping and cache_org variables
   */
  /* IMPORTANT: *IF* YOU ADD COMMAND LINE PARAMETERS (you really don't need to),
   * MAKE SURE TO ADD THEM IN THE END AND CHOOSE SENSIBLE DEFAULTS SUCH THAT WE
   * CAN RUN THE RESULTING BINARY WITHOUT HAVING TO SUPPLY MORE PARAMETERS THAN
   * SPECIFIED IN THE UNMODIFIED FILE (cache_size, cache_mapping and cache_org)
   */
  if (argc != 4)
  { /* argc should be 2 for correct execution */
    printf(
        "Usage: ./cache_sim [cache size: 128-4096] [cache mapping: dm|fa] "
        "[cache organization: uc|sc]\n");
    exit(0);
  }
  else
  {
    /* argv[0] is program name, parameters start with argv[1] */

    /* Set cache size */
    cache_size = atoi(argv[1]);

    /* Set Cache Mapping */
    if (strcmp(argv[2], "dm") == 0)
    {
      cache_mapping = dm;
    }
    else if (strcmp(argv[2], "fa") == 0)
    {
      cache_mapping = fa;
    }
    else
    {
      printf("Unknown cache mapping\n");
      exit(0);
    }

    /* Set Cache Organization */
    if (strcmp(argv[3], "uc") == 0)
    {
      cache_org = uc;
    }
    else if (strcmp(argv[3], "sc") == 0)
    {
      cache_org = sc;
    }
    else
    {
      printf("Unknown cache organization\n");
      exit(0);
    }
  }

  /* Open the file mem_trace.txt to read memory accesses */
  FILE *ptr_file;
  ptr_file = fopen("mem_trace2.txt", "r");
  if (!ptr_file)
  {
    printf("Unable to open the trace file\n");
    exit(1);
  }

  if (cache_mapping == dm)
  {
    uint32_t total_cache_size_blocks = cache_size / block_size;
    uint32_t single_cache_size_blocks = (cache_org == sc) ? (total_cache_size_blocks / 2) : total_cache_size_blocks;
    uint32_t block_offset_bits = ilog2(block_size);
    uint32_t index_bits = ilog2(single_cache_size_blocks);

    uint32_t index_mask = 0b0;
    for (uint32_t i = 0; i < index_bits; i++)
      index_mask = (index_mask << 1) | 0b1;

    printf("total_cache_size_blocks=%d\n", total_cache_size_blocks);
    printf("single_cache_size_blocks=%d\n", single_cache_size_blocks);
    printf("cache_size_blocks=%d\n", total_cache_size_blocks);
    printf("block_offset_bits=%d\n", block_offset_bits);
    printf("index_bits=%d\n", index_bits);
    printf("index_mask=%x\n", index_mask);

    uint8_t *cache_valid = (uint8_t *)malloc(total_cache_size_blocks * sizeof(uint8_t));
    uint32_t *cache = (uint32_t *)malloc(total_cache_size_blocks * sizeof(uint32_t));

    for (uint32_t i = 0; i < total_cache_size_blocks; i++)
      cache_valid[i] = 0;

    /* Loop until whole trace file has been read */
    mem_access_t access;
    while (1)
    {
      access = read_transaction(ptr_file);
      // If no transactions left, break out of loop
      if (access.address == 0)
        break;
      // printf("%d %x ", access.accesstype, access.address);
      /* Do a cache access */

      cache_statistics.accesses++;

      uint32_t index = access.address & index_mask;
      if (cache_org == sc && access.accesstype == data)
        index += single_cache_size_blocks;
      uint32_t tag = access.address >> (index_bits + block_offset_bits);

      if (cache_valid[index] && (tag == cache[index]))
      {
        cache_statistics.hits++;
      }
      else
      {
        cache_valid[index] = 1;
        cache[index] = tag;
      }
    }
    free(cache_valid);
    free(cache);
  }
  else if (cache_mapping == fa)
  {
    if (cache_org == uc)
    {
    }
    else if (cache_org == sc)
    {
    }
  }

  /* Print the statistics */
  // DO NOT CHANGE THE FOLLOWING LINES!
  printf("\nCache Statistics\n");
  printf("-----------------\n\n");
  printf("Accesses: %ld\n", cache_statistics.accesses);
  printf("Hits:     %ld\n", cache_statistics.hits);
  printf("Hit Rate: %.4f\n",
         (double)cache_statistics.hits / cache_statistics.accesses);
  // DO NOT CHANGE UNTIL HERE
  // You can extend the memory statistic printing if you like!

  /* Close the trace file */
  fclose(ptr_file);
}
