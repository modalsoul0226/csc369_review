# CSC369 Review :weary: :weary: :weary:

## Table of Contents

- [Week 6: MEMORY MANAGEMENT](#Week 6: MEMORY MANAGEMENT)


## Week 6: MEMORY MANAGEMENT
\- <strong>GOALs of VM</strong>:
* `6.1 Efficiency`: since only have a limited amount of physical memory, must use efficiently.</br>
* `6.2 Transparency`: since must provide the illusion of “infinite” memory to each process.</br>
* `6.3 Protection and sharing`: since physical memory is accessed by multiple processes, and must ensure data privacy.


**6.1: Efficiency**</br> 
*Efficiency explained*:</br> - Some portions are in DRAM.</br> - Some portions are stored on disk.</br> - Transfer data back and forth as needed.

**6.2: Transparency**</br> 
*Transparency explained*:</br> - Give each process its own view of memory.</br> - Decouple the data layout from where the data is actually stored in physical memory.

**6.2.1: Fixed Partitioning:**</br>
<img align="right" src="./fixed_paritioning.png" width="100">
* Each process gets a fixed partition to use. (OS occupies a seperate partition)
* *Three* main problems introduced by `fixed partitioning`:</br>
\- If process is smaller than the partition, this will cause **internal fragmentation** (wastes memory)</br> 
\- If program needs more memory than the partition size, programmer has to deal with that. (**overlays**)</br>
\- Number of partitions is **limited**.
<br></br><br></br>


**6.2.2: Dynamic partitioning**</br>
Partitions will vary in length and numbers over time. When a process is brought into memory, a partition of exactly the right size is created to hold it.
* If a process exited, a "hole" will be created, and some holes may be too small to be re-used. This will cause **external fragmentation**.</br>
* OS may move processes around to create large chunks of free space. This is called **compaction** and requires processes to be **relocatable**.

**6.2.3: Solution - VM Paging**</br>
Paging works by decoupling address space completely from actual physical data location and splitting both virtual and physical memory in same-size **pages**.
</br>
<img src="process_co_exist_in_memory.png" width="600">
</br>
* Each process gets the illusion of its personal address space (<em>AS</em>) from
**0 to 2^64-1**.
* Each virtual page can be mapped to any physical page.
* Data is either in **memory** or on **disk**.