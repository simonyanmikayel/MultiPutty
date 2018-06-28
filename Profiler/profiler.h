#pragma once

extern"C" void  on_enter(void* func_addr);
extern"C" void  on_exit(void* func_addr);

// Build steps:
// for 64-bit builds : add /EXPORT:_penter /EXPORT:_pexit in the linker commandline
// that is required to export out the functions.  __declspec( dllexport ) doesnt seem
// to export out the symbols, if the definition is in .asm file
// for release mode , turn off all the optimizations to make this library work
// do not use /LTCG linker flag( that creates a mess).
// turn off whole program optimization (general settings)
// for the code being hooked, it must be compiled with the following 
// Code Generation > Basic Runtime Checks > Both (/RTCS1, equiv. to /RTCsu)(/RTC1)
// I think this is because the Assem64.asm is incomplete/incorrect.
