==============
    README
==============

Known Issues:

1) Occassional errors in bne/beq instructions. Branch portion (last 16 bits)
   appears to be incorrect - off by one. These are likely the second part of
   ble/blt pseudoinstructions. It is unlikely I will have time to investigate
   and fix these issues before the submission deadline. 

2) Strings in .data segment produce incorrect output, such as in the p11.asm
   testing file. For padded strings, the last remaining char(s) should occur
   after the padding byte(s), rather than before the padding zero bytes. In 
   at least one case, a padded string should be output, but a non-padded string
   is output instead. 

Unfortunately, I will probably not have enough time before submission to fix
these issues.