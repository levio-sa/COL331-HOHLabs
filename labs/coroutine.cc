#include "labs/coroutine.h"

void long_task_new(coroutine_t* pf_coro, f_t* pf_locals, int* pret, bool* pdone, int n){ 
    coroutine_t& f_coro = *pf_coro; // boilerplate: to ease the transition from existing code
    int& ret            = *pret;
    bool& done          = *pdone;
    int& i = pf_locals->i;
    int& j = pf_locals->j;
    int& k = pf_locals->k;
    
    h_begin(f_coro);

    for(i=0;i<=n;i++){
      for(j=0;j<=n;j++){
        for(k=0;k<=n;k++){
          if(i*i+j*j-k*k==0){
            ret++;
          }
          done = false; 
          h_yield(f_coro);
        }
      }
    }
    done=true;
    h_end(f_coro);
}

void shell_step_coroutine(shellstate_t& shellstate, coroutine_t& f_coro, f_t& f_locals){

    //insert your code here
    if(shellstate.state == 2 && shellstate.menu == 4){
      if(shellstate.cor_flag == 0){
        coroutine_reset(f_coro);
        shellstate.cor_done = false;
        shellstate.result = 0;
        shellstate.cor_flag = 1;
      }

      int temp_n = sanity_check(shellstate);
      if(temp_n!=-1) long_task_new(&f_coro, &f_locals, &shellstate.result, &shellstate.cor_done, temp_n);

      if(temp_n == -1){
        if(shellstate.cur_line >= 12){ // Maximum lines that will be printed
          shift(shellstate);
          shellstate.cur_line -= 1;
        }
        store_input(shellstate, "$ longcoroutine", 15, 0, shellstate.cur_line);
        store_input(shellstate, shellstate.inp, shellstate.inp_size, 16, shellstate.cur_line);
        store_input(shellstate, "Invalid Argument", 16, 2, shellstate.cur_line + 1);
        shellstate.state = 3;
        shellstate.cur_line += 2;
        shellstate.cor_flag = 0;
      }

      if(shellstate.cor_done){ 
        char str[100];
        int l = itoa(shellstate.result, str);
        if(shellstate.cur_line >= 12){ // Maximum lines that will be printed
          shift(shellstate);
          shellstate.cur_line -= 1;
        }
        store_input(shellstate, "$ longcoroutine", 15, 0, shellstate.cur_line);
        store_input(shellstate, shellstate.inp, shellstate.inp_size, 16, shellstate.cur_line);
        store_input(shellstate, str, l, 2, shellstate.cur_line + 1);
        shellstate.state = 3;
        shellstate.cur_line += 2;
        shellstate.cor_flag = 0;
      } 
    }

 }




