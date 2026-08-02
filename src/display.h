#include "driver.h"
#include "TFT_eSPI.h"

void initDisplay(void);
void displayMeasurementTest(float value);

#ifdef EPAPER_ENABLE
extern EPaper epaper;
#endif
