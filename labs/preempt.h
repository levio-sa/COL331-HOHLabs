#pragma once

#include "util/config.h"
#include "util/fiber.h"


//
// preempt_t : State for your timer/preemption handler
//
// Note:
//  We've one instance of core_t per each core.
//  To access this instance, you need to use %gs:0
//  (The entire kernel doesn't have any global/static variables)
//
// %gs:core_offset_preempt will point to start of preempt_t instance
//
// for example: 
// %gs:0 will return pointer to core_t
// %gs:core_offset_mainstack will return core_t::main_stack
// %gs:core_offset_preempt+0 will return core_t::saved_stack
//
// etc.
//
// See Definition of core_t in x86/main.h
//

struct preempt_t{
  // your data structure, if any
  addr_t saved_stack; //feel free to change it - provided as an example
  uint32_t flag=0; // flag to detect preemption
  uint32_t race_flag=0; // flag to detect race condition

};


//
// 
// This macro is being called from x86/except.cc
//
//
// _name: label name
// _f   : C function to be called 
//        ex: we may have to do send EOI to LAPIC or PIC etc.
//
#  define  _ring0_preempt(_name,_f)            \
  __asm(                                       \
      "  .text                            \n\t"\
      " " STR(_name) ":                   \n\t"\
      "  pushl %edx                       \n\t"\
      "  pushl %ecx                       \n\t"\
      "  pushl %eax                       \n\t"\
      "  call " STR(_f) "                 \n\t"\
      "  popl  %eax                       \n\t"\
      "  popl  %ecx                       \n\t"\
      "  popl  %edx                       \n\t"\
      "                                   \n\t"\
      "  # insert your code here          \n\t"\
      "                                   \n\t"\
      "  # Compare if already yielded     \n\t"\
      "  cmpl $0x1, %gs:" STR(core_offset_preempt+8) "  \n\t"\
      "  je 2                             \n\t"\
      "                                   \n\t"\
      "  # Save the CPU state to the current stack      \n\t"\
      "  pushal                           \n\t"\
      "                                   \n\t"\
      "  movl %esp, %ebp                  \n\t"\
      "  subl $512, %esp                  \n\t"\
      "  andl $0xfffffff0, %esp           \n\t"\
      "  fxsave (%esp)                    \n\t"\
      "  pushl %ebp                       \n\t"\
      "  pushl $1f                        \n\t"\
      "                                   \n\t"\
      "  # switch context and stack       \n\t"\
      "  movl  %esp, %gs:" STR(core_offset_preempt)"    \n\t"\
      "  movl  %gs:" STR(core_offset_mainstack) ", %esp \n\t"\
      "  movl $0x1, %gs:" STR(core_offset_preempt+4)"   \n\t"\
      "  sti                              \n\t"\
      "  ret                              \n\t"\
      "                                   \n\t"\
      "  # Restore the state              \n\t"\
      "1:                                 \n\t"\
      "  popl %ebp                        \n\t"\
      "  fxrstor (%esp)                   \n\t"\
      "  movl %ebp, %esp                  \n\t"\
      "  popal                            \n\t"\
      "2:                                 \n\t"\
      "  sti                              \n\t"\
      "  jmp iret_toring0                 \n\t"\
      )                                        \


