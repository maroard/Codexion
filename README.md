*This project has been created as part of the 42 curriculum by maroard.*

# Codexion

## Description

**Codexion** is a concurrency simulation written in C.

The project models a circular co-working hub where several coders compete for a limited number of USB dongles in order to compile their quantum code. Each coder is represented by a dedicated thread. To compile, a coder must acquire the two dongles located on their left and right sides. After compiling, the coder releases both dongles, then starts debugging and refactoring before trying to compile again.

The goal of the project is to manage shared resources safely and fairly using POSIX threads, mutexes, condition variables, and a custom priority queue. The simulation ends when either:

- a coder burns out because they did not start compiling before their deadline;
- every coder has compiled at least the required number of times.

This project focuses on synchronization, race-condition prevention, deterministic resource arbitration, cooldown handling, and precise monitoring of time-based failures.

## Instructions

### Compilation

Compile the project with:

```bash
make
```

The Makefile builds the executable:

```bash
./codexion
```

The project is compiled with:

```bash
-Iincludes -Wall -Wextra -Werror -pthread
```

The public project header lives in `includes/codexion.h`. Source files include it as:

```c
#include "codexion.h"
```

The `-Iincludes` compiler flag makes that header available without hard-coding the directory path in every source file.

### Usage

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

### Arguments

| Argument | Description |
| --- | --- |
| `number_of_coders` | Number of coders and dongles. |
| `time_to_burnout` | Maximum time in milliseconds before a coder burns out if they do not start compiling. |
| `time_to_compile` | Time in milliseconds spent compiling while holding two dongles. |
| `time_to_debug` | Time in milliseconds spent debugging after compilation. |
| `time_to_refactor` | Time in milliseconds spent refactoring after debugging. |
| `number_of_compiles_required` | Simulation stops when every coder has compiled at least this many times. |
| `dongle_cooldown` | Time in milliseconds during which a dongle remains unavailable after being released. |
| `scheduler` | Arbitration policy: `fifo` or `edf`. |

### Scheduler policies

Codexion supports two scheduling policies for dongle arbitration:

- `fifo`: First In, First Out. The request that arrived first is served first.
- `edf`: Earliest Deadline First. The request with the earliest burnout deadline is served first.

Each dongle assigns a strictly increasing sequence number when a request enters
its queue. The sequence is assigned while holding the dongle mutex, so every
request in a given queue has a unique insertion order. This remains reliable
even when several requests are created during the same millisecond:

1. `fifo` serves the smallest arrival sequence number;
2. `edf` serves the earliest deadline, then the smallest arrival sequence
   number when deadlines are equal.

### Examples

```bash
./codexion 1 800 200 200 200 3 0 fifo
```

With only one coder, there is only one dongle. Since compiling requires two distinct dongles, the coder cannot compile and should eventually burn out.

```bash
./codexion 5 800 200 200 200 3 100 fifo
```

Runs five coders with FIFO scheduling and a 100 ms cooldown after each dongle release.

```bash
./codexion 5 800 200 200 200 3 100 edf
```

Runs five coders with EDF scheduling and a 100 ms cooldown.

## Output format

Every state change is printed using the required format:

```text
timestamp_in_ms coder_id message
```

Possible messages are:

```text
has taken a dongle
is compiling
is debugging
is refactoring
burned out
```

Example:

```text
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
401 1 is refactoring
1204 3 burned out
```

Logs are protected by a dedicated mutex so that messages from different threads never interleave on the same line.

## Technical overview

### Main structures

The project is organized around a global context structure, usually passed to all parts of the simulation instead of using global variables.

The main conceptual structures are:

- `t_config`: parsed command-line configuration, including timing values, compile target, cooldown, and scheduler policy;
- `t_init_state`: initialization tracker used during cleanup, storing how many coders and dongles were successfully initialized and which global mutexes are ready;
- `t_ctx`: global simulation context, including configuration, coders, dongles, monitor thread, stop state, log mutex, start time, and `init_state`;
- `t_coder`: one coder thread, its mutex-protected state, its neighboring dongles, its globally ordered dongle pointers, and its two reusable requests;
- `t_dongle`: shared resource protected by a mutex and condition variable, with ownership state, cooldown timestamp, arrival counter, and request heap;
- `t_request`: request made by a coder for one dongle, carrying its insertion order, deadline, and target dongle;
- `t_heap`: custom priority queue used for FIFO/EDF arbitration on each dongle.

`init_state` is especially important for partial initialization failures. If allocation or mutex creation fails midway, cleanup only destroys resources that were actually initialized.

### General execution flow

1. Parse and validate arguments.
2. Initialize the global context, coders, dongles, mutexes, condition variables, and heaps.
3. Start one thread per coder.
4. Start the monitor thread.
5. Coders repeatedly request dongles, compile, release dongles, debug, and refactor.
6. The monitor checks for burnout or completion.
7. When the simulation stops, all waiting coders are awakened.
8. The main thread joins all threads.
9. All allocated memory and synchronization primitives are cleaned up.

## Blocking cases handled

### Deadlock prevention

Compiling requires two dongles. The dangerous pattern is circular wait: each coder holds one dongle and waits for another one held by a neighbor.

Without a global order, a cycle can appear:

```text
Circular wait risk

Coder 1 holds D0 and waits for D1
Coder 2 holds D1 and waits for D2
Coder 3 holds D2 and waits for D0

D0 -> Coder 1 -> D1 -> Coder 2 -> D2 -> Coder 3 -> D0
```

Codexion prevents this by assigning every coder a deterministic acquisition order based on dongle ids:

```text
first_dongle  = dongle with the smaller id
second_dongle = dongle with the larger id
```

For a ring of five coders, the logical left/right pairing is still circular, but the acquisition order is not:

```text
Physical ring

        D0 -- C1 -- D1
      /                \
    C5                  C2
     |                  |
    D4                  D2
      \                /
        C4 -- D3 -- C3

Acquisition order by coder

Coder 1: D0 -> D1
Coder 2: D1 -> D2
Coder 3: D2 -> D3
Coder 4: D3 -> D4
Coder 5: D0 -> D4
```

The important detail is `Coder 5`: even though this coder sits between `D4` and `D0`, they still request `D0` first because `0 < 4`.

This removes the circular wait condition from Coffman's deadlock conditions:

```text
Allowed waits always point upward by dongle id

D0 -> D1 -> D2 -> D3 -> D4

No coder can hold D4 and wait for D0.
Therefore the wait graph cannot close into a cycle.
```

A coder may hold a lower-id dongle while waiting for a higher-id dongle, but never the opposite. That single rule is enough to break the deadlock cycle.

### Starvation and priority handling

Each dongle owns a request queue implemented as a heap and an arrival counter.
The counter is incremented under the dongle mutex when a request enters the
queue, so two requests can never receive the same insertion order. Requests are
served according to the selected scheduler:

- `fifo` preserves request arrival order, which gives older requests priority over newer ones;
- `edf` prioritizes coders closest to burnout by selecting the earliest deadline first.

EDF is useful for urgency, while FIFO is useful for strict arrival fairness. Both policies use deterministic tie-breakers to avoid undefined ordering when priorities are equal.

### Dongle cooldown handling

After a dongle is released, it cannot be immediately taken again. The dongle stores an absolute timestamp:

```text
cooldown_until = current_time + dongle_cooldown
```

A dongle is considered available only if:

```text
has_owner == false && current_time >= cooldown_until
```

Condition variables are used to avoid busy waiting. When a dongle is released, waiting coders are awakened and re-check whether they are allowed to take it. Timed waits are used so that a coder can wake up when a cooldown expires even if no other thread broadcasts at that exact time.

### Precise burnout detection

A dedicated monitor thread periodically checks every coder. For each coder, it reads `last_compile_start` under the coder mutex and compares it with the current elapsed time.

A coder burns out if:

```text
current_elapsed_time - last_compile_start >= time_to_burnout
```

When a burnout is detected, the monitor sets the global stop flag, logs the burnout event, and wakes all dongles so that waiting coders can exit cleanly.

### Completion detection

The monitor also checks whether all coders have reached:

```text
compile_count >= number_of_compiles_required
```

When this condition is true for every coder, the monitor stops the simulation without printing a burnout message.

### Log serialization

All output is protected by a dedicated log mutex. This guarantees that two threads cannot write partial messages on the same line at the same time.

Normal messages are ignored after the simulation stop flag is set. The `burned out` message is allowed through so that the final failure event is still printed.

### Safe shutdown on thread creation failure

If a thread creation fails after some coder threads have already started, the simulation startup is aborted safely:

1. set the global stop flag;
2. wake all dongles;
3. join every coder thread that was successfully created;
4. return an error;
5. clean up initialized resources.

This prevents cleanup from destroying mutexes, condition variables, or allocated memory while active threads may still be using them.

## Thread synchronization mechanisms

### `pthread_mutex_t`

Mutexes are used to protect shared state:

- each dongle has its own mutex for ownership, cooldown, and request queue state;
- each coder has its own mutex for `last_compile_start` and `compile_count`;
- the context has a stop mutex for the global stop flag;
- the context has a log mutex for serialized output.

### `pthread_cond_t`

Each dongle has a condition variable. Coders wait on a dongle condition variable when they cannot currently take it.

A coder waiting for a dongle wakes up when:

- the dongle is released;
- the dongle cooldown expires through a timed wait;
- the monitor stops the simulation and broadcasts to all dongles.

After every wake-up, the coder re-checks the full condition before taking the dongle:

```text
simulation is not stopped
request is first in the dongle heap
dongle has no owner
cooldown has expired
```

This protects against spurious wake-ups and against cases where another coder takes the dongle first.

### Custom heap priority queue

The project implements its own heap-based priority queue for dongle request arbitration.

Each dongle has one heap:

```text
D0.request_queue
D1.request_queue
D2.request_queue
...
```

The heap stores pointers to coder requests. It supports:

- pushing a request;
- peeking the highest-priority request;
- popping the highest-priority request;
- restoring heap order with sift-up and sift-down operations.

The heap is protected by the mutex of the dongle that owns it.

Conceptually, the request with the highest priority is always at index `0`:

```text
Array storage

index:    0       1       2       3       4
        +-------+-------+-------+-------+-------+
heap:   | Req B | Req D | Req A | Req C | Req E |
        +-------+-------+-------+-------+-------+
          peek()

Tree view

              Req B
            /       \
        Req D       Req A
       /     \
   Req C     Req E
```

With `fifo`, priority is based on the insertion sequence assigned under the
dongle mutex:

```text
smaller arrival_order = higher priority
```

With `edf`, priority is based on deadline:

```text
smaller deadline = higher priority
```

When a coder requests a dongle, its request is pushed into that dongle heap. The coder may take the dongle only when all of these are true:

```text
heap_peek(&dongle->request_queue) == request
dongle->has_owner == false
current_time >= dongle->cooldown_until
simulation is not stopped
```

This means a free dongle is not automatically taken by whichever thread wakes first. It is taken by the thread whose request is currently first according to the selected scheduler.

## Project structure

My organization is:

```text
.
├── Makefile
├── README.md
├── includes/
│   └── codexion.h
└── src/
    ├── cleanup/
    │   └── cleanup.c
    ├── core/
    │   ├── coder.c
    │   ├── context_state.c
    │   ├── dongle.c
    │   ├── monitor.c
    │   ├── pthread_utils.c
    │   └── simulation.c
    ├── heap/
    │   ├── heap.c
    │   └── heap_utils.c
    ├── init/
    │   ├── init.c
    │   └── init_utils.c
    ├── parsing/
    │   ├── parsing.c
    │   ├── parsing_error_utils.c
    │   └── parsing_utils.c
    ├── utils/
    │   ├── log.c
    │   └── time.c
    └── codexion.c
```

Build artifacts such as `obj/` and the `codexion` executable are generated by `make` and are not part of the source layout.

## Error handling

The program rejects invalid inputs, including:

- missing arguments;
- non-integer numeric arguments;
- `number_of_coders` values lower than `1`;
- time values lower than `1` millisecond;
- `number_of_compiles_required` values lower than `0`;
- `dongle_cooldown` values lower than `0`;
- invalid scheduler values other than `fifo` or `edf`.

Memory allocation failures and thread creation failures are handled by stopping the simulation if needed, joining already-created threads, and cleaning up initialized resources.

## Resources

Useful references for this project:

- [`pthreads(7)`](https://man7.org/linux/man-pages/man7/pthreads.7.html) — overview of POSIX threads and synchronization;
- [`pthread_create`](https://man7.org/linux/man-pages/man3/pthread_create.3.html) and [`pthread_join`](https://man7.org/linux/man-pages/man3/pthread_join.3.html) — thread creation and termination;
- [`pthread_mutex_init`](https://man7.org/linux/man-pages/man3/pthread_mutex_init.3p.html) and [`pthread_mutex_lock`](https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html) — mutex initialization and shared-state protection;
- [`pthread_cond_wait`](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3.html) and [`pthread_cond_timedwait`](https://man7.org/linux/man-pages/man3/pthread_cond_wait.3.html) — condition variables and timed waiting;
- [`gettimeofday`](https://man7.org/linux/man-pages/man2/gettimeofday.2.html) — wall-clock time measurement;
- [Binary heap](https://en.wikipedia.org/wiki/Binary_heap) — data structure used to implement the FIFO/EDF priority queues;
- [Dining philosophers problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem) — classic shared-resource synchronization problem;
- [Coffman conditions](https://en.wikipedia.org/wiki/Deadlock_(computer_science)#Necessary_conditions) — conditions required for a deadlock to occur.

## AI usage

AI was used as a learning and design assistant during the project. It helped with:

- understanding the subject requirements;
- discussing architecture before implementation;
- reasoning about thread synchronization and mutex ownership;
- designing the global context and cleanup strategy;
- clarifying condition variables, timed waits, and cooldown handling;
- drafting this README.

All implementation decisions and final code were reviewed and adapted manually. The AI was used for guidance and explanation, not for blindly copying a complete solution.
