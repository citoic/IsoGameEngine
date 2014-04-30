

#ifndef isoGame_gstructures_h
#define isoGame_gstructures_h
#include <GLEW/glew.h>
struct vector
{
    float magnitude;
    float xComp;
    float yComp;
};

struct chunk
{
    int **chunkBase;
    int **chunkBlocks; 
};

struct point
{
    float x;
    float y;
};

struct projectionVals
{
    float min;
    float max;
};

struct polyLineSegment
{
    struct vector p1;
    struct vector p2;
    struct vector magnitude;
};

struct polygon
{
    struct polyLineSegment l1;
    struct polyLineSegment l2;
    struct polyLineSegment l3;
    struct polyLineSegment l4;
    
    // used as points rather than coordinates here
    struct vector p1;
    struct vector p2;
    struct vector p3;
    struct vector p4;
    
};

struct Tile
{
    char type;
    int num;
    float xPos;
    float yPos;
    GLuint* image;
    int xScale;
    int yScale;
    int stack;
    int collidable;
    //struct coords pts; //replace with polygon.
    struct polygon pol;
    
};


#endif
