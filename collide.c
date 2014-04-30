

#include <stdio.h>
#include "gstructures.h"
#include "collide.h"
#include <math.h>


//Function for determining the dot product between two vectors.
float dotProduct(struct vector v1, struct vector v2)
{
    float value;
    value = v1.xComp * v2.xComp + v1.yComp * v2.yComp;
    
    return value;
}

//function for calculating unit vector.
struct vector normalizeVector(struct vector v)
{
    float magnitude  = sqrt(v.xComp * v.xComp + v.yComp * v.yComp);
    if(magnitude == 0)
    {
        printf("ERROR");
    }
    struct vector nV;
    nV.xComp = v.xComp / magnitude;
    nV.yComp = v.yComp / magnitude;
    return nV;
}

//function for determining the normal / perpendicular of a vector
struct vector normalVector(struct vector v)
{
    struct vector nV;
    nV.xComp = v.yComp;
    nV.yComp = -v.xComp;
    nV.magnitude = v.magnitude;
    return nV;
}

//fucntion for creating a line segment between two points. Polygon is collection of segments
struct polyLineSegment createPolySegment(struct vector v1, struct vector v2)
{
    struct vector segmentMagnitude;
    struct polyLineSegment lineSegment;
    segmentMagnitude.xComp = v2.xComp - v1.xComp;
    segmentMagnitude.yComp = v2.yComp - v1.yComp;
    lineSegment.p1 = v1;
    lineSegment.p2 = v2;
    lineSegment.magnitude = segmentMagnitude;
    return lineSegment;
}

//takes polygon stuct with only points, and assembles the segments. 
struct polygon assemblePolygon(struct polygon p)
{
    struct polygon pol;
    
    struct polyLineSegment ls1;
    struct polyLineSegment ls2;
    struct polyLineSegment ls3;
    struct polyLineSegment ls4;
    

    ls1 = createPolySegment(p.p1, p.p2);
    ls2 = createPolySegment(p.p2, p.p3);
    ls3 = createPolySegment(p.p3, p.p4);    
    ls4 = createPolySegment(p.p4, p.p1);

    pol.l1 = ls1;
    pol.l2 = ls2;
    pol.l3 = ls3;
    pol.l4 = ls4;
    
    pol.p1 = p.p1;
    pol.p2 = p.p2;
    pol.p3 = p.p3;
    pol.p4 = p.p4;
    
    return pol;
    
}


//this should be done in a loop
//i'll fix it later.
struct projectionVals projectPolygon(struct polygon p, struct vector projectionAxis) // projection axis is a linesegment normal
{
    struct vector axis = normalizeVector(projectionAxis);
    struct projectionVals projVal;
    float maxVal = dotProduct(p.p1, axis);
    float minVal = maxVal;
    float projection = dotProduct(p.p1, axis);
    //printf("%f\n", projection);
    if (projection < minVal)
    {
        minVal = projection;
    }
    if (projection > maxVal)
    {
        maxVal = projection;
    }
    
    projection = dotProduct(p.p2, axis);
    //printf("%f\n", projection);
    if (projection < minVal)
    {
        minVal = projection;
    }
    if (projection > maxVal)
    {
        maxVal = projection;
    }
    
    projection = dotProduct(p.p3, axis);
    //printf("%f\n", projection);
    if (projection < minVal)
    {
        minVal = projection;
    }
    if (projection > maxVal)
    {
        maxVal = projection;
    }
    
    projection = dotProduct(p.p4, axis);
    //printf("%f\n", projection);
    if (projection < minVal)
    {
        minVal = projection;
    }
    if (projection > maxVal)
    {
        maxVal = projection;
    }
    
    //printf("MAX: %f\n", maxVal);
    //printf("MIN: %f\n", minVal);
    projVal.max = maxVal;
    projVal.min = minVal;
    
    return projVal;
}


int intersect(struct projectionVals pv1, struct projectionVals pv2)
{
    float r1 = pv2.max;
    float r2 = pv2.min;
    if(r2 < r1)
    {
        r1 = r2;
        r2 = pv2.max;
    }
    if(pv1.max >= r1 && pv1.max <= r2 )
    {
        return 1;
    }
    if(pv1.min >= r1 && pv1.min <= r2 )
    {
        return 1;
    }
    
    
    r1 = pv1.max;
    r2 = pv1.min;
    if(r2 < r1)
    {
        r1 = r2;
        r2 = pv1.max;
    }
    
    if(pv2.max >= r1 && pv2.max <= r2 )
    {
        return 1;
    }
    if(pv2.min >= r1 && pv2.min <= r2 )
    {
        return 1;
    }
    
    return 0;
}


//probably better in a loop
int doesCollide(struct polygon p1, struct polygon p2)
{

    //int collision
    struct vector axis;
    struct projectionVals v1;
    struct projectionVals v2;
    axis = p1.l1.magnitude;
    axis = normalVector(axis);
    v1 = projectPolygon(p1, axis);
    v2 = projectPolygon(p2, axis);
    if(!intersect(v1, v2))
    {
        return 0;
    }
    

    
    axis = p1.l2.magnitude;
    axis = normalVector(axis);
    v1 = projectPolygon(p1, axis);
    v2 = projectPolygon(p2, axis);
    if(!intersect(v1, v2))
    {
        return 0;
    }
    
    axis = p1.l3.magnitude;
    axis = normalVector(axis);
    v1 = projectPolygon(p1, axis);
    v2 = projectPolygon(p2, axis);
    if(!intersect(v1, v2))
    {
        return 0;
    }
    
    axis = p1.l4.magnitude;
    axis = normalVector(axis);
    v1 = projectPolygon(p1, axis);
    v2 = projectPolygon(p2, axis);
    if(!intersect(v1, v2))
    {
        return 0;
    }
    
    //shhhh
    
    axis = p2.l1.magnitude;
    axis = normalVector(axis);
    v1 = projectPolygon(p1, axis);
    v2 = projectPolygon(p2, axis);
    if(!intersect(v1, v2))
    {
        return 0;
    }
    
    
    axis = p2.l2.magnitude;
    axis = normalVector(axis);
    v1 = projectPolygon(p1, axis);
    v2 = projectPolygon(p2, axis);
    if(!intersect(v1, v2))
    {
        return 0;
    }
    
    axis = p2.l3.magnitude;
    axis = normalVector(axis);
    v1 = projectPolygon(p1, axis);
    v2 = projectPolygon(p2, axis);
    if(!intersect(v1, v2))
    {
        return 0;
    }

    axis = p2.l4.magnitude;
    axis = normalVector(axis);
    v1 = projectPolygon(p1, axis);
    v2 = projectPolygon(p2, axis);
    if(!intersect(v1, v2))
    {
        return 0;
    }
    

    
    return 1;
}


int checkCollision(struct polygon p1, struct Tile t[], int size, int* index)
{
    int collides = 0;
    int x;
    for(x = 0; x < size; x++)
    {
        if(t[x].stack > 1)
        {
            collides = doesCollide(p1, t[x].pol);
            
        }
        if(collides == 1)
        {
            *index = x;
            return 1;
        }
    }
    
    return 0;
}








