# Vexriscv Fork with a Cache Based on a Latency Accurate RTM model

This VexRiscv fork contains a data cache based on a latency accurate RTM model (racetrack memory model).

## How to Use This

Change into repo root dir  
`cd [repo_root]`  
Boot sbt  
`sbt`  
Compile verilog with config [GenSmallestAxi](./src/main/scala/vexriscv/demo/GenSmallestAxi.scala)  
`runMain vexriscv.demo.GenSmallestAxi`  
You can find the compiled verilog file in repo root dir.

A few things to keep in mind:

- Cached dbus plugins only work with mmus or pmps
- DBus and IBUS are instantiated to have a AXI port
- AMOs, LRSCs, invalidates and exclusives are not supported in the [cache configs](https://github.com/dockside-code/VexRiscv_simple/blob/bd8fbf6994c7e067e08034ad3ab56d2ccd32918a/src/main/scala/vexriscv/ip/DataCache.scala#L27)

## The RTM Cache

In a racetrack memory, since the data are stored on the tracks in a linear fashion, the track has to be shifted during a read/write access for the read/write head to access the corresponding data location. This would produce a variable latency during each read and write access. The main advantage of RTMs, is its smaller cell size, so that when fitted to a cache, it'd have more storage capacity in comparison to a standard SRAM - based cache, producing less cache misses during runtime to compensate for the larger access latency. Furthermore, the RTM is a NVRAM (non-volatile RAM), meaning that even when powered - off, the data would still be retained on the RTM tracks.

In this implementation, the latency was modelled to be porpotionate to the relative difference between current access address and last access address. This implementation is used to determine the latency introduced by the RTMs in comparison to the normal SRAM based caches, intended to be a test and evaluation platform for testing out software - based optimization strategies targetting RTM based systems. Other types of memory models could also be easily fitted into the current system, providing that it follows certain behaviors listed below.

This document will focus on the data cache of the vexriscv core. If you are targetting other parts of the riscv system, refer to the original repo [here](https://github.com/SpinalHDL/VexRiscv).

The structure of our implemented cache:

![current cache block diagram](./img/cache.png)

The Data Cache is a bridging component between the rest of the processor and the data memory. The CPU, during a memory access, first reaches into the cache for the purpose of reducing access time (DRAMs take 10-20 cycles to complete a operation, whilst  SRAMs take 1-3, depending on the technology). During the access, the data cache of the vexriscv core observes the following policies:

![cache policies](./img/cachepolicies.png)

The behavior of the cache: during read process, if hit, after examining the tags (SRAM based), read directly from the cache. If there's no hit, the cache load an entire line of data (regulated by a single burst from the AXI protocol based memory) from the main memory and write into the data bank. The cache issues a redo current command and the data is re - read from the cache.

During write process, if there's a hit, write both into cache and the main memory. Otherwise write ONLY to the main memory.

Due to the main memory being faster(!) in this case, a buffer was introduced between the memory and the cache. (See [here](<https://github.com/dockside-code/VexRiscv_simple/blob/bd8fbf6994c7e067e08034ad3ab56d2ccd32918a/src/main/scala/vexriscv/ip/DataCache.scala#L872>)) This is only intended for the refill line operation, during which a burst is read into the cache. When the data bank is ready to take in more values, it informs the loader(See [here](<https://github.com/dockside-code/VexRiscv_simple/blob/bd8fbf6994c7e067e08034ad3ab56d2ccd32918a/src/main/scala/vexriscv/ip/DataCache.scala#L1331>)), which in turn informs the buffer to give out the next value. The loader calculates the next cache address and writes into the cache. After the data bank is ready again, the loader and the buffer increments again.

The RTM model was implemented with a state machine. It enters counter incrementation mode upon read/write enable signals and exits upon the counter reaching the address difference between this access address and the last, (See [here](https://github.com/dockside-code/VexRiscv_simple/blob/bd8fbf6994c7e067e08034ad3ab56d2ccd32918a/src/main/scala/vexriscv/ip/DataCache.scala#L755))during this it will emit a ready signal, informing others that it's ready to take in more values.

The track length (max latency) is customizable, [here](https://github.com/dockside-code/VexRiscv_simple/blob/bd8fbf6994c7e067e08034ad3ab56d2ccd32918a/src/main/scala/vexriscv/ip/DataCache.scala#L756) as well as the cache line configs. (Multi - way configs are theoretically supported but not tested)

The cache is programmed to halt the rest of the pipeline (execution and writeback) when:  
[See here](https://github.com/dockside-code/VexRiscv_simple/blob/bd8fbf6994c7e067e08034ad3ab56d2ccd32918a/src/main/scala/vexriscv/ip/DataCache.scala#L1389)

- There's a refill process
- There's a W/R access to the cache and the databanks are not yet ready

## Supporting Other Memory Models

Other memories could easily be modelled with existing infrastructure [here](https://github.com/dockside-code/VexRiscv_simple/blob/bd8fbf6994c7e067e08034ad3ab56d2ccd32918a/src/main/scala/vexriscv/ip/DataCache.scala#L755), as long as they follows:

- Able to do read/write operations continously. Meaning that the memory model should be able to accept a series of accesses during read/write (latency is a non - issue here, the RE/WE enable signals will be high for the entirety of the serial access).
- Emits a ready signal when read/write is complete (valid_dout).
- Accepts address (access_addr) for reading/writing, accepts data (din) for writing and gives out (dout) when read is complete.

## TODOs

- Correct DataBank/Main Memory Access Timing:  
  
  1. Hit, Read: Read from Cache (takes n cycles) (Basically done).
  
  2. Hit, Write: Write to memory and cache (takes n cycles) (Basically done).
  
  3. No Hit, Write: Write Through (1 cycle due to write to BRAM) (done).
  
  4. No Hit, Read: Read entire line from BRAM to refill cache line. Write to cache and redo current instruction  
    (takes n by burstsize plus n cycles) (Basically done).  

- Correct Pipeline Timing:

  1. Halt execution phase during cache read/write, and treat it like a memory access. (Kind of sorted out.)
  
  2. Make sure such pipeline stalls does not affect operations. (Now haltsthe pipeline when there's an individual w/r request)

- Model a Latency - Accurate RTM model
  
  1. In progress (Basically done. Latency porpotionate to relative distance between addresses for access).

- Construct a more conprehensive and illustraing test program  (Done)

  1. Problem was probably casued by vivado simulation errors.
