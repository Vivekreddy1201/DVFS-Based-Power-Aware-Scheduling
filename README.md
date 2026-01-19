DVFS-Based Power-Aware Scheduling in Cloud Computing
 Overview

This project implements a DVFS-based power-aware scheduling simulation for cloud computing environments.
The goal is to reduce energy consumption in cloud datacenters while maintaining task execution efficiency and SLA compliance.

The simulation dynamically adjusts CPU frequency using Dynamic Voltage and Frequency Scaling (DVFS) based on VM utilization and predicted workload. It also performs VM consolidation by transitioning underutilized virtual machines into a sleep state and reactivating them when needed.

 Key Features

Workload Prediction (Low / Medium / High)

DVFS-based CPU Frequency Scaling

Energy Consumption Estimation

Baseline vs Optimized Energy Comparison

VM Consolidation & Reactivation

Simplified SLA Compliance Check

Simulation Summary with Energy Savings

 Scheduling Strategy

CPU frequency is adjusted dynamically:

High utilization → High frequency

Medium utilization → Medium frequency

Low utilization → Low frequency

Underutilized VMs are consolidated into sleep mode

Sleeping VMs are reactivated when task demand increases

Energy usage is compared against a baseline (fixed frequency) model

 Technologies Used

Language: C

Concepts:

Cloud Computing

DVFS (Dynamic Voltage and Frequency Scaling)

Power-Aware Scheduling

VM Consolidation

Energy Efficiency

 Project Structure
DVFS-Based-Power-Aware-Scheduling/
│
├── dvfs_scheduler.c      # Main simulation code
├── README.md             # Project documentation

 How to Compile and Run
Compile
gcc dvfs_scheduler.c -o dvfs_scheduler

Run
./dvfs_scheduler

 Sample Output

Predicted workload level

Task-to-VM assignment details

CPU frequency adjustments

Energy consumption per task

VM consolidation events

Final energy savings summary

 Results

The simulation demonstrates that DVFS-based scheduling combined with VM consolidation can significantly reduce energy consumption compared to a baseline fixed-frequency approach, while still maintaining task execution feasibility.

 Future Enhancements

Integration with real cloud simulators (e.g., CloudSim)

Advanced workload prediction models

More accurate SLA and power models

Multi-datacenter support

 Author

M. Vivek Reddy
B.Tech – Computer Science and Engineering
IIIT Nagpur
