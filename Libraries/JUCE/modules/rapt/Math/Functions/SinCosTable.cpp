template<class T>
rsSinCosTable<T>::rsSinCosTable(int tableSize)
{
  sinTbl.resize(tableSize);
  cosTbl.resize(tableSize);
  for(int n = 0; n < tableSize; n++) 
  {
    T x = T(2*PI*n) / tableSize;
    sinTbl[n] = sin(x);
    cosTbl[n] = cos(x); 
  }
  scaler = T(tableSize/(2*PI));
  mask   = tableSize-1;
}
