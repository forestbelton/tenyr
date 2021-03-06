#include "common.th"

.global dw_mul

// Performs B:C <- D * E
dw_mul:
  // Load mask.
  m <- [rel(mask)]
  
  // Split up factors into 16-bit components.
  b <- d >> 16
  c <- d & m
  d <- e >> 16
  e <- e & m
  
  // Compute upper and lower sums as well as an intermediate component.
  m <- b * e
  b <- b * d
  d <- c * d
  m <- m + d
  c <- c * e
  
  // Adjust lower sum.
  c <- m << 16 + c
  
  // Adjust upper sum.
  b <- m >> 16 + b
  
  ret

mask: .word 0xffff
