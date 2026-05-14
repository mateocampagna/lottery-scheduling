#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#define MAX_NAME_LEN 32

int global_tickets = 0;
int total_process = 0;

typedef enum {
    READY,
    RUNNING,
    BLOCKED
} ProcessState;

typedef struct node{
    int pid; 
    char name[MAX_NAME_LEN];
    int tickets; // current tickets
    int initial_tickets; // original tickets
    int wins;
    ProcessState state;
    int total_cpu_time;
    struct node* next;
} Process;

Process* head = NULL;

void insert(int pid, char* name, int tickets){
    if(tickets <= 0){
        fprintf(stderr, "ERROR: tickets must be positive\n");
        return;
    }
    Process *temp = malloc(sizeof(Process));
    if(temp == NULL){
        fprintf(stderr, "ERROR: no space in memory\n");
        return;
    }
    temp->pid = pid;
    snprintf(temp->name, MAX_NAME_LEN, "%s", name);
    temp->tickets = tickets;
    temp->initial_tickets = tickets;
    temp->wins = 0;
    temp->state = READY;
    temp->total_cpu_time = 0;
    temp->next = head;
    
    head = temp;
    global_tickets += tickets;
    total_process++;
}

void remove_process(int pid){
    Process* curr = head;
    Process* prev = NULL;
    
    while(curr){
        if(curr->pid == pid){
            if(prev==NULL) head = curr->next;
            else prev->next = curr->next;

            global_tickets -= curr->tickets;
            total_process--;
            free(curr);
            return;
        }    
        prev = curr;
        curr = curr->next;
    }
}

Process* find_process(int pid){
    Process* curr = head;
    while(curr){
        if(curr->pid == pid){
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// tickets mechanism!!
void transfer_tickets(int from, int to, int amount){
    Process* from_process = find_process(from);
    Process* to_process = find_process(to);

    if(!from_process){
        fprintf(stderr, "ERROR: process '%d' not found\n", from);
        return;
    }

    if(!to_process){
        fprintf(stderr, "ERROR: process '%d' not found\n", to);
        return;
    }

    if(amount <= 0){
        fprintf(stderr, "ERROR: transfer amount must be positive\n");
        return;
    }
    if(from_process->tickets < amount){
        fprintf(stderr, "ERROR: insufficient tickets to transfer\n");
        return;
    }

    from_process->tickets -= amount;
    to_process->tickets += amount;

    printf("TRANSFER: process PID: %d (%s) -> process PID: %d (%s) tickets\n", from, from_process->name, to, to_process->name);
}

void inflate_tickets(int pid, int new_amount) {
    Process *proc = find_process(pid);
    if (!proc) {
        fprintf(stderr, "ERROR: process %d not found\n", pid);
        return;
    }
    
    if (new_amount <= 0) {
        fprintf(stderr, "ERROR: new amount must be positive\n");
        return;
    }
    
    int old_tickets = proc->tickets;
    if(new_amount <= old_tickets){
        fprintf(stderr, "ERROR: inflate amount invalid\n");
        return;
    }

    global_tickets -= old_tickets;
    proc->tickets = new_amount;
    global_tickets += new_amount;
    
    printf("[INFLATE] process %d (%s): %d -> %d tickets (+%d)\n", pid, proc->name, old_tickets, new_amount, new_amount - old_tickets);
}

void deflate_tickets(int pid, int new_amount) {
    Process *proc = find_process(pid);
    if (!proc) {
        fprintf(stderr, "ERROR: process %d not found\n", pid);
        return;
    }
    
    if (new_amount <= 0) {
        fprintf(stderr, "ERROR: new amount must be positive\n");
        return;
    }

    int old_tickets = proc->tickets;
    if(new_amount >= old_tickets){
        fprintf(stderr, "ERROR: deflate amount invalid\n");
        return;
    }

    global_tickets -= old_tickets;
    proc->tickets = new_amount;
    global_tickets += new_amount;
    
    printf("[DEFLATE] process %d (%s): %d -> %d tickets (+%d)\n", pid, proc->name, old_tickets, new_amount, old_tickets - new_amount);
}

void print_list(){
    Process *current = head;
    printf("\n=== Process List ===\n");
    printf("%-5s %-20s %-10s %-10s %-10s %-15s\n", 
           "PID", "Name", "Tickets", "Init", "State", "Wins");
    printf("---------------------------------------------------------------\n");
    
    while(current){
        const char* state_str;
        switch(current->state) {
            case READY: state_str = "READY"; break;
            case RUNNING: state_str = "RUNNING"; break;
            case BLOCKED: state_str = "BLOCKED"; break;
            default: state_str = "UNKNOWN"; break;
        }
        
        printf("%-5d %-20s %-10d %-10d %-10s %-15d\n",
            current->pid,
            current->name,
            current->tickets,
            current->initial_tickets,
            state_str,
            current->wins
        );
        current = current->next;
    }
    printf("Global Tickets: %d | Total Processes: %d\n\n", global_tickets, total_process);
}

void print_stats(int loops){
    Process *curr = head;
    printf("========== Lottery Scheduling Statistics ========== \n");
    printf("Total Simulations: %d\n", loops);
    printf("Global Tickets: %d\n\n", global_tickets);
    printf("%-5s %-20s %-10s %-10s %-10s %-15s %-15s\n",
           "PID", "Name", "Tickets", "Wins", "Actual%%", "Expected%%", "Deviation");
    printf("----------------------------------------------------------------------------------------------\n");
    
    double total_deviation = 0;
    
    while(curr){
        double expected = (curr->tickets / (double)global_tickets) * 100.0;
        double actual = (curr->wins / (double)loops) * 100.0;
        double deviation = actual - expected;
        total_deviation += (deviation * deviation);  // for standard deviation
        
        printf("%-5d %-20s %-10d %-10d %-15.2f%% %-15.2f%% %-15.2f%%\n",
            curr->pid,
            curr->name,
            curr->tickets,
            curr->wins,
            actual,
            expected,
            deviation
        );
        curr = curr->next;
    }
    
    double std_dev = sqrt(total_deviation / total_process);
    printf("Standard Deviation: %.2f%%\n", std_dev);
    printf("==================================================== \n");
}

int main(int argc, char* argv[]) {
    if(argc < 3){
        fprintf(stderr, "USAGE: lottery <seed> <loops> [--demo]\n");
        fprintf(stderr, "--demo: Run with ticket transfer/inflation demo\n");
        exit(1);
    }
    
    int seed = atoi(argv[1]);
    int loops = atoi(argv[2]);
    int demo_mode = (argc > 3 && strcmp(argv[3], "--demo") == 0);
    if(loops <= 0){
        fprintf(stderr, "loops must be > 0 \n"); 
        exit(1); 
    }
    srandom(seed);
    srand(seed);
    
    insert(1, "WebServer", 100);
    insert(2, "Database", 200);
    insert(3, "Cache", 300);
    insert(4, "Worker", 400);

    print_list();
    
    if(demo_mode){
        printf("\n DEMO MODE: Applying ticket mechanisms... \n");
        transfer_tickets(2, 1, 50);        
        inflate_tickets(1, 200);        
        deflate_tickets(3, 150);
        print_list();
        printf("\n Starting lottery with modified ticket distribution...\n\n");
    }
    
    for(int i=0; i<loops; i++) {
        int counter = 0;
        int winner = random() % global_tickets;
        Process *current = head;
        
        while(current){
            counter += current->tickets;
            if(counter>winner) break;  // found winner
            current = current->next;
        }
        
        if(current){
            current->wins++;
            current->total_cpu_time++;
        }
    }
    
    print_stats(loops);
    
    // free all memory
    Process *curr = head;
    while(curr){
        Process *temp = curr;
        curr = curr->next;
        free(temp);
    }
    head = NULL;
    return 0;
}
