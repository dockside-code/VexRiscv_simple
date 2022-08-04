# TODOs

- Correct DataBank/Main Memory Access Timing:  
  
  1. Hit, Read: Read from Cache (takes n cycles) (Basically done).
  
  2. Hit, Write: Write to memory and cache (takes n cycles) (Not observable in current program - In progress).
  
  3. No Hit, Write: Write Through (1 cycle due to write to BRAM) (done).
  
  4. No Hit, Read: Read entire line from BRAM to refill cache line. Write to cache and redo current instruction  
    (takes n by burstsize plus n cycles) (Basically done).  

- Correct Pipeline Timing:

  1. Halt execution phase during cache read/write, and treat it like a memory access. (Kind of sorted out. Turns out I have to halt the writeback as well to get valid        writeback)
  
  2. Make sure such pipeline stalls does not affect operations. (Still need to do the write-to-cache-when-hit thing)

- Model a Latency - Accurate RTM model
  
  1. In progress (Basically done. Latency porpotionate to relative distance between addresses for access. May contain still a
     few errors).

- Construct a more conprehensive and illustraing test program  (In progress)
