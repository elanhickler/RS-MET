#ifndef UNITTESTS_INCLUDED
#define UNITTESTS_INCLUDED

//#include "DataUnitTests.h"
//#include "MathUnitTests.h"
//#include "ImageUnitTests.h"
//#include "DrawingUnitTests.h"
//#include "FilterUnitTests.h"
//#include "MiscUnitTests.h"


// The function that runs all the unit tests:
bool runAllUnitTests();

// Data:
bool arrayUnitTest();
bool doubleEndedQueueUnitTest();
bool float64x2UnitTest();
bool float32x4UnitTest();
bool complexFloat64x2UnitTest();

// Drawing:
bool triangleRasterization();
bool triangleRasterization2();  // absorb in function above
bool imagePainterUnitTest();    // implementation is in extra file - maybe mae a GraphicsUnitTests.cpp file for all

// Filter:
bool prototypeDesignUnitTest();
bool filterSpecUnitTest();
bool movingMaximumUnitTest();

// Math:
bool coordinateMapperUnitTest();
bool correlationUnitTest();
bool fitRationalUnitTest();
bool interpolatingFunctionUnitTest();
bool resampleNonUniform();
bool rootFinderUnitTest();
bool polynomialRootsUnitTest(); // the new explicit formulas - move to PolynomialUnitTests

// Misc:
bool blepUnitTest();  // move to GeneratorTests
bool syncUnitTest();
bool spectrogramUnitTest();
bool sineModelingUnitTest();

// newley added (need some adaptions):
bool testBufferFunctions(std::string &reportString);
bool testSortAndSearch(std::string &reportString);



#endif