# TODOs

- Correct DataBank/Main Memory Access Timing:  
  
  1. Hit, Read: Read from Cache (takes n cycles) (partially done).
  
  2. Hit, Write: Write to memory and cache (takes n cycles).
  
  3. No Hit, Write: Write Through (1 cycle due to write to BRAM) (done).
  
  4. No Hit, Read: Read entire line from BRAM to refill cache line. Write to cache and redo current instruction  
    (takes n by burstsize plus n cycles) (partially done).  

- Correct Pipeline Timing:

  1. Halt execution phase during cache read/write, and treat it like a memory access.  
  
  2. Make sure such pipeline stalls does not affect operations.
