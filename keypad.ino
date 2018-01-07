void process_keypad()
/*
  All the keypad runtime stuff goes here
*/
{

  // The previous value of the key 0:
  g.key_old = keypad.key[0].kchar;

  // This is a trick to generate multiple actions when you press certain keys (like "2") long enough
  // (longer than T_KEY_LAG). The multiple actions are separated by delays T_KEY_REPEAT.
  // The trick is to generate fake key press events for the currently pressed key. Flag fake_key
  // is used to differentiate bwetween a real key press (then it is '0') and fake key press (it is '1').
  char fake_key = 0;
  if (g.t - g.t_key_pressed > T_KEY_LAG)
    // We are here when a change parameter key was pressed longer than T_KEY_LAG
  {
    if (g.N_repeats == 0)
      // Generating the first fake key event:
    {
      g.t_last_repeat = g.t;
      g.N_repeats = 1;
      fake_key = 1;
    }
    if (g.t - g.t_last_repeat > T_KEY_REPEAT)
      // Generating subsequent fake key events:
    {
      g.N_repeats++;
      g.t_last_repeat = g.t;
      fake_key = 1;
    }
  }


  // Rescanning the keys. Most of the calls return false (no scan performed), exiting immediately if so
  if (!keypad.getKeys() && !fake_key)
    return;

  KeyState state0, state1;
  char key0;
  bool state0_changed;
  if (fake_key)
    // Simulating a fake key (for repetitive key actions when certain keys are pressed long enough)
  {
    state0 = PRESSED;
    key0 = g.key_old;
    state0_changed = 1;
  }
  else
    // Processing real (not fake) key press
  {
    state0 = keypad.key[0].kstate;
    state1 = keypad.key[1].kstate;
    key0 = keypad.key[0].kchar;
    state0_changed = keypad.key[0].stateChanged;
  }


  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // Single-key commands (fake key events are allowed)
  if (state0_changed && (state0 == PRESSED || state0 == RELEASED))
  {

    if (state0 == PRESSED)
    {
      if (state0_changed && !fake_key)
      {
        // Memorizing the time when a key was pressed:
        // (Only when it is a true state change, not a fake key)
        g.t_key_pressed = g.t;
      }

      // Here we interpret key presses
      //      lcd.print(key0);
      //      lcd.display();

      switch (key0)
      {
        case '#': // Start/stop
          if (g.run == 0 && g.shooting == 0)
          {
            g.run = 1;
            g.shooting = 1;
            g.init = 1;
            g.i_shot = 0;
            g.AF_on = 0;
            g.flash_on = 2;
            // Minus FLASH_DELAY so the first flash will happen g.reg.dt_shots after initiating a run:
            g.t_shoot_start = g.t - g.flash_delay;
            g.i_step = 0;
            g.t_step = g.t;
            g.N_tot = g.reg.N_shots + g.reg.extra_shots;
            g.last_step = (unsigned long int)((float)g.N_tot / (float)g.reg.N_shots * NTOT_MICROSTEPS);
            g.dt_microstep = (float)(g.reg.N_shots * g.reg.dt_shots * 100000) / NTOT_MICROSTEPS; // Time step for one microstep, us:
          }
          else
          {
            g.run = 0;
            g.shooting = 0;
            g.AF_on = 0;
            g.flash_on = 0;
            g.en = 1;
            digitalWrite(ENABLE_PIN, g.en);
            g.AF_on = 0;
            digitalWrite(AF_PIN, g.AF_on);
            digitalWrite(FLASH_PIN, 0);
            Display(-1);
          }
          break;

        case '1':  //Reduce time between shots
          if (g.run == 1)
            break;
          if ((g.reg.dt_shots - 1) * 100000 > DT_AF)
          {
            g.reg.dt_shots--;
            EEPROM.put(g.ireg * SIZE_REG, g.reg);
            Display(1);
          }
          break;

        case '2':  //Increase time between shots
          if (g.run == 1)
            break;
          if (g.reg.dt_shots < DT_SHOTS_MAX)
          {
            g.reg.dt_shots++;
            EEPROM.put(g.ireg * SIZE_REG, g.reg);
            Display(1);
          }
          break;

        case '3':  //Reduce N_shots per rotation
          if (g.run == 1)
            break;
          if (g.reg.N_shots > 1)
          {
            g.reg.N_shots--;
            EEPROM.put(g.ireg * SIZE_REG, g.reg);
            Display(2);
          }
          break;

        case 'A':  //Increase N_shots per rotation
          if (g.run == 1)
            break;
          if (g.reg.N_shots < N_SHOTS_MAX)
          {
            g.reg.N_shots++;
            EEPROM.put(g.ireg * SIZE_REG, g.reg);
            Display(2);
          }
          break;

        case '4':  //Reduce ireg
          if (g.run == 1)
            break;
          if (g.ireg > 0)
          {
            g.ireg--;
            EEPROM.put(ADDR_IREG, g.ireg);
            // Reading the new register:
            EEPROM.get(g.ireg * SIZE_REG, g.reg);
            Display(0);
          }
          break;

        case '5':  //Increase ireg
          if (g.run == 1)
            break;
          if (g.ireg < N_REGS - 1)
          {
            g.ireg++;
            EEPROM.put(ADDR_IREG, g.ireg);
            // Reading the new register:
            EEPROM.get(g.ireg * SIZE_REG, g.reg);
            Display(0);
          }
          break;

        case '6':  //Reduce extra_shots
          if (g.run == 1)
            break;
          if (g.reg.extra_shots > 0)
          {
            g.reg.extra_shots--;
            EEPROM.put(g.ireg * SIZE_REG, g.reg);
            Display(3);
          }
          break;

        case 'B':  //Increase extra_shots
          if (g.run == 1)
            break;
          if (g.reg.extra_shots < EXTRA_SHOTS_MAX)
          {
            g.reg.extra_shots++;
            EEPROM.put(g.ireg * SIZE_REG, g.reg);
            Display(3);
          }
          break;

        case '7':  //Reduce backlight
          if (g.backlight > 0)
          {
            g.backlight--;
            set_backlight();
            EEPROM.put(ADDR_BACKLIGHT, g.backlight);
            Display(4);
          }
          break;

        case '8':  //Increase backlight
          if (g.backlight < N_BACKLIGHT - 1)
          {
            g.backlight++;
            set_backlight();
            EEPROM.put(ADDR_BACKLIGHT, g.backlight);
            Display(4);
          }
          break;

        case '0':  //Make a test shot
          if (g.run == 0 && g.shooting == 0)
          {
            g.shooting = 1;
            g.i_shot = 0;
            g.AF_on = 0;
            g.flash_on = 2;
            // Ensuring the test shot starts immediately:
            g.t_shoot_start = g.t - g.reg.dt_shots * 100000;
            g.N_tot = 1;
            Display(5);
          }
          break;

        case '*':  //Rewind (rotate CW)
          if (g.shooting == 1)
            break;
          if (fake_key == 0 && g.run == 0 && g.rewind == 0)
          {
            g.run = 1;
            g.rewind = -1;
            g.init = 1;
          }
          break;

        case 'D':  //Fast forward (rotate CCW)
          if (g.shooting == 1)
            break;
          if (fake_key == 0 && g.run == 0 && g.rewind == 0)
          {
            g.run = 1;
            g.rewind = 1;
            g.init = 1;
          }
          break;

#ifdef DEBUG
        // Fine-tuning the FLASH_DELAY parameter, with 5 ms steps
        case '9':
          if (g.flash_delay > 100000)
          {
            g.flash_delay = g.flash_delay - 5000;
            Display(-1);
          }
          break;

        case 'C':
          if (g.flash_delay < DT_AF - DT_FLASH - 5000)
          {
            g.flash_delay = g.flash_delay + 5000;
            Display(-1);
          }
          break;
#endif
      }

      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // End of processing specific key presses


    }  // if (PRESSED)

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    else
      // if a key was just released
    {
      // Resetting the counter of key repeats:
      g.N_repeats = 0;
      if (g.rewind != 0)
        g.run = 0;
    }

  }  // End of if(keyStateChanged)


  return;
}

