#include "labs/shell.h"
#include "labs/vgatext.h"

//
// initialize shellstate
//
void shell_init(shellstate_t& state){
  state.key_press = 0;
  state.menu = 0;
  state.state = 0; // 0 select menu, 1 take input, 2 computing, 3 displaying output
  state.inp_size=0;
  state.result = 0;
  state.num_menu = 9;
  state.cur_line = 0;
  state.funk = 0;
  state.max_length = 13;
  for(int i = 0; i < 100; i++){
    for(int j = 0; j < 100; j++){
      state.display[i][j] = ' ';
    }
  }

  // 2.1
  state.cor_flag = 0;
  state.cor_done = false;
  state.cor_line = 0;
  state.cor_result = 0;
  state.cor_n = 0;

  // 2.2
  state.fib_flag = 0; 
  state.fib_done = false;
  state.fiber_line = 0;
  state.fib_result = 0;
  state.fib_n = 0;

  // 2.3
  state.num_long_task[0]=0;
  state.num_long_task[1]=0;
  state.pos_long_task = 0;

  for(int i = 0;i < 5;i++){
    state.long_task_state[i] = -1;
    state.long_task_line[i] = 0;
    state.long_task_result[i] = 0;
    state.long_task_n[i] = 0;
  }
}

//
// handle keyboard event.
// key is in scancode format.
// For ex:
// scancode for following keys are:
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | esc |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 |  0 |  - |  = |back|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 01  | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 0a | 0b | 0c | 0d | 0e |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     | tab |  q |  w |  e |  r |  t |  y |  u |  i |  o |  p |  [ |  ] |entr|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 0f  | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 1a | 1b | 1c |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | keys     |ctrl |  a |  s |  d |  f |  g |  h |  j |  k |  l |  ; |  ' |    |shft|
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//      | scancode | 1d  | 1e | 1f | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 2a |
//      +----------+-----+----+----+----+----+----+----+----+----+----+----+----+----+----+
//
// so and so..
//
// - restrict yourself to: 0-9, a-z, esc, enter, arrows
// - ignore other keys like shift, control keys
// - only handle the keys which you're interested in
// - for example, you may want to handle up(0x48),down(0x50) arrow keys for menu.
//

char get_char_from_scan(uint8_t scankey){ // returns character from scancode
    if(2 <= scankey && scankey <= 11){ //0 to 9
      return (scankey-1)%10+'0';
    }
    else{
      switch (scankey)
      {
        case 0x1e:
          return 'a';
          break; 

        case 0x30:
          return 'b';
          break;

        case 0x2e:
          return 'c';
          break;

        case 0x20:
          return 'd';
          break;

        case 0x12:
          return 'e';
          break;
          
        case 0x21:
          return 'f';
          break;
          
        case 0x22:
          return 'g';
          break;

        case 0x23:
          return 'h';
          break;

        case 0x17:
          return 'i';
          break;

        case 0x24:
          return 'j';
          break;

        case 0x25:
          return 'k';
          break;

        case 0x26:
          return 'l';
          break;

        case 0x32:
          return 'm';
          break;


        case 0x31:
          return 'n';
          break;


        case 0x18:
          return 'o';
          break;


        case 0x19:
          return 'p';
          break;

        case 0x10:
          return 'q';
          break;

        case 0x13:
          return 'r';
          break;


        case 0x1f:
          return 's';
          break;


        case 0x14:
          return 't';
          break;

        case 0x16:
          return 'u';
          break;


        case 0x2f:
          return 'v';
          break;


        case 0x11:
          return 'w';
          break;

        case 0x2d:
          return 'x';
          break;


        case 0x15:
          return 'y';
          break;

        case 0x2c:
          return 'z';
            break;
        
        case 0x39:
          return ' ';
          break;

        default:
          return '$';  //default value
          break;
      }
    }
}

void shell_update(uint8_t scankey, shellstate_t& stateinout){
    
    if(scankey) stateinout.key_press+=1; // Update Keypresses
    
    if(stateinout.state == 0){ // Select menu item
      switch(scankey)
      {
        case 72:
          stateinout.menu = (stateinout.num_menu-1+stateinout.menu)%stateinout.num_menu;;
          break;

        case 80:
          stateinout.menu = (1+stateinout.menu)%stateinout.num_menu;
          break; 

        case 28:
          stateinout.state = 1;
          stateinout.inp_size = 0;
          break;

        default:
          break;
      }

    }
    else if(stateinout.state == 1){ // Take input
      if(scankey == 28) stateinout.state = 2;
      // if(scankey == 28){
      //   stateinout.state = 0;
      //   stateinout.enter = 1; 
      // }
      else if(scankey == 0x0e && stateinout.inp_size>0) stateinout.inp_size--;
      else if(scankey == 1) stateinout.state = 0;
      else {
        char tmp = get_char_from_scan(scankey);
        if(tmp!='$') stateinout.inp[stateinout.inp_size++] = tmp;
      }      
    }
}




int factors(int arg){ // Number of factors
  int ct=0;
  for(int i=1;i<=arg;i++){
    if(arg%i==0){
      ct+=1;
    }
  }
  return ct;
}

int long_task(int n){  //No of tuples(i,j,k) such that i^2+j^2=k^2 where k<=n  and i,j,k>=0 
    int ct=0;           //O(n^3) time
    for(int i=0;i<=n;i++){
      for(int j=0;j<=n;j++){
        for(int k=0;k<=n;k++){
          if(i*i+j*j-k*k==0){
            ct++;
          }
        }
      }
    }
    return ct;
}

int factorial(int arg){  //Factorial Function
  if(arg<0){
    return -1;
  }
  else if(arg==0 || arg==1){
    return 1;
  }
  else{
    int pro=1;
    for(int j=1;j<=arg;j++){
      pro*=j;
    }
    return pro;
  }
}

int fib(int n){ //nth Fibonacci number

  int arr[50];
  if(n==0){
    return 0;
  }
  else if(n==1){
    return 1;
  }
  else{
    arr[0]=0;
    arr[1]=1;
    for(int i=2;i<=n;i++){
      arr[i]=arr[i-1]+arr[i-2];
    }
    return arr[n];
    
  }

}




int sanity_check(shellstate_t& stateinout){ //Returns number if given string is numeric otherwise -1
  
  int result = 0;
  for(int i=0; i<stateinout.inp_size; ++i){
    result*=10;
    char temp=stateinout.inp[i];
    if(temp < '0' || temp > '9'){
      return -1;
    }
    else result+=(temp-'0'); 
  }
  return result;
}

void store_input(shellstate_t& stateinout, char *str, int length, int xpos, int ypos){ // Stores input in display
  for(int j = 0; j < length; j++){
     stateinout.display[ypos][xpos + j] = str[j];
  }
}

int itoa(int value, char *str){  //Convert integer to string with no leading zeros
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

void shift(shellstate_t& stateinout){ //Shifts  up the rows in display
  for(int i = 1; i < stateinout.cur_line; i++){
    for(int j = 0; j < 100; j++){
      stateinout.display[i - 1][j] = stateinout.display[i][j];
    }
  }

  for(int j = 0; j < 100; j++){
    stateinout.display[stateinout.cur_line - 1][j] = ' ';
  }
  stateinout.cur_line-=1;
  stateinout.cor_line-=1;
  stateinout.fiber_line-=1;
  for(int i=0;i<5;++i){
    stateinout.long_task_line[i]-=1;
  }
}

int strcmp(const char *X, const char *Y){ // Compare strings
  while (*X && *X==*Y){
    X++;
    Y++;
  }
  return *(const unsigned char*)X - *(const unsigned char*)Y;
}

int sanity_check_cl(shellstate_t& stateinout){ //Sanity check for Command Line and returns length of argument
  char fun[100];
  int i=0;
  for(i=0; i<stateinout.inp_size; ++i){
    if(stateinout.inp[i]!=' ') fun[i]=stateinout.inp[i];
    else{
      fun[i]='\0';
      break;
    }
  }
  store_input(stateinout, fun, i, 2, stateinout.cur_line);
  for(i;i<stateinout.inp_size;++i){
    if(stateinout.inp[i]!=' ') break;
  }
  int fun_length=i;
  int k = 0;
  for(i;i<stateinout.inp_size;++i){
    stateinout.inp[k++] = stateinout.inp[i];
  }
  stateinout.inp[k] = '\0';
  stateinout.inp_size = k;
  if(strcmp(fun, "echo")==0){
    stateinout.funk = 0;
    return 6;
  }
  else if(strcmp(fun,"tripletcount")==0){
    stateinout.funk = 1;
    stateinout.result = sanity_check(stateinout);
    if(stateinout.result!=-1) stateinout.result = long_task(stateinout.result);
    return 14;
  }
  else if(strcmp(fun, "factorial")==0){
    stateinout.funk = 2;
    stateinout.result = sanity_check(stateinout);
    if(stateinout.result!=-1) stateinout.result = factorial(stateinout.result);
    return 11;
  }
  else if(strcmp(fun,"fib")==0){
    stateinout.funk = 3;
    stateinout.result = sanity_check(stateinout);
    if(stateinout.result!=-1) stateinout.result = fib(stateinout.result);
    return 5;
  }
  else if(strcmp(fun,"factors")==0){
    stateinout.funk = 4;
    stateinout.result = sanity_check(stateinout);
    if(stateinout.result!=-1) stateinout.result = factors(stateinout.result);
    return 9;
  }
  else{
    stateinout.funk = -1;
    stateinout.result=-1;
    return fun_length+1;

  }
}

//
// do computation
//
void shell_step(shellstate_t& stateinout){

  //
  //one way:
  // if a function is enabled in stateinout
  //   call that function( with arguments stored in stateinout) ;
//stateinout.args[0] = 5;
//stateinout.args[1]= 5;
  if(stateinout.state == 2 && (stateinout.menu == 4 || stateinout.menu == 5 || stateinout.menu == 6 || stateinout.menu == 7)){
    return;
  }
  int margin = 0;
  if(stateinout.cur_line >= stateinout.max_length){ // Maximum lines that will be printed
    shift(stateinout);
  } 
  
  if(stateinout.state == 2){  //echo fib factorial factors NOTA
    switch(stateinout.menu){
      case 0: // echo
        store_input(stateinout, "$ echo", 6, 0, stateinout.cur_line);
        margin = 6;
        break;
      case 1:
        store_input(stateinout, "$ tripletcount", 14, 0, stateinout.cur_line);
        margin = 14;
        stateinout.result = sanity_check(stateinout);
        if(stateinout.result!=-1) stateinout.result = long_task(stateinout.result);
        break;
      case 2:
        store_input(stateinout, "$ factorial", 11, 0, stateinout.cur_line);
        margin = 11;
        stateinout.result = sanity_check(stateinout);
        if(stateinout.result!=-1) stateinout.result = factorial(stateinout.result);
        break;
       
      case 3:
        store_input(stateinout, "$ fib",5, 0, stateinout.cur_line);
        margin = 5;
        stateinout.result = sanity_check(stateinout);
        if(stateinout.result!=-1) stateinout.result = fib(stateinout.result);
        break;
      
      case 8:
        store_input(stateinout, "$ ", 2, 0, stateinout.cur_line);
        margin=2;
        margin = sanity_check_cl(stateinout);        
        
      default:
        break;
    }
    store_input(stateinout, stateinout.inp, stateinout.inp_size, margin + 1, stateinout.cur_line);

    if(stateinout.menu == 0 ){
      store_input(stateinout, stateinout.inp, stateinout.inp_size, 2, stateinout.cur_line + 1);
    }
    else if(stateinout.menu == 8 && stateinout.funk == 0){
      store_input(stateinout, stateinout.inp, stateinout.inp_size, 2, stateinout.cur_line + 1);
    }
    else if(stateinout.result == -1){
      store_input(stateinout, "Invalid Argument", 16, 2, stateinout.cur_line + 1);
    }
    else{
      char str[100];
      int l = itoa(stateinout.result, str);
      store_input(stateinout, str, l, 2, stateinout.cur_line + 1);
    }
    stateinout.state = 0;
    stateinout.inp_size = 0;
    stateinout.cur_line += 2;
  }
  
}


//
// shellstate --> renderstate
//
void shell_render(const shellstate_t& shell, renderstate_t& render){
  render.key_press = shell.key_press;
  render.state = shell.state; // 0 select menu, 1 take input, 3 displaying output
  if(render.inp_size <= shell.inp_size){
    for(int i=render.inp_size; i<shell.inp_size; ++i) render.inp[i] = shell.inp[i];
  }
  for(int i = 0; i < shell.cur_line; i++){
    render.display[i] = const_cast<char*> (shell.display[i]);
  }
  render.cur_line = shell.cur_line;
  render.inp_size = shell.inp_size;
  render.menu = shell.menu; // Highlighted menu
  render.result = shell.result; 
  render.cor_done = shell.cor_done;
  render.fib_done = shell.fib_done;
  render.cor_flag = shell.cor_flag;
  for(int i=0;i<5;i++){
    render.long_task_state[i] = shell.long_task_state[i];
    render.long_task_line[i] = shell.long_task_line[i];
    render.long_task_done[i] = shell.long_task_done[i];
    render.long_task_result[i] = shell.long_task_result[i];
    render.long_task_n[i] = shell.long_task_n[i];
  }
  //
  // renderstate. number of keys pressed = shellstate. number of keys pressed
  //
  // renderstate. menu highlighted = shellstate. menu highlighted
  //
  // renderstate. function result = shellstate. output argument
  //
  // etc.
  //
}


//
// compare a and b
//
bool render_eq(const renderstate_t& a, const renderstate_t& b){
  if(a.key_press == b.key_press && a.state == b.state && a.menu == b.menu && a.result == b.result && 
  a.inp_size==b.inp_size && a.cur_line == b.cur_line && a.cor_flag==b.cor_flag && a.cor_done==b.cor_done
   && a.fib_done==b.fib_done && a.fib_flag==b.fib_flag){
    for(int i=0; i<a.inp_size; ++i){
      if(a.inp[i] != b.inp[i]) return false;
    }
    for(int i=0; i<b.cur_line; ++i){
      if(strcmp(a.display[i], b.display[i]) != 0) return false;
    }
    for(int i=0;i<5;i++){
      if(a.long_task_state[i] != b.long_task_state[i]) return false;
      if(a.long_task_line[i] != b.long_task_line[i]) return false;
      if(a.long_task_done[i] != b.long_task_done[i]) return false;
      if(a.long_task_result[i] != b.long_task_result[i]) return false;
      if(a.long_task_n[i] != b.long_task_n[i]) return false;
    }
    return true;
  }
  else{
    return false;
  }
}


static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);
static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base);


//
// Given a render state, we need to write it into vgatext buffer
//
void render(const renderstate_t& state, int w, int h, addr_t vgatext_base){

  char presses[10];      
  
  fillrect(0,0,w,h,14,2,w,h,vgatext_base); //Filling the whole space
  fillrect(0,h-1,w,h,12,2,w,h,vgatext_base); //Filling bottom for KeyPresses status bar
  drawtext(35,0,"Menu",9,14,0,w,h,vgatext_base); //Text Menu at center in top
  drawtext(1,h-1,"Key Presses",16,12,4,w,h,vgatext_base); //Key Presses text
  
  int l=itoa(state.key_press,presses);//convert number of presses to string
  drawtext(19,h-1,presses,3,12,4,w,h,vgatext_base); //Count of keypresses
  if(state.menu == 0){  //Coloring the corresponding menu click in grey
    fillrect(0,1,w,2,8,2,w,h,vgatext_base);    
    drawtext(1,1,"echo",4,8,0,w,h,vgatext_base);
    drawtext(1,2,"tripletcount",12,14,0,w,h,vgatext_base);
    drawtext(1,3,"factorial",9,14,0,w,h,vgatext_base);
    drawtext(1,4,"fib",3,14,0,w,h,vgatext_base); 
    drawtext(1,5,"longcoroutine",13,14,0,w,h,vgatext_base);
    drawtext(1,6,"longfiber",9,14,0,w,h,vgatext_base);       
    drawtext(1,7,"fibrec",6,14,0,w,h,vgatext_base);
    drawtext(1,8,"towerofhanoi",12,14,0,w,h,vgatext_base);
    drawtext(1,9,"cli",3,14,0,w,h,vgatext_base);           
  }
  else if(state.menu == 1){
    fillrect(0,2,w,3,8,2,w,h,vgatext_base);    
    drawtext(1,1,"echo",4,14,0,w,h,vgatext_base);
    drawtext(1,2,"tripletcount",12,8,0,w,h,vgatext_base);
    drawtext(1,3,"factorial",9,14,0,w,h,vgatext_base);
    drawtext(1,4,"fib",3,14,0,w,h,vgatext_base);
    drawtext(1,5,"longcoroutine",13,14,0,w,h,vgatext_base);
    drawtext(1,6,"longfiber",9,14,0,w,h,vgatext_base);  
    drawtext(1,7,"fibrec",6,14,0,w,h,vgatext_base);
    drawtext(1,8,"towerofhanoi",12,14,0,w,h,vgatext_base);
    drawtext(1,9,"cli",3,14,0,w,h,vgatext_base);          
  }
  else if(state.menu == 2){
    fillrect(0,3,w,4,8,2,w,h,vgatext_base);    
    drawtext(1,1,"echo",4,14,0,w,h,vgatext_base);
    drawtext(1,2,"tripletcount",12,14,0,w,h,vgatext_base);
    drawtext(1,3,"factorial",9,8,0,w,h,vgatext_base);
    drawtext(1,4,"fib",3,14,0,w,h,vgatext_base); 
    drawtext(1,5,"longcoroutine",13,14,0,w,h,vgatext_base);
    drawtext(1,6,"longfiber",9,14,0,w,h,vgatext_base); 
    drawtext(1,7,"fibrec",6,14,0,w,h,vgatext_base);
    drawtext(1,8,"towerofhanoi",12,14,0,w,h,vgatext_base);
    drawtext(1,9,"cli",3,14,0,w,h,vgatext_base);        
  }
  else if(state.menu == 3){
    fillrect(0,4,w,5,8,2,w,h,vgatext_base);    
    drawtext(1,1,"echo",4,14,0,w,h,vgatext_base);
    drawtext(1,2,"tripletcount",12,14,0,w,h,vgatext_base);
    drawtext(1,3,"factorial",9,14,0,w,h,vgatext_base);
    drawtext(1,4,"fib",3,8,0,w,h,vgatext_base); 
    drawtext(1,5,"longcoroutine",13,14,0,w,h,vgatext_base);
    drawtext(1,6,"longfiber",9,14,0,w,h,vgatext_base);    
    drawtext(1,7,"fibrec",6,14,0,w,h,vgatext_base);
    drawtext(1,8,"towerofhanoi",12,14,0,w,h,vgatext_base);
    drawtext(1,9,"cli",3,14,0,w,h,vgatext_base);   
  }
  else if(state.menu == 4){
    fillrect(0,5,w,6,8,2,w,h,vgatext_base);    
    drawtext(1,1,"echo",4,14,0,w,h,vgatext_base);
    drawtext(1,2,"tripletcount",12,14,0,w,h,vgatext_base);
    drawtext(1,3,"factorial",9,14,0,w,h,vgatext_base);
    drawtext(1,4,"fib",3,14,0,w,h,vgatext_base); 
    drawtext(1,5,"longcoroutine",13,8,0,w,h,vgatext_base);
    drawtext(1,6,"longfiber",9,14,0,w,h,vgatext_base);    
    drawtext(1,7,"fibrec",6,14,0,w,h,vgatext_base);
    drawtext(1,8,"towerofhanoi",12,14,0,w,h,vgatext_base);
    drawtext(1,9,"cli",3,14,0,w,h,vgatext_base);    
  }
  else if(state.menu == 5){
    fillrect(0,6,w,7,8,2,w,h,vgatext_base);    
    drawtext(1,1,"echo",4,14,0,w,h,vgatext_base);
    drawtext(1,2,"tripletcount",12,14,0,w,h,vgatext_base);
    drawtext(1,3,"factorial",9,14,0,w,h,vgatext_base);
    drawtext(1,4,"fib",3,14,0,w,h,vgatext_base); 
    drawtext(1,5,"longcoroutine",13,14,0,w,h,vgatext_base);
    drawtext(1,6,"longfiber",9,8,0,w,h,vgatext_base);     
    drawtext(1,7,"fibrec",6,14,0,w,h,vgatext_base);
    drawtext(1,8,"towerofhanoi",12,14,0,w,h,vgatext_base);
    drawtext(1,9,"cli",3,14,0,w,h,vgatext_base);       
  }
  else if(state.menu == 6){
    fillrect(0,7,w,8,8,2,w,h,vgatext_base);    
    drawtext(1,1,"echo",4,14,0,w,h,vgatext_base);
    drawtext(1,2,"tripletcount",12,14,0,w,h,vgatext_base);
    drawtext(1,3,"factorial",9,14,0,w,h,vgatext_base);
    drawtext(1,4,"fib",3,14,0,w,h,vgatext_base); 
    drawtext(1,5,"longcoroutine",13,14,0,w,h,vgatext_base); 
    drawtext(1,6,"longfiber",9,14,0,w,h,vgatext_base);
    drawtext(1,7,"fibrec",6,8,0,w,h,vgatext_base);
    drawtext(1,8,"towerofhanoi",12,14,0,w,h,vgatext_base);
    drawtext(1,9,"cli",3,14,0,w,h,vgatext_base);        
      
  }
  else if(state.menu == 7){
    fillrect(0,8,w,9,8,2,w,h,vgatext_base);    
    drawtext(1,1,"echo",4,14,0,w,h,vgatext_base);
    drawtext(1,2,"tripletcount",12,14,0,w,h,vgatext_base);
    drawtext(1,3,"factorial",9,14,0,w,h,vgatext_base);
    drawtext(1,4,"fib",3,14,0,w,h,vgatext_base); 
    drawtext(1,5,"longcoroutine",13,14,0,w,h,vgatext_base); 
    drawtext(1,6,"longfiber",9,14,0,w,h,vgatext_base);
    drawtext(1,7,"fibrec",6,14,0,w,h,vgatext_base);
    drawtext(1,8,"towerofhanoi",12,8,0,w,h,vgatext_base);
    drawtext(1,9,"cli",3,14,0,w,h,vgatext_base);     
      
  }
  else if(state.menu == 8){
    fillrect(0,9,w,10,8,2,w,h,vgatext_base);    
    drawtext(1,1,"echo",4,14,0,w,h,vgatext_base);
    drawtext(1,2,"tripletcount",12,14,0,w,h,vgatext_base);
    drawtext(1,3,"factorial",9,14,0,w,h,vgatext_base);
    drawtext(1,4,"fib",3,14,0,w,h,vgatext_base); 
    drawtext(1,5,"longcoroutine",13,14,0,w,h,vgatext_base); 
    drawtext(1,6,"longfiber",9,14,0,w,h,vgatext_base);
    drawtext(1,7,"fibrec",6,14,0,w,h,vgatext_base);
    drawtext(1,8,"towerofhanoi",12,14,0,w,h,vgatext_base);
    drawtext(1,9,"cli",3,8,0,w,h,vgatext_base);    
      
  }
  
  for(int i = 0; i < state.cur_line; i++){ //Printing to CLI
    drawtext(1, i + 11, state.display[i], 100, 14,4,w,h,vgatext_base);
  }

  
  if(state.state != 3){ //Displaying for Menu options
    int margin=0;
    if(state.menu == 0){
      drawtext(1,state.cur_line + 11,"$ echo",6,14,4,w,h,vgatext_base);
      margin=6;
    }
    else if(state.menu == 1){
      drawtext(1,state.cur_line + 11,"$ tripletcount",14,14,4,w,h,vgatext_base);
      margin=14;
    }
    else if(state.menu == 2){
      drawtext(1,state.cur_line + 11,"$ factorial",11,14,4,w,h,vgatext_base);
      margin=11;
    }
    else if(state.menu == 3){
      drawtext(1,state.cur_line + 11,"$ fib",5,14,4,w,h,vgatext_base); 
      margin=5;
    }
    else if(state.menu == 4){
      drawtext(1,state.cur_line + 11,"$ longcoroutine",15,14,4,w,h,vgatext_base); 
      margin=15;
    }
    else if(state.menu == 5){
      drawtext(1,state.cur_line + 11,"$ longfiber",11,14,4,w,h,vgatext_base); 
      margin=11;
    }
    else if(state.menu == 6){
      drawtext(1,state.cur_line + 11,"$ fibrec",8,14,4,w,h,vgatext_base); 
      margin=8;
    }
    else if(state.menu == 7){
      drawtext(1,state.cur_line + 11,"$ towerofhanoi",14,14,4,w,h,vgatext_base); 
      margin=14;
    }
    else if(state.menu == 8){
      drawtext(1,state.cur_line + 11,"$",2,14,4,w,h,vgatext_base); 
      margin=1;
    }

    drawtext(margin+2,state.cur_line + 11,state.inp,state.inp_size,14,4,w,h,vgatext_base); //Input display
  }
  
  


  // this is just an example:
  //
  // Please create your own user interface
  //
  // You may also have simple command line user interface
  // or menu based interface or a combination of both.
  //

}


//
//
// helper functions
//
//

static void writecharxy(int x, int y, uint8_t c, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  vgatext::writechar(y*w+x,c,bg,fg,vgatext_base);
}

static void fillrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int y=y0;y<y1;y++){
    for(int x=x0;x<x1;x++){
      writecharxy(x,y,0,bg,fg,w,h,vgatext_base);
    }
  }
}

static void drawrect(int x0, int y0, int x1, int y1, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){

  writecharxy(x0,  y0,  0xc9, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y0,  0xbb, bg,fg, w,h,vgatext_base);
  writecharxy(x0,  y1-1,0xc8, bg,fg, w,h,vgatext_base);
  writecharxy(x1-1,y1-1,0xbc, bg,fg, w,h,vgatext_base);

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y0, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int x=x0+1; x+1 < x1; x++){
    writecharxy(x,y1-1, 0xcd, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x0,y, 0xba, bg,fg, w,h,vgatext_base);
  }

  for(int y=y0+1; y+1 < y1; y++){
    writecharxy(x1-1,y, 0xba, bg,fg, w,h,vgatext_base);
  }
}

static void drawtext(int x,int y, const char* str, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  for(int i=0;i<maxw;i++){
    writecharxy(x+i,y,str[i],bg,fg,w,h,vgatext_base);
    if(!str[i]){
      break;
    }
  }
}

static void drawnumberinhex(int x,int y, uint32_t number, int maxw, uint8_t bg, uint8_t fg, int w, int h, addr_t vgatext_base){
  enum {max=sizeof(uint32_t)*2+1};
  char a[max];
  for(int i=0;i<max-1;i++){
    a[max-1-i-1]=hex2char(number%16);
    number=number/16;
  }
  a[max-1]='\0';

  drawtext(x,y,a,maxw,bg,fg,w,h,vgatext_base);
}



