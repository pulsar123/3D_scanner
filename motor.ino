void motor()
{

  // Rewind module
  if (g.rewind != 0)
  {
    // As long as a rewind key is being pressed, we continue to accelerate (up to the speed limit)
    if (g.init == 1 && g.run == 1)
    {
      g.init = 0;
      // Enabling the driver:
      g.en = 0;
      digitalWrite(ENABLE_PIN, g.en);
      if (g.rewind == 1)
        digitalWrite(DIR_PIN, HIGH);
      else
        digitalWrite(DIR_PIN, LOW);
      // First time step - corresponds to the initial rewind speed:
      g.dt = DT_INIT;
      g.t_step = g.t + (unsigned long int)g.dt;
      lcd.setCursor(0, 5 * ROW);
      if (g.rewind == -1)
        sprintf(g.buffer, " <--");
      else
        sprintf(g.buffer, "          --> ");
      lcd.print(g.buffer);
      lcd.display();
    }
    else
    {
      if (g.t >= g.t_step)
      {
        // Making a microstep:
        delayMicroseconds(2);
        digitalWrite(STEP_PIN, 1);
        delayMicroseconds(2);
        digitalWrite(STEP_PIN, 0);
        if (g.run == 1)
          // Constant acceleration; next time step in us:
        {
          g.dt = g.dt / (1.0 + ACCEL * g.dt * g.dt);
          if (g.dt < DT_MIN)
            // Maximum speed limit:
            g.dt = DT_MIN;
        }
        else
          // Constant decelaration:
        {
          g.dt = g.dt / (1.0 - ACCEL * g.dt * g.dt);
          if (g.dt > DT_INIT || g.dt < 1e-20)
            // We slowed down enough, and will stop now:
          {
            g.rewind = 0;
            if (g.en == 0)
              // Disabling the motor:
            {
              g.en = 1;
              digitalWrite(ENABLE_PIN, g.en);
            }
            Display(-1);
            return;
          }
        }
        g.t_step = g.t + g.dt;
      }
    }
    return;
  }


  if (g.run == 0)
    return;


  if (g.init == 1)
  {
    g.init = 0;
    // Enabling the driver:
    g.en = 0;
    digitalWrite(ENABLE_PIN, g.en);
    digitalWrite(DIR_PIN, HIGH);
    g.t0 = g.t;
    g.i_step = 0;
    g.t_step = g.t0 + (unsigned long int)g.dt_microstep;
    lcd.setCursor(0, 5 * ROW);
    sprintf(g.buffer, "  0");
    lcd.print(g.buffer);
    lcd.display();
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
    g.t_step = g.t0 + (unsigned long int)(g.i_step * g.dt_microstep);
  }


}

