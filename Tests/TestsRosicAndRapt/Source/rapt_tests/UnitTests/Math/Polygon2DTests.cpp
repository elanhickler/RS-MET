#include "Polygon2DTests.h"


bool testPolygon2D()
{
  std::string testName = "Polygon2D";
  std::string dummy; 
  bool r = true;


  //r &= testRegularPolygonCreation2D(dummy);
  //r &= testPointInsidePolygon2D(dummy);
  r &= convexPolygonClipping(dummy);

  //appendTestResultToReport(reportString, testName, testResult);
  return r;
}

bool testRegularPolygonCreation2D(std::string &reportString)
{
  std::string testName = "RegularPolygonCreation2D";
  bool testResult = true;

  /*
  typedef rsPoint2D<double>   Point;
  typedef rsPolygon2D<double> Polygon;

  Point c1(0, 0);

  // equilateral triangle centered at origin, pointing to the right:
  Polygon p1(3, 1, c1, 0);
  */

  appendTestResultToReport(reportString, testName, testResult);
  return testResult;
}

bool testPointInsidePolygon2D(std::string &reportString)
{
  std::string testName = "PointInsidePolygon2D";
  bool testResult = true;

  /*
  typedef rsPoint2D<double>   Point;
  typedef rsPolygon2D<double> Polygon;


  Point a(1, 1);
  Point b(6, 5);
  Point c(5, 0);
  Triangle t(a, b, c);

  // test some points inside the triangle:
  testResult &=  t.containsPoint( Point(1, 1) );
  testResult &=  t.containsPoint( Point(2, 1) );
  testResult &=  t.containsPoint( Point(3, 1) );
  testResult &=  t.containsPoint( Point(4, 1) );
  testResult &=  t.containsPoint( Point(3, 2) );
  testResult &=  t.containsPoint( Point(4, 2) );
  testResult &=  t.containsPoint( Point(5, 2) );
  testResult &=  t.containsPoint( Point(4, 3) );
  testResult &=  t.containsPoint( Point(5, 3) );
  testResult &=  t.containsPoint( Point(5, 4) );

  // test some points outside the triangle:
  testResult &= !t.containsPoint( Point(1, 2) );
  testResult &= !t.containsPoint( Point(2, 2) );
  testResult &= !t.containsPoint( Point(3, 3) );
  testResult &= !t.containsPoint( Point(4, 4) );
  testResult &= !t.containsPoint( Point(5, 5) );
  testResult &= !t.containsPoint( Point(6, 4) );
  testResult &= !t.containsPoint( Point(6, 3) );
  testResult &= !t.containsPoint( Point(6, 2) );
  testResult &= !t.containsPoint( Point(6, 1) );
  testResult &= !t.containsPoint( Point(4, 0) );
  testResult &= !t.containsPoint( Point(3, 0) );
  testResult &= !t.containsPoint( Point(2, 0) );
  testResult &= !t.containsPoint( Point(1, 0) );
  */

  appendTestResultToReport(reportString, testName, testResult);
  return testResult;
}


bool convexPolygonClipping(std::string &reportString)
{
  bool r = true;

  typedef rsVector2DF Vec2;       // 2D vector (for poylgon vertices
  typedef std::vector<Vec2> Poly; // a polygon is an array of vertices
  typedef Vec2 V;

  // a square:
  Vec2 
    s0(3, 2), 
    s1(3, 6),
    s2(7, 6),
    s3(7, 2);
  Poly square = { s0, s1, s2, s3 };  // counter clockwise


  // a triangle:
  Vec2 
    t0(5, 1),
    t1(1, 5),
    t2(9, 5); 
  Poly triangle = { t0, t1, t2 };  // counter clockwise

  // test edge function:
  float d;

  d = edgeFunction(V(4,1), V(4,3), V(3,8)); r &= d ==  2;
  d = edgeFunction(V(4,1), V(4,3), V(4,8)); r &= d ==  0;
  d = edgeFunction(V(4,1), V(4,3), V(5,8)); r &= d == -2;
  // rename edgeFunction To LeftDistance
  // returns a value proportional to the distance of the point p to the left of the directed
  //

  d = edgeFunction(t0, t1, s0);        r &= d >  0;
  d = edgeFunction(t0, t1, t2);        r &= d <  0;
  d = edgeFunction(t0, t1, Vec2(4,2)); r &= d == 0;
  d = edgeFunction(t0, t1, Vec2(3,3)); r &= d == 0;

  
  // test line intersection:
  Vec2 v = lineIntersection(Vec2(3,0), Vec2(4,1), Vec2(0,3), Vec2(2.5,3.5));
  r &= v == Vec2(7.5, 4.5);

  Poly clipped, target;

  clipped = clipAgainstEdge(triangle, s0, s1);  
  target  = { V(5,1), V(3,3), V(3,5), V(9,5) }; r &= clipped == target;

  clipped = clipAgainstEdge(triangle, s1, s2);
  target  = { V(5,1), V(1,5), V(9,5) }; r &= clipped == target;

  clipped = clipAgainstEdge(triangle, s2, s3);
  target  = { V(7,3), V(5,1), V(1,5), V(7,5) }; r &= clipped == target;

  clipped = clipAgainstEdge(triangle, s3, s0);
  target  = { V(6,2), V(4,2), V(1,5), V(9,5) }; r &= clipped == target;

  clipped = clipAgainstEdge(triangle, s0, s3);
  target  = { V(6,2), V(5,1), V(4,2) }; r &= clipped == target;

  clipped = clipAgainstEdge(triangle, s3, s2); 
  target  = { V(7,3), V(7,5), V(9,5) }; r &= clipped == target;
  // we need to test the case where none of the vertices is inside the edge and also
  // when adges of clipping and subject polygon coincide (maybe just clip again - this should
  // change nothing


  clipped = clipPolygon(triangle, square);
  r &= clipped.size() == 6;
  target  = { V(7,3), V(6,2), V(4,2), V(3,3), V(3,5), V(7,5) }; r &= clipped == target;
  clipped = clipPolygon(square, triangle);
  target  = { V(6,2), V(4,2), V(3,3), V(3,5), V(7,5), V(7,3) }; r &= clipped == target;
  // clipping the triangle against the square or vice versa results in the same clipped polygon,
  // however, the start vertex is different in both cases

  // counter-clockwise convention is used (to be consistent with OpenGL and DirectX)
  // info to vertex order in OpenGL
  // https://stackoverflow.com/questions/8142388/in-what-order-should-i-send-my-vertices-to-opengl-for-culling
  // By default, counterclockwise polygons are taken to be front-facing.

  return r;
}