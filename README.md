# Dstn-assignment1

A complete memory management implementation in C.

## Hardware Description
The simulated hardware consists of a Direct-Mapped split **L1 Cache** (4KB, 32B block size) along with a [Victim cache](https://www-inst.eecs.berkeley.edu//~cs152/fa04/handouts/p388-jouppi.pdf) (8 blocks) and an 8-way Set-Associative **L2 Cache** (32KB, 32B block size), along with a **Main Memory** (32MB) module that supports multi-level paging with 2 segments. There is also an identifier based (32 Line) **TLB (Translation Lookaside Buffer)** associated with the core. A Working Set Strategy mechanism detects for thrashing in the system.

## Replacement Policies

### TLB
LRU Square Matrix method

### L1 Cache:
No replacement policy since it's direct mapped.

### Victim Cache: 
LRU Counter method

### L2 Cache:
First In First Out

### Main Memory:
LRU Sliding Window method
