
#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "Library.h"
#ifdef __cplusplus
	};
#endif

/* Simulated end switches: active when position leaves [neg_value; pos_value] */
void FB_Endswitch(struct FB_Endswitch* inst)
{
	inst->neg_reached = (inst->in <= inst->neg_value);
	inst->pos_reached = (inst->in >= inst->pos_value);
}
