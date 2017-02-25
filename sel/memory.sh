#define NULL            0

global
                     
  function MemAlloc   (int)        : *void
  function MemReAlloc (*void, int) : *void
  function MemFree    (*void)      : void

  function MemAvail   ()           : int
  
end