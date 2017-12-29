void set_backlight()
// Setting the LCD backlight.
{

  analogWrite(PIN_LCD_LED, 255 - Backlight[g.backlight]);

  return;
}


void message(char * str1, char * str2)
// Printing help messages
{

  g.t_message = g.t;
  g.message = 1;

  lcd.setCursor(0, 4 * ROW);
  lcd.print(str1);

  lcd.setCursor(0, 5 * ROW);
  lcd.print(str2);

}


void Display(int code)
// Displaying stuff
{
  lcd.clearDisplay();

// Row 1:
  lcd.setCursor(0, 0);
  sprintf(g.buffer, "t=%d.%1d", g.reg.dt_shots / 10, g.reg.dt_shots % 10);
  lcd.print(g.buffer);

  lcd.setCursor(8 * COL, 0);
  sprintf(g.buffer, "N=%3d", g.reg.N_shots);
  lcd.print(g.buffer);
  
// Row 2:
  lcd.setCursor(0, 1 * ROW);
  sprintf(g.buffer, "Reg=%1d", g.ireg);
  lcd.print(g.buffer);

  lcd.setCursor(8 * COL, 1 * ROW);
  sprintf(g.buffer, "dN=%2d", g.reg.extra_shots);
  lcd.print(g.buffer);

// Row 3:
  lcd.setCursor(0, 2 * ROW);
  sprintf(g.buffer, "BL=%1d", g.backlight);
  lcd.print(g.buffer);

  switch (code)
  {
    case 0:  // Display memory register
      message("Memory",
              "register");
      break;

    case 1: // Display dt_shots
      message("Time between",
              "shots");
      break;

    case 2: // Display N_shots
      message("Shots per",
              "rotation");
      break;

    case 3: // Display extra_shots
      message("Extra shots",
              "");
      break;

    case 4: // Display backlight
      message("Backlight",
              "level");
      break;

  }

  lcd.drawLine(0, 26, LCDWIDTH - 1, 26, BLACK);
  lcd.drawLine(42, 0, 42, 25, BLACK);
  lcd.display();

}

