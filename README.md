# Dstn-assignment1

A complete memory management implementation in C.

## Hardware Description
The simulated hardware consists of a Direct-Mapped split **L1 Cache** (4KB, 32B block size) along with a [Victim cache](https://www-inst.eecs.berkeley.edu//~cs152/fa04/handouts/p388-jouppi.pdf) (8 blocks) and an 8-way Set-Associative **L2 Cache** (32KB, 32B block size), along with a **Main Memory** (32MB) module that supports multi-level paging with 2 segments. There is also an identifier based (32 Line) **TLB (Translation Lookaside Buffer)** associated with the core.
