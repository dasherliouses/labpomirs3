
#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "Library.h"
#ifdef __cplusplus
	};
#endif

#define UNITS_PER_REV   3000.0f   /* AxisInit: encoder_if.scaling.load.units  */
#define INCR_PER_REV    24.0f     /* AcpParTab: SCALE_ENCOD_INCR (ParID 109)  */
#define CYCLE_TIME      0.002f    /* task class Cyclic#1, 2 ms               */

/* Axis speed in units/s from the X20MM4456 pulse counter */
void FB_Axis(struct FB_Axis* inst)
{
	/* difference in INT arithmetic: correct across counter overflow 32767 -> -32768 */
	INT delta = (INT)(inst->counter - inst->last_counter);

	inst->speed = ((REAL)delta * UNITS_PER_REV / INCR_PER_REV) / CYCLE_TIME;
	inst->last_counter = inst->counter;
}
