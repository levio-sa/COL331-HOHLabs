#include "labs/coroutine.h"

int sanity_check_cr(shellstate_t& shellstate){ //Returns number if given string is numeric otherwise -1
  
  int result = 0;
  for(int i=0; i<shellstate.inp_size; ++i){
    result*=10;
    char temp=shellstate.inp[i];
    if(temp < '0' || temp > '9'){
      return -1;
    }
    else result+=(temp-'0'); 
  }
  return result;
}

void store_input_cr(shellstate_t& shellstate, char *str, int length, int xpos, int ypos){ // Stores input in display
  for(int j = 0; j < length; j++){
     shellstate.display[ypos][xpos + j] = str[j];
  }
}

int itoa_cr(int value, char *str){  //Convert integer to string with no leading zeros
  if(value == 0){   
    *str = '0';
    return 1;
  }
  int length = 0;
  int temp;
  for(temp = value; temp > 0; temp/=10, str++);
  *str = '\0';
  for(temp = value; temp > 0;temp/=10){
    *--str = temp % 10 + '0';
    length++;
  }
  return length;
}

void shift_cr(shellstate_t& shellstate){ //Shifts  up the rows in display
  for(int i = 1; i < shellstate.cur_line; i++){
    for(int j = 0; j < 100; j++){
      shellstate.display[i - 1][j] = shellstate.display[i][j];
    }
  }

  for(int j = 0; j < 100; j++){
    shellstate.display[shellstate.cur_line - 1][j] = ' ';
  }
}



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
        shellstate.done = false;
        shellstate.result = 0;
        shellstate.cor_flag = 1;
      }

      int temp_n = sanity_check_cr(shellstate);
      if(temp_n!=-1) long_task_new(&f_coro, &f_locals, &shellstate.result, &shellstate.done, temp_n);

      if(temp_n == -1){
        if(shellstate.cur_line >= 13){ // Maximum lines that will be printed
          shift_cr(shellstate);
          shellstate.cur_line -= 1;
        }
        store_input_cr(shellstate, "$ longcoroutine", 15, 0, shellstate.cur_line);
        store_input_cr(shellstate, shellstate.inp, shellstate.inp_size, 16, shellstate.cur_line);
        store_input_cr(shellstate, "Invalid Argument", 16, 2, shellstate.cur_line + 1);
        shellstate.state = 3;
        shellstate.cur_line += 2;
        shellstate.cor_flag = 0;
      }

      if(shellstate.done){ 
        char str[100];
        int l = itoa_cr(shellstate.result, str);
        if(shellstate.cur_line >= 13){ // Maximum lines that will be printed
          shift_cr(shellstate);
          shellstate.cur_line -= 1;
        }
        store_input_cr(shellstate, "$ longcoroutine", 15, 0, shellstate.cur_line);
        store_input_cr(shellstate, shellstate.inp, shellstate.inp_size, 16, shellstate.cur_line);
        store_input_cr(shellstate, str, l, 2, shellstate.cur_line + 1);
        shellstate.state = 3;
        shellstate.cur_line += 2;
        shellstate.cor_flag = 0;
      } 
    }

 }




