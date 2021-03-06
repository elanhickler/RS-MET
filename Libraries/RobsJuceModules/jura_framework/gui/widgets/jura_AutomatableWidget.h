#ifndef jura_AutomatableWidget_h
#define jura_AutomatableWidget_h

// rename file

class rsModulationConnectionWidget;
class rsAutomatableWidget;
//class rsNodeEditor;

/** Baseclass for rsAutomationSetup and rsModulationSetup. */

class JUCE_API rsParameterSetupBase : public ColourSchemeComponent, public RButtonListener, 
  public rsDeletionRequester, public rsGarbageCollector
{

public:

  rsParameterSetupBase(rsAutomatableWidget* widgetToModulate, MetaParameterManager* metaManager);
  virtual ~rsParameterSetupBase() {}

protected:

  rsAutomatableWidget* widget;                 // our owner widget
  MetaParameterManager* metaManager;
  RClickButtonNotifyOnMouseUp* closeButton; 

  static const int sliderHeight = 16, sliderDistance = 4;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(rsParameterSetupBase)
};

//=================================================================================================

/** Customized subclass of rsNodeBasedFunctionEditor to incorporate additional features such as 
showing a vertical line at the current position of the meta-value. */ 

class JUCE_API rsMetaMapEditor : public rsNodeBasedFunctionEditor
{

public:

  rsMetaMapEditor(MetaParameterManager* metaManagerToUse);

  virtual ~rsMetaMapEditor();

  /** Sets up the parameter, whose meta-mapping curve we control. */
  virtual void setParameterToControl(MetaControlledParameter* p);

  /** Returns a pointer to the meta-parameter that is assigned to the parameter whose meta-mapping 
  curve this editor controls. May be a nullptr, if none is assigned. */
  MetaParameter* getAttachedMetaParameter();

  // overrides:
  virtual void paint(Graphics& g) override;

  virtual void parameterChanged(Parameter* p) override; // overriden to update vertcial line

  // overriden to update parameter value when nodes are edited:
  virtual int addNode(double pixelX, double pixelY) override;
  virtual bool removeNode(int index) override;
  virtual int moveNodeTo(int index, int pixelX, int pixelY) override;
  //virtual int nodeChanged(int nodeIndex) override; // seems to be enough to override moveNodeTo


  /** Overriden to register ourselves as listener for the node's x,y- and shape parameters. */
  virtual rsNodeBasedFunctionEditor::NodeParameterSet* 
    addNodeParameters(rsDraggableNode* node) override;

protected:

  /** Called from our overriden addNodeParameters function to additionally register ourselves
  as listener to the node-related parameters such as coordinates and shape settings. We need to
  observe these parameters in order to call updateParameter on any change of such a paremeter such
  that changes become immediately audible. */
  void setupNodeParameterObservation(rsDraggableNode* node, NodeParameterSet* params);

  /** Called from functions that change the mapping function (by adding/removing/moving nodes) to 
  update the dependent parameter according to the new mapping function. */
  void updateParameter();

  MetaControlledParameter* param;     // parameter, whose meta-map we control
  MetaParameterManager* metaManager;  // actually, this is redundant bcs "param" knows *its* 
                                      // meta-manager, which is the same - maybe remove
};

//=================================================================================================

/** A component for setting up the the connection of a parameter to a meta-parameter, the mapping
and smoothing. 
todo: maybe handle midi controller assignment here, too
*/

class JUCE_API rsAutomationSetup : public rsParameterSetupBase, public RComboBoxObserver,
  public rsNodeEditorObserver
{

public:

  rsAutomationSetup(rsAutomatableWidget* widgetToAutomate, MetaParameterManager* metaManager);
  virtual ~rsAutomationSetup();

  virtual void rButtonClicked(RButton *button) override;
  virtual void rComboBoxChanged(RComboBox* cb) override;
  virtual void resized() override;
  virtual void setVisible(bool shouldBeVisible) override;

  virtual void nodeWasAdded(     rsNodeEditor* editor, int nodeIndex) override;
  virtual void nodeWillBeRemoved(rsNodeEditor* editor, int nodeIndex) override;
  virtual void nodeWasSelected(  rsNodeEditor* editor, int nodeIndex) override;

protected:

  /** Shows the popup menu with the attachable MetaParameters. */
  void showMetaAttachPopUp();

  /** (Re)assigns our widgets to manipulate node parameters to the parameter-objects in the node
  with given index. */
  void assignNodeParameterWidgets(int nodeIndex);



  /** Creates our widgets. Called in constructor. */
  void createWidgets();

  /** Makes the node parameter widgets (sliderX, sliderY, etc.) in/visible, depending on whether 
  or not a node is currently selected. */
  void updateWidgetVisibility();

  rsMetaMapEditor* metaMapEditor;
  RNamedComboBox* boxMetaAttach;
  RSlider *sliderSmoothing;  // smoothing time

  RNamedComboBox* boxShapeType;
  RSlider *sliderNodeX, *sliderNodeY, *sliderShapeParam;

  // add a slider to set up the meta-value/parameter-value

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(rsAutomationSetup)
};


//=================================================================================================

/** A component for setting up the modulations of some ModulationTarget. */

class JUCE_API rsModulationSetup : public rsParameterSetupBase, public RTextEntryFieldObserver,
  public RPopUpMenuObserver, public ModulationTargetObserver
{

public:

  /** Constructor. You must pass the widget that you want to assign modulations to (actually, they
  will be assigned to its underlying Parameter assumed to be a ModulatableParameter) and you must 
  also pass a pointer to the MetaParameterManager object that should be used for attaching a
  meta-parameter to the modulation-depths of the connections to be made. */
  rsModulationSetup(rsAutomatableWidget* widgetToModulate, MetaParameterManager* metaManager);

  /** Destructor. */
  virtual ~rsModulationSetup();

  // callbacks:
  //virtual void paint(Graphics& g) override;
  virtual void resized() override;
  virtual void rButtonClicked(RButton *button) override;
  virtual void rPopUpMenuChanged(RPopUpMenu* menuThatHasChanged) override;
  virtual void textChanged(RTextEntryField *rTextEntryFieldThatHasChanged) override;
  virtual void modulationsChanged() override;

protected:

  /** Adds a modulation connection between the ModulationTarget (underlying our widget member) and 
  the source with given index among the available and not yet connected sources. */
  void addConnection(int index);

  /** Removes a modulation connection between the ModulationTarget (underlying our widget member) 
  and the source with given index among the connected sources. */
  void removeConnection(int index);

  /** Creates or removes widget-sets (depth-sliders, etc.) for the modulation the existing 
  moduldation connections. */
  void updateConnectionWidgetsArray();

  /** Shows the popup menu with the available (and not yet connected) ModulationSources. */
  void showConnectableSourcesPopUp();

  /** Shows the popup menu with the connected ModulationSources (for removal). */
  void showRemovableSourcesPopUp();

  /** Returns true, if we have a slider associated with the passed parameter. */
  bool hasSlider(MetaControlledParameter* p);

  /** Adds a slider for the passed parameter to our amountSliders array. You must also pass a 
  pointer to the ModulationConnection whose depth this parameter controls so the slider's popup
  menu can access it in order to set up some other aspects of the connection as well. */
  void addWidgetsForConnection(ModulationConnection* c); 

  /** Removes the widgets for the connection with given index. */
  //void removeWidgetsForConnection(int index); 

  /** Clears the array of connections widgets. */
  void clearConnectionWidgets();

  /** Updates the size in order to provide space for all required widgets. */
  void updateSize();

  // functions for getting/setting the clipping limits:
  void setClipMin(double newMin);
  void setClipMax(double newMax);
  double getClipMin();
  double getClipMax();

  //ModulationManager*    modManager;  // needed for debugging

  // owned widgets:
  RTextField* modulationsLabel; // rename, move to baseclass
  std::vector<rsModulationConnectionWidget*> connectionWidgets;

  RButton *addButton, *removeButton;
  RLabeledTextEntryField *clipMinField, *clipMaxField; // maybe move to baseclass

  RPopUpMenu *connectableSourcesPopUp = nullptr; // created when needed the first time
  RPopUpMenu *removableSourcesPopUp   = nullptr; // ditto

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(rsModulationSetup)
};

//=================================================================================================

/** RWidget subclass that adds automation facilities either via MIDI or the host automation system 
using setParameter(). If you want to make a widget automatable, derive it from some widget class 
and also from this class, for example, like: 
class JUCE_API rsAutomatableSlider : public RSlider, public rsAutomatableWidget
\todo: maybe move into jura_processors module (it's relevant only for audio plugins). 

maybe rename to something that fits better (we now have also lumped in the modulation stuff)

*/

class JUCE_API rsAutomatableWidget : virtual public RPopUpMenuObserver, public rsDeletor
{

public:

  rsAutomatableWidget(RWidget *widgetToWrap);

  virtual ~rsAutomatableWidget();

  /** Returns true if the popup menu is currently open, false otherwise. */
  bool isPopUpOpen();

  /** Tries to cast the Parameter that is underlying the wrapped widget into an 
  AutomatableParameter and returns the pointer to it. Note that this may return a nullptr, when 
  the Parameter is not of type AutomatableParameter. */
  AutomatableParameter* getAutomatableParameter();

  /** Similar to @see getAutomatbleParameter. */
  MetaControlledParameter* getMetaControlledParameter();

  /** Similar to @see getAutomatbleParameter. */
  ModulatableParameter* getModulatableParameter();

  /** Returns the name of the underlying parameter. */
  String getParameterName();

  /** Returns a pointer to the MetaParameterManager that is used by the underlying parameter, if 
  any - a nullptr otherwise. */
  MetaParameterManager* getMetaParameterManager();

  /** Returns a pointer to the wrapped widget. */
  RWidget* getWrappedWidget() const { return  wrappedWidget; }

  // overrides:
  void rPopUpMenuChanged(RPopUpMenu* menuThatHasChanged) override;
  void rPopUpMenuDismissed(RPopUpMenu* menuThatwasDismissed) override;


  /** Shows the setup for the automation (attachment to meta parameters, mapping functions, 
  midi controllers and smoothing). */
  virtual void showAutomationSetup();

  /** Shows the setup for the modulation connections. */
  virtual void showModulationSetup();




protected:

  /** Enumeration of the identifiers to used as return-values for the right-click popup menu. */
  enum popUpIds
  {
    ENTER_VALUE = 1,  // nope - these two should be available in the slider baseclass already
    DEFAULT_VALUE,    

    MIDI_ASSIGN,
    MIDI_LEARN,
    MIDI_MIN,
    MIDI_MAX,
    MIDI_REVERT,

    META_ATTACH,
    META_DETACH,
    META_ATTACH_FLAT,
    AUTOMATION_SETUP,

    MODULATION_SETUP
  };

  /** Clears the popup-menu and then calls createPopUpMenuItems() */
  virtual void updatePopUpMenu();

  /** Populates the right-click popup menu with items, according to the settings of this RSlider. */
  virtual void addPopUpMenuItems();
  // rename to populateRightClickPopupMenu...or something

  /** Adds the MIDI related items to the popoup menu (if applicable). */
  virtual void addPopUpMidiItems();

  /** Adds the MetaParameter related items to the popup menu (if applicable). */
  virtual void addPopUpMetaItems();

  /** Adds the Modulation related items to the popup menu (if applicable). */
  virtual void addPopUpModulationItems();

  /** Opens the PopupMenu that appears on right clicks. */
  virtual void openRightClickPopupMenu();

  /** Closes the popup menu. */
  virtual void closePopUp();



  /** This is called from the modulation setup popup window when the user clicks on its 
  close-button. We don't actually delete the object here, we just make it invisible, so we can 
  reuse the same object when the modulation setup is opened again. It's deleted in our 
  destructor. */
  virtual void deleteObject(rsDeletionRequester* objectToDelete) override;

  RWidget *wrappedWidget;                  // widget that is being made automatable
  bool popUpIsOpen = false;                // maybe we can get rid of this?
  RPopUpMenu *rightClickPopUp  = nullptr;  // object created when it's needed for the 1st time
  rsModulationSetup* modSetup  = nullptr;  // ditto for modulation setup
  rsAutomationSetup* metaSetup = nullptr;  // ditto

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(rsAutomatableWidget)
};

//=================================================================================================

class JUCE_API rsAutomatableSlider : public RSlider, public rsAutomatableWidget
{

public:

  rsAutomatableSlider();
  virtual void rPopUpMenuChanged(RPopUpMenu* menuThatHasChanged) override;
  virtual void mouseDown(const MouseEvent& e) override;

protected:

  virtual void addPopUpMenuItems() override;
  virtual void addPopUpEnterValueItem();
  virtual void addPopUpDefaultValueItems();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(rsAutomatableSlider)
};


class JUCE_API rsAutomatableComboBox : public RComboBox, public rsAutomatableWidget
{

public:

  rsAutomatableComboBox();
  virtual void mouseDown(const MouseEvent& e) override;
  virtual void parameterChanged(Parameter* p) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(rsAutomatableComboBox)
};

class JUCE_API rsAutomatableButton : public RButton, public rsAutomatableWidget
{

public:

  rsAutomatableButton(const juce::String& buttonText = juce::String());
  virtual void mouseDown(const MouseEvent& e) override;
  virtual void parameterChanged(Parameter* p) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(rsAutomatableButton)
};

//=================================================================================================

/** A special slider subclass specifically for being used for the depth-sliders in a modulation
setup. They need some special additional options in their popup menu such as facilities to set 
min/max values and the modulation mode. */

class JUCE_API rsModulationDepthSlider : public rsAutomatableSlider
{

public:

  /** Constructor. You must pass a pointer to the ModulationConnection object on which this slider
  operates such that we can use our popup menu to set up the modulation mode (absolute vs 
  relative), too. */
  rsModulationDepthSlider(ModulationConnection* connection) : modConnection(connection) 
  {
    setStringConversionFunction(&valueToStringTotal5);
  }

  virtual ~rsModulationDepthSlider() {}

  virtual void rPopUpMenuChanged(RPopUpMenu* menuThatHasChanged) override;

  virtual void addPopUpMenuItems() override;
  virtual void addPopUpMinMaxAndModeItems();

protected:

  // for getting and setting the min/max values of the mod-depth slider and for the clipping of the
  // modulated value and getting/setting relative mode:
  inline double getModDepthMin() { return assignedParameter->getMinValue(); }
  inline double getModDepthMax() { return assignedParameter->getMaxValue(); }
  inline int    getModMode()     { return modConnection->getMode(); }

  inline void setModDepthMin(double newMin)  { assignedParameter->setMinValue(newMin); }
  inline void setModDepthMax(double newMax)  { assignedParameter->setMaxValue(newMax); }
  inline void setModMode(    int    mode)    { modConnection->setMode(mode); }

  /** Additional item ids for this subclass. */
  enum popUpIds2
  {
    MOD_DEPTH_MIN = popUpIds::MODULATION_SETUP+1,
    MOD_DEPTH_MAX,
    MOD_MODE_ABSOLUTE,
    MOD_MODE_RELATIVE,
    MOD_MODE_EXPONENTIAL,
    MOD_MODE_MULTIPLICATIVE
  };

  ModulationConnection* modConnection = nullptr; // needs to be assigned in constructor

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(rsModulationDepthSlider)
};

//=================================================================================================

/** A class that encapsulates the depth-slider and remove-button for a ModulationConnection. 

\todo:
-add an on/off button, maybe also solo - i think, this needs a new member in ModulationConnection
 isActive ...hmm...but this will cause overhead...we'll see
*/

class JUCE_API rsModulationConnectionWidget : public RWidget, public rsDeletionRequester
{

  friend class rsModulationSetup;

public:

  rsModulationConnectionWidget(ModulationConnection* connection, rsGarbageCollector* deletor);

  virtual ~rsModulationConnectionWidget() {}

  void resized() override;
  void paint(Graphics& g) override {}

protected:

  rsModulationDepthSlider* depthSlider;
  RClickButtonNotifyOnMouseUp* removeButton;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(rsModulationConnectionWidget)
};

//=================================================================================================

/** A slider subclass specifically for sliders that have modulatable parameters assigned. It may
paint itself differently, depending on the modulations that are applied to its underlying 
parameter. 

\todo:
-let it regularly repaint itself, showing the value of the parameter with modulations applied
-for this, make a class RepaintManager (subclass of juce::Timer) where repaintable widgets can
 register themselves, this should have functions: setRepaintInterval(int milliseconds), 
 registerRepaintee(Component* c), deRegisterRepaintee(Component* c), 
-maybe the Repaintee class should be a subclass of component and have a method needsRepaint that 
 checks, if there are incoming modulation connection and returns false, if not to avoid repainting
 when it's not necessarry)...but that would make it less flexible than passing just a 
 juce::Component, ...hmm.....
-RepaintManager could also be used for metering widgets
-hmm...maybe the needsRepaint or needsPeriodicReapint or similar can be done on the level of 
 RWidget (return false by default and overriden by subclasses)...maybe RWidget can have an array
 of flags for such things
*/

class JUCE_API rsModulatableSlider : public rsAutomatableSlider, public ModulationTargetObserver
{

public:

  rsModulatableSlider();
  virtual ~rsModulatableSlider();

  void modulationsChanged() override;
  void assignParameter(Parameter* parameterToAssign) override;
  void paint(Graphics& g) override;

  bool hasModulation();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(rsModulatableSlider)
};

//=================================================================================================

class JUCE_API rsModulatableSliderAnimated : public rsModulatableSlider, public rsRepaintClient
{

public:

  rsModulatableSliderAnimated();
  virtual ~rsModulatableSliderAnimated();

  void modulationsChanged() override;
  bool needsRepaint() override { return hasModConnections; }
  void paint(Graphics& g) override;

protected:

  bool hasModConnections = false;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(rsModulatableSliderAnimated)
};

#endif   
