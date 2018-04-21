#ifndef RS_MISCMATHTESTS_H
#define RS_MISCMATHTESTS_H

#include "../../../Shared/Shared.h"

bool testMiscMath();

bool testExponentialCurveFitting(  std::string &reportString);
bool testRootFinding(              std::string &reportString);
bool testGradientBasedOptimization(std::string &reportString);

bool testMultiLayerPerceptronOld(     std::string &reportString);
bool testMultiLayerPerceptron(     std::string &reportString);


#endif