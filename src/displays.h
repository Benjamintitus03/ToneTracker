#ifndef DISPLAYS_H_
#define DISPLAYS_H_

#include "Grlib/grlib/grlib.h"


void display_idle_state(Graphics_Context *g_sContext);
void display_menu(Graphics_Context *g_sContext);
void display_tracking(Graphics_Context *g_sContext, int current_round, int total_rounds);

#endif /* DISPLAYS_H_ */