void flash()
{
  if (g.shooting == 1 && g.i_shot <= g.N_tot)
  {
    if (g.t - g.t_shoot_start >= FLASH_DELAY && g.flash_on == 0)
    {
      g.flash_on = 1;
      // Triggering flash:
      digitalWrite(FLASH_PIN, 1);
    }
  }


  if (g.t - g.t_shoot_start >= FLASH_DELAY + DT_FLASH && g.flash_on == 1)
  {
    if (g.shooting == 1)
      g.flash_on = 2;
    else
      g.flash_on = 0;
    // End of flash triggering:
    digitalWrite(FLASH_PIN, 0);
  }

}

