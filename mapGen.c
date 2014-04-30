

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mapGen.h"

//generates the base layer of map
int** generateBase()
{
    int hw = 100;
    int **base;
    base = (int**) malloc(hw * sizeof(int*));
    int x, y;
    for(x = 0; x < hw; x++)
    {
        base[x] = (int*) malloc(hw * sizeof(int));
    }
  
    
    for(x = 0; x < hw; x++)
    {
        for(y = 0; y < hw; y++)
        {
            base[x][y] = 1;
        }
    }
    
    return base;
}

//generates the structures in each chunk (stacks of blocks)
int** generateBlocks(void)
{
    int hw = 100;
    int **blocks;
    blocks = (int**)  malloc(hw * sizeof(int*));
    int x, y;
    for(x = 0; x < hw; x++)
    {
        blocks[x] = (int*) malloc(hw * sizeof(int));
    }
    
    time_t t;
    srand((unsigned) time(&t));
    int val;
    for(x = 0; x < hw; x++)
    {
        for(y = 0; y < hw; y++)
        {
            val = rand() % 100;
            if(val < 90)
            {
                blocks[x][y] = 1;
                
            }
            else if(val < 92)
            {
                blocks[x][y] = 2;
              
            }
            else if(val < 95)
            {
                blocks[x][y] = 3;
              
            }
            else
            {
                blocks[x][y] = 4;
            }
            
        }
    }
    
    return blocks;
}

struct chunk generateChunk(void)
{
    struct chunk map;
    map.chunkBase = generateBase();
    map.chunkBlocks = generateBlocks();
    return map;
}



