#include "common.th"
#include "serial.th"

// argument in C

    .global puts
puts:
_outer:
    b <- [c]            // load word from string
    d <- b == 0         // if it is zero, we are done
    jnzrel(d,_done)
    c <- c + 1          // increment index for next time
_inner:
    e <- b & 0xff       // mask off top bits
    d <- e == 0         // compare 
    jnzrel(d,_outer)    // skip to next word if zero
    b <- b >> 8         // shift down next character
    emit(e)             // output character to serial device
    goto(_inner)        // and continue with same word
_done:
    ret

