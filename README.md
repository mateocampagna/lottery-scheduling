# Lottery Scheduling Simulator

## References
- [Lottery Scheduling - Wikipedia](https://en.wikipedia.org/wiki/Lottery_scheduling)
- [OSTEP - Lottery Scheduling](https://pages.cs.wisc.edu/~remzi/OSTEP/cpu-sched-lottery.pdf)

## Usage

### Basic Mode
```bash
./lottery_sched <seed> <loops>
```

**Parameters:**
- `<seed>`: Seed for the random number generator
- `<loops>`: Number of lottery iterations

**Example:**
```bash
./lottery_sched 42 10000
```

### Demo Mode (with ticket mechanisms)
```bash
./lottery_sched <seed> <loops> --demo
```

This mode executes transfer, inflation, and deflation operations on tickets before running the simulation, demonstrating how these mechanisms work.

**Example:**
```bash
./lottery_sched 42 5000 --demo
```

## Results Interpretation

Typical output:

```
PID   Name        Tickets  Wins    Actual%   Expected%   Deviation
----  --------    -------  ----    -------   ---------   ---------
1     WebServer   100      1539    15.39%    15.38%      +0.01%
2     Database    200      3147    31.47%    30.77%      +0.70%
3     Cache       300      4583    45.83%    46.15%      -0.32%
4     Worker      50       731     7.31%     7.69%       -0.38%

Standard Deviation: 0.43%
```

**Explanation:**
- **Tickets**: Current number of tickets for the process
- **Wins**: Number of times the process won the lottery
- **Actual%**: Actual percentage of wins
- **Expected%**: Theoretical percentage based on tickets
- **Deviation**: Difference between actual and expected
- **Standard Deviation**: How consistent the distribution is

A low deviation (< 1%) indicates that the algorithm is working correctly.

## Available Functions

| Function | Description |
|----------|-------------|
| `insert(pid, name, tickets)` | Create a new process |
| `remove_process(pid)` | Remove a process |
| `find_process(pid)` | Find a process by ID |
| `transfer_tickets(from_pid, to_pid, amount)` | Transfer tickets between processes |
| `inflate_tickets(pid, amount)` | Increase a process's tickets |
| `deflate_tickets(pid, amount)` | Decrease a process's tickets |
| `print_list()` | Display process list |
| `print_stats(loops)` | Display simulation statistics |