#include "libat09config.h"
#include "libat09.h"

using namespace at09;

int i = 0;
AT09 blueDash;

void setup()
{ 
#ifdef DEBUG
	blueDash.Initialize(3, 2);
#else
	blueDash.Initialize();
#endif
}

void loop()
{
	blueDash.HandleSerialEvent();
}
