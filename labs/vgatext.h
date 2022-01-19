#include "util/io.h"


namespace vgatext{

   static inline void writechar(int loc, uint8_t c, uint8_t bg, uint8_t fg, addr_t base){
     //your code goes here
    mmio::write16(base, loc<<1, ((uint16_t)(((uint16_t)bg<<(uint16_t)12)+((uint16_t)fg<<(uint16_t)8)+(uint16_t)c)));
   }

}//namespace vgatext