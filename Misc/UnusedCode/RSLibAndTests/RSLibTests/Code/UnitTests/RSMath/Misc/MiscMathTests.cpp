#include "MiscMathTests.h"

bool testMiscMath(std::string &reportString)
{
  std::string testName = "MiscMath";
  bool testResult = true;

  testResult &= testExponentialCurveFitting(  reportString);
  testResult &= testRootFinding(              reportString);
  testResult &= testGradientBasedOptimization(reportString);
  //testResult &= testMultiLayerPerceptronOld(  reportString);
  //testResult &= testMultiLayerPerceptron(     reportString);


  appendTestResultToReport(reportString, testName, testResult);
  return testResult;
}

bool testExponentialCurveFitting(std::string &reportString)
{
  std::string testName = "ExponentialCurveFitting";
  bool testResult = true;

  // example from Hamming's "Numerical Methods ..." page 620
  static const int N = 4;
  static const int k = N/2;
  //double x[N] = { 0.0,  1.0,  2.0,  3.0};
  double y[N] = {32.0, 20.0, 14.0, 11.0};

  // find exponents and weights:
  double a[k];   // exponents
  double A[k];   // weights
  bool success = rsFitSumOfExponentials(y, N, A, a, k);
  rsAssert(success);

  // now we should have: y[n] = A[0]*exp(a[0]*n) + A[1]*exp(a[1]*n) - verify this:
  double yc[N];
  int n;
  for(n = 0; n < N; n++)
    yc[n] = A[0]*exp(a[0]*n) + A[1]*exp(a[1]*n);
  for(n = 0; n < N; n++)
    testResult &= (yc[n] == y[n]);

  appendTestResultToReport(reportString, testName, testResult);
  return testResult;
}

bool testRootFinding(std::string &reportString)
{
  std::string testName = "RootFinding";
  bool testResult = true;

  double r;
  double tol = std::numeric_limits<double>::epsilon();

  UnivariateScalarFunctionViaPointer sine(&sin, &cos);

  // Newton iteration:
  r = sine.findRootViaNewtonNonRobust(3.0);
  testResult &= rsIsCloseTo(r, PI, tol);
  r = sine.findRootViaNewtonNonRobust(6.0);
  testResult &= rsIsCloseTo(r, 2*PI, tol);

  // Chebychev method (generalization of Newton iteration using also 2nd derivative):
  // ...stopping criterion still not working - so it's commented:
  //r = sine.findRootViaChebychevNonRobust(3.0);
  //testResult &= rsIsCloseTo(r, PI, tol);
  //r = sine.findRootViaChebychevNonRobust(6.0);
  //testResult &= rsIsCloseTo(r, 2*PI, tol);

  // Ridders' method does not yet converge here, so it's commented
  //r = sine.findRootViaRidders(6.2, 6.3);
  //r = sine.findRootViaRidders(3.1, 3.2);
  //r = sine.findRootViaRidders(2.5, 3.5);

  appendTestResultToReport(reportString, testName, testResult);
  return testResult;
}

bool testGradientBasedOptimization(std::string &reportString)
{
  std::string testName = "GradientBasedOptimization";
  bool testResult = true;

  // set up the minimizer:
  double tol = 0.00001; // tolerance - later, pass this to the minimizer
  GradientBasedMinimizer minimizer;
  minimizer.setBetaFormula(GradientBasedMinimizer::POLAK_RIBIERE);
  //minimizer.setBetaFormula(GradientBasedMinimizer::FLETCHER_REEVES);
  //minimizer.setBetaFormula(GradientBasedMinimizer::HESTENES_STIEFEL);
  //minimizer.setPrintInfo(true);

  // create the error function object:
  QuadraticTestErrorFunction error;
  double aMin[2]  = { 2, -2};  // x = [ 2, -2] is the desired minimum
  double aInit[2] = {-2, -2};  // x = [-2, -2] is ithe intial guess
  rsVectorDbl xMin( 2, aMin);
  rsVectorDbl xInit(2, aInit);
  rsVectorDbl xFinal;

  // do the minimization using different algorithms:
  minimizer.setAlgorithm(GradientBasedMinimizer::GRADIENT_DESCENT);
  xFinal = minimizer.minimizeFunction(&error, xInit);
  testResult &= (xFinal-xMin).getEuclideanNorm() < tol;

  minimizer.setAlgorithm(GradientBasedMinimizer::BOLD_DRIVER_WITH_MOMENTUM);
  xFinal = minimizer.minimizeFunction(&error, xInit);
  testResult &= (xFinal-xMin).getEuclideanNorm() < tol;

  minimizer.setAlgorithm(GradientBasedMinimizer::CONJUGATE_GRADIENT);
  xFinal = minimizer.minimizeFunction(&error, xInit);
  testResult &= (xFinal-xMin).getEuclideanNorm() < tol;

  minimizer.setAlgorithm(GradientBasedMinimizer::SCALED_CONJUGATE_GRADIENT);
  xFinal = minimizer.minimizeFunction(&error, xInit);
  testResult &= (xFinal-xMin).getEuclideanNorm() < tol;


  appendTestResultToReport(reportString, testName, testResult);
  return testResult;
}

bool testMultiLayerPerceptronOld(std::string &reportString)
{
  std::string testName = "MultiLayerPerceptron";
  bool testResult = true;

  // this should actually be in the "Experiments" suite

  //int hiddenNeuronsInLayers[2] = {2, 4};
  //MultiLayerPerceptron mlp(3, 2, 2, hiddenNeuronsInLayers);

  //int hiddenNeuronsInLayers[3] = {2, 4, 3};
  //MultiLayerPerceptron mlp(1, 1, 3, hiddenNeuronsInLayers);

  //int hiddenNeuronsInLayers[3] = {4, 3, 5};
  //MultiLayerPerceptron mlp(1, 1, 3, hiddenNeuronsInLayers);

  int hiddenNeuronsInLayers[1] = {10};
  MultiLayerPerceptron mlp(1, 1, 1, hiddenNeuronsInLayers);

  //int hiddenNeuronsInLayers[2] = {3, 5};
  //MultiLayerPerceptron mlp(1, 1, 2, hiddenNeuronsInLayers);

  //int hiddenNeuronsInLayers[2] = {2,4};
  //MultiLayerPerceptron mlp(1, 1, 2, hiddenNeuronsInLayers);

  // create the error-function object and pass some training data to it:
  MultiLayerPerceptronErrorFunction mlpError(&mlp);
  static const int N = 200;
  rsVectorDbl x[N];
  rsVectorDbl y[N];
  for(int n=0; n<N; n++)
  {
    x[n].setDimensionality(1);
    y[n].setDimensionality(1);
  }
  x[0].v[0] = rsRandomUniform(-3.0, 3.0, 7);
  for(int n=0; n<N; n++)
  {
    x[n].v[0] = rsRandomUniform(-3.0, 3.0);
    //x[n].v[0] = (double) n / (double) (N-1);
    //x[n].v[0] = linToLin(x[n].v[0], 0.0, 1.0, -3.0, 3.0);
    y[n].v[0] = x[n].v[0] * x[n].v[0];            // y = x^2
    //y[n].v[0] = fabs(x[n].v[0]);                    // y = |x|
    //y[n].v[0] = sign(x[n].v[0]);                    // y = sign(x)
    //y[n].v[0] = sin(x[n].v[0] * x[n].v[0]);            // y = sin(x^2)
  }
  mlpError.setTrainingData(x, y, N);

  // create the minimizer and minimize the training error:
  GradientBasedMinimizer mlpTrainer;
  mlpTrainer.setPrintInfo(true);
  //mlpTrainer.setAlgorithm(GradientBasedMinimizer::GRADIENT_DESCENT);
  //mlpTrainer.setAlgorithm(GradientBasedMinimizer::BOLD_DRIVER_WITH_MOMENTUM);
  //mlpTrainer.setAlgorithm(GradientBasedMinimizer::CONJUGATE_GRADIENT);
  mlpTrainer.setAlgorithm(GradientBasedMinimizer::SCALED_CONJUGATE_GRADIENT);
  //mlpTrainer.setBetaFormula(GradientBasedMinimizer::POLAK_RIBIERE);
  //mlpTrainer.setBetaFormula(GradientBasedMinimizer::FLETCHER_REEVES);
  //mlpTrainer.setBetaFormula(GradientBasedMinimizer::HESTENES_STIEFEL);
  rsVectorDbl w = mlpTrainer.minimizeFunction(&mlpError, mlp.getWeightsAsVector());
  mlp.setWeightVector(w);

  int dummy = 0;


  //double *test = new double[0];
  //test[0] = 1.0;

  //double x[3]  = {3,2,1};
  //double yt[2] = {1,-1};
  //double y[2];
  //mlp.computeNetworkOutput(x, y);
  //mlp.printWeights();
  //mlp.printActivations();

  //mlpError.computePatternGradientByWeightPerturbation(Vector(2, yt));
  //mlpError.printPatternGradient();

  //mlpError.computePatternGradient(Vector(2, yt));
  //mlpError.printPatternGradient();



  //Vector wv = mlp.getWeightsAsVector();
  //wv.print();
  //mlp.setWeightVector(wv);
  //mlp.printWeights();
  //Vector gv = mlpTrainer.getGradientVector();
  //gv.print();


  double xTest[N];
  double yTest[N];
  for(int n=0; n<N; n++)
  {
    xTest[n] = (double) n / (double) (N-1);
    xTest[n] = rsLinToLin(xTest[n], 0.0, 1.0, -6.0, 6.0);
    mlp.computeNetworkOutput(&xTest[n], &yTest[n]);
    rsVectorDbl yVec = mlp.getOutput();
    int dummy = 0;
  }
  //Plotter::plotData(N, xTest, yTest);

    
  printf("%s", "\n Press Key \n");
  getchar();



  appendTestResultToReport(reportString, testName, testResult);
  return testResult;
}


rsVectorDbl mlpTestFunction(rsVectorDbl x, double noiseAmplitude)
{
  rsAssert(x.dim == 3);
  rsVectorDbl y(2);

  // compute output values according to deterministic function:
  y[0]  =  2*x[0] + 3*x[1] - 1*x[2] - 2*x[0]*x[1] + 1*x[0]*x[2] + 3*x[1]*x[2];
  y[1]  = -3*x[0] + 1*x[1] + 2*x[2] + 3*x[0]*x[1] - 2*x[0]*x[2] + 1*x[1]*x[2];

  // add some noise:
  y[0] += noiseAmplitude * rsRandomUniform(-1.0, 1.0);
  y[1] += noiseAmplitude * rsRandomUniform(-1.0, 1.0);

  return y;
}
rsVectorDbl randomVector(int numDimensions, double min, double max)
{
  rsVectorDbl x(numDimensions);
  for(int i = 0; i < numDimensions; i++)
    x[i] = rsRandomUniform(min, max);
  return x;
}
bool testMultiLayerPerceptron(std::string &reportString)
{
  std::string testName = "MultiLayerPerceptron";
  bool testResult = true;

  // we try to approximate the function f: R^3 -> R^2, defined by:
  // y1 =  2*x1 + 3*x2 - 1*x3 - 2*x1*x2 + 1*x1*x3 + 3*x2*x3
  // y2 = -3*x1 + 1*x2 + 2*x3 + 3*x1*x2 - 2*x1*x3 + 1*x2*x3
  // by using noisy datapoints

  // ...that particular function seems to be quite difficult to approximate by an MLP
  // maybe use a function that is easier to represent - possibly by using outputs of an MLP
  // itself (with randomized weights) - see if the weights can be retrieved by training (or an 
  // equivalent weight-vector - there are weight-space symmetries)

  // create test data:
  int n;
  static const int numPatterns    = 200;
  double noiseAmplitude = 0.2;
  rsVectorDbl x[numPatterns];
  rsVectorDbl y[numPatterns];
  for(n = 0; n < numPatterns; n++)
  {
    x[n].setDimensionality(3);
    y[n].setDimensionality(2);
  }
  rsRandomUniform(-1.0, 1.0, 7); // init PRNG
  for(n = 0; n < numPatterns; n++)
  {
    x[n] = randomVector(3, -2.0, 2.0);
    y[n] = mlpTestFunction(x[n], noiseAmplitude);
  }

  // create an MLP with 3 hidden layers with 4, 3 and 5 neurons respectively:
  //int hiddenNeuronsInLayers[3] = {4, 3, 5};
  //MultiLayerPerceptron mlp(3, 2, 3, hiddenNeuronsInLayers);

  int hiddenNeuronsInLayers[1] = {10};
  MultiLayerPerceptron mlp(3, 2, 1, hiddenNeuronsInLayers);

  // create the error-function object and pass the training data to it:
  MultiLayerPerceptronErrorFunction mlpError(&mlp);
  mlpError.setTrainingData(x, y, numPatterns);

  // create the minimizer for the error-function find the weights that minimize the training error:
  GradientBasedMinimizer mlpTrainer;
  mlpTrainer.setPrintInfo(true);
  //mlpTrainer.setAlgorithm(GradientBasedMinimizer::GRADIENT_DESCENT);
  //mlpTrainer.setAlgorithm(GradientBasedMinimizer::BOLD_DRIVER_WITH_MOMENTUM);
  mlpTrainer.setAlgorithm(GradientBasedMinimizer::CONJUGATE_GRADIENT);
  //mlpTrainer.setAlgorithm(GradientBasedMinimizer::SCALED_CONJUGATE_GRADIENT);
  rsVectorDbl w = mlpTrainer.minimizeFunction(&mlpError, mlp.getWeightsAsVector());
  mlp.setWeightVector(w);  // set up the network with the optimal weight-vector


  appendTestResultToReport(reportString, testName, testResult);
  return testResult;
}



