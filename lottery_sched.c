#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int global_tickets = 0;
struct node{
    int tickets; 
    int wins;
    struct node* next;
};

struct node *head = NULL;

void insert(int tickets){
    struct node *temp = malloc(sizeof(struct node));
    assert(temp != NULL);
    temp->tickets = tickets;
    temp->next = head;
    
    head = temp;
    global_tickets += tickets;
}

void print_list(){
    struct node *current_node = head;
    printf("List: ");
    while(current_node){
        printf("[%d] ", current_node->tickets);
        current_node = current_node->next;
    }
    printf("\n");
}

void print_stats(int loops){
    struct node *curr=head;
    printf("--- Stats --- \n");
    printf("Global Tickets: %d\n", global_tickets);
    while (curr) {
        printf("process [%3d tickets]: %4d wins  (%5.1f%%  expected %5.1f%%)\n",
            curr->tickets,
            curr->wins,
            (curr->wins  / (double)loops)    * 100.0,
            (curr->tickets / (double)global_tickets) * 100.0
        );
        curr = curr->next;
    }
}


int main(int argc, char* argv[]){
    if(argc != 3) {
	    fprintf(stderr, "usage: lottery <seed> <loops>\n");
	    exit(1);
    }
    int seed  = atoi(argv[1]);
    int loops = atoi(argv[2]);
    srandom(seed);

    insert(100);
    insert(200);
    insert(300);

    
    int i;
    for(i=0; i<loops; i++){
        int counter = 0;
        int winner = random() % global_tickets; // get winner
        struct node *current = head;

        while(current){
            counter += current->tickets;
            if (counter > winner) break; // found the winner
            current = current->next;
        }
        current->wins++;
        // print_list();
        // printf("winner: %d %d\n\n", winner, current->tickets);
    }   
    print_stats(loops);
    return 0;
}


