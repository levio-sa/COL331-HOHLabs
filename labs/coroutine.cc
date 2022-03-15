#include "labs/coroutine.h"

// Long task which counts the number of triplets of the form a^2 + b^2 = c^2
void long_task_new(coroutine_t *pf_coro, f_t *pf_locals, int *pret, bool *pdone, int n)
{
  coroutine_t &f_coro = *pf_coro; // boilerplate: to ease the transition from existing code
  int &ret = *pret;
  bool &done = *pdone;
  int &i = pf_locals->i;
  int &j = pf_locals->j;
  int &k = pf_locals->k;

  h_begin(f_coro);

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
        h_yield(f_coro);
      }
    }
  }
  done = true;
  h_end(f_coro);
}

// Kernel become responsive to key presses
// Only one long task can run, if we try to run another long task the kernel will not take any new task
// But It remains responsive to key presses.
void shell_step_coroutine(shellstate_t &shellstate, coroutine_t &f_coro, f_t &f_locals)
{

  if (shellstate.state == 2 && shellstate.menu == 4)
  {
    if (shellstate.cor_flag == 0 && shellstate.fib_flag == 0)
    { // Another long task not running

      shellstate.cor_n = sanity_check(shellstate);
      if (shellstate.cur_line >= shellstate.max_length)
      { // Maximum lines that will be printed
        shift(shellstate);
      }
      store_input(shellstate, "$ longcoroutine", 15, 0, shellstate.cur_line);
      store_input(shellstate, shellstate.inp, shellstate.inp_size, 16, shellstate.cur_line);
      shellstate.state = 0;
      shellstate.inp_size = 0;
      if (shellstate.cor_n == -1)
      {
        store_input(shellstate, "Invalid Argument", 16, 2, shellstate.cur_line + 1);
        shellstate.cur_line += 2;
        shellstate.cor_flag = 0;
        return;
      }
      else
      {
        coroutine_reset(f_coro);
        shellstate.cor_line = shellstate.cur_line;
        shellstate.cur_line += 2;
        shellstate.cor_done = false;
        shellstate.cor_result = 0;
        shellstate.cor_flag = 1;
      }
    }
  }
  if (shellstate.cor_flag == 1)
  {
    if (shellstate.cor_line < 0)
    {
      shellstate.cor_flag = 0;
      return;
    }
    // Coroutine
    long_task_new(&f_coro, &f_locals, &shellstate.cor_result, &shellstate.cor_done, shellstate.cor_n);
    if (shellstate.cor_done)
    {
      char str[100];
      int l = itoa(shellstate.cor_result, str);
      store_input(shellstate, str, l, 2, shellstate.cor_line + 1);
      shellstate.cor_flag = 0;
    }
  }
}
