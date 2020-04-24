#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Data structure of meta_data 
struct 
  __attribute__((__packed__)) // compiler directive, avoid "gcc" padding bytes to struct
  meta_data {
    size_t size;             // 8 bytes (in 64-bit OS)
    char free;               // 1 byte ('f' or 'o')
    struct meta_data *next;  // 8 bytes (in 64-bit OS)
    struct meta_data *prev;  // 8 bytes (in 64-bit OS)
};

// calculate the meta data size and store as a constant (exactly 25 bytes)
const size_t meta_data_size = sizeof(struct meta_data); 

// Global variables
void *start_heap = NULL; // pointing to the start of the heap, initialize in main()
struct meta_data dummy_head_node; // dummy head node of a doubly linked list, initialize in main()
struct meta_data *head = &dummy_head_node;

// The implementation of the following functions are given:
void list_add(struct meta_data *new, struct meta_data *prev, struct meta_data *next);
void list_add_tail(struct meta_data *new, struct meta_data *head);
void init_list(struct meta_data *list);

// Students are required to implement these functions below
void *mm_malloc(size_t size);
void mm_free(void *p);
void mm_print();


int main() {
    start_heap = sbrk(0);
    init_list(head);
    // Assume there are at most 26 different malloc/free
    // Here is the mapping rule
    // a=>0, b=>1, ..., z=>25
    void *pointers[26] = {NULL};
	
	char cmdline[255];
    fgets(cmdline, 255, stdin);
	printf("%s",cmdline);
	printf("%s",cmdline);
    // TODO: Complete the main function here
    return 0;
}
void *mm_malloc(size_t size) {
    // TODO: Complete mm_malloc here   
    return NULL;
}
void mm_free(void *p) {
    // TODO: Complete mm_free here
}

void init_list(struct meta_data *list) {
    list->next = list;
    list->prev = list;
}

void list_add(struct meta_data *new,
            struct meta_data *prev,
            struct meta_data *next) {
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

void list_add_tail(struct meta_data *new,
                    struct meta_data *head) {
    list_add(new, head->prev, head);
}


void mm_print() {
    struct meta_data *cur = head->next;
    int i = 1;
    while ( cur != head ) {
        printf("Block %02d: [%s] size = %d bytes\n", 
             i,  // block number - counting from bottom
            (cur->free=='f') ? "FREE" : "OCCP", // free or occupied
            cur->size ); // size, in term of bytes
        i = i+1;
        cur = cur->next;
    } 
}

void ParseArgument(char** args, char* cmd)
{
	int num = 0;
	char* token = strtok(cmd," ");
	while (token != NULL)
	{
		args[num++] = token;
		token = strtok(NULL, " ");
	}
	args[num++] = NULL;
}
