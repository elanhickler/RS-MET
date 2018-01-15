#include "UnitTestsView.h"
using namespace jura;

UnitTestsView::UnitTestsView()
{
  createWidgets();
}

void UnitTestsView::runTest(int testIndex)
{
  juce::Array<UnitTest*> tests;

  tests.add(new UnitTestParameter);

  //beginTest();
  runTests(tests);

  // do we need to clear the array? ..or maybe use OwnedArray?
}

void UnitTestsView::resized()
{
  int m  = 4;   // margin
  int x  = m;
  int y  = m;

  runTestsLabel  ->setBounds(x, y,  76, 20); x +=  76+m;
  testSelectorBox->setBounds(x, y, 120, 20); x += 120+m;
  runButton      ->setBounds(x, y,  40, 20); x +=  40+m;

  x = 0;
  y = runButton->getBottom() + m;
  int w = getWidth();
  int h = getHeight();
  testResultView->setBounds(x, y, w, h);
}

void UnitTestsView::rButtonClicked(RButton* b)
{
  if(b == runButton)
    runTest(ALL); // todo: inquire combobox, which test to run
}

void UnitTestsView::createWidgets()
{
  runTestsLabel  = new RTextField("Select Test:");
  addWidget(runTestsLabel);

  testSelectorBox = new RComboBox;
  testSelectorBox->addItem(ALL,        "All Tests");
  testSelectorBox->addItem(PARAMETERS, "Parameters");
  //testSelectorBox->addItem(WIDGETS, "Widgets");
  addWidget(testSelectorBox);

  runButton = new RClickButtonNotifyOnMouseUp("Run");
  runButton->addRButtonListener(this);
  addWidget(runButton);

  testResultView = new RTextEditor;
  addWidget(testResultView);

  // todo: add descriptions
}

