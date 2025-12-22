#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_VMS 5
#define MAX_TASKS 20
#define CONSOLIDATION_THRESHOLD 50

#define DVFS_HIGH 3.0
#define DVFS_MED  2.5
#define DVFS_LOW  1.8

#define POWER_ACTIVE 120.0
#define POWER_IDLE   60.0
#define POWER_SLEEP  10.0
#define WAKEUP_COST  20.0

typedef enum { ACTIVE, IDLE, SLEEP } VMState;

typedef struct {
    int id;
    int utilization;
    float cpu_freq;
    float energy_used;
    int task_count;
    VMState state;
    int idle_counter;
} VM;

typedef struct {
    int id;
    int load;
    int runtime;
} Task;

/* ===== FUNCTION PROTOTYPES (IMPORTANT) ===== */
const char *predict_workload();
void adjust_frequency(VM *vm);
float estimate_energy(int runtime, float freq);
float estimate_baseline_energy(int runtime);
void initialize_vms(VM vms[], int count);
void initialize_tasks(Task tasks[], int count);
void consolidate_vms(VM vms[], int count);
void assign_task_to_vm(VM *vm, Task task, float *baseline_energy_total);
void check_sla_compliance(Task task, float energy_used);
void reactivate_sleeping_vm(VM vms[], int count, Task task, float *baseline_energy_total);
void display_vm_status(VM vms[], int count);
void simulation_summary(VM vms[], int count, float baseline_energy);
void calculate_power_efficiency(float energy_saved, float total_tasks);

/* ===== WORKLOAD PREDICTION ===== */
const char *predict_workload() {
    int r = rand() % 3;
    if (r == 0) return "low";
    if (r == 1) return "medium";
    return "high";
}

/* ===== DVFS LOGIC ===== */
void adjust_frequency(VM *vm) {
    if (vm->utilization > 80)
        vm->cpu_freq = DVFS_HIGH;
    else if (vm->utilization < 40)
        vm->cpu_freq = DVFS_LOW;
    else
        vm->cpu_freq = DVFS_MED;
}

/* ===== ENERGY MODELS ===== */
float estimate_energy(int runtime, float freq) {
    float power = freq * 0.1f;
    float time_hr = runtime / 3600.0f;
    return power * time_hr;
}

float estimate_baseline_energy(int runtime) {
    float power = 3.0f * 0.1f;
    float time_hr = runtime / 3600.0f;
    return power * time_hr;
}

/* ===== INITIALIZATION ===== */
void initialize_vms(VM vms[], int count) {
    for (int i = 0; i < count; i++) {
        vms[i].id = i;
        vms[i].utilization = rand() % 40 + 30;
        vms[i].cpu_freq = DVFS_MED;
        vms[i].energy_used = 0;
        vms[i].task_count = 0;
        vms[i].state = ACTIVE;   // FIXED
        vms[i].idle_counter = 0;
    }
}

void initialize_tasks(Task tasks[], int count) {
    for (int i = 0; i < count; i++) {
        tasks[i].id = i;
        tasks[i].load = rand() % 16 + 5;
        tasks[i].runtime = rand() % 540 + 60;
    }
}

/* ===== CONSOLIDATION ===== */
void consolidate_vms(VM vms[], int count) {
    printf("\n Consolidation Phase:\n");
    for (int i = 0; i < count; i++) {
        if (vms[i].utilization < CONSOLIDATION_THRESHOLD && vms[i].task_count < 2) {
            vms[i].state = SLEEP;
            printf("  VM-%d turned OFF due to underutilization\n", vms[i].id);
        }
    }
}

/* ===== TASK ASSIGNMENT ===== */
void assign_task_to_vm(VM *vm, Task task, float *baseline_energy_total) {
    if (vm->utilization + task.load > 100) return;

    vm->utilization += task.load;
    vm->task_count++;
    adjust_frequency(vm);

    float energy = estimate_energy(task.runtime, vm->cpu_freq);
    vm->energy_used += energy;
    *baseline_energy_total += estimate_baseline_energy(task.runtime);

    printf(" Task-%d -> VM-%d | Load:%d%% | Freq:%.1fGHz | Energy:%.3fkWh\n",
       task.id, vm->id, task.load, vm->cpu_freq, energy);

    check_sla_compliance(task, energy);
}

/* ===== SLA CHECK (SIMPLIFIED) ===== */
void check_sla_compliance(Task task, float energy_used) {
    if (energy_used > task.runtime * 0.1f) {
        printf("  SLA Warning: Task-%d may miss deadline\n", task.id);
    }
}

/* ===== VM REACTIVATION ===== */
void reactivate_sleeping_vm(VM vms[], int count, Task task, float *baseline_energy_total) {
    for (int i = 0; i < count; i++) {
        if (vms[i].state == SLEEP) {
            vms[i].state = ACTIVE;
            vms[i].utilization = 0;
            vms[i].task_count = 0;
            vms[i].energy_used = 0;
            vms[i].cpu_freq = DVFS_MED;

            printf("  Reactivated VM-%d for Task-%d\n", vms[i].id, task.id);
            assign_task_to_vm(&vms[i], task, baseline_energy_total);
            return;
        }
    }
    printf("  Task-%d could not be scheduled\n", task.id);
}

/* ===== STATUS DISPLAY ===== */
void display_vm_status(VM vms[], int count) {
    printf("\n VM STATUS:\n");
    for (int i = 0; i < count; i++) {
        if (vms[i].state == SLEEP) continue;
        printf("  VM-%d | Util:%d%% | Freq:%.1fGHz | Tasks:%d | Energy:%.3fkWh\n",
               vms[i].id, vms[i].utilization, vms[i].cpu_freq,
               vms[i].task_count, vms[i].energy_used);
    }
}

/* ===== SUMMARY ===== */
void simulation_summary(VM vms[], int count, float baseline_energy) {
    float total_energy = 0;
    int total_tasks = 0;

    for (int i = 0; i < count; i++) {
        if (vms[i].state == SLEEP) continue;
        total_energy += vms[i].energy_used;
        total_tasks += vms[i].task_count;
    }

    float energy_saved = baseline_energy - total_energy;

    printf("\n SIMULATION SUMMARY:\n");
    printf("  Tasks Scheduled: %d\n", total_tasks);
    printf("  Baseline Energy: %.3f kWh\n", baseline_energy);
    printf("  Actual Energy:   %.3f kWh\n", total_energy);
    printf("  Energy Saved:    %.3f kWh\n", energy_saved);

    calculate_power_efficiency(energy_saved, total_tasks);
}

void calculate_power_efficiency(float energy_saved, float total_tasks) {
    if (total_tasks > 0) {
        float efficiency = energy_saved / total_tasks;
        // optional metric
    }
}

/* ===== MAIN ===== */
int main() {
    srand(time(NULL));

    VM vms[MAX_VMS];
    Task tasks[MAX_TASKS];
    float baseline_energy_total = 0;

    printf("DVFS-Based Power-Aware Scheduling Simulation\n");

    initialize_vms(vms, MAX_VMS);
    initialize_tasks(tasks, MAX_TASKS);

    const char *load = predict_workload();
    printf("\n Predicted Workload: %s\n", load);

    consolidate_vms(vms, MAX_VMS);

    printf("\n Task Scheduling:\n");
    for (int i = 0; i < MAX_TASKS; i++) {
        int assigned = 0;
        for (int j = 0; j < MAX_VMS; j++) {
            if (vms[j].state == ACTIVE &&
                vms[j].utilization + tasks[i].load <= 100) {
                assign_task_to_vm(&vms[j], tasks[i], &baseline_energy_total);
                assigned = 1;
                break;
            }
        }
        if (!assigned) {
            reactivate_sleeping_vm(vms, MAX_VMS, tasks[i], &baseline_energy_total);
        }
    }

    display_vm_status(vms, MAX_VMS);
    simulation_summary(vms, MAX_VMS, baseline_energy_total);

    return 0;
}
