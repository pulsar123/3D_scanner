void initialize(byte factory_reset)
/* Initializing all the variables, with the optional factory reset (resetting all the EEPROM data).
*/
{
  // Keypad stuff:
  // No locking for keys:
  keypad.setHoldTime(65000);
  keypad.setDebounceTime(50);
  g.key_old = '=';

  g.run = 0;
  g.shooting = 0;
  g.t_message = g.t;
  g.t_shoot_start = g.t;
  g.i_shot = 0;
  g.AF_on = 0;
  g.flash_on = 0;
  g.t_key_pressed = g.t;
  g.N_repeats = 0;
  g.init = 0;
  g.en = 1;
  g.i_step = 0;
  g.t_step = g.t;
  g.message = 0;
  g.flash_delay = FLASH_DELAY;

  if (factory_reset)
  {
    // Default memory register is 0:
    g.ireg = 0;
    // We store the memory register index right after the registers' contents:
    EEPROM.put(ADDR_IREG, g.ireg);

    // Default values for all registers' parameters:
    g.reg.dt_shots = 60;  // Time between shots in 0.1s
    g.reg.N_shots = 35; // Number of shots per rotation
    g.reg.extra_shots = 0; // Number of extra shots

    g.backlight = 0;  // Default level of backlight

    for (unsigned char jj = 0; jj < N_REGS; jj++)
    {
      int address = SIZE_REG * jj;
      EEPROM.put(address, g.reg);
    }

    // Backlight is stored right after ireg:
    EEPROM.put(ADDR_BACKLIGHT, g.backlight);

  }
  else
  {
    EEPROM.get(ADDR_IREG, g.ireg);
    EEPROM.get(g.ireg * SIZE_REG, g.reg);
    EEPROM.get(ADDR_BACKLIGHT, g.backlight);
  }

  set_backlight(); // Using the new value of backlight

  lcd.begin();  // Always call lcd.begin() first.
  lcd.setContrast(20);
  lcd.clearDisplay();
  lcd.setTextSize(1);
  lcd.setTextColor(BLACK, WHITE);
  lcd.setRotation(2);
  Display(-1);

  return;
}
