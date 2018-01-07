void camera()
{
  // Making a shot every g.reg.dt_shots
  if (g.shooting == 1 && g.i_shot < g.N_tot && g.t >= g.t_shoot_start + g.reg.dt_shots * 100000)
  {
    g.i_shot++;
    g.flash_on = 0;
    g.t_shoot_start = g.t;
    if (g.i_shot <= g.N_tot)
    {
      // Initiating shutter half-press:
      g.AF_on = 1;
      digitalWrite(AF_PIN, g.AF_on);
      //         unsigned long int t1, t2;
      //         t1 = micros();
            lcd.setCursor(0, 5 * ROW);
            sprintf(g.buffer, "%3d", g.i_shot);
            lcd.print(g.buffer);
            lcd.display();
      //         t2 = micros();
      //         dt=t2-t1;
    }
  }


  if (g.t - g.t_shoot_start >= DT_AF && g.AF_on == 1)
  {
    // Ending shutter half-press:
    g.AF_on = 0;
    digitalWrite(AF_PIN, g.AF_on);
    if (g.i_shot >= g.N_tot)
    {
      // Finished shooting
      g.shooting = 0;
      Display(-1);
    }
  }

}

