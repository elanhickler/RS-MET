

bool romos::testFormulaModules()
{
  bool result = true;

  typedef romos::Module MDL;
  ContainerModule* cm = (ContainerModule*)moduleFactory.createModule("Container",
    "FormulaTest", 0, 0, true);
  // actually, we don't really need this container...yet...but later we may want to make tests for
  // the updating of the connectivity when the input variables change...

  MDL* constant2 = cm->addChildModule("Constant", "2", 2, 4, false, false);
  MDL* constant3 = cm->addChildModule("Constant", "3", 2, 4, false, false);
  MDL* constant5 = cm->addChildModule("Constant", "5", 2, 6, false, false);
  MDL* constant7 = cm->addChildModule("Constant", "7", 2, 8, false, false);
  MDL* audioOutput1 = cm->addChildModule("AudioOutput", "Out1", 20, 4, false, false);
  MDL* audioOutput2 = cm->addChildModule("AudioOutput", "Out2", 20, 6, false, false);
  MDL* audioOutput3 = cm->addChildModule("AudioOutput", "Out3", 20, 8, false, false);



  FormulaModule_N_M* formula_N_M = (FormulaModule_N_M*)
    cm->addChildModule("Formula", "Formula", 13, 6, false, false);

  cm->sortChildModuleArray();

  double ins[4] = { 2, 3, 5, 7};
  double outs[4];

  // 3 inputs, all used in formula:
  formula_N_M->setFormula("y = a*x + b");
  formula_N_M->setInputVariables("x,a,b");
  FormulaModule_N_M::process(formula_N_M, ins, outs, 0);
  result &= outs[0] == 3*2 + 5;

  // 3 inputs, 3rd not used:
  formula_N_M->setFormula("y = a*x");
  formula_N_M->setInputVariables("x,a,b");
  FormulaModule_N_M::process(formula_N_M, ins, outs, 0);
  result &= outs[0] == 3*2;

  // for c, there's no input - should be treated as 0:
  formula_N_M->setFormula("y = a*x + b + c"); 
  formula_N_M->setInputVariables("x,a,b");
  FormulaModule_N_M::process(formula_N_M, ins, outs, 0);
  result &= outs[0] == 3*2 + 5;

  // now there is a 4th input for c = 7 - 4 inputs, all used:
  formula_N_M->setFormula("y = a*x + b + c");
  formula_N_M->setInputVariables("x,a,b,c");  
  FormulaModule_N_M::process(formula_N_M, ins, outs, 0);
  result &= outs[0] == 3*2 + 5 + 7;

  // 4 inputs, all used:
  formula_N_M->setFormula("y = a + b + c + d");
  formula_N_M->setInputVariables("a,b,c,d");
  FormulaModule_N_M::process(formula_N_M, ins, outs, 0);
  result &= outs[0] == 2 + 3 + 5 + 7;

  // 4 inputs a,b,c,d - d is not used in the formula, but an x is used which doesn't exist as 
  // input (so is treated as x = 0) - so we should get b + c
  formula_N_M->setFormula("y = a*x + b + c");
  formula_N_M->setInputVariables("a,b,c,d");
  FormulaModule_N_M::process(formula_N_M, ins, outs, 0);
  result &= outs[0] == 3 + 5;

  // check, how the module connectivity behaves when inputs are added or removed...

  formula_N_M->setFormula("y = a + b + c + d");
  formula_N_M->setInputVariables("a,b,c,d");
  
  cm->addAudioConnection(formula_N_M, 0, audioOutput1, 0);
  cm->addAudioConnection(constant2, 0, formula_N_M,  0);  // a = 2
  cm->addAudioConnection(constant3, 0, formula_N_M,  1);  // b = 3
  cm->addAudioConnection(constant5, 0, formula_N_M,  2);  // c = 5
  cm->addAudioConnection(constant7, 0, formula_N_M,  3);  // d = 7

  std::vector<AudioConnection> inCons;
  inCons = formula_N_M->getIncomingAudioConnections();
  RAPT::rsAssert(inCons.size() == 4);
  result &= inCons.size() == 4;
  result &= inCons[0].getSourceModule() == constant2;
  result &= inCons[1].getSourceModule() == constant3;
  result &= inCons[2].getSourceModule() == constant5;
  result &= inCons[3].getSourceModule() == constant7;

  // remove 2nd input variable ("b") - this should result in the connection from 3 to b 
  // (pinIndex 1) to disappear, instead 5 should now be connected to pin 1 and 7 to pin 2:
  formula_N_M->setInputVariables("a,c,d");
  inCons = formula_N_M->getIncomingAudioConnections();
  RAPT::rsAssert(inCons.size() == 3);
  result &= inCons.size() == 3;
  result &= inCons[0].getSourceModule() == constant2;
  result &= inCons[1].getSourceModule() == constant5;
  result &= inCons[2].getSourceModule() == constant7;


  //formula_N_1->setInputVariables("d,a,c"); // permute inputs, see if connections get permuted accordingly

  // test formula with mutiple outputs:
  formula_N_M->setInputVariables("a,b,c,d");
  formula_N_M->setOutputVariables("x,y,z");
  formula_N_M->setFormula("x = a + b; y = b + c; z = c + d");
  FormulaModule_N_M::process(formula_N_M, ins, outs, 0);
  result &= outs[0] == 2 + 3;
  result &= outs[1] == 3 + 5;
  result &= outs[2] == 5 + 7;

  // connect and check the outputs of the multi-out formula:
  cm->addAudioConnection(formula_N_M, 0, audioOutput1, 0);
  cm->addAudioConnection(formula_N_M, 1, audioOutput2, 0);
  cm->addAudioConnection(formula_N_M, 2, audioOutput3, 0);

  std::vector<AudioConnection> outCons;
  outCons = formula_N_M->getOutgoingAudioConnections();
  RAPT::rsAssert(outCons.size() == 3);
  result &= outCons.size() == 3;
  result &= outCons[0].getTargetModule() == audioOutput1;
  result &= outCons[1].getTargetModule() == audioOutput2;
  result &= outCons[2].getTargetModule() == audioOutput3;

  // remove the middle output variable y, check if x and z outputs stay connected to their
  // respective target modules/pins:
  formula_N_M->setOutputVariables("x,z");
  outCons = formula_N_M->getOutgoingAudioConnections();
  RAPT::rsAssert(outCons.size() == 2);
  result &= outCons.size() == 2;
  result &= outCons[0].getTargetModule() == audioOutput1;
  result &= outCons[1].getTargetModule() == audioOutput3;

  // check, if the output variables can be used as memory:
  formula_N_M->setInputVariables("x");
  formula_N_M->setOutputVariables("y");
  formula_N_M->setFormula("y = y + x");  // a simple integrator / running sum
  formula_N_M->resetStateForAllVoices();
  FormulaModule_N_M::process(formula_N_M, ins, outs, 0);
  result &= outs[0] == 2;
  FormulaModule_N_M::process(formula_N_M, ins, outs, 0);
  result &= outs[0] == 4;
  FormulaModule_N_M::process(formula_N_M, ins, outs, 0);
  result &= outs[0] == 6;

  // maybe try a biquad, then a lorenz-system - we somehow need a way to initialize all variables
  // (to zero, or better, to user-defined values)



  // todo: do real-world tests with liberty, introduce memory variables (in
  // FormulaModule_1_1)

  moduleFactory.deleteModule(cm);
  return result;
}