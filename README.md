# CSC369 Review :weary: :weary: :weary:

## Table of Contents

- [Week 6: Memory Management](#week-6-memory-management)
- [Week 7: Paged Virtual Memory](#week-7-paged-virtual-memory)
- [Week 8: Disk and File System](#week-8-file-systems)

## Week 6 Memory Management
\- <strong>GOALs of VM</strong>:
* `6.1 Efficiency`: since only have a limited amount of physical memory, must use efficiently.</br>
* `6.2 Transparency`: since must provide the illusion of “infinite” memory to each process.</br>
* `6.3 Protection and sharing`: since physical memory is accessed by multiple processes, and must ensure data privacy.
<br></br>

### 6.1 Efficiency</br> 
*Efficiency explained*:</br> - Some portions are in DRAM.</br> - Some portions are stored on disk.</br> - Transfer data back and forth as needed.
<br></br>
### 6.2 Transparency</br> 
*Transparency explained*:</br> - Give each process its own view of memory.</br> - Decouple the data layout from where the data is actually stored in physical memory.

**6.2.1 Fixed Partitioning**</br>
<img align="right" src="./images/fixed_paritioning.png" width="100">
* Each process gets a fixed partition to use. (OS occupies a seperate partition)
* *Three* main problems introduced by `fixed partitioning`:</br>
\- If process is smaller than the partition, this will cause **internal fragmentation** (wastes memory)</br> 
\- If program needs more memory than the partition size, programmer has to deal with that. (**overlays**)</br>
\- Number of partitions is **limited**.
<br></br><br></br>


### 6.2.2 Dynamic partitioning</br>
Partitions will vary in length and numbers over time. When a process is brought into memory, a partition of exactly the right size is created to hold it.
* If a process exited, a "hole" will be created, and some holes may be too small to be re-used. This will cause **external fragmentation**.</br>
* OS may move processes around to create large chunks of free space. This is called **compaction** and requires processes to be **relocatable**.

### 6.2.3 Solution - VM Paging</br>
Paging works by decoupling address space completely from actual physical data location and splitting both virtual and physical memory in same-size **pages**.
</br>
<img src="./images/process_co_exist_in_memory.png" width="600">
</br>
* Each process gets the illusion of its personal address space (<em>AS</em>) from
**0 to 2^64-1**.
* Each virtual page can be mapped to any physical page.
* Data is either in **memory** or on **disk** (must bring data in memory if needed).

### 6.2.4 Translation
* Each process needs to have its address space pages translated to real physcial address.
* Hardware (MMU) converts VAs into PAs using the **Page Table**.
* Each process running in the OS has its own page table.

### 6.2.5 Page Faults
* If the page table entry indicates that the page is not in memory, then this causes a **page fault**.
* Os is responsible for loading page from the disk and the process will stop until the data is brought into memory.
* Page replacement policy is up to the OS.

Notice that it will not be way too slow due to the localized patterns:
- **Temporal locality:** Locations referenced recently likely to be referenced again.
- **Spacial locality:** Locations near recently referenced locations are likely to be referenced soon.
- Although cost of paging is high, if it's infrequent enough (due to locality), it's acceptable.
<br></br>


### 6.3 Protection and sharing
- Processes should not access other processes' memory. (Must protect process address spaces, implies we need rights for pages like file permissions)
- Priviledged OS data shouldn't be accessible to users.

### 6.3.1 VM enforces protection
- Access rights are kept in the page tables. Hardware enforces protection, OS is called when violation happens. Page tables cannot be modified by users since they exist in protected OS memory (only OS can modify them).
- Avoid leaked information from deallocated pages. Programmer should not have to zero out each page on dealloc. OS uses a "zero-ed" page and give the VA of that page to ensure that newly allocated pages are zero-ed out.

### 6.4 Address binding
- Option 1: Compile time</br>
&nbsp;&nbsp;- Must know what memory the process will use, during compilation.</br>
&nbsp;&nbsp;- Called <em>absolute code</em> since binary contains real addresses.</br>
&nbsp;&nbsp;- No relocation is possible.
- Option 2: Load time (static relocation)</br>
&nbsp;&nbsp;- Compiler: translates(binds) symbolic addresses to <en>logical,</em> <em>relocatable</em> addresses within the source file.</br>
&nbsp;&nbsp;- Linker: translates addresses from obj files to <em>logical,</em> <em>absolute</em> addresses within executable. Also takes care of &nbsp;&nbsp;references to symbols from other files.</br>
&nbsp;&nbsp;- Loader translates logical absolute address to <em>physical</em> addresses when program is loaded into memory.</br>

### 6.5 A better plan: Dynamic Relocation</br>
Process refers to **relative** addresses, hardware translates to physical address as instruction is executed.
* Two registers called **base** and **bound** on the MMU, these two registers get saved in the PCB when we do a context switch since MMU only has one base and bound register.
* The executable simply stores virtual addresses (starting at 0), and instructions in the executable do not need to be modified.
* When a program starts to run: - Set the base register. - Physical address can be translated as the following: *physcial address = base + virtual address*
* The bound register ensures that we don't access outside a process' address space.

### 6.5.1 Placement algorithms</br>
Need *dynamic relocation*.
* Best-fit: choose the block that is closest in size to the request</br>
&nbsp;&nbsp;- left-over fragments tend to be small(unusable)
&nbsp;&nbsp;- in practise, similar storage utilization to first-fit
* First-fit: choose the first block that is large enough, search can start at beginning, or where previous search ended</br>
&nbsp;&nbsp;- simplest, and often fastest and most efficient</br>
&nbsp;&nbsp;- may leave many small fragments near start of memory that must be searched repeatedly</br>
&nbsp;&nbsp;- next-fit variant tends to allocate from end of memory (free space becomes fragmented more rapidly)</br>
* Worst-fit: choose the largest block</br>
&nbsp;&nbsp;- not as good as best-fit or first-fit in practice</br>
* quick-fit: keep multiple free lists for common block sizes</br>
&nbsp;&nbsp;- great for fast allocation, generally harder to coalesce</br>

### 6.6 Paging</br>
* **Page frames** - logically partition physical memory into equal, fixed-size chunks
* **Virtual pages** - divide processes' memory into chunks of the same size
* Any page can be assigned to any free page frame.
* Possible page frame sizes are restricted to powers of 2 to simplify address translation.
* Virtual addresses now interpreted as *page number + page offset*:</br>
&nbsp;&nbsp; - page number = vaddr / page_size</br>
&nbsp;&nbsp; - page offset = vaddr % page_size</br>
<img src="./images/pa_translation.png" width="600">
</br>Details:
* MMU hardware generates address of page table entry, does lookup without OS.</br>
* OS has to load PTBR (*page table base register*) for new process on context switch since page tables are per process.

### 6.6.1 The Page Table
Stored in protected memory, and a user process cannot access its own page table.
* A linear array of *page table* entries, 1 entry per virtual page.
* Stored in **OS memory, attached to process structure**.
* Virtual page number (VPN) serves as array index.
* Allocate enough physical memory (ppages) for entire page table.

### 6.6.2 Page Table Entries
<img src="./images/pte.png" width="650"></br>
* **Modify bit**(M) says whether or not page has been written. Set when a write to a page occurs.
* **Reference bit**(R) says whether page has been accessed. Set when a read or write to the page occurs.
* **Valid bit**(V) says whether PTE can be used. Checked on each use of virtual address.
* **Protection bit**(Prot) specifies what operations are allowed on page (read/write/execute)
* **Page frame number**(PFN) determines physcial page.</br>Not all bits are provided by all architectures.

## Week 7: Paged Virtual Memory
**Multi-level page tables** split page table into *pages of PTEs*, use a *page table directory* to point to different pages of PTEs.</br>
Following is the comparison between linear page table and multi-level page table.</br>
<img src="./images/l_ml_compare.png" width="650"></br>

### 7.1 Two-level Page Tables</br>
<img src="./images/two-level_page_table.png" width="650"></br>
**Tradeoff: space vs. time**
* Saves space
* Adds more levels of indirection when translating addresses which means more complexity in the translation algorithm.

### 7.2 Hashed Page Tables</br>
<img src="./images/hashed_page_table.png" width="650">

### 7.3 Paging Limitations - Time
* 2 memory reads(references) per address loopup. (First read page table, then actual memory)
* Hierarchical page tables make it worse due to one read per level of page table. (Solution: use a hardware cache of lookups)

### 7.4 Translation Lookaside Buffer (TLB)
Small, fully-associative hardware cache of recently used translations. It translates virtual page numbers (VPNs) into PTEs (not physical addresses) and can be done in a single machine cycle.</br>
TLBs are implemented in hardware:
* Fully associative cache (all entries looked up in parallel).
* Cache tags are virtual page numbers.
* Cache values are PTEs (entries from page tables).
* With PTE + offset, can directly calculate physical address.</br>
<img src="./images/tlb.png" width="600"></br>

\- TLB exploit **locality**
* Processes only use a handful of pages at a time, and we only need those pages to be "mapped".
* Hit rates: >99% of translations are hits.

\- Managing TLBs
* OS ensures that *TLB and page tables are consistent*. e.g. when it changes the protection bits of a PTE, it needs to update the copy of the PTE in the TLB(if one exists). Typically, just invalidate the entry.
* Reload TLB on a *process context switch*. Invalidate all entries.
* When the TLB misses and a new PTE has to be loaded, *a cached PTE must be evicted*. Choosing PTE to evict is called the *TLB replacement policy* and is implemented in hardware.

### 7.5 Demand paging
Pages can be moved between memory and disk, this is called *demand paging*.
1. Initially, pagees are allocated to memory.
2. When memory fills up, allocating a page in memory requires some other page to be evicted from memory.</br>
&nbsp;&nbsp; - When a process accesses the page, the invalid PTE will cause a trap(*page fault*).</br>
&nbsp;&nbsp; - The trap will run the OS page fault handler.</br>
&nbsp;&nbsp; - Handler uses the invalid PTE to locate page in swap file.</br>
&nbsp;&nbsp; - Reads page into a physical frame, update PTE to point to it.</br>
3. Evicted pages go to disk (swap file).</br>
&nbsp;&nbsp; - When it evicts a page, the OS sets the PTE as invalid and stores the location of the page in swap file in PTE.</br>
* The movement of pages between memory and disk is done by the OS, and is transparent to the application.
* Actually, only **dirty** pages need to be written to disk. Clean pages do not, but you need to know where on disk to read them from again.
* Demand paging is also used when a process first starts up. When a process is created, it has a brand new page table with all valid bits off and no pages in memory. When the process starts executing, instructions fault on code and data pages, faulting stops when all necessary code and data pages are in memory. Only code and data needed by a process needs to be loaded.
* Alternative of *prepaging* which predicts future page that will be used.</br>
<img src="./images/pp_vs_dp.png" width="550">

### 7.6 Placement Policy
In paging systems, memory management hardware can translate any virtual-to-physical mapping equally well.
* Non-uniform Memory Access Multiprocessors. Any processor can access entire memory, but local memory is faster.
* Cache performance. Choose physical pages to minimize cache conflicts.

### 7.7 Replacement Policy
The page replacement algorithm determines how a victim is chosen.

### 7.7.1 Belady's algorithm (OPT)
It has the lowest fault rate for any page reference string. It works by replacing the page that will not be used for the longest period of time.

### 7.7.2 First-In-First-Out (FIFO)
* Maintain a list of pages in order in which they were paged in. On replacement, simply evict the one brought in longest time ago.
* The fault rate **increases** when the algorithm is given more memory.

### 7.7.3 Least Recently Used (LRU)
* Uses reference information to make a more informed replacement decision. Can make a guess of the future based opon past experience.
* On replacement, evict the page that has not been used for the longest time in the past.
* Options for implementing LRU:
&nbsp;&nbsp; \- *Time stamp* every reference and evict the page with oldest time stamp. The problem is we need to make PTE large enough to hold meaningful time stamp and need to examing every page on eviction to find one with oldest time stamp.</br>
&nbsp;&nbsp; \- Keep pages in a *stack*. On reference, move the page to the top of the stack. On eviction, replace page at bottom. The problem is we need costly software operation to manipulate stack on *EVERY* memory reference.
* We can implement *Approximating LRU* to avoid too much cost. It uses the PTE *reference bit*. Keep a counter for each page. At regular intervals, for every page: shift R bits into high bit of counter register, shift other bits to the right and pages with "larger" counters were used more recently.

### 7.7.4 Clock
Replace page that is "*old enough*".
* Arrange all of physical page frames in a big circle (clock).
* A clock hand is used to select a good LRU candidate.</br>
&nbsp;&nbsp; \- Sweep through the pages in circular order like a clock.</br>
&nbsp;&nbsp; \- If the ref bit is off, it hasn't been used recently.</br>
&nbsp;&nbsp; \- If the ref bit is on, turn it off and go to next page.
* Arm moves quickly when pages are needed and low overhead when plenty of memory.
* If memory is large, "accuracy" of information degrades.

### 7.8 Thrashing
Thrashing takes place when more time is spent by the OS in paging data back and forth from disk than executing user programs caused by memory demands of the set of running processes simply exceeding the available physcial memory.
* `Admission control`: Given a set of processes, a system could decide not to run a subset of processes, with the hope that the reduced set of processes' *working sets* (the pages that they are using actively) fit in memory and thus can make progress. (states that it is sometimes better to do less work well than to do everything at once poorly)
* `Out-of-memory killer`: this daemon chooses a memory-intensive process and kills it, thus reducing memory in a none-too-subtle manner. While successful at reducing memory pressure, this approach can have problems, if, for example, it kills the X server and thus renders any applications requiring the display unusable.

### 7.9 CPU utilization
<p align="center">
<img src="./images/multiprogramming.png" width="250"></p>

As the illustration shows, *CPU utilization* of a system can be improved by using multiprogramming. Let P be the fraction of time that a process spends away from the CPU. If there is one process in memory, the CPU utilization is 1-P. If there are N processes in memory, the probability of N processes waiting for an I/O is P\*P...*p (N times). The CPU utilization is (1-P^N) where N is the degree of multiprogramming. As N increases, the CPU utilization increases.</br>
However, once the system passes the point of optimal CPU utilization, as less memory is available to each program will cause higher page fault likelihood i.e. it thrashes.

### 7.10 Page Buffering
Previously, we assumed the replacement algorithm is run and a victim selected when a new page needs to be brought in. Most of these algorithms are too costly to run on every page fault.</br>
In actuality, OS keeps a small portion of memory free by having some kind of *high watermark* and *low watermark* to help decide when to start evicting pages from memory. When the OS notices that there are fewer than *LW* pages available, a background thread (`swap daemon`) is responsible for freeing memory runs. It evicts pages until there are *HW* pages available, then goes to sleep.

### 7.11 Swap Space
The space on disk for moving pages back from memory and forth to memory. </br>
Swap space is not the only on-disk location for swapping. Assume you are running a program binary. The code pages from this binary are initially found on disk, and when the program runs, they are loaded into memory (one page at a time when needed). However, if the system needs to make room in physical memory for other needs, it can safely re-use the memory space for these code pages, knowing that it can later swap them in again from the on-disk binary in the file system.

### 7.12 TLB misses
When a process is executing on the CPU, and it issues a read to an address. The read address goes to the TLB in the MMU.</br>
*Notice that this is all done by the hardware.*
1. TLB does a lookup using the page number of the address.
2. Common case is that the page number matches, returning a PTE for the mapping for this address.
3. TLB validates that the PTE protection allows reads.
4. PTE specifies which physical frame holds the page.
5. MMU combines physical frame & offset into a physical address.
6. MMU reads from that physical address, returns value to CPU.</br>

---
If TLB does not have a PTE mapping for this virtual address, known as a *minor page fault* as no I/O will be needed in the following solutions. There are two possibilities:
1. MMU loads PTE from page table in memory. Hardware managed TLB, OS not involved in this step, and OS has already set up the page tables so that the hardware can access it directly.
2. Trap to OS. In this case, software manages TLB. OS does lookup in page table, loads PTE into TLB and returns from exception, then retries memory access.

---
If access is not permitted by PTE. PTE will indicate a protection fault, either operation is not permitted on page (r/w/x) or virtual page not allocated/not in physical memory.</br>
TLB traps to the OS:
* Permission denied: OS may send fault back up to process, or might be using protection for other purposes (e.g. cow, mapped files).
* Invalid: If virtual page not allocated in address space, OS sends fault to process (e.g. segmentation fault). If page not in physical memory (major page fault), OS allocates frame and reads it in.

### 7.13 Working Set Model
A *working set* of a process is used to model the dynamic locality of its memory usage.
* W(t, r) = {pages P s.t. P was referenced ini the time interval (t, t-r)}
* w(t, r) = number of pages in W(t, r)</br>
<img align="left" src="./images/Wtr.png" width="375">
<img align="center" src="./images/wtr.png" width="350">


<p>
The working set size <strong>changes with program locality</strong>. During periods of poor locality, more pages are referenced, and within this period of time, the working set size is larger. So, intuitively, want the working set to be the set of pages a process needs in <em>memory</em> to prevent heavy faulting.</br>
e.g. when people ask, "How much memory does Firefox need?", they are in effect asking for the size of Firefox's working set.</p>

### 7.14 Shared memory
Since private virtual address spaces protect applications from each other, but this makes it difficult to share data. Parents and children in a forking web server or proxy will want to share an in-memory cache without copying. So, we can use shared memory to allow processes to share data using direct memory references.
</br><img src="./images/sharing.png" width="450"></br>
Implement sharing using page tables:
* Have PTEs in both tables map to the same physical frame.
* Each PTE can have different protection values.
* Must update both PTEs when page becomes invalid.
In the illustration above, we map shareed memory at same virtual addresses. We can also map at different virtual addresses in each process' address space. It will be more flexible since we can avoid address space conflicts, but pointers inside the shared memory segment are invalid.

### 7.15 Copy On Write
Copy on write (*CoW*) is used to defer large copies as long as possible, hoping to avoid them altogether. 
* Instead of copying pages, create *shared mappings* of parent pages in child virtual address space.
* Shared pages are protected as read-only in child. Reads happen as usual. Writes generate a protection fault, trap to OS, copy page, change page mapping in client page table, restart write instruction.

### 7.16 Memory Mapped Files
Bind a file to a virtual memory region:
* PTEs map virtual addresses to physical frames holding file data.
* Virtual address base + N corresponds to offset N in file.
* Can read or write at various offsets in file, using memory operations.

---
Initially, all pages mapped to file are invalid:
* OS reads a page from file when invalid page is accessed.
* OS write a page to file when evicted, or region unmapped.

---
Advantages:
- Uniform access for files and memory (just use pointers)
- Less copying.
</br>
Drawbacks:
- Since OS handles faults transparently, process has less control over data movement.
- Does not generalize to streamed I/O. (pipes, sockets, etc. only works for files)

## Week 8 File Systems
File system should provide long-term information storage. It is required to store very large amounts of information which must survive the termination of process using it, and multiple processes must be able to access info concurrently.</br>
Two views of file systems:
- User view \- convenient logical origanization of information.
- OS view \- managing physical storage media, enforcing access restrictions.

### 8.1.1 File Table (handling operations on files)
When file is first used actively, store its attribute info in a system-wide open-file table. The index (*fd*) into this table is used on subsequent operations, hence no searching is required.</br>
<img src="./images/file_table.png" width="650"></br>

### 8.1.2 File Sharing
Have 2 levels of internal tables:
- A per-process table of all files that each process has open (also holds the current file positions)
- Each entry in the per-process table points to an entry in the system-wide open-file table.</br>
<img src="./images/file_sharing.png" width="650">

### 8.1.3 File Access Methods
Modern systems typically only support simple access:
- Sequential access \- read bytes one at a time, in order
- Direct access \- random access given block/byte number

### 8.1.4 File links
* Hard link \- second directory indentical to the first one
* Symbolic link \- Directory entry refers to file that holds "true" path to the linked file.

### 8.2.1 Directories
Directories serves multiple purposes:
* For users, they provide a structured way to organize files.
* For the file system, they provide a convenient naming interface that allows the implementation to separate logical file organization from physical file placement on the disk.
* Stores information about files

### 8.2.2 Directory Structure
* A directory is a list of entries \- names and associated metadata (information that describes properties of the data e.g. size, protection, location, etc.)
* List is usually unordered.
* Directories typically stored in files. Only need to manage one kind of secondary unit.</br>

### 8.2.3 Issues with acyclic graphs
* With links, a file may have multiple absolute path names.
* Sharing can occur with duplication of information, but maintaining consistensy is a problem, e.g. updating permissions in directory entry with hardlink.

### 8.3.1 File system Implementation
* File systems define a block size (e.g. 1KB for A4).
* A Super Block determines location of root directory.
* A free map determines which blocks are free. Usually a bitmap, one bit block on the disk. Also stored on disk, cached in memory for performance.
* Remaining disk blocks used to store files.

### 8.3.2 Disk Layout Strategies
**Contiguous Allocation**</br>
* Like memory, fast, simplifies directory access.
* Inflexibal, causes fragmentation, needs compaction.
<img src="./images/contiguous_alloc.png" width="600"></br>

---
**Linked Allocation**
* Each block points to the next, directory points to the first.
* Good for sequential access, bad for all others.
<img src="./images/linked_alloc.png" width="600"></br>

---
**Indexed structure** (*indirection, hierarchy*)
* An "index block" contains pointers to many other blocks.
* Handles random better, still good for sequential.
* May need muti-level index blocks.


UNIX inodes implement an indexed structure for files. All file metadata is stored in inode. (UNIX dir entries map file names to inode)</br>
Each inode contains 15 block pointers (i_block):
- First 12 are direct block pointers (disk addresses of the first 12 data blocks).
- Then *single indirect* block pointer.
- Then *double indirect* block pointer.
- Then *triple indirect* block pointer.
<img src="./images/indexed_alloc" width="600">

### 8.4 Very Simple File System (VSFS)
Overall Organization:
- block size: 4KB
- Number of blocks: 64
- Total size: 256KB
</br>
    <img src="./images/vsfs.png" width="600">
</br>

---
*Super block*: when mounting a file system, the OS first reads the superblock, identifies its type and other parameters, then attaches the volume to the file system tree with proper settings.

### 8.5 Other Approaches
*Extent-based*</br>
An **extent** == a disk pointer plus a length (in number of blocks). So, instead of requiring a pointer to every block of a file, we just need a pointer to every several blocks.
- Disadvantage: Less flexible than the pointer-based approach.
- Advantage: Uses smaller amount of metadata per file, and file allocation is more compact.
- adopted by ext4, NTFS.

---
*Linked-based*</br>
Instead of pointers to all blocks, the inode just has one pointer to the first data block of the file, then the first block points to the second block, etc.
- Disadvantage: works poorly if we want to access the last block of a big file
- Advantage: uses an in-memory *File Allocation Table*, indexed by address of data block, so faster in finding a block.
- adopted by FAT

### 8.6.1 Inode
- Data structure representing an FS object.
- Attributes, disk block locations.
- No file name, just metadata.

### 8.6.2 Directories
- List of (name, inode) mappings.
- Each directory entry: itself(.), parent dir(..), a file, another directory, link.

### 8.6.3 Hard links
- Multiple file names (and directory entries) mapped to the same inode.
- Reference count - only remove file when it reaches 0

### 8.6.4 Symbolic links
- "Pointer" to a given file
- contains the path as its contents

### 8.6.5 The content of a data block
- *Regular file*: data of the file
- *Directory*: list of directory entries
- *Symbolic Link*: path of the file that it links to 

### 8.6.6 Inodes and Path search
Unix inodes are **not** directories. They describe where on the disk the blocks for a file are placed.
On the contrary, directory entries map file names to inodes.
1. To open "/somefile", use super block to read the inode for root directory.
2. Read data block for root dir, look for an entry with name "somefile".
3. This entry will identify the inode for "somefile".
4. Read the inode for "somefile" into memory.
5. The inode says where first data block is on disk.
6. Read that block into memory to access the data in the file. 

### 8.7.1 File Buffer Cache
Cache file blocks in memory to capture locality.
- Cache is system wide, used and shared by all processes
- Reading from the cache makes a disk perform like memory.
- Significant reuse: spatial and temporal locality.</br>
Items cached:
- Inodes
- Directory entries
- disk blocks for frequently used files

### 8.7.2 Caching and Buffering
- Static partitioning: at boot time, allocate a fixed-size cache in memory (10 % of total memory).
- Dynamic partitioning: integrate virtual memory pages and file system pages into a unified page cache, so pages of memory can be flexibly allocated for either virtual memory or file system, used by modern systems.

### 8.7.3 Caching writes
Buffering a batch of disk writes is helpful because:
- Combine multiple writes into one write.
- Can improve performance by scheduling the buffered writes (lazy updates), e.g. can schedule buffered writes in such a way that they happen sequentially on disk.
- Can avoid some unnecessary writes.

### 8.7.4 Tradeoff using caching
Caching and buffering improves the speed of file system reads and writes. However, it sacrifies the durability of data, e.g. if crash occurs, buffered writes not written to disk are lost, however, if we sync to disk more frequently will incur a lower speed.

### 8.7.5 Read Ahead
Very helpful for sequentially accessed files, unless blocks for the file are scattered across the disk.
- FS predicts that the process will request next block.
- It goes ahead and requests it from the disk.
- This can happen while the process is computing on previous block i.e. overlapping I/O with execution.
- When the process requests block, it will be in cache.
- Compliments the on-disk cache, which also is doing read ahead.

### 8.8.1 Disk Components
<img src="./images/disk_components.png" width="500">

### 8.8.2 Disk Performance
* Seek - moving the disk arm to the correct cylinder
* Rotation - waiting for the sector to rotate under the head
* Transfer - transferring data from surface into disk controller electronics, sending it back to the host.


<img src="./images/disk_steps.png" width="600">

### 8.8.3 Hardware optimizations
**Track Skew**
</br>
    <img src="./images/track_skew.png" width="600">
</br>

---
**Zones**
</br>
    <img src="./images/zones.png" width="600">
</br>

---
**Cache** (Track Buffer)
* A small memory chip, part of the hard drive, usually 8-16MB.
* Different from cache that OS has. Unlike the OS cache, it is aware of the disk geometry. When reading a sector, may cache the whole track to speed up future reads on the same track.

### 8.8.4 Enhancing achieved disk performance
- *Closeness*: reduce seek times by putting related things close to each other
- *Amortization*: amortize each positioning delay by grabbing lots of useful data

### 8.9.1 Problems for the original UNIX fs
- Problem #1: On a new FS, blocks are allocated sequentially, close to each other. As the FS gets older, files are being deleted and create random gaps. In aging FS, data blocks end up allocated **far from each other**. Therefore, data blocks for new files end up **scattered across the disk**. Fragmentation of an aging FS requires **more seeking**.
- Problem #2: Inodes allocated far from blocks as all inodes are at the beginning of disk, far from data. Traversing file name paths, manipulating files, directories requires **going back and forth from inodes to data blocks**. Hence, incur a lot of seekings.

### 8.9.2 Cylinder Groups for FFS
FFS addressed placement problems using the notion of a *cylinder group* (aka allocation groups).
* Disk partitioned into groups of cylinders.
* Data blocks in same file allocated in same cylinder group.
* Files in same directory allocated in same cylinder group.
* Inodes for files are allocated in same cylinder group as file data blocks.

<img src="./images/cylinder_group.png" width="600"></br>
Allocation in cylinder groups provides *closeness*, hence reduces number of long seeks.

### 8.10 Disk Scheduling Algorithms
Goal: minimize seeks

---
**FCFS** (do nothing)
- Reasonable when load is low.
- Long waiting time for long request queues.

<img src="./images/fcfs.png" width="600">

---
**SSTF** (shortest seek time first)
- Minimize arm movement (seek time), maximize request rate
- Favors middle blocks

<img src="./images/sstf.png" width="600">

---
**SCAN**
- Service requests in one direction util done, then reverse.

<img src="./images/scan.png" width="600">

---
**C-SCAN**
- Like SCAN, but only go in one direction.
---

**LOOK / C-LOOK**
- Like SCAN/C-SCAN but only go as far as last request in each direction.

<img src="./images/look.png" width="600">