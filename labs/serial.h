#pragma once

#include "util/io.h"

namespace serial{

  static inline bool is_transmitter_ready(io_t baseport){
    //insert your code here
    int v=32 & (io::read8(baseport, 5));
    return(v!=0);
  }

  static inline void writechar(uint8_t c, io_t baseport){
    //insert your code here
    io::write8(baseport, (uint16_t)0, c);
  }

} //end serial
