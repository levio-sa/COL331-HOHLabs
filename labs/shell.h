#pragma once
#include "util/config.h"
#include "util/debug.h"

struct shellstate_t{
    int key_press = 0;  // Number of keys pressed
    int state = 0; // 0 select menu, 1 take input, 2 computing, 3 displaying output
    char inp[100];
    int inp_size;
    int menu = 0; // Menu items (functions)
    int result = 0;
    int num_menu=0;
    char display[100][100];
    int cur_line = 0;
    int funk = -1; //Function ID
};

struct renderstate_t{
    int key_press = 0; //Number of key presses
    int state = 0; // 0 select menu, 1 take input, 3 displaying output
    char inp[100];
    int inp_size;
    int menu = 0; // Highlighted menu
    int result = 0; 
    char* display[100];
    int cur_line = 0;
    int funk=-1; //Function ID
};

void shell_init(shellstate_t& state);
void shell_update(uint8_t scankey, shellstate_t& stateinout);
void shell_step(shellstate_t& stateinout);
void shell_render(const shellstate_t& shell, renderstate_t& render);

bool render_eq(const renderstate_t& a, const renderstate_t& b);
void render(const renderstate_t& state, int w, int h, addr_t display_base);

