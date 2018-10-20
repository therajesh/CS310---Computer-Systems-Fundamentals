
/*  Rajesh Narayan
    narayar@whitman.edu
    */

/*  http://www.andrew.cmu.edu/user/marjorie/recitation_slides/rec7.pdf
    was very helpful, I originally wasn't sure how important the LRU bit was,
    but I understood pretty soon.
*/ 

#define _GNU_SOURCE
#include "cachelab.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

int VERBOSE;

/* represents a single cache line.
     if this were more than a simulator, it would 
     also hold actual data in addition to tag field 
     and valid bit. */
typedef struct {
    long tag;
    int valid;
    unsigned LRU; //Least recently used
} cache_line;

/* The struct that represents all of cache */
typedef struct {
    /* 2d array of cache lines. Each set will be an array of lines */
    cache_line** set_array;
    int s;  // set_bits
    int E;  // lines_per_set
    int b;  // block_bits

    int hits; 
    int misses;
    int evictions;
} cache_struct;


/* creates a cache_struct object and returns a pointer 
  to that object */
cache_struct* create_cache(int s, int E, int b) {
    // malloc for cache_struct
    cache_struct* cs = malloc(sizeof(cache_struct));

    cs->s = s;
    cs->E = E;
    cs->b = b;
    cs->hits = 0;
    cs->misses = 0;
    cs->evictions = 0;

    int total_sets = 1 << s;
    //int size_of_block = 1 << b;
    // malloc for cache_struct's set array:
    cs->set_array = malloc(sizeof(cache_line*) * total_sets);

    // malloc enough space for each set
    for(int i = 0; i < total_sets; i++) {
        // each set is E lines
        cs->set_array[i] = malloc(sizeof(cache_line) * E);
        for(int j = 0; j < E; j++) {
            // set the valid and LRU bit to 0 in each line
            cs->set_array[i][j].valid = 0;
            cs->set_array[i][j].LRU = 0;
            cs->set_array[i][j].tag = 0;
        }
    }

    return cs;
}

void access_cache(cache_struct* cache, long set_index, long tag) {
    

    int empty_line_index = 0; //holds empty lines index
    int full_cache = 1; // checks to see if the cache is full
    char hit = 0; //check which valid lines have similar tags


    for(int index = 0; index < cache->E; ++index) {
        if ((cache->set_array[set_index][index]).valid) {
            if ((cache->set_array[set_index][index]).tag == tag) {
                //printf("hit\n");
                ++cache->hits;
                //increase LRU counter of all lines then reset current line
                for (int i = 0; i < cache->E; ++i) {
                    if ((cache->set_array[set_index][i]).valid) {
                        ++((cache->set_array[set_index][i]).LRU);
                    }
                }
                (cache->set_array[set_index][index]).LRU = 0;
                hit = 1;
                break;
            }
        } else {
            empty_line_index = index;
            full_cache = 0;
        }
    }

    if (!hit){ // in case of a miss
        ++cache->misses;
        if(!full_cache) { //there is still space
            (cache->set_array[set_index][empty_line_index]).valid = 1;
            (cache->set_array[set_index][empty_line_index]).tag = tag;
            //increase LRU counter for all lines then reset current line
            for (int i = 0; i < cache->E; ++i)
                if ((cache->set_array[set_index][i]).valid)
                    ++((cache->set_array[set_index][i]).LRU);
            (cache->set_array[set_index][empty_line_index]).LRU = 0;   
        }
        else {
            ++cache->evictions;
            //Evict the line with the highest LRU number
            int highest_LRU_index = 0;
            int highest_LRU_count = 0;
            for (int i = 0; i < cache->E; ++i) {
                if ((cache->set_array[set_index][i]).LRU > highest_LRU_count) {
                    highest_LRU_count = (cache->set_array[set_index][i]).LRU;
                    highest_LRU_index = i;
                }
            }
            //Evict line by overwriting the tag
            (cache->set_array[set_index][highest_LRU_index]).tag = tag;
            //increase LRU counter for all lines then reset current line
            for (int i = 0; i < cache->E; ++i) {
                ++((cache->set_array[set_index][i]).LRU);
            }
            (cache->set_array[set_index][highest_LRU_index]).LRU = 0;
        }
    }
}

void access_cache_setup(cache_struct* cache, char type, long address) {
    long block_mask = (1 << cache->b) - 1; //only needed for tag mask
    long set_mask = (1 << cache->s) - 1; //mask for set index bits
    set_mask = set_mask << cache->b;
    long tag_mask = ~(block_mask | set_mask);

    //The address has 't' tag bits, 's' set index bits and 'b' block bits
    long set_index = (set_mask & address) >> cache->b;
    long tag = (tag_mask & address) >> (cache->s + cache->b);
    //printf("set index = %lx\n", set_index);
    //printf("tag = %lx\n", tag);

    //Stores, Loads and Moves all have similar hit/miss/eviction counts, therefore
    //they can all use the same function call

    switch(type) {
        case 'I':
            break;
        case 'L':
            access_cache(cache, set_index, tag);
            //printf("hits = %d misses = %d evictions = %d\n", cache->hits, cache->misses, cache->evictions);
            break;
        case 'S':
            access_cache(cache, set_index, tag);
            //printf("hits = %d misses = %d evictions = %d\n", cache->hits, cache->misses, cache->evictions);
            break;
        case 'M':
            access_cache(cache, set_index, tag);
            //printf("hits = %d misses = %d evictions = %d\n", cache->hits, cache->misses, cache->evictions);
            access_cache(cache, set_index, tag);
            //printf("hits = %d misses = %d evictions = %d\n", cache->hits, cache->misses, cache->evictions);
            break; 
        default:
            break;
    }
}

/* Prints a message about how to use the program */
void usage() {
    fprintf(stderr, 
            "Usage: csim -s <s> -E <E> -b <b> -t <tracefile> [-v]\n");
    exit(0);
}

int main(int argc, char* argv[])
{
    /* This first section is for parsing command line
             arguments. You should be able to leave it intact */
    int opt;
    int bval = 2; // default 2 block bits (block size of 4)
    int sval = 3; // default 3 set bits (8 sets)
    int Eval = 1; // default 1 line per set
    char* tracefile = NULL;
    VERBOSE = 0; // default not verbose
    while( (opt = getopt(argc, argv, "s:E:b:t:vh")) != -1) {
        switch(opt) {
            case 'b':
                bval = atoi(optarg);
                break;
            case 's':
                sval = atoi(optarg);
                break;
            case 'E':
                Eval = atoi(optarg);
                break;
            case 't':
                tracefile = malloc(strlen(optarg) + 1);
                strncpy(tracefile, optarg, strlen(optarg)+1);
                break;
            case 'v':
                VERBOSE = 1;
                break;
            case 'h':
                usage();
        }
    }
    if(tracefile == NULL) {
        usage();
    }
    if(VERBOSE) {
        printf("s: %d b: %d E: %d t: %s\n", sval, bval, Eval, tracefile);
    }
    /* End of command-line argument parsing code */

    // read tracefile line by line

    /* Begin your code to simulate your cache here */

    cache_struct* cache = create_cache(sval, Eval, bval); // creating a cache object
    // set_bits, lines_per_set, and block_bits
    cache->b = bval;
    cache->s = sval;    // setting the values for the cache to be
    cache->E = Eval;  // equal to the inputted values


    FILE *fp = fopen(tracefile, "r");
    if (fp == NULL) {
        printf("Couldn't read file\n");
    } else {
        //read line by line using getline
        char * line;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, fp)) != -1) {
            printf("%s", line);

            // parse line into accesstype, address, bytesread
            long address, bytesread;
            char type;
            sscanf(line, " %c %lx,%ld", &type, &address, &bytesread);

            //printf("access type: %c, memory addr: %lx, Bytes read: %ld\n", type, address, bytesread);

            access_cache_setup(cache, type, address);
        }
    }


    printSummary(cache->hits, cache->misses, cache->evictions);
    //fclose(fp);
    return 0;
}
