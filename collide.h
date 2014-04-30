

#ifdef isoGame_collide_h
#define isoGame_collide_h

#include "gstructures.h"


float dotProduct(struct vector v1, struct vector v2);

struct vector normalizeVector(struct vector v);

struct vector normalVector(struct vector v);

struct polyLineSegment createPolySegment(struct vector v1, struct vector v2);

struct polygon assemblePolygon(struct polygon p);

struct projectionVals projectPolygon(struct polygon p, struct vector projectionAxis);

int intersect(struct projectionVals pv1, struct projectionVals pv2);

int doesCollide(struct polygon p1, struct polygon p2);

int checkCollision(struct polygon p1, struct Tile t[], int size, int* val);


#endif
