### 1. The Basics</br>
\- What is an operating system ?</br>
- An OS is a **virtual machine**. Extends and simplifies interface to physical machine. Provides a library of functions accessible through an API.
- An OS is a **resource allocator**. Allows the proper use of resources (hardware, software, data) in the operation of the computer system. Provides an environment within which other programs can do useful work.
- An OS is a **control program**. Controls the execution of user programs to prevent errors and improper use of the computer. Especially concerned with the operation and control of I/O devices.

\- Why do we have OS ?
-  **Virtualization**. Present physical resource as a more general, powerful, or easy-to-use form of itself. Present illuion of multiple (or unlimited) resources where only a few really exist, e.g. CPU, memory.
- **Concurrency**. Coordinate multiple activities to ensure correctness.
- **Persistence**. Some data needs to survive crashes and power failures.
---
### 2. Processes & Threads
\- What is a process ? What is a thread ?
- Process is an active entity, a **program in execution** (Programs are static entities with the *potential* for execution). It contains: address space (code, heap) + PC + reg + os resources (open files, network), stored in **PCB**.
- Thread is a single *control flow through a program*. Represented by a PC, reg, stack and small TCB.

\- What is the difference between kernel and user level threads ?
- Kernel level threads: modern OSs **separate the execution of a process into thread abstraction** in order to **make concurrency cheaper** (*much less state to allocate and initialize*). The OS manages threads and processes, all **thread operations are implemented in the kernel**, and **OS schedules all of the threads in the system**. However, suffer from too much overhead: still require system calls (slow down), have to be general to support all languages, runtime, etc.
- User level threads: need to be implemented at user level to make threads **small and fast**, **managed entirely by the run-time system** (user-level library). Thread operations (create, switch, synchronize) are done via **procedure call**. However, user-level threads are **invisible to OS** i.e. not well-integrated with OS. So, OS can make poor decisions: schedule a process with only idle threads, block a process whose thread initiated an I/O, de-schedule a process with a thread holding a lock.

\- How are processes created ? Delete ? Zombies ?
1. Create a new process by creating **new PCB**, **user address space**, and **allocating memory**.
2. **Load the executable** of the program, initilize start state for process, and change state to **ready**.
3. Dispatch the process, and change state from ready to **running** using **context switch** (saving the state of old process, load the save state for the new process). *Note: cs happens when: some process calls yield(), process makes other syscall and is blocked, time interrupt*.</br>
<img src="./images/context_switch.png" width="400">

4. On **exit()**, a process voluntarily releases all resources. OS **stops running the process to free everything**, **requires context switch** to another process, **parent may be waiting** or asking for the return value.
5. The process is a **zombie** until its parent cleans it up.

\- What states can a process be in ?</br>
<img src="./images/process_states.png" width="400">

\- What does the address space look like ? PCB ?</br>
PCB includes: **process state** (ready, running, blocked), **PC** (address of the next instruction), CPU **regs** (must be saved at an interrupt); CPU scheduling info: **process priority**; **page table**; resource use info; list of **open files**.</br>
<img src="./images/address_space.png" width="300">

\- How do threads relate to virtual address space ?</br>
<img src="./images/threaded_address_space.png" width="400">

---
### 3. System Calls
System call is a function call that invokes the OS. Invoked when an application wants to use a resource that OS manages.

\- What are the protection domains ? Why do we need them ?</br>
Hardware runs in **user mode** or **system mode**. Some instructions are **privileged instructions** - they cn only run in system mode: access I/O device, manipulate memory management, configure mode bits, etc. CPU will check current protection level on each instruction. Without control, a process could simply **run forever** and **take over the machine**, or **access information** that is **should not be alowed** to access.

\- How do interrupt work ? Why do we need them ?</br>
Interrupt can be caused by hardware (I/O device) or software (syscall). When an interrupt occurs, there must be a **reason**. The reason is stored in a register, and that register is used to invoke a **handler**, and CPU will jump to the interrupt handler when an interrupt takes place.

\- What happens when a process makes a system call ?</br>
Kernel assigns each system call a **system call number**. Initialize syscall table and map each system call number to a function. User process sets up syscall number and arguments. HW **switches to kernel mode** and invokes kernel's **interrupt handler**. Kernel **looks up** syscall table using the syscall number. Kernel invokes the corresponding function, then returns.

\- How and when does a context switch happen ?</br>
When the **interrupt handler** is called, HW must make sure to **save enough of the caller's registers** (*push them to kernel stack*) in order to be able to return correctly when the OS issues the return-from-trap instruction (*return-from-trap will pop those values from kernel stack and resume execution of the user-mode program*).

---
### 4. Concurrency
We need **synchronization** since processes interact in a multiprogrammed system , it can help to share resources and to coordinate execution. Also, arbitrary interleaving of thread executions can have unexpected consequences, synchronization can **restrict the possible interleavings of executions**.

\- What is the critical section problem ?</br>
Enforce **single use of a shared resource**. Using a lock to ensure that only one thread can access a critical section (shared resource).

\- What properties does a solution need to have ?</br>
1. **Mutual Exclusion**: if one thread is in CS, then no other is.
2. **Progress**: if no thread is in the CS, and some threads want to enter CS, they should be able to, unrestricted by threads in the "remainder".
3. **Bounded Waiting** (no starvation): if some thread T is waiting on the CS, then there is a limit on the number of times other threads can enter CS before this thread is granted access. (i.e. thread T cannot wait in the "remainder" indefinitely)
4. **Performance**: the overhead of entering and exiting the CS is small with respect to the work being done within it. 

\- What is a race condition ?</br>
When two concurrent threads manipulated a **shared resource** without any synchronization. Outcome depends on the order in which accesses take place.

\- Synchronization primitives</br>
\- S/W - Peterson's Algorithm</br>
<img src="./images/petersons_algo.png" width="400">

\- S/W - Bakery Algorithm</br>
- Upon entering, each thread gets a #.
- Thread with the lowest number is served next.
- No guarantee that 2 threads do not get same #. In case of a tie, thread with the lowest id is served first. Thread ids are unique and totally ordered.

\- H/W - Test-And-Set</br>
<img src="./images/spin_lock_tas.png" width="600"></br>
<img src="./images/test_and_set.png" width="600">

\- H/W - Compare-And-Swap</br>
<img src="./images/compare_and_swap.png" width="400">

\- Lock: a binary semaphore.

\- Semaphores</br>
The initial value of N determines its behavior. Value of the semaphore, when **negative**, equals to the **number of threads waiting**. However, can be hard to reason about synchronization.
``` 
int sem_wait(sem_t *s) {
    decrement the value of semaphore by one,
    wait if value of semaphore s is negative. 
}

int sem_post(sem_t *s) {
    increment the value of semaphore by one,
    if there are one or more threads waiting, wake one.
}
```

\- CV</br>
* **cv_wait()**: release lock, waits, re-acquires mutex before return
* **cv_signal()**: wake one enqueued thread
* **cv_broadcast()**: wake all enqueued thread

**Caution**: if no one is waiting, signal or broadcast has no effect. Not recorded for later use, as with semaphore.

Signaling a thread only wakes them up, it is thus a hint that the state of the world has changed, but there is no guarantee taht when the woken thread runs, the state will still be desired. This interpretation of what a signal means is often referred to as **Mesa** semantics.

**Always** use while loop, also handles the case where spurious wakeups occur i.e. two threads get woken up by a single signal.

\- Monitor

\- Producer-Consumer Problem

\- Reader-Writers Problem

---
### 5. Scheduling

\- Goals in developing a good scheduling algorithm: </br>
**Increase CPU utilization** and **job throughput** by overlapping I/O and computation.
* **Fairness** - each thread receives fair share of CPU
* **Avoid starvation**
* **Balance** - all parts of the system should be busy
* **Throughput** - maximize jobs completed per hour
* **Turnaround time** - minimize time between submission and completion
* **CPU utilization** - keep CPU busy all the time
* **Response time** - minimize time between receiving request and starting to produce output

**Non-preemptive scheduling**: once the CPU has been allocated to a thread, it keeps the CPU until it terminates or blocks</br>
**Preemptive scheduling**: CPU can be taken from a running thread and allocated to another 

\- FCFS</br>
- **Non-preemptive**
- choose the thread at the **head of the ready queue** where queue is maintained in FIFO order.
- Average waiting time with FCFS often quite long, it happens when all other threads wait for the one big thread to release CPU (convoy effect).

\- SJF</br>
- **Preemptive**
- choose the thread with the **shortest expected processing time**.
- **Optimal** with respect to **average wait time**.

\- Round Robin</br>
- **Preemptive**
- Ready queue is circular. Each thread is allowed to **run for time quantum** *q* before being preempted and put back on queue.
- choice of quantum is critical: as *q* approaches infinity, RR becomes FCFS, as *q* approaches 0, RR becomes processor sharing. We want *q* to be **large** with respect to **context switch time**.

\- Multi-Level Feedback Queue</br>
<img src="./images/mlfq.png" width="400">