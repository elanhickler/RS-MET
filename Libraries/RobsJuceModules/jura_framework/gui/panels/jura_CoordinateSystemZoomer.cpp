
//-------------------------------------------------------------------------------------------------
// construction/destruction:

CoordinateSystemZoomer::CoordinateSystemZoomer()
{
  // init member variables:
  theCoordinateSystem     = NULL;
  supressZoomability      = false;
  scrollBarIsHiddenX      = false;
  scrollBarIsHiddenY      = false;
  zoomerSize              = 16;
  verticalMouseWheelMode  = noResponseToVerticalMouseWheel;

  zoomFactorPerClickX     = 1.25;
  zoomFactorPerClickY     = 1.25;
  shiftPerClickX          = 0.1;
  shiftPerClickY          = 0.1;

  relativeMarginLeft      = 0.0; 
  relativeMarginRight     = 0.0;
  relativeMarginTop       = 0.0;
  relativeMarginBottom    = 0.0;

  // create the horizontal zoom-buttons and ScrollBar:
  addWidget( zoomInButtonX = new RButton(RButton::PLUS) );
  zoomInButtonX->addRButtonListener(this);
  zoomInButtonX->setDescription("Zoom in horizontally");
  zoomInButtonX->setClickingTogglesState(false);

  addWidget( zoomToAllButtonX = new RButton(String("h")) );
  zoomToAllButtonX->addRButtonListener(this);
  zoomToAllButtonX->setDescription(String("Zoom maximally out horizontally"));
  zoomToAllButtonX->setClickingTogglesState(false);

  addWidget( zoomOutButtonX = new RButton(RButton::MINUS) );
  zoomOutButtonX->addRButtonListener(this);
  zoomOutButtonX->setDescription(String("Zoom out horizontally"));
  zoomOutButtonX->setClickingTogglesState(false);
  //zoomOutButtonX->setConnectedEdges(15);

  scrollBarX = new RScrollBar(false);
  scrollBarX->setRangeLimits(0.0, 1.0);
  scrollBarX->addListener(this);
  //scrollBarX->setDescription(String(T("scroll horizontally")));
  addWidget(scrollBarX);

  // create the vertical zoom-buttons and ScrollBar:
  addWidget( zoomInButtonY = new RButton(RButton::PLUS) );
  zoomInButtonY->addRButtonListener(this);
  zoomInButtonY->setDescription(String("Zoom in vertically"));
  zoomInButtonY->setClickingTogglesState(false);

  addWidget( zoomToAllButtonY = new RButton(String("v")) );
  zoomToAllButtonY->addRButtonListener(this);
  zoomToAllButtonY->setDescription(String("Zoom maximally out vertically"));
  zoomToAllButtonY->setClickingTogglesState(false);

  addWidget( zoomOutButtonY = new RButton(RButton::MINUS) );
  zoomOutButtonY->addRButtonListener(this);
  zoomOutButtonY->setDescription(String("Zoom out vertically"));
  zoomOutButtonY->setClickingTogglesState(false);

  scrollBarY = new RScrollBar(true);
  scrollBarY->setRangeLimits(0.0, 1.0);
  scrollBarY->addListener(this);
  //scrollBarY->setDescription(String(T("scroll vertically")));
  addWidget(scrollBarY);

  // create the zoom-to-all button for X- and Y:
  addWidget( zoomToAllButtonXY = new RButton(String("a")) );
  zoomToAllButtonXY->addRButtonListener(this);
  zoomToAllButtonXY->setDescription(String("Zoom maximally out both axes"));
  zoomToAllButtonXY->setClickingTogglesState(false);
}

CoordinateSystemZoomer::~CoordinateSystemZoomer()
{
  deleteAllChildren();
}

//-------------------------------------------------------------------------------------------------
// parameter-settings:

void CoordinateSystemZoomer::setCoordinateSystem(CoordinateSystem *newSystemToBeShown)
{
  // assign the pointer to the new CoordinateSystem:
  theCoordinateSystem = newSystemToBeShown;

  // align the widgets:
  alignWidgetsToCoordinateSystem();

  // update the scrollbar ranges (this sets up out currentMinX, etc. members as well):
  updateScrollbars();
}

void CoordinateSystemZoomer::invalidateCoordinateSystemPointer()
{
  theCoordinateSystem = NULL;
}

void CoordinateSystemZoomer::setWidgetDescriptionField(RTextField* newDescriptionField)
{
  zoomInButtonX->setDescriptionField(newDescriptionField);
  zoomOutButtonX->setDescriptionField(newDescriptionField);
  zoomInButtonY->setDescriptionField(newDescriptionField);
  zoomOutButtonY->setDescriptionField(newDescriptionField);
  zoomToAllButtonX->setDescriptionField(newDescriptionField);
  zoomToAllButtonY->setDescriptionField(newDescriptionField);
  zoomToAllButtonXY->setDescriptionField(newDescriptionField);
}

void CoordinateSystemZoomer::setVerticalMouseWheelMode(int newMode)
{
  verticalMouseWheelMode = newMode;
}

void CoordinateSystemZoomer::setRelativeMargins(double newRelativeMarginLeft,                                        
  double newRelativeMarginRight, double newRelativeMarginTop, double newRelativeMarginBottom)
{
  jassert(newRelativeMarginLeft   >= 0.0);
  jassert(newRelativeMarginRight  >= 0.0);
  jassert(newRelativeMarginTop    >= 0.0);
  jassert(newRelativeMarginBottom >= 0.0);  
  if( newRelativeMarginLeft >= 0.0 )
      relativeMarginLeft = newRelativeMarginLeft;
  if( newRelativeMarginRight >= 0.0 )
    relativeMarginRight = newRelativeMarginRight;
  if( newRelativeMarginTop >= 0.0 )
    relativeMarginTop = newRelativeMarginTop;
  if( newRelativeMarginBottom >= 0.0 )
    relativeMarginBottom = newRelativeMarginBottom;
}


//-------------------------------------------------------------------------------------------------
// callbacks:

void CoordinateSystemZoomer::rButtonClicked(RButton *button)
{
  // check, if we have a valid pointer to a CoordinateSystem:
  if( theCoordinateSystem == NULL ) 
    return;

  if( button == zoomInButtonX )
    zoomX(zoomFactorPerClickX);  
  else if( button == zoomOutButtonX )
    zoomX(1.f/zoomFactorPerClickX);  
  else if( button == zoomToAllButtonX )
    zoomToAllX();
  else if( button == zoomInButtonY )
    zoomY(zoomFactorPerClickY);  
  else if( button == zoomOutButtonY )
    zoomY(1.f/zoomFactorPerClickY);  
  else if( button == zoomToAllButtonY )
    zoomToAllY();
  else if( button == zoomToAllButtonXY )
    zoomToAllXY();
}

void CoordinateSystemZoomer::scrollBarMoved(RScrollBar *scrollBarThatHasMoved, const double newRangeStart)
{
  // check, if we have a valid pointer to a CoordinateSystem:
  if( theCoordinateSystem == NULL ) 
    return;

  double x1, x2, y1, y2, w, h;
  if( scrollBarThatHasMoved == scrollBarX )
  {
    x1 = newRangeStart; // 
    w  = scrollBarX->getCurrentRangeSize();
    x2 = x1+w;
    x1 = transformFromScrollBarCoordinateX(x1);
    x2 = transformFromScrollBarCoordinateX(x2);
    theCoordinateSystem->setCurrentRangeX(x1, x2);
  }
  else if( scrollBarThatHasMoved == scrollBarY )
  {
    y2 = newRangeStart;
    h  = scrollBarY->getCurrentRangeSize();
    y1 = y2+h;
    y1 = transformFromScrollBarCoordinateY(1.0-y1);
    y2 = transformFromScrollBarCoordinateY(1.0-y2);
    theCoordinateSystem->setCurrentRangeY(y1, y2);
  }
}

void CoordinateSystemZoomer::mouseWheelMove(const MouseEvent& e, const MouseWheelDetails &wheel)
{
  // ToDo: the mouseEvent class has also such a thing as horizontal mouse-wheel events - someday
  // we may want to respond to those also...

  if( verticalMouseWheelMode == noResponseToVerticalMouseWheel )
    return; // nothing to do
  else if( verticalMouseWheelMode == forwardToCoordinateSystem )
  {
    if( theCoordinateSystem != NULL )
      theCoordinateSystem->mouseWheelMove(e, wheel);
    return;
  }

  double relativeX =        (double) e.x / (double) theCoordinateSystem->getWidth();
  double relativeY =  1.0 - (double) e.y / (double) theCoordinateSystem->getHeight();
  relativeX = jlimit(0.01, 0.99, relativeX);
  relativeY = jlimit(0.01, 0.99, relativeY);

  if( wheel.deltaY >= 0.f )
  {
    switch( verticalMouseWheelMode )
    {
    case horizontalScrollViaVerticalMouseWheel: shiftX(shiftPerClickX);                 break;
    case verticalScrollViaVerticalMouseWheel:   shiftY(shiftPerClickY);                 break;
    case horizontalZoomViaVerticalMouseWheel:   zoomX(zoomFactorPerClickX, relativeX);  break;
    case verticalZoomViaVerticalMouseWheel:     zoomY(zoomFactorPerClickY, relativeY);  break;
    case zoomViaVerticalMouseWheel:
      {
        zoomX(zoomFactorPerClickX, relativeX);
        zoomY(zoomFactorPerClickY, relativeY); 
      } break;
    } // end of switch( verticalMouseWheelMode )
  } // end of  if( wheelIncrementY > 0.f )
  else if( wheel.deltaY < 0.f )
  {
    switch( verticalMouseWheelMode )
    {
    case horizontalScrollViaVerticalMouseWheel: shiftX(-shiftPerClickX);                    break;
    case verticalScrollViaVerticalMouseWheel:   shiftY(-shiftPerClickY);                    break;
    case horizontalZoomViaVerticalMouseWheel:   zoomX(1.f/zoomFactorPerClickX, relativeX);  break;
    case verticalZoomViaVerticalMouseWheel:     zoomY(1.f/zoomFactorPerClickY, relativeY);  break;
    case zoomViaVerticalMouseWheel:
      {
        zoomX(1.f/zoomFactorPerClickX, relativeX);
        zoomY(1.f/zoomFactorPerClickY, relativeY);
      } break;
    } // end of switch( verticalMouseWheelMode )
  } // end of else if( wheelIncrementY < 0.f )

  // update the scrollbars according to the new zoom-levels:
  updateScrollbars();
}

// forwarding of mouse-events to the CoordinateSystem which are not handled by the zoomer:
void CoordinateSystemZoomer::mouseMove(const MouseEvent& e) 
{ 
  if (theCoordinateSystem != NULL) 
  {
    theCoordinateSystem->mouseMove(e); 
    setMouseCursor(theCoordinateSystem->currentMouseCursor);
  }
}

void CoordinateSystemZoomer::mouseEnter(const MouseEvent& e) 
{ 
  if (theCoordinateSystem != NULL) 
  {
    theCoordinateSystem->mouseEnter(e); 
    setMouseCursor(theCoordinateSystem->currentMouseCursor);
  }
}

void CoordinateSystemZoomer::mouseExit(const MouseEvent& e) 
{ 
  if (theCoordinateSystem != NULL) 
  {
    theCoordinateSystem->mouseExit(e); 
    setMouseCursor(theCoordinateSystem->currentMouseCursor);
  }
}

void CoordinateSystemZoomer::mouseDown(const MouseEvent& e) 
{
  // zoom to all on middle button
  if( e.mods.isMiddleButtonDown() )
    zoomToAllXY();
  else if (theCoordinateSystem != NULL) 
  {
    theCoordinateSystem->mouseDown(e); 
    setMouseCursor(theCoordinateSystem->currentMouseCursor);
  }
}

void CoordinateSystemZoomer::mouseDrag(const MouseEvent& e) 
{ 
  if (theCoordinateSystem != NULL) 
  {
    theCoordinateSystem->mouseDrag(e); 
    setMouseCursor(theCoordinateSystem->currentMouseCursor);
  }
}

void CoordinateSystemZoomer::mouseUp(const MouseEvent& e) 
{ 
  if (theCoordinateSystem != NULL) 
  {
    theCoordinateSystem->mouseUp(e); 
    setMouseCursor(theCoordinateSystem->currentMouseCursor);
  }
}

void CoordinateSystemZoomer::mouseDoubleClick(const MouseEvent& e) 
{ 
  if (theCoordinateSystem != NULL) 
  {
    theCoordinateSystem->mouseDoubleClick(e); 
    setMouseCursor(theCoordinateSystem->currentMouseCursor);
  }
}

void CoordinateSystemZoomer::paint(Graphics &g)
{

}

//-------------------------------------------------------------------------------------------------
// manipulation of the coordinate-system:

void CoordinateSystemZoomer::shiftX(double shiftAmount)
{
  if( theCoordinateSystem == NULL ) 
    return;

  double x1, x2, w;

  // convert the leftmost and rightmost currently visible x-value to normalized coordinates:
  x1 = theCoordinateSystem->getCurrentRangeMinX();
  x2 = theCoordinateSystem->getCurrentRangeMaxX();
  x1 = transformToScrollBarCoordinateX(x1);
  x2 = transformToScrollBarCoordinateX(x2);
  w  = scrollBarX->getCurrentRangeSize();

  // shift in the normalized coordinate domain, thereby make sure to keep the thumb-size rigid and 
  // not exceed the boundaries:
  if( shiftAmount >= 0.0 )
  {
    x2 += shiftAmount;
    x2  = jlimit(0.0, 1.0, x2);
    x1  = x2-w;
    x1  = jlimit(0.0, 1.0, x1);
  }
  else
  {
    x1 += shiftAmount;         // shiftAmount is itself negative
    x1  = jlimit(0.0, 1.0, x1);
    x2  = x1+w;
    x2  = jlimit(0.0, 1.0, x2);
  }

  // convert back to unnormalized coordinates and set up the CoordinateSystem's current range:
  x1 = transformFromScrollBarCoordinateX(x1);
  x2 = transformFromScrollBarCoordinateX(x2);
  theCoordinateSystem->setCurrentRangeX(x1, x2);
}

void CoordinateSystemZoomer::shiftY(double shiftAmount)
{
  if( theCoordinateSystem == NULL ) 
    return;

  double y1, y2, h;

  // convert the leftmost and rightmost currently visible x-value to normalized coordinates:
  y1 = theCoordinateSystem->getCurrentRangeMinY();
  y2 = theCoordinateSystem->getCurrentRangeMaxY();
  y1 = transformToScrollBarCoordinateY(y1);
  y2 = transformToScrollBarCoordinateY(y2);
  h  = scrollBarY->getCurrentRangeSize();

  // shift in the normalized coordinate domain, thereby make sure to keep the thumb-size rigid and 
  // not exceed the boundaries:
  if( shiftAmount >= 0.0 )
  {
    y2 += shiftAmount;
    y2  = jlimit(0.0, 1.0, y2);
    y1  = y2-h;
    y1  = jlimit(0.0, 1.0, y1);
  }
  else
  {
    y1 += shiftAmount;         // shiftAmount is itself negative
    y1  = jlimit(0.0, 1.0, y1);
    y2  = y1+h;
    y2  = jlimit(0.0, 1.0, y2);
  }

  // convert back to unnormalized coordinates and set up the CoordinateSystem's current range:
  y1 = transformFromScrollBarCoordinateY(y1);
  y2 = transformFromScrollBarCoordinateY(y2);
  theCoordinateSystem->setCurrentRangeY(y1, y2);
}

void CoordinateSystemZoomer::zoomX(double zoomFactor, double relativeCenter)
{
  // check, if we have a valid pointer to a CoordinateSystem:
  if( theCoordinateSystem == NULL ) 
    return;

  double x1, x2, center, left, right;

  // convert the leftmost and rightmost currently visible x-value to normalized coordinates:
  x1     = theCoordinateSystem->getCurrentRangeMinX();
  x2     = theCoordinateSystem->getCurrentRangeMaxX();
  x1     = transformToScrollBarCoordinateX(x1);
  x2     = transformToScrollBarCoordinateX(x2);

  // get the current center-value and the deviations from it to the left and right:
  center = x1 + relativeCenter*(x2-x1);
  left   = center-x1;    // deviation to the left
  right  = x2-center;    // deviation to the right

  // obtain the new normalized coordinates for the currently visible range:
  x1     = center - left/zoomFactor;
  x2     = center + right/zoomFactor;

  // convert back to unnormalized coordinates and set up the CoordinateSystem's current range:
  x1     = transformFromScrollBarCoordinateX(x1);
  x2     = transformFromScrollBarCoordinateX(x2);


  //bool tmp = theCoordinateSystem->autoReRenderImage;
  //theCoordinateSystem->autoReRenderImage = false;
  theCoordinateSystem->setCurrentRangeX(x1, x2);
  //theCoordinateSystem->autoReRenderImage = tmp;
    // todo: when the CoordinateSystem itself checks whether or not there was an actual, 
    // range-change, all that stuff except setCurrentRange may be strippedd off


  // update the x-axis scrollbar according to the new zoom-level:
  updateScrollbars();
}

void CoordinateSystemZoomer::zoomY(double zoomFactor, double relativeCenter)
{
  double y1, y2, center, bottom, top;

  // convert the bottommost and topmost currently visible y-value to normalized coordinates:
  y1     = theCoordinateSystem->getCurrentRangeMinY();
  y2     = theCoordinateSystem->getCurrentRangeMaxY();
  y1     = transformToScrollBarCoordinateY(y1);
  y2     = transformToScrollBarCoordinateY(y2);

  // get the current center-value and the deviations from it to the bottom and top:
  center = y1 + relativeCenter*(y2-y1);
  bottom = center-y1;    // deviation to the bottom
  top    = y2-center;    // deviation to the top

  // obtain the new normalized coordinates for the currently visible range:
  y1     = center - bottom/zoomFactor;
  y2     = center + top/zoomFactor;

  // convert back to unnormalized coordinates and set up the CoordinateSystem's current range:
  y1     = transformFromScrollBarCoordinateY(y1);
  y2     = transformFromScrollBarCoordinateY(y2);


  //bool tmp = theCoordinateSystem->autoReRenderImage;
  //theCoordinateSystem->autoReRenderImage = false;
  theCoordinateSystem->setCurrentRangeY(y1, y2);
  //theCoordinateSystem->autoReRenderImage = tmp;
    // todo: when the CoordinateSystem itself checks whether or not there was an actual, 
    // range-change, all that stuff except setCurrentRange may be strippedd off

  // update the x-axis scrollbar according to the new zoom-level:
  updateScrollbars();
}

void CoordinateSystemZoomer::zoomToAllX()
{
  if( theCoordinateSystem == NULL ) 
    return;
  theCoordinateSystem->setCurrentRangeX(theCoordinateSystem->getMaximumRangeMinX(), 
    theCoordinateSystem->getMaximumRangeMaxX());
  updateScrollbars();
}

void CoordinateSystemZoomer::zoomToAllY()
{
  if( theCoordinateSystem == NULL ) 
    return;
  theCoordinateSystem->setCurrentRangeY(theCoordinateSystem->getMaximumRangeMinY(), 
    theCoordinateSystem->getMaximumRangeMaxY());
  updateScrollbars();
}

void CoordinateSystemZoomer::zoomToAllXY()
{
  // check, if we have a valid pointer to a CoordinateSystem:
  if( theCoordinateSystem == NULL ) 
    return;
  theCoordinateSystem->setCurrentRange(theCoordinateSystem->getMaximumRange());
  updateScrollbars();
}

/*
void CoordinateSystemZoomer::setCurrentRangeAndUpdateScrollBarsX(double newMinX, double newMaxX)
{
  if( theCoordinateSystem == NULL ) 
    return;
  bool tmp = theCoordinateSystem->autoReRenderImage;
  theCoordinateSystem->autoReRenderImage = false;
  theCoordinateSystem->setCurrentRangeX(newMinX, newMax

  theCoordinateSystem->autoReRenderImage = tmp;
}
*/

//-------------------------------------------------------------------------------------------------
// appearance-setup:

void CoordinateSystemZoomer::hideScrollBarX(bool shouldBeHidden)
{
  scrollBarIsHiddenX = shouldBeHidden;
  scrollBarX->setVisible(!scrollBarIsHiddenX);
}

void CoordinateSystemZoomer::hideScrollBarY(bool shouldBeHidden)
{
  scrollBarIsHiddenY = shouldBeHidden;
  scrollBarY->setVisible(!scrollBarIsHiddenY);
}

void CoordinateSystemZoomer::alignWidgetsToCoordinateSystem()
{
  // check, if we have a valid pointer to a CoordinateSystem:
  if( theCoordinateSystem == NULL ) 
    return;

  setBounds(theCoordinateSystem->getX(), 
    theCoordinateSystem->getY(), 
    theCoordinateSystem->getWidth()+zoomerSize, 
    theCoordinateSystem->getHeight()+zoomerSize);

  int x, y, w, h;

  x = theCoordinateSystem->getWidth()-zoomerSize + 2;
  y = theCoordinateSystem->getHeight();
  zoomInButtonX->setBounds(x, y, zoomerSize, zoomerSize);

  x = theCoordinateSystem->getWidth()-2*zoomerSize + 4;
  y = theCoordinateSystem->getHeight();
  zoomToAllButtonX->setBounds(x, y, zoomerSize, zoomerSize);

  x = theCoordinateSystem->getWidth()-3*zoomerSize + 6;
  y = theCoordinateSystem->getHeight();
  zoomOutButtonX->setBounds(x, y, zoomerSize, zoomerSize);

  x = 0;
  y = theCoordinateSystem->getHeight();
  //w = theCoordinateSystem->getWidth()-3*zoomerSize;
  w = zoomOutButtonX->getX();
  h = zoomerSize;
  scrollBarX->setBounds(x, y, w, h);

  x = theCoordinateSystem->getWidth();
  y = theCoordinateSystem->getHeight()-zoomerSize + 2;
  zoomInButtonY->setBounds(x, y, zoomerSize, zoomerSize);

  x = theCoordinateSystem->getWidth();
  y = theCoordinateSystem->getHeight()-2*zoomerSize + 4;
  zoomToAllButtonY->setBounds(x, y, zoomerSize, zoomerSize);

  x = theCoordinateSystem->getWidth();
  y = theCoordinateSystem->getHeight()-3*zoomerSize + 6;
  zoomOutButtonY->setBounds(x, y, zoomerSize, zoomerSize);

  x = theCoordinateSystem->getWidth();
  y = 0;
  w = zoomerSize;
  //h = theCoordinateSystem->getHeight()-3*zoomerSize;
  h = zoomOutButtonY->getY();
  scrollBarY->setBounds(x, y, w, h);

  x = theCoordinateSystem->getWidth();
  y = theCoordinateSystem->getHeight();
  zoomToAllButtonXY->setBounds(x, y, zoomerSize, zoomerSize);

  //scrollBarX->setVisible(!scrollBarIsHiddenX);
  //scrollBarY->setVisible(!scrollBarIsHiddenY);
}

//-------------------------------------------------------------------------------------------------
// internal functions:

void CoordinateSystemZoomer::updateScrollbars()
{
  if( theCoordinateSystem == NULL ) 
    return;

  double x1, x2, y1, y2, w, h;

  x1 = theCoordinateSystem->getCurrentRangeMinX();
  x2 = theCoordinateSystem->getCurrentRangeMaxX();
  x1 = transformToScrollBarCoordinateX(x1);
  x2 = transformToScrollBarCoordinateX(x2);
  w  = x2-x1;
  scrollBarX->setCurrentRange(x1, w);
  scrollBarX->setSingleStepSize(0.1*w);
 
  y1 = theCoordinateSystem->getCurrentRangeMinY();
  y2 = theCoordinateSystem->getCurrentRangeMaxY();
  y1 = transformToScrollBarCoordinateY(y1);
  y2 = transformToScrollBarCoordinateY(y2);
  h  = y2-y1;
  scrollBarY->setCurrentRange(1.0-y2, h);
  scrollBarY->setSingleStepSize(0.1*h);
}

double CoordinateSystemZoomer::transformToScrollBarCoordinateX(double x)
{
  if( theCoordinateSystem == NULL ) 
    return 0.0;
  if( theCoordinateSystem->isLogScaledX() )
  {
    return RAPT::rsExpToLin(x, theCoordinateSystem->getMaximumRangeMinX(), 
      theCoordinateSystem->getMaximumRangeMaxX(), 0.0, 1.0);
  }
  else
  {
    return RAPT::rsLinToLin(x, theCoordinateSystem->getMaximumRangeMinX(), 
      theCoordinateSystem->getMaximumRangeMaxX(), 0.0, 1.0);
  }
}

double CoordinateSystemZoomer::transformFromScrollBarCoordinateX(double x)
{
  if( theCoordinateSystem == NULL ) 
    return 0.0;
  if( theCoordinateSystem->isLogScaledX() )
  {
    return RAPT::rsLinToExp(x, 0.0, 1.0, theCoordinateSystem->getMaximumRangeMinX(), 
      theCoordinateSystem->getMaximumRangeMaxX());
  }
  else
  {
    return RAPT::rsLinToLin(x, 0.0, 1.0, theCoordinateSystem->getMaximumRangeMinX(), 
      theCoordinateSystem->getMaximumRangeMaxX());
  }
}

double CoordinateSystemZoomer::transformToScrollBarCoordinateY(double y)
{
  if( theCoordinateSystem == NULL ) 
    return 0.0;
  if( theCoordinateSystem->isLogScaledY() )
  {
    return RAPT::rsExpToLin(y, theCoordinateSystem->getMaximumRangeMinY(), 
      theCoordinateSystem->getMaximumRangeMaxY(), 0.0, 1.0);
  }
  else
  {
    return RAPT::rsLinToLin(y, theCoordinateSystem->getMaximumRangeMinY(), 
      theCoordinateSystem->getMaximumRangeMaxY(), 0.0, 1.0);
  }
}

double CoordinateSystemZoomer::transformFromScrollBarCoordinateY(double y)
{
  if( theCoordinateSystem == NULL ) 
    return 0.0;
  if( theCoordinateSystem->isLogScaledY() )
  {
    return RAPT::rsLinToExp(y, 0.0, 1.0, theCoordinateSystem->getMaximumRangeMinY(), 
      theCoordinateSystem->getMaximumRangeMaxY());
  }
  else
  {
    return RAPT::rsLinToLin(y, 0.0, 1.0, theCoordinateSystem->getMaximumRangeMinY(), 
      theCoordinateSystem->getMaximumRangeMaxY());
  }
}

