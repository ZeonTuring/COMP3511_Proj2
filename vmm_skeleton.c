#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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

/*
 * 	1.		Get input command from files and parse them by lines
 * 	2.		While not end of file, do
 * 		2.1		Eliminate eol and extra space for each command
 * 		2.2		Print the command with specific format
 * 		2.3 	Parse the command into at most 3 arguments to recongise a mm_malloc() or mm_free() operation
 * 		2.4		If there are 2 argument, do
 * 			2.4.1	invoke mm_free()
 * 			2.4.2	set corresponding pointer to NULL
 * 		2.5		else invoke mm_malloc() and assign return address to corresponding pointer entry
 * 		2.6
 * 	5.	invoke mm_print() to print result
 * 	6.	set command counter to 0 for next iteration
 */

int main() {
    start_heap = sbrk(0);
    init_list(head);
    // Assume there are at most 26 different malloc/free
    // Here is the mapping rule
    // a=>0, b=>1, ..., z=>25
    void *pointers[26] = {NULL};
	
	char cmdline[255];
	char* cmd[4];
	int cmdCount = 0;
	while (fgets(cmdline,255, stdin) != NULL)
	{
		if (cmdline[strlen(cmdline)-1] == '\n')
			cmdline[strlen(cmdline)-1] = '\0';
		while (cmdline[strlen(cmdline)-1] == ' ')
			cmdline[strlen(cmdline)-1] = '\0';
		printf("=== %s ===\n",cmdline);
		char *token = strtok(cmdline, " ");
		while (token != NULL)
		{
			cmd[cmdCount++] = token;
			token = strtok(NULL, " ");
		}
		cmd[cmdCount++] = NULL;
		cmdCount--;
		if (cmdCount == 2)
		{
			mm_free(pointers[(int)*cmd[1]-97]);
			pointers[(int)*cmd[1]-97] = NULL;
		}
		else 
			pointers[(int)*cmd[1]-97] = mm_malloc(strtol(cmd[2],NULL,10));
		
		mm_print();
		cmdCount = 0;
	}
    return 0;
}

/*
 * 	1.	Construct two meta_data struct for iterating(init to cur->next) and storing new data
 * 	2.	While the meta data block is not the dummy head node, do
 * 		2.1	If the current block is free, do
 * 			2.1.1	If the current block size is big enough to be split, do
 * 				2.1.1.1	Constrcut a meta_data block after the current block
 * 				2.1.1.2	Initialise the block with attributes 'f' and appoint corresponding size
 * 				2.1.1.3	Set the current blcok to occupied and assigned suitable size
 * 				2.1.1.4 return with pointer of memory of current meta_data block
 * 			2.1.2	else if current block size is large enough for memory allocation, do
 * 				2.1.2.1	Set cuurent block to occupied.
 * 				2.1.2.2 return pointer the memory of the current meta_data block
 * 		2.2	Set current meta_data block pointer to be the next one
 * 	3.	Construct a new meta_data block at the end of heap
 * 	4.	Increase the heap size by sbrk() with corresponding size
 * 	5.	Set the new meta_data block attributes
 * 	6.	Add the new meta_data block at the end of the doubly linked list
 * 	7.	Return the address of the newly created memory.
 */

void *mm_malloc(size_t size) {
    // TODO: Complete mm_malloc here
	struct meta_data *cur = head->next;
	struct meta_data* new_meta_data;
	while ( cur != head) 
	{	
		if (cur->free == 'f')
			if (cur->size >= (size + meta_data_size))
			{	
				new_meta_data = (struct meta_data*)((char*)cur + meta_data_size + size);
				new_meta_data->free = 'f';
				new_meta_data->size = cur->size - size - meta_data_size;
				cur->free = 'o';
				cur->size=size;
				list_add(new_meta_data, cur, cur->next);
				return (void*)((char*)cur + meta_data_size);
			}
			else if (cur->size >= size)
			{
				cur->free = 'o';
				return (void*)((char*)cur + meta_data_size);
			}
        	cur = cur->next;
   	}
	new_meta_data = (struct meta_data*)sbrk(0);
	sbrk(size + meta_data_size);
	new_meta_data->free = 'o';
	new_meta_data->size = size;
	list_add_tail(new_meta_data,head);
	return (void*)(sbrk(0) - size);
}

/*
 *	1.	Init a meta_data pointer pointing to the head node
 *	2.	While the next meta_data block is not head node and not the meta_data block of the target memory region, do
 *		2.1 Set current meta_data pointer to the next one
 *	3.	Set the next meta_data block to occupied
 */

void mm_free(void *p) {
	struct meta_data *cur = head;
    while ( cur->next != head && (char*)cur->next+meta_data_size != p) 
	{
        cur = cur->next;
    }
	cur->next->free = 'f';	
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
    while ( cur != head) {
        printf("Block %02d: [%s] size = %d bytes\n", 
             i,  // block number - counting from bottom
            (cur->free=='f') ? "FREE" : "OCCP", // free or occupied
            cur->size); // size, in term of bytes
        i = i+1;
        cur = cur->next;
    } 
}
