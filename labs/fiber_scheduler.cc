#include "labs/fiber_scheduler.h"

// Helper Function
int fib_foo(int n, bool *pdone, addr_t *pmain_stack, addr_t *pf_stack, preempt_t *ppreempt)
{
  //F(n)=F(n-1)+F(n-2)

  addr_t &main_stack = *pmain_stack; // boilerplate: to ease the transition from existing code
  addr_t &f_stack = *pf_stack;
  bool &done = *pdone;
  preempt_t &preempt = *ppreempt;

  if (n == 0 || n == 1)
  {
    return n;
  }
  else
  {
    int temp = fib_foo(n - 1, &done, &main_stack, &f_stack, &preempt);
    // done=false; preempt.race_flag = 1; stack_saverestore(f_stack,main_stack);

    temp = temp + fib_foo(n - 2, &done, &main_stack, &f_stack, &preempt);
    // done=false; preempt.race_flag = 1; stack_saverestore(f_stack,main_stack);

    return temp;
  }
}

// Recursive implementation of fib (execute without yields in between the computation)
void long_task_fiber_fib_rec(addr_t *pmain_stack, addr_t *pf_stack, int *pret, bool *pdone, int *pn, preempt_t *ppreempt)
{
  addr_t &main_stack = *pmain_stack; // boilerplate: to ease the transition from existing code
  addr_t &f_stack = *pf_stack;
  int &ret = *pret;
  bool &done = *pdone;
  int &n = *pn;
  preempt_t &preempt = *ppreempt;

  ret = fib_foo(n, &done, &main_stack, &f_stack, &preempt);

  for (;;)
  {
    done = true;
    preempt.race_flag = 1;
    stack_saverestore(f_stack, main_stack);
  }
}

// Helper Function
int hanoi_foo(int n, bool *pdone, addr_t *pmain_stack, addr_t *pf_stack, preempt_t *ppreempt)
{
  //F(n)=F(n-1)+F(n-1)+1

  addr_t &main_stack = *pmain_stack; // boilerplate: to ease the transition from existing code
  addr_t &f_stack = *pf_stack;
  bool &done = *pdone;
  preempt_t &preempt = *ppreempt;

  if (n == 0 || n == 1)
  {
    return 1;
  }
  else
  {
    int temp = hanoi_foo(n - 1, &done, &main_stack, &f_stack, &preempt);
    done = false;
    preempt.race_flag = 1;
    stack_saverestore(f_stack, main_stack);

    temp = temp + hanoi_foo(n - 1, &done, &main_stack, &f_stack, &preempt) + 1;
    done = false;
    preempt.race_flag = 1;
    stack_saverestore(f_stack, main_stack);

    return temp;
  }
}

// Recursive implementation of tower of hanoi (execute with yields - stack_saverestore in between the computation)
void long_task_fiber_hanoi(addr_t *pmain_stack, addr_t *pf_stack, int *pret, bool *pdone, int *pn, preempt_t *ppreempt)
{
  addr_t &main_stack = *pmain_stack; // boilerplate: to ease the transition from existing code
  addr_t &f_stack = *pf_stack;
  int &ret = *pret;
  bool &done = *pdone;
  int &n = *pn;
  preempt_t &preempt = *ppreempt;

  ret = hanoi_foo(n, &done, &main_stack, &f_stack, &preempt);

  for (;;)
  {
    done = true;
    preempt.race_flag = 1;
    stack_saverestore(f_stack, main_stack);
  }
}

//
// stackptrs:      Type: addr_t[stackptrs_size].  array of stack pointers (generalizing: main_stack and f_stack)
// stackptrs_size: number of elements in 'stacks'.
//
// arrays:      Type: uint8_t [arrays_size]. array of memory region for stacks (generalizing: f_array)
// arrays_size: size of 'arrays'. equal to stackptrs_size*STACK_SIZE.
//
// Tip: divide arrays into stackptrs_size parts.
// Tip: you may implement a circular buffer using arrays inside shellstate_t
//      if you choose linked lists, create linked linked using arrays in
//      shellstate_t. (use array indexes as next pointers)
// Note: malloc/new is not available at this point.

// Find position to insert new fiber
int find_pos(shellstate_t &shellstate)
{
  for (int i = 0; i < 5; i++)
  {
    if (shellstate.long_task_state[i] == -1)
    {
      shellstate.long_task_state[i] = shellstate.menu - 6;
      return i;
    }
  }
}

int min(int a, int b)
{
  if (a < b)
  {
    return a;
  }
  return b;
}

// Run multiple instances of a long tasks in parallel with timer interrupts
// Atmost 3 instances of long task will be active at any instant.
void shell_step_fiber_scheduler(shellstate_t &shellstate, addr_t &main_stack, preempt_t &preempt,
                                addr_t stackptrs[], size_t stackptrs_size, addr_t arrays, size_t arrays_size, dev_lapic_t &lapic)
{

  int STACK_SIZE = arrays_size / stackptrs_size;

  // If enter pressed at menu 6 or 7
  if (shellstate.state == 2 && (shellstate.menu == 6 || shellstate.menu == 7))
  {
    // If less than 5 long tasks pending
    if (shellstate.num_long_task[0] + shellstate.num_long_task[1] < min(5, stackptrs_size))
    {
      // If particular task has less than 3 tasks pending
      if ((shellstate.menu == 6 && shellstate.num_long_task[0] < 3) || (shellstate.menu == 7 && shellstate.num_long_task[1] < 3))
      { // can run now
        int add_pos = find_pos(shellstate);
        shellstate.long_task_n[add_pos] = sanity_check(shellstate);
        if (shellstate.cur_line >= shellstate.max_length)
        {
          shift(shellstate);
        }
        if (shellstate.menu == 6)
        {
          store_input(shellstate, "$ fibrec", 8, 0, shellstate.cur_line);
          store_input(shellstate, shellstate.inp, shellstate.inp_size, 9, shellstate.cur_line);
        }
        else
        {
          store_input(shellstate, "$ towerofhanoi", 14, 0, shellstate.cur_line);
          store_input(shellstate, shellstate.inp, shellstate.inp_size, 15, shellstate.cur_line);
        }
        shellstate.state = 0;
        shellstate.inp_size = 0;
        if (shellstate.long_task_n[add_pos] == -1)
        {
          store_input(shellstate, "Invalid Argument", 16, 2, shellstate.cur_line + 1);
          shellstate.cur_line += 2;
          shellstate.long_task_state[add_pos] = -1;
          return;
        }
        else
        {
          if (shellstate.menu == 6)
            stack_init6(stackptrs[add_pos + 1], &arrays[(add_pos + 1) * STACK_SIZE], STACK_SIZE, &long_task_fiber_fib_rec,
                        &main_stack, &stackptrs[add_pos + 1], &shellstate.long_task_result[add_pos],
                        &shellstate.long_task_done[add_pos], &shellstate.long_task_n[add_pos], &preempt);
          else
            stack_init6(stackptrs[add_pos + 1], &arrays[(add_pos + 1) * STACK_SIZE], STACK_SIZE, &long_task_fiber_hanoi,
                        &main_stack, &stackptrs[add_pos + 1], &shellstate.long_task_result[add_pos],
                        &shellstate.long_task_done[add_pos], &shellstate.long_task_n[add_pos], &preempt);

          shellstate.num_long_task[shellstate.menu - 6] += 1;
          shellstate.long_task_line[add_pos] = shellstate.cur_line;
          shellstate.cur_line += 2;
          shellstate.long_task_done[add_pos] = false;
          shellstate.long_task_result[add_pos] = 0;
        }
      }
    }
  }

  if (shellstate.num_long_task[0] + shellstate.num_long_task[1] > 0)
  { // If there is a RUNNABLE long_task

    for (int i = 0; i < 5; i++)
    {
      if (shellstate.long_task_state[shellstate.pos_long_task] == -1)
        shellstate.pos_long_task = (shellstate.pos_long_task + 1) % 5;
      else
      {
        if (shellstate.long_task_line[shellstate.pos_long_task] < 0) // Not visible in Interface, so don't compute
        {
          shellstate.num_long_task[shellstate.long_task_state[shellstate.pos_long_task]] -= 1;
          shellstate.long_task_state[shellstate.pos_long_task] = -1;
          shellstate.pos_long_task = (shellstate.pos_long_task + 1) % 5;
          return;
        }

        preempt.race_flag = 0; // Race Flag 1 during voluntary yield
        preempt.flag = 0; // Flag 1 if Preemption through timer interrupt
        lapic.reset_timer_count(5); //Call Interrupt

        //Fiber
        stack_saverestore(main_stack, stackptrs[shellstate.pos_long_task + 1]);

        preempt.race_flag = 0;
        if (preempt.flag == 1) // Arrived here by timer interrupt
        {
          stackptrs[shellstate.pos_long_task + 1] = preempt.saved_stack;
          preempt.flag = 0;
        }

        lapic.reset_timer_count(0); //Kills interrupt

        //If completed the selected long task
        if (shellstate.long_task_done[shellstate.pos_long_task])
        {
          char str[100];
          int l = itoa(shellstate.long_task_result[shellstate.pos_long_task], str);
          store_input(shellstate, str, l, 2, shellstate.long_task_line[shellstate.pos_long_task] + 1);
          shellstate.num_long_task[shellstate.long_task_state[shellstate.pos_long_task]] -= 1;
          shellstate.long_task_state[shellstate.pos_long_task] = -1;
        }
        shellstate.pos_long_task = (shellstate.pos_long_task + 1) % 5; // Pass to next position
        return;
      }
    }
  }
}
