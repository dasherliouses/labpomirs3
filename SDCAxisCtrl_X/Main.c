
#include <bur/plctypes.h>
#include <string.h>

#ifdef _DEFAULT_INCLUDES
	#include <AsDefault.h>
#endif

#define V_MAX_OUTPUT   6500.0f   /* SERVO_V_MAX_OUTPUT (ParID 64201), units/s */
#define SET_POS_SCALE  32767.0f  /* full scale of the 16-bit SDC set value    */
#define U_MAX          24.0f     /* motor supply voltage, V                   */
#define PWM_SCALE      32767.0f  /* full scale of X20MM4456 PWM pulse width   */
#define UNITS_PER_REV  3000.0f
#define INCR_PER_REV   24.0f

void increase_counters(void);

void _INIT ProgramInit(void)
{
	/* SDC module types */
	AxisX_HW.EncIf1_Typ = ncSDC_ENC16;
	AxisX_HW.DiDoIf_Typ = ncSDC_DIDO;
	AxisX_HW.DrvIf_Typ  = ncSDC_DRVSERVO16;

	/* names of the interface variables */
	strcpy((char*)AxisX_HW.EncIf1_Name, "AxisX_EncIf");
	strcpy((char*)AxisX_HW.DrvIf_Name,  "AxisX_DrvIf");
	strcpy((char*)AxisX_HW.DiDoIf_Name, "AxisX_DiDoIf");

	/* ready / normal operation inputs */
	AxisX_EncIf.iEncOK          = 1;
	AxisX_DrvIf.iDrvOK          = 1;
	AxisX_DrvIf.iStatusEnable   = 1;
	AxisX_DiDoIf.iDriveReady    = 1;

	/* PI speed controller (output in volts) */
	fb_controller.dt            = 0.002f;
	fb_controller.k_i           = 0.16f;
	fb_controller.k_p           = 0.0064f;
	fb_controller.max_abs_value = U_MAX;

	pwm_period = 200;

	/* motor model for simulation: 24 V -> 6500 units/s */
	sim_motor.k_e = U_MAX / V_MAX_OUTPUT;
	sim_motor.T_m = 0.05f;
	sim_motor.dt  = 0.002f;
}

void _CYCLIC ProgramCyclic(void)
{
	BOOL sw_a, sw_b;

	/* ---- simulation of the object: encoder pulses and end switches ---- */
	if (simulation)
	{
		sim_motor.in = voltage;
		FB_Motor(&sim_motor);
		sim_counter = (INT)(sim_motor.out_phi * INCR_PER_REV / UNITS_PER_REV);

		sim_endswitch.in        = sim_motor.out_phi;
		sim_endswitch.neg_value = sim_neg_pos;
		sim_endswitch.pos_value = sim_pos_pos;
		FB_Endswitch(&sim_endswitch);

		axis_X.counter = sim_counter;
		sw_a = sim_endswitch.neg_reached;
		sw_b = sim_endswitch.pos_reached;
	}
	else
	{
		sw_a = axis_X.endswitch_b_reached;
		sw_b = axis_X.endswitch_a_reached;
	}

	/* actual speed of the axis, units/s */
	FB_Axis(&axis_X);
    if (!simulation)
        axis_X.speed = -axis_X.speed;

	/* excitation winding follows the switch on the stand */
	coil_pwm_value = coil_powered ? 0x7FFF : 0;

	/* SDC life counters and time stamp */
	increase_counters();
	AxisX_EncIf.iActTime = (INT)AsIOTimeCyclicStart();

	/* feedback to the SDC axis */
    AxisX_EncIf.iActPos     =  simulation ? axis_X.counter: (INT)(-axis_X.counter);
	AxisX_DiDoIf.iPosHwEnd  = sw_b;
	AxisX_DiDoIf.iNegHwEnd  = sw_a;
	AxisX_DiDoIf.iReference = sw_a;

	/* speed set value from the SDC position controller */
	fb_controller.in = AxisX_DrvIf.oSetPos;
	if (fb_controller.in < min_task && fb_controller.in > -min_task)
		fb_controller.in = 0;

	/* error in units/s: set value (scaled) minus actual speed */
	fb_controller.in = fb_controller.in * V_MAX_OUTPUT / SET_POS_SCALE - axis_X.speed;
	if(AxisX_DiDoIf.oDriveEnable)
	{
		FB_Controller(&fb_controller);
		voltage = fb_controller.out;
	}
	else
	{
		fb_controller.integrator.state = 0;
		fb_controller.out = 0;
		voltage = 0;	
	}
		
	if(simulation)
		axis_X.pwm_value = 0;
	else
		axis_X.pwm_value = (INT)(voltage / U_MAX * PWM_SCALE);
}

void increase_counters(void)
{
	AxisX_EncIf.iLifeCnt++;
	AxisX_DiDoIf.iLifeCntDriveEnable++;
	AxisX_DiDoIf.iLifeCntDriveReady++;
	AxisX_DiDoIf.iLifeCntNegHwEnd++;
	AxisX_DiDoIf.iLifeCntPosHwEnd++;
	AxisX_DiDoIf.iLifeCntReference++;
	AxisX_DrvIf.iLifeCnt++;
}

void _EXIT ProgramExit(void)
{
}
