#include "rojue_RComboBox.h"
using namespace rojue;


//=========================================================================================================================================
// class RComboBox:

//-----------------------------------------------------------------------------------------------------------------------------------------
// construction/destruction:

RComboBox::RComboBox(const String& name) : RTextField(name)
{
  popUpMenu = new RPopUpMenu(this);
  popUpMenu->registerPopUpMenuObserver(this);
  // popUpMenu->setAttachPosition(BELOW); // later
  addChildWidget(popUpMenu, false, false);
  setNoBackgroundAndOutline(false);
  dontOpenPopUpOnNextMouseClick = false;;
}

RComboBox::~RComboBox()
{
  popUpMenu->deRegisterPopUpMenuObserver(this);
  closePopUp();
  delete popUpMenu;
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// setup:

void RComboBox::addItem(int itemResultId, const juce::String& itemText, bool isEnabled, bool isTicked)
{
  popUpMenu->addItem(itemResultId, itemText, isEnabled, isTicked);
}

void RComboBox::setItemEnabled(int index, bool shouldBeEnabled)
{
  popUpMenu->setItemEnabled(index, shouldBeEnabled);
}

void RComboBox::setItemText(int index, const juce::String& newText)
{
  popUpMenu->setItemText(index, newText);
}

void RComboBox::clear(const bool dontSendChangeMessage)
{
  popUpMenu->clear();
}

void RComboBox::selectItemByIndex(int indexToSelect, bool sendNotification)
{
  jassert( indexToSelect >= 0 && indexToSelect < popUpMenu->getNumItems() );  // index out of range
  if( indexToSelect < 0 || indexToSelect >= popUpMenu->getNumItems() )
    return;

  popUpMenu->selectItemByIndex(indexToSelect, false);

  if( assignedParameter != NULL )
    assignedParameter->setValue((double) indexToSelect, sendNotification, sendNotification);

  setText(getItemText(indexToSelect));

  if( sendNotification == true )
    sendComboBoxChangeNotifications();

  repaint(); 
}

void RComboBox::selectItemFromText(const juce::String& textToSelect, bool sendNotification)
{
  for(int i=0; i<popUpMenu->getNumItems(); i++)
  {
    if( getItemText(i) == textToSelect )
    {
      selectItemByIndex(i, sendNotification);
      return;
    }
  }
  jassertfalse; // the passed text is not among the items
}

void RComboBox::setStateFromString(const juce::String &stateString, bool sendChangeMessage)
{
  selectItemFromText(stateString, sendChangeMessage);
}

void RComboBox::registerComboBoxObserver(RComboBoxObserver* const observerToRegister)
{
  comboBoxObservers.addIfNotAlreadyThere(observerToRegister);
}

void RComboBox::deRegisterComboBoxObserver(RComboBoxObserver* const observerToDeRegister)
{
  comboBoxObservers.removeValue(observerToDeRegister);
}

void RComboBox::assignParameter(Parameter* parameterToAssign)
{
  //RWidget::assignParameter(parameterToAssign);
  if( assignedParameter != NULL )
    assignedParameter->deRegisterParameterObserver(this);
  assignedParameter = NULL;
  clear(true); 
  assignedParameter = parameterToAssign;
  if( assignedParameter != NULL )
  {
    assignedParameter->registerParameterObserver(this);
    jassert( assignedParameter->isStringParameter() ); // trying to assign a non-string parameter to a combobox
    for(int i=0; i<assignedParameter->getNumStringValues(); i++)
      addItem(i, assignedParameter->getOptionStringAtIndex(i));
    updateWidgetFromAssignedParameter(false);
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// inquiry:

const String RComboBox::getItemText(const int index) const
{
  jassert( index >= 0 && index < popUpMenu->getNumItems() );  // index out of range
  if( index >= 0 && index < popUpMenu->getNumItems() )
    return popUpMenu->getItemByIndex(index)->getNodeText();
  else
    return String::empty;
}

int RComboBox::getSelectedItemIdentifier() const
{
  RTreeViewNode* selectedItem = getSelectedItem();
  if( selectedItem != NULL )
    return selectedItem->getNodeIdentifier();
  else
    return -1;
}

juce::String RComboBox::getSelectedItemText() const
{
  RTreeViewNode* selectedItem = getSelectedItem();
  if( selectedItem != NULL )
    return selectedItem->getNodeText();
  else
    return juce::String(T("nothing selected"));
}

juce::String RComboBox::getStateAsString() const
{
  return getSelectedItemText();
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// callbacks:

void RComboBox::rPopUpDismissedByClickOnOwner(ROwnedPopUpComponent *popUp)
{
  dontOpenPopUpOnNextMouseClick = true;
}

void RComboBox::rPopUpMenuChanged(RPopUpMenu* menuThatHasChanged)
{
  if( menuThatHasChanged == popUpMenu )
  {
    closePopUp();
    //sendComboBoxChangeNotifications();
    selectItemFromText(popUpMenu->getSelectedItem()->getNodeText(), true);
    repaint();
  }
}

void RComboBox::updateWidgetFromAssignedParameter(bool sendNotification)
{
  if( assignedParameter != NULL )
    selectItemByIndex((int) assignedParameter->getValue(), sendNotification);
}

void RComboBox::mouseDown(const MouseEvent& e)
{
  if( isEnabled() && e.eventComponent == this )
  {
    if( dontOpenPopUpOnNextMouseClick == false )
      openPopUp();
    dontOpenPopUpOnNextMouseClick = false;
  }
}

void RComboBox::paint(Graphics& g)
{
  g.fillAll(getBackgroundColour());
  g.setColour(getOutlineColour());
  g.drawRect(0, 0, getWidth(), getHeight(), 2);

  int x = 4;  
  int y = getHeight()/2 - boldFont10px.getFontAscent()/2;  

  drawBitmapFontText(g, x, y, getSelectedItemText(), &boldFont10px, getTextColour());

  //grayOutIfDisabled(g);
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// others:

void RComboBox::sendComboBoxChangeNotifications()
{
  for(int i=0; i<comboBoxObservers.size(); i++)
    comboBoxObservers[i]->rComboBoxChanged(this);
}

void RComboBox::openPopUp()
{
  popUpMenu->setSize(getWidth(), popUpMenu->getRequiredHeight(true));  
    // maybe introduce a max-height or something - it's strange but if don't set the size here, the popup appears black
  //popUpMenu->showAttachedTo(this, true, RPopUpComponent::BELOW, getWidth(), popUpMenu->getRequiredHeight(), 0, -outlineThickness);
  popUpMenu->show(true, RPopUpComponent::BELOW, getWidth(), popUpMenu->getRequiredHeight(true), 0, -outlineThickness);
}

void RComboBox::closePopUp()
{
  popUpMenu->dismiss();
}


//=========================================================================================================================================
// class RNamedComboBox:

//-----------------------------------------------------------------------------------------------------------------------------------------
// construction/destruction:

RNamedComboBox::RNamedComboBox(const String& componentName, const String& comboBoxName)
: RComboBox(componentName)
{
  addAndMakeVisible( nameLabel = new RTextField(comboBoxName) );
  nameLabel->setNoBackgroundAndOutline(true);
  nameLabel->setJustification(Justification::centredLeft);
  nameLabelWidth = rojue::boldFont10px.getTextPixelWidth(comboBoxName, rojue::boldFont10px.getDefaultKerning()) + 8;
  nameLabelPosition = LEFT_TO_BOX;
}

RNamedComboBox::~RNamedComboBox()
{
  deleteAllChildren();
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// setup:

void RNamedComboBox::setColourScheme(const WidgetColourScheme& newColourScheme)
{
  RComboBox::setColourScheme(newColourScheme);
  nameLabel->setColourScheme(newColourScheme);
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// callbacks:

void RNamedComboBox::paint(Graphics &g)
{
  int x, y, w, h;
  if( nameLabelPosition == LEFT_TO_BOX )
  {
    x = nameLabel->getRight();
    y = 0;
    w = getWidth()-x;
  }
  else  // above
  {
    x = 0;
    y = getHeight()/2;
    w = getWidth()-x;
  }
  h = nameLabel->getHeight();

  g.setColour(getBackgroundColour());
  g.fillRect(x, y, w, h);
  g.setColour(getOutlineColour());
  g.drawRect(x, y, w, h, 2);

  if( nameLabelPosition == LEFT_TO_BOX )
  {
    x = nameLabel->getRight()+4; 
    y = getHeight()/2 - boldFont10px.getFontAscent()/2;  
  }
  else
  {
    x = 4;
    y = 3*getHeight()/4 - boldFont10px.getFontAscent()/2;  
  }

  drawBitmapFontText(g, x, y, getSelectedItemText(), &boldFont10px, getTextColour());

  //if( !isEnabled() )
  //  g.fillAll(Colours::lightgrey.withAlpha(0.75f));
}

void RNamedComboBox::resized()
{
  if( nameLabelPosition == LEFT_TO_BOX )
    nameLabel->setBounds(0, 0, nameLabelWidth, getHeight());
  else if( nameLabelPosition == ABOVE_BOX )
    nameLabel->setBounds(0, 0, nameLabelWidth, getHeight()/2);
  RComboBox::resized();
}

//-----------------------------------------------------------------------------------------------------------------------------------------
// others:

void RNamedComboBox::openPopUp()
{
  int x = 0;
  int w = getWidth();
  if( nameLabelPosition == LEFT_TO_BOX )
  {
    x += nameLabelWidth+4;
    w -= nameLabelWidth+4;
  }
  popUpMenu->setSize(w, popUpMenu->getRequiredHeight(true));  

  popUpMenu->show(false, RPopUpComponent::BELOW, w, popUpMenu->getRequiredHeight(true), x, -outlineThickness);
  //popUpMenu->show(true, RPopUpComponent::BELOW, w, popUpMenu->getRequiredHeight(), x, -outlineThickness); 
    // true for the 1st argument (showModally) causes problems - why? reproduce: in EngineersFilter open/close the same combobox twice
}
