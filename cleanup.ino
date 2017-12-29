void cleanup()
// End of Arduino loop stuff
{

  g.init = 0;
  
  if (g.message == 1 && g.t > g.t_message + DT_MESSAGE)
  {
    g.message = 0;
    Display(-1);
  }
}
