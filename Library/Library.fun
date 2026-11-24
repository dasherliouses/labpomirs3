
FUNCTION_BLOCK FB_Motor (*DC motor model (LR1): voltage -> speed, angle*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		in : REAL;
	END_VAR
	VAR_OUTPUT
		out_w : REAL;
		out_phi : REAL;
	END_VAR
	VAR
		k_e : REAL;
		T_m : REAL;
		dt : REAL;
		integrator : FB_Integrator;
		integrator_phi : FB_Integrator;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK FB_Integrator (*Euler integrator*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		in : REAL;
		dt : REAL;
	END_VAR
	VAR_OUTPUT
		out : REAL;
	END_VAR
	VAR
		state : REAL := 0.0;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK FB_Controller (*PI controller with saturation and anti-windup*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		in : REAL;
	END_VAR
	VAR_OUTPUT
		out : REAL;
	END_VAR
	VAR
		k_i : REAL;
		sum : REAL;
		k_p : REAL;
		dt : REAL;
		max_abs_value : REAL;
		integrator : FB_Integrator;
		last_sum : REAL;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK FB_Endswitch (*Simulated end switches by position*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		in : REAL;
		neg_value : REAL;
		pos_value : REAL;
	END_VAR
	VAR_OUTPUT
		neg_reached : BOOL;
		pos_reached : BOOL;
	END_VAR
END_FUNCTION_BLOCK

FUNCTION_BLOCK FB_Axis (*One URTK axis: encoder counter -> speed, PWM output*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		reset_error : BOOL;
		endswitch_a_reached : BOOL;
		endswitch_b_reached : BOOL;
	END_VAR
	VAR_OUTPUT
		reset_counter : BOOL;
		pwm_value : INT;
	END_VAR
	VAR_OUTPUT RETAIN
		counter : INT;
	END_VAR
	VAR_OUTPUT
		speed : REAL;
	END_VAR
	VAR
		last_counter : INT;
	END_VAR
END_FUNCTION_BLOCK
