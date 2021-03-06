#ifndef __JUCE_RSPLOTDATASETUP_JUCEHEADER__
#define __JUCE_RSPLOTDATASETUP_JUCEHEADER__

#include "../JuceLibraryCode/JuceHeader.h"
using namespace juce;
using namespace jura;

//class RSPlotDataSetup	: public Component
class RSPlotDataSetup	: public Editor
{

public:

  RSPlotDataSetup();
  /**< Constructor. */

  virtual ~RSPlotDataSetup();
  /**< Destructor. */

  //virtual void paint(Graphics& g) override;
  virtual void resized() override;

  //=============================================================================================
  juce_UseDebuggingNewOperator;

  RClickButton* loadButton;
  RButton*      tExponentialSpacingButton;


  // the widgets are public in order to let the outlying 
  // RSPlotContentComponent listen to them and access them:
  Label*            typeOfDataLabel;
  ComboBox*         typeOfDataComboBox;
  Label*            dataSourceLabel;
  ComboBox*         dataSourceComboBox;

  // for entering the strings for the curve-family:
  Label*            numCurvesLabel;
  Slider*           numCurvesSlider;
  Label*            xCurveLabel;
  RTextEntryField*  xCurveEditLabel;
  Label*            yCurveLabel;
  RTextEntryField*  yCurveEditLabel;

  // for the parameters:
  Label*            aLabel;
  Label*            aMinEditLabel;
  Slider*           aSlider;
  Label*            aMaxEditLabel;
  Label*            bLabel;
  Label*            bMinEditLabel;
  Slider*           bSlider;
  Label*            bMaxEditLabel;
  Label*            cLabel;
  Label*            cMinEditLabel;
  Slider*           cSlider;
  Label*            cMaxEditLabel;
  Label*            dLabel;
  Label*            dMinEditLabel;
  Slider*           dSlider;
  Label*            dMaxEditLabel;

  // for the minimum and maximum value of the parameter t
  Label*            samplingLabel;
  Label*            tMinLabel;
  RTextEntryField*  tMinEditLabel;
  Label*            tMaxLabel;
  RTextEntryField*  tMaxEditLabel;
  //ToggleButton*     tExponentialSpacingButton;

  Label*            quickCalcLabel;
  Label*            quickCalcInputLabel;
  Label*            quickCalcOutputLabelX;
  Label*            quickCalcOutputLabelY;

  // for the nuber of data points at which the curve (or function) will be evaluated:
  Label*            numSamplesLabel;
  Slider*           numSamplesSlider;

  Label*            fileLabel;
  Label*            fileNameLabel;
  //TextButton*       loadButton;

  // for generating or importing data:
  //ToggleButton*     importDataButton;
  //ToggleButton*     generateDataButton;

  virtual void setMinMaxA(double newMin, double newMax);
  /**< Sets the minimum and maximum value for the a-parameter. */

  virtual void setMinMaxB(double newMin, double newMax);
  /**< Sets the minimum and maximum value for the b-parameter. */

  virtual void setMinMaxC(double newMin, double newMax);
  /**< Sets the minimum and maximum value for the c-parameter. */

  virtual void setMinMaxD(double newMin, double newMax);
  /**< Sets the minimum and maximum value for the d-parameter. */

  virtual void setMinMaxT(double newMin, double newMax);
  /**< Sets the minimum and maximum value for the t-parameter. */
};

#endif	
