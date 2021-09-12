#include "libat09config.h"
#include "libat09.h"
#include "libat09utilities.h"

using namespace at09;

unsigned long i = 0;
AT09* blueDash;

void setup()
{ 
#ifdef DEBUG
	blueDash = new AT09(3, 2);
#else
	blueDash = new AT09();
#endif

	blueDash->SetRole(BTRole::PERIPHERAL);
	blueDash->SetName("BlueDash");
}

void loop()
{
	blueDash->HandleSerialRelay();
}
