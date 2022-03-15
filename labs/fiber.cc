#include "labs/fiber.h"

void long_task_fiber(addr_t *pmain_stack, addr_t *pf_stack, int *pret, bool *pdone, int *pn)
{
  addr_t &main_stack = *pmain_stack; // boilerplate: to ease the transition from existing code
  addr_t &f_stack = *pf_stack;
  int &ret = *pret;
  bool &done = *pdone;
  int &n = *pn;

  int i;
  int j;
  int k;

  for (i = 0; i <= n; i++)
  {
    for (j = 0; j <= n; j++)
    {
      for (k = 0; k <= n; k++)
      {
        if (i * i + j * j - k * k == 0)
        {
          ret++;
        }
        done = false;
        stack_saverestore(f_stack, main_stack);
      }
    }
  }
  for (;;)
  {
    done = true;
    stack_saverestore(f_stack, main_stack);
  }
}

void shell_step_fiber(shellstate_t &shellstate, addr_t &main_stack, preempt_t &preempt, addr_t &f_stack, addr_t f_array,
                      uint32_t f_arraysize, dev_lapic_t &lapic)
{
  if (shellstate.state == 2 && shellstate.menu == 5)
  {
    if (shellstate.fib_flag == 0 && shellstate.cor_flag == 0)
    { // Another long task already not running
      shellstate.fib_n = sanity_check(shellstate);
      if (shellstate.cur_line >= shellstate.max_length)
      { // Maximum lines that will be printed
        shift(shellstate);
      }
      store_input(shellstate, "$ longfiber", 11, 0, shellstate.cur_line);
      store_input(shellstate, shellstate.inp, shellstate.inp_size, 12, shellstate.cur_line);
      shellstate.state = 0;
      shellstate.inp_size = 0;
      if (shellstate.fib_n == -1)
      {
        store_input(shellstate, "Invalid Argument", 16, 2, shellstate.cur_line + 1);
        shellstate.cur_line += 2;
        shellstate.fib_flag = 0;
        return;
      }
      else
      {
        stack_init5(f_stack, &f_array, f_arraysize, &long_task_fiber, &main_stack, &f_stack, &shellstate.fib_result, &shellstate.fib_done, &shellstate.fib_n); // temp_n needs to be a permanent variable??
        shellstate.fiber_line = shellstate.cur_line;
        shellstate.cur_line += 2;
        shellstate.fib_done = false;
        shellstate.fib_result = 0;
        shellstate.fib_flag = 1;
      }
    }
  }
  if (shellstate.fib_flag == 1)
  {
    if (shellstate.fiber_line < 0)
    {
      shellstate.fib_flag = 0;
      return;
    }
    // Fiber
    stack_saverestore(main_stack, f_stack);
    if (shellstate.fib_done)
    {
      char str[100];
      int l = itoa(shellstate.fib_result, str);
      store_input(shellstate, str, l, 2, shellstate.fiber_line + 1);
      shellstate.fib_flag = 0;
    }
  }
}
