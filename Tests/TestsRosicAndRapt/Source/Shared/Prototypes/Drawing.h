#pragma once

// Lines

void drawLineWuPrototype(rsImageF& img, float x0, float y0, float x1, float y1, float color);
void drawLineBresenham(rsImageF& img, int x0, int y0, int x1, int y1, float color);

void drawThickLine(rsImageF& img, float x0, float y0, float x1, float y1, float color,
  float thickness, bool roundCaps = false);
void drawThickLine2(rsImageF& img, float x0, float y0, float x1, float y1, float color,
  float thickness, int endCaps = 0);
void drawThickLineViaWu(rsImageF& img, float x0, float y0, float x1, float y1, float color, 
  float thickness);

void plotLineWidth(rsImageF& img, int x0, int y0, int x1, int y1, float wd); //?

// line profile functions
float lineIntensity1(float d, float t2);
float lineIntensity2(float d, float t2);
float lineIntensity3(float d, float t2);
float lineIntensity4(float d, float t2);

// Triangles

/** Fills all pixels whose centers are inside the given triangle with the given color. If a pixel
center is on an edge, it will be considered inside, if it's a top or a left edge 
("top-left rule"). It corresponds to a convention where a pixel with coordinate i (i being the 
integer x or y coordinate) being defined as covering the range [i, i+1), i.e. an interval that 
is closed to the left and open to ther right, i.e. including i but excluding i+1.
The order of the vertices doesn't matter.
A top edge, is an edge that is exactly horizontal and is above the other edges.
A left edge, is an edge that is not exactly horizontal and is on the left side of the triangle. 
A triangle can have one or two left edges.
see:
https://docs.microsoft.com/en-us/windows/desktop/direct3d11/d3d10-graphics-programming-guide-rasterizer-stage-rules#triangle-rasterization-rules-without-multisampling
If this algorithms is used to render several triangles with shared edges (as in a triangle mesh),
each pixel will be visited only once

*/
void drawTriangle(rsImageDrawerFFF& drw, 
  const rsVector2DF& v0, const rsVector2DF& v1, const rsVector2DF& v2, float color);

// Polygons:

/** Returns a value proportional to how much the given point p is to the left of the directed edge
from a to b. The proportionlatity constant is the (signed) length of b-a. A point p to the left of 
the edge will return positive values, a point p to the right, negative values and a point on the 
edge zero. */
float edgeFunction(const rsVector2DF& a, const rsVector2DF& b, const rsVector2DF& p);
// rename to leftOf (or rightTo) a->b is a directed edge or leftDistance

/** Intersection point between lines through p0,p1 and q0,q1 */
rsVector2DF lineIntersection(const rsVector2DF& p0, const rsVector2DF& p1,
  const rsVector2DF& q0, const rsVector2DF& q1);

/** Clips a subject polygon against a convex clipping polygon. The subject needs to to be 
convex. */
std::vector<rsVector2DF> clipPolygon(const std::vector<rsVector2DF>& subject, 
  const std::vector<rsVector2DF>& clipper);
// actually not a drawing algorithm but a polygon clipping algorithm (needed here for prototype for
// anti-aliased triangle drawing)

/** Clips the given polygon p against the edge from e0 to e1. */
std::vector<rsVector2DF> clipAgainstEdge(const std::vector<rsVector2DF>& p,
  const rsVector2DF& e0, const rsVector2DF& e1);
// rename to clipPolygonAgainstedge

std::vector<rsVector2DF> clipConvexPolygons2(const std::vector<rsVector2DF>& p, 
  const std::vector<rsVector2DF>& c);
// obsolete

/** Given pixel coordinates x,y and the 3 vertices a,b,c of a triangle (in counterclockwise order), 
this function computes the area in which the triangle and pixel-square overlap. It's a value 
between 0 and 1, where 0 means the pixel and triangle do not intersect at all, 1 means the pixel is
fully covered by the triangle. */
float pixelCoverage(int x, int y, 
  const rsVector2DF& a, const rsVector2DF& b, const rsVector2DF& c);
