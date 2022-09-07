double PID_CONTROLLER(double FEEDBACK, double dt){
  double ERROR_VALUE;
  double PREVIOUS_ERROR_VALUE = 0;
  double SETPOINT = <desired_value>; //put the desired value decided by you here.
  double KP = <value>; //put the value you want here.
  double KI = <value>; //put the value you want here.
  double KD = <Value>; //put the value you want here.
  double P;
  double I;
  double D;
  double INTEGRAL;
  double DERIVATIVE;
  double OUTPUT_VALUE;

  ERROR_VALUE = SETPOINT - FEEDBACK; //Here the feedback is YAW from MPU6050

  P = KP*ERROR_VALUE; 

  INTEGRAL += ERROR_VALUE*dt;
  I = KI*INTEGRAL;

  DERIVATIVE += (ERROR_VALUE-PREVIOUS_ERROR_VALUE)/dt;
  D = KD*DERIVATIVE;

  OUTPUT_VALUE = P + I + D ;

  PREVIOUS_ERROR_VALUE = ERROR_VALUE;

  return OUTPUT_VALUE;
}
