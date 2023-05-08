GLOBAL screenBackground

;stores enough space to create a screen, in 1024*768 at 24 bpp
section .bss
   align 64       ; align to improve memmov efficiency
   screenBackground db 1024*768*3