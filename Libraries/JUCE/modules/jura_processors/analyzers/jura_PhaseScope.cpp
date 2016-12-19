
PhaseScopeBuffer::PhaseScopeBuffer()
{
  antiAlias   = true;
  frameRate   = 25.0;
  decayTime   = 0.5;
  lineDensity = 0.0f;
  thickness   = 0.707f;
  updateDecayFactor();

  bufferFlat = nullptr;
  buffer     = nullptr;
  width      = 200;
  height     = 200;
  allocateBuffer();

  setSampleRate(44100.0);
  reset();
}

PhaseScopeBuffer::~PhaseScopeBuffer()
{
  freeBuffer();
}

void PhaseScopeBuffer::setSampleRate(double newSampleRate)
{
  sampleRate = newSampleRate;
  double brightness = 4000.0;   // make this a member variable later, perhaps it should depend on 
                                // the deacy time as well
  insertFactor = (float) (brightness/sampleRate);
}

void PhaseScopeBuffer::setFrameRate(double newFrameRate)
{
  frameRate = newFrameRate;
  updateDecayFactor();
}

void PhaseScopeBuffer::setDecayTime(double newDecayTime)
{
  decayTime = newDecayTime;
  updateDecayFactor();
}

void PhaseScopeBuffer::setSize(int newWidth, int newHeight)
{
  if(newWidth != width || newHeight != height)
  {
    freeBuffer();
    width  = newWidth;
    height = newHeight;
    allocateBuffer();
    reset();           // avoid flickering at size-change
  }
}

void PhaseScopeBuffer::setAntiAlias(bool shouldAntiAlias)
{
  antiAlias = shouldAntiAlias;
}

void PhaseScopeBuffer::setLineDensity(float newDensity)
{
  lineDensity = newDensity;
}

void PhaseScopeBuffer::convertAmplitudesToMatrixIndices(double &x, double &y)
{
  x  = 0.5*(x+1);  // convert -1..+1 into 0..1
  y  = 0.5*(y+1);
  x *= width;
  y *= height;
  // maybe we should add 0.5 after multiplication by width/height?
}

void PhaseScopeBuffer::bufferSampleFrame(double x, double y)
{
  convertAmplitudesToMatrixIndices(x, y);
  addLineTo((float)x, (float)y);
}

void PhaseScopeBuffer::applyPixelDecay()
{
  ArrayTools::rsScale(bufferFlat, width*height, decayFactor);
}

void PhaseScopeBuffer::reset()
{
  ArrayTools::rsFillWithZeros(bufferFlat, width*height);

  // (xOld,yOld) = (0,0) - but in pixel coordinates:
  xOld = 0.5f*width;   
  yOld = 0.5f*height;
}

float PhaseScopeBuffer::pixelDistance(float x1, float y1, float x2, float y2)
{
  float dx = x2-x1;
  float dy = y2-y1;
  //return sqrt(dx*dx + dy*dy);   // Euclidean distance
  return max(fabs(dx), fabs(dy)); // maybe try Euclidean distance instead
}

void PhaseScopeBuffer::addLineTo(float x, float y)
{
  if(lineDensity == 0.f)
  {
    addDot(x, y);
    return;
  }

  int numDots = max(1, (int)floor(lineDensity*pixelDistance(xOld, yOld, x, y)));
  float k;
  for(int i = 1; i <= numDots; i++)
  {
    k = (float)i / (float)numDots;  // optimize: use scaler * i, where scaler = 1 / numDots
    addDot((1-k)*xOld + k*x, (1-k)*yOld + k*y); // maybe use xOld + k*dx, where dx = x - xOld

    // maybe we should add the dot with an intesity scaled by 1/numDots
  }
  xOld = x;
  yOld = y;
}

void PhaseScopeBuffer::addDot(float x, float y)
{
  if(!antiAlias)
  {
    addDotFast(x, y);
    return;
  }

  int i = (int)floor(x);  // integer part of x
  int j = (int)floor(y);  // integer part of y
  x -= i;                 // fractional part of x
  y -= j;                 // fractional part of y

  // compute weights for bilinear deinterpolation (maybe factor out):
  float a, b, c, d;
  d = x*y;
  b = x-d;
  c = y-d;
  a = d-x-y+1;

  // compute values to accumulate into the 4 pixels at (i,j), (i+1,j), (i,j+1), (i+1,j+1):
  a *= insertFactor;  // (i,   j)
  b *= insertFactor;  // (i+1, j)
  c *= insertFactor;  // (i,   j+1)
  d *= insertFactor;  // (i+1, j+1)

  // accumulate values into the matrix:
  if(i >= 0 && i < width-1 && j >= 0 && j < height-1)
  {
    accumulate(buffer[i]  [j],   a);
    accumulate(buffer[i+1][j],   b);
    accumulate(buffer[i]  [j+1], c);
    accumulate(buffer[i+1][j+1], d);
  }

  // apply thickness:
  if(thickness > 0.f && i >= 1 && i < width-2 && j >= 1 && j < height-2)
  {
    float t, s, sa, sb, sc, sd, ta, tb, tc, td;
    t = thickness;             // weight for direct neighbour pixels
    s = t * (float)SQRT2_INV;  // weight for diagonal neighbour pixels

    sa = s*a;
    sb = s*b;
    sc = s*c;
    sd = s*d;

    ta = t*a;
    tb = t*b;
    tc = t*c;
    td = t*d;

    accumulate(buffer[i-1][j-1], sa);
    accumulate(buffer[i-1][j],   ta+sc);
    accumulate(buffer[i-1][j+1], tc+sa);
    accumulate(buffer[i-1][j+2], sc);

    accumulate(buffer[i]  [j-1], ta+sb);
    accumulate(buffer[i]  [j],   tb+tc+sd);
    accumulate(buffer[i]  [j+1], ta+td+sb);
    accumulate(buffer[i]  [j+2], tc+sd);

    accumulate(buffer[i+1][j-1], tb+sa);
    accumulate(buffer[i+1][j],   ta+td+sc);
    accumulate(buffer[i+1][j+1], tb+tc+sa);
    accumulate(buffer[i+1][j+2], td+sc);

    accumulate(buffer[i+2][j-1], sb);
    accumulate(buffer[i+2][j],   tb+sd);
    accumulate(buffer[i+2][j+1], td+sb);
    accumulate(buffer[i+2][j+2], sd);
  }
}

void PhaseScopeBuffer::addDotFast(float x, float y)
{
  int i = (int)round(x);
  int j = (int)round(y);

  if(i >= 0 && i < width && j >= 0 && j < height)
    accumulate(buffer[i][j], insertFactor);

  // apply thickness:
  if(thickness > 0.f && i >= 1 && i < width-1 && j >= 1 && j < height-1)
  {
    float a, ta, sa;
    a  = insertFactor;
    ta = a  * thickness;
    sa = ta * (float)SQRT2_INV;

    accumulate(buffer[i-1][j-1], sa);
    accumulate(buffer[i-1][j],   ta);
    accumulate(buffer[i-1][j+1], sa);

    accumulate(buffer[i]  [j-1], ta);
    accumulate(buffer[i]  [j+1], ta);

    accumulate(buffer[i+1][j-1], sa);
    accumulate(buffer[i+1][j],   ta);
    accumulate(buffer[i+1][j+1], sa);
  }
}

void PhaseScopeBuffer::allocateBuffer()
{
  bufferFlat = new float[width*height];

  buffer = new float*[width];
  for(int i = 0; i < width; i++)
    buffer[i] = &bufferFlat[i*height];


  // old - it's wrong and worked only so far because width and height were always equal:

  //buffer = new float*[height];
  //for(int i = 0; i < height; i++)
  //  buffer[i] = &bufferFlat[i*width]; 
  
  // We have the second index running over the y-coordinate and the first index running over the 
  // x-coordinate here...this is actually the other way around than in images where we have the 
  // first index pointing to a particular horizontal line. Maybe we should use a memory layout here
  // that corresponds to images (x-coordinate as 2nd index) ..i have to verify/modify all formulas 
  // again...i think, we just need to replace i with j and vice versa in all the "accumulate" calls
}

void PhaseScopeBuffer::freeBuffer()
{
  delete[] buffer;
  delete[] bufferFlat;
  buffer = nullptr;
  bufferFlat = nullptr;
}

void PhaseScopeBuffer::updateDecayFactor()
{
  decayFactor = (float) exp(-1 / (decayTime*frameRate));
}

//=================================================================================================

PhaseScope::PhaseScope(CriticalSection *lockToUse) : AudioModule(lockToUse)
{
  ScopedLock scopedLock(*plugInLock);
  moduleName = "PhaseScope";
  setActiveDirectory(getApplicationDirectory() + "/PhaseScopePresets");
  needsPixelDecay = false;
}

void PhaseScope::setPixelSize(int width, int height)
{
  phaseScopeBuffer.setSize(width, height);
}

AudioModuleEditor* PhaseScope::createEditor()
{
  //return new PhaseScopeDisplay(this);
  return new PhaseScopeEditor(this);
}

void PhaseScope::processBlock(double **inOutBuffer, int numChannels, int numSamples)
{
  jassert(numChannels == 2);
  if(needsPixelDecay)
  {
    phaseScopeBuffer.applyPixelDecay();
    needsPixelDecay = false;
  }
  for(int n = 0; n < numSamples; n++)
    phaseScopeBuffer.bufferSampleFrame(inOutBuffer[0][n], inOutBuffer[1][n]);
}

void PhaseScope::setSampleRate(double newSampleRate)
{
  phaseScopeBuffer.setSampleRate(newSampleRate);
}

void PhaseScope::reset()
{
  phaseScopeBuffer.reset();
}

//=================================================================================================

PhaseScopeDisplay::PhaseScopeDisplay(jura::PhaseScope *newPhaseScopeToEdit)
  : AudioModuleEditor(newPhaseScopeToEdit)
{
  phaseScope = newPhaseScopeToEdit;
  setSize(400, 400);

  startTimerHz(25);
  phaseScope->phaseScopeBuffer.setFrameRate(25);
}

void PhaseScopeDisplay::resized()
{
  phaseScope->setPixelSize(getWidth(), getHeight());
  image = Image(Image::ARGB, getWidth(), getHeight(), false);
}


// \todo move these 2 helper functions to jura_GraphicsTools, maybe templatize so it can be used 
// for double also:
void dataMatrixToPixelBrightness(float **data, uint8 *pixels, int width, int height)
{
  uint8 *p = pixels;
  for(int y = 0; y < height; y++)     // loop over lines
  {
    for(int x = 0; x < width; x++)    // loop over pixels
    {
      //jassert(data[y][x] <= 1.f);   // for test

      // we assume here, that the alpha channel comes last in the byte order of the pixels
      p[0] = p[1] = p[2] = (uint8) (255 * data[y][x]);  // data determines white-value
      p[3] = 255;                                       // set to full opacity ("alpha")
      p   += 4;                                         // jump to next pixel
    }
  }
  // todo: write a version of this function that uses a colormap
}
void dataMatrixToImage(float **data, Image &image)
{
  // We assume here that the size of the image (width and height) matches the dimensions of the 
  // data matrix)
  Image::BitmapData bitmap(image, Image::BitmapData::writeOnly);
  jassert(bitmap.pixelStride == 4);
  uint8 *pixelPointer = bitmap.getPixelPointer(0, 0);
  dataMatrixToPixelBrightness(data, pixelPointer, bitmap.width, bitmap.height);
}


void PhaseScopeDisplay::paint(Graphics &g)
{
  dataMatrixToImage(phaseScope->phaseScopeBuffer.getDataMatrix(), image);
  g.drawImageAt(image, 0, 0);

  //// this is the old, direct and horribly inefficient way to do it:
  //g.fillAll(Colours::black);
  //for(int x = 0; x < getWidth(); x++)
  //{
  //  for(int y = 0; y < getHeight(); y++)
  //  {
  //    g.setColour(phaseScope->getColourAt(x, y));
  //    g.setPixel(x, y);
  //  }
  //}
}

void PhaseScopeDisplay::timerCallback()
{
  repaint();
  //phaseScope->phaseScopeBuffer.applyPixelDecay();
  phaseScope->triggerPixelDecay();

  // there's a kind of bug - the display flickers and the flickering seems to be different if
  // we apply the pixel decay in the GUI thread (by calling 
  // phaseScope->phaseScopeBuffer.applyPixelDecay() here) or apply it in the audio thread
  // (by calling phaseScope->triggerPixelDecay()). maybe we need to do some kind of thread
  // synchronization
}

//=================================================================================================

PhaseScopeEditor::PhaseScopeEditor(jura::PhaseScope *newPhaseScopeToEdit)
  : AudioModuleEditor(newPhaseScopeToEdit)
  , display(newPhaseScopeToEdit)
{
  ScopedLock scopedLock(*plugInLock);
  scope = newPhaseScopeToEdit;
  widgetMargin = 120; 

  addAndMakeVisible(display);

  setSize(400+widgetMargin, 400);  // preliminary

  //setDisplayPixelSize(400, 400);
}

//void PhaseScopeEditor::setDisplayPixelSize(int newWidth, int newHeight)
//{
//  ScopedLock scopedLock(*plugInLock);
//  setSize(newWidth+widgetMargin, newHeight);
//
//  // hmm...maybe, we should also allow for the headline, preset section, etc.
//}

void PhaseScopeEditor::resized()
{
  ScopedLock scopedLock(*plugInLock);
  AudioModuleEditor::resized();

  int w = getWidth();
  int h = getHeight();
  int y = getPresetSectionBottom() + 4;

  display.setBounds(0, y, w-widgetMargin, h-y);

  // set up widgets:
  //...
}
