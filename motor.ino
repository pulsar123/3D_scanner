void motor()
{

  if (g.run == 0)
    return;

  if (g.init == 1)
  {
    g.init = 0;
    // Enabling the driver:
    g.en = 0;
    digitalWrite(ENABLE_PIN, g.en);
    g.t0 = g.t;
    g.i_step = 0;
    g.t_step = g.t0 + (unsigned long int)g.dt_microstep;
  }

  // Making a microstep every DT_MICROSTEP
  if (g.t >= g.t_step)
  {
    // End of shooting; disabling the motor
    if (g.i_step > g.last_step)
    {
      g.run = 0;
      if (g.en == 0)
      {
        g.en = 1;
        digitalWrite(ENABLE_PIN, g.en);
      }
      return;
    }
    // Making a microstep:
    delayMicroseconds(2);
    digitalWrite(STEP_PIN, 1);
    delayMicroseconds(2);
    digitalWrite(STEP_PIN, 0);
    g.i_step++;
    g.t_step = g.t0 + (unsigned long int)(g.i_step*g.dt_microstep);
  }


}

