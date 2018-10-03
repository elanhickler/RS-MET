//#include "rosic_String.h"
//using namespace rosic;

//-------------------------------------------------------------------------------------------------
// static data members:

//const rsString rsString::empty;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

rsString::rsString()
{
  length       = 0;
  reservedSize = 1;                      // for the terminating zero
  cString      = new char[reservedSize]; // array of size 1
  strcpy(cString, "");                   // init as empty c-string
}

rsString::rsString(const char *initialString)
{
  if( initialString != NULL )
  {
    length       = (int) strlen(initialString);
    reservedSize = length+1;
    cString      = new char[reservedSize];
    strcpy(cString, initialString);
  }
  else
  {
    length       = 0;
    reservedSize = 1;
    cString      = new char[reservedSize];
    strcpy(cString, "");
  }
}

rsString::rsString(const std::string& str)
{
  length = (int)str.length();
  reservedSize = length+1;
  cString      = new char[reservedSize];
  for(int i = 0; i < length; i++)
    cString[i] = str[i];
  cString[length] = '\0';
}

rsString::rsString(const rsString &other)
{
  length       = other.length;
  reservedSize = other.reservedSize;
  cString      = new char[reservedSize];
  strcpy(cString, other.cString);
}

rsString::rsString(const int intValue)
{
  length       = numberOfRequiredCharacters(intValue);
  reservedSize = length+1;
  cString      = new char[reservedSize];
  sprintf(cString, "%d", intValue);      // old: itoa(intValue, cString, 10);
}

rsString::rsString(const unsigned int uintValue)
{
  length       = numberOfRequiredCharacters(uintValue);
  reservedSize = length+1;
  cString      = new char[reservedSize];
  sprintf(cString, "%d", uintValue);     // old: itoa(uintValue, cString, 10);
}

rsString::rsString(const float floatValue)
{
  initFromDoubleValue((double) floatValue);
}

rsString::rsString(const double doubleValue)
{
  initFromDoubleValue(doubleValue);
}

rsString::~rsString()
{
  delete[] cString;
}

//-------------------------------------------------------------------------------------------------
// static member functions:

int rsString::numberOfRequiredCharacters(int number)
{
  if( number == 0 )
    return 1;
  int numChars           = 0;
  int absValue           = abs(number);
  unsigned long long tmp = 1;             // must be able to go above INT_MAX
  while( tmp <= (unsigned long long) absValue && tmp < (unsigned long long) INT_MAX )
  {
    tmp *= 10;
    numChars++;
  }
  if(number < 0)
    numChars++; // for the minus
  return numChars;
}

int rsString::compareCharacterArrays(char *left, int leftLength, char *right, int rightLength)
{
  int minLength = rmin(leftLength, rightLength);
  for(int i=0; i<minLength; i++)
  {
    if(      compareCharacters(left[i], right[i]) == -1 )
      return -1;
    else if( compareCharacters(left[i], right[i]) == +1 )
      return +1;
  }
  if(      leftLength < rightLength )
    return -1;
  else if( leftLength > rightLength )
    return +1;
  else
    return 0;
}

int rsString::compareCharacters(char left, char right)
{
  if( left == right )
    return 0;

  char leftUpper  = toUpperCase(left);
  char rightUpper = toUpperCase(right);
  if( leftUpper == rightUpper )
  {
    if( left < right )       // example: 'A' < 'a'
      return -1;
    else if( left > right )  // example: 'a' > 'A'
      return +1;
    else                     // case actually already handled in if(left == right)
      return 0;
  }
  else
  {
    if( leftUpper < rightUpper )
      return -1;
    else
      return +1;
  }
}

char rsString::toUpperCase(char c)
{
  if( islower(c) )
    return c-32;
  else
    return c;
}

rsString rsString::fromIntWithLeadingSpaces(int value, int minNumCharacters, bool prependPlusForPositiveNumbers)
{
  int absVal     = abs(value);
  int numSpaces  = 0;
  int tmp        = 10*rmax(absVal, 1);
  int upperLimit = powInt(10, minNumCharacters)-1;
  while( tmp < upperLimit )
  {
    tmp *= 10;
    numSpaces++;
  }

  if( value < 0 )
    return createWhiteSpace(numSpaces) + rsString("-") + rsString(absVal);
  else if( value > 0)
  {
    if( prependPlusForPositiveNumbers == true )
      return createWhiteSpace(numSpaces) + rsString("+") + rsString(absVal);
    else
      return createWhiteSpace(numSpaces) + rsString(absVal);
  }
  else
  {
    if( prependPlusForPositiveNumbers == true )
      return createWhiteSpace(numSpaces+1) + rsString(absVal);
    else
      return createWhiteSpace(numSpaces) + rsString(absVal);
  }
}

rsString rsString::fromDouble(double value)
{
  char tmpString[64];
  sprintf(tmpString, "%lg", value);
  return rsString(tmpString);
}

rsString rsString::createWhiteSpace(int length)
{
  return createSpanOfCharacters(' ', length);
}

rsString rsString::createSpanOfCharacters(char character, int length)
{
  char *cString = new char[length+1];
  for(int i=0; i<length; i++)
    cString[i] = character;
  cString[length] = '\0';
  rosic::rsString result = rosic::rsString(cString);
  delete[] cString;
  return result;
}


//-------------------------------------------------------------------------------------------------
// setup:

void rsString::padToLength(int desiredLength, char paddingCharacter)
{
  int missingLength = rmax(desiredLength - getLength(), 0);
  *this += createSpanOfCharacters(paddingCharacter, missingLength);
}

void rsString::prePadToLength(int desiredLength, char paddingCharacter)
{
  int missingLength = rmax(desiredLength - getLength(), 0);
  *this = createSpanOfCharacters(paddingCharacter, missingLength) + *this;
}

void rsString::reserveSize(int numCharactersToReserve)
{
  int requiredMemory = numCharactersToReserve + 1;
  if( reservedSize < requiredMemory )
  {
    char *tmpString = new char[requiredMemory];
    strcpy(tmpString, cString);
    delete[] cString;
    reservedSize = requiredMemory;
    cString      = tmpString;
  }
}

double rsString::asDouble() const
{
  if( *this == rsString("INF") )
    return INF;
  else if( *this == rsString("-INF") )
    return -INF;
  else if( *this == rsString("NaN") )
    return NAN;
  return atof(cString);
}

rsString rsString::toLowerCase(int start, int end) const
{
  rsString result = *this;
  start = rmax(start, 0);
  end   = rmin(end, result.getLength()-1);
  for(int i=start; i<=end; i++)
    result.cString[i] = tolower(result.cString[i]);
  return result;
}

rsString rsString::toUpperCase(int start, int end) const
{
  rsString result = *this;
  start = rmax(start, 0);
  end   = rmin(end, result.getLength()-1);
  for(int i=start; i<=end; i++)
    result.cString[i] = toupper(result.cString[i]);
  return result;
}

//-------------------------------------------------------------------------------------------------
// inquiry:

bool rsString::containsNonPrintableCharacters() const
{
  for(int i=0; i<length; i++)
  {
    if( isprint(cString[i]) == false )
      return true;
  }
  return false;
}

//-------------------------------------------------------------------------------------------------
// others:

void rsString::printToStandardOutput() const
{
  printf("%s", getRawString());
}

void rsString::readFromBuffer(char *sourceBuffer)
{
  int sourceLength = (int) strlen(sourceBuffer);
  allocateMemory(sourceLength);
  strcpy(cString, sourceBuffer);
}

void rsString::writeIntoBuffer(char *targetBuffer, int maxNumCharactersToWrite) const
{
  int upperLimit = rmin(length+1, maxNumCharactersToWrite);
  for(int i=0; i<upperLimit; i++)
    targetBuffer[i] = cString[i];
}

//-------------------------------------------------------------------------------------------------
// internal functions:

void rsString::allocateMemory(int numCharactersToAllocateExcludingZero)
{
  if( reservedSize-1 != numCharactersToAllocateExcludingZero )
  {
    delete[] cString;
    length       = numCharactersToAllocateExcludingZero;
    reservedSize = length+1;
    cString      = new char[reservedSize];
  }
}

void rsString::initFromDoubleValue(double doubleValue)
{
  length       = 0;
  reservedSize = 0;
  cString      = NULL;
  if( doubleValue == INF )
  {
    *this = rsString("INF");
    return;
  }
  else if( doubleValue == -INF )
  {
    *this = rsString("-INF");
    return;
  }
  else if( RAPT::rsIsNaN(doubleValue) )
  {
    *this = rsString("NaN");
    return;
  }

  // create temporary string long enough to hold all the characters, determine the actually
  // required length and copy the repsective part of the temporary string - the c-book says,
  // strncpy is unreliable with respect to copying the terminating zero, so we do it manually:
  char tmpString[64];
  sprintf(tmpString, "%-.17gl", doubleValue);
  length = (int) strspn(tmpString, "0123456789+-.eE");

  tmpString[length] = '\0'; // maybe removed later
  length = removeGarbageFromDoubleString(tmpString, length);

  reservedSize = length+1;
  cString      = new char[reservedSize];
  for(int i=0; i<length; i++)
    cString[i] = tmpString[i];
  cString[length] = '\0';
}

int rsString::removeGarbageFromDoubleString(char *s, int length)
{
  //int dummy;

  // leave scientifically notated strings as is:
  if( RAPT::rsArray::findIndexOf(s, 'e', length) != -1 || RAPT::rsArray::findIndexOf(s, 'E', length) != -1 )
    return length;

  // leave integer notated strings (without point) as is:
  int dotIndex = RAPT::rsArray::findIndexOf(s, '.', length);
  if( dotIndex == -1 )
    return length;

  int digitsBeforeDot;
  if( s[0] == '-' )
    digitsBeforeDot = dotIndex - 1;
  else
    digitsBeforeDot = dotIndex;
  int digitsAfterDot = length - digitsBeforeDot - 1;

  int numDigits = digitsBeforeDot + digitsAfterDot;

  if( numDigits > 15 )
  {
    // round, if last digit is very small (1, 2) or very large (8, 9) and there's a range of 0s or 9s before it (in this case, the total
    // length is not required:
    if( s[length-2] == '0' )
    {
      // round down:
      while( s[length-2] == '0' )
      {
        s[length-2] = '\0';
        length--;
      }
      length--;
    }
    else if( s[length-2] == '9' )
    {
      // round up:
      while( s[length-2] == '9' )
      {
        s[length-2] = '\0';
        length--;
      }
      length--;

      // increase the last digit by one, taking into account carry-over in we hit the dot:
      if( s[length-1] != '.' )
        s[length-1] += 1;
      else
      {
        // we have rounded to an integer...
        //DEBUG_BREAK; // not yet implemented ...nothing to do because integers will be represented without gabage anyway?
      }
    }
  }

  return length;
}
