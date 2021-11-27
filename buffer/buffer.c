#include "buffer.h"
#include <linux/kernel.h>
#include <linux/syscalls.h>

static ring_buffer_421_t buffer;
struct semaphore mutex;
struct semaphore fill_count;
struct semaphore empty_count;

SYSCALL_DEFINE0(init_buffer_421){
	// Note: You will need to initialize semaphores in this function.
	// Ensure we're not initializing a buffer that already exists.
	if (buffer.read || buffer.write) {
		printk("init_buffer_421(): Buffer already exists. Aborting.\n");
		return -1;
	}

	// Create the root node.
	node_421_t *node;
	node = (node_421_t *) kmalloc(sizeof(node_421_t), GFP_KERNEL);
	// Create the rest of the nodes, linking them all together.
	node_421_t *curr;
	int i;
	curr = node;
	// Note that we've already created one node, so i = 1.
	for (i = 1; i < SIZE_OF_BUFFER; i++) {
		curr->next = (node_421_t *) kmalloc(sizeof(node_421_t), GFP_KERNEL);
		curr = curr->next;
	}
	// Complete the chain.
	curr->next = node;
	buffer.read = node;
	buffer.write = node;
	buffer.length = 0;

	// Initialize your semaphores here.	
	sema_init(&mutex, 1);	
	sema_init(&fill_count, 0);
	sema_init(&empty_count, SIZE_OF_BUFFER);
	return 0;
}

SYSCALL_DEFINE1(enqueue_buffer_421, char*, data){
	// NOTE: You have to modify this function to use semaphores.
	printk("Entering enqueue\n");

	if (!buffer.write) {
		printk("write_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}

	// decrement empty count because we are enqueueing
	// put a lock on for enqueue
	down(&empty_count);
	down(&mutex);

	// copy memory from data to the current write node
	if(copy_from_user(buffer.write->data, data, DATA_LENGTH) != 0)
		return -1;

	printk(":: Enqueueing element into buffer. ::\n%s\n", buffer.write->data);
	// Advance the pointer.
	buffer.write = buffer.write->next;
	buffer.length++;
	printk("%i items in the buffer.\n", buffer.length);

	// release lock on enqueue
	// increment fill count because we successfully enqueued
	up(&mutex);
	up(&fill_count);

	printk("Exiting enqueue\n");
	return 0;
}

SYSCALL_DEFINE1(dequeue_buffer_421, char*, data){
	// NOTE: Implement this function.
	printk("Entering dequeue\n");

	if(!buffer.read){
		printk("read_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}

	// decrement fill count because we are dequeueing
	down(&fill_count);
	down(&mutex);

	// get memory from current read node and display it
	if(copy_to_user(data, buffer.read->data, DATA_LENGTH) != 0)
		return -1;

	printk(":: Dequeueing element from buffer. ::\n%s\n", data);
	buffer.read = buffer.read->next;
	buffer.length--;
	printk("%i items in the buffer.\n", buffer.length);

	// increment empty count because we successfully dequeued
	up(&mutex);
	up(&empty_count);

	printk("Exiting dequeue\n");
	return 0;
}

SYSCALL_DEFINE0(delete_buffer_421){
	// Tip: Don't call this while any process is waiting to enqueue or dequeue.
	if (!buffer.read) {
		printk("delete_buffer_421(): The buffer does not exist. Aborting.\n");
		return -1;
	}
	// Get rid of all existing nodes.
	node_421_t *temp;
	node_421_t *curr = buffer.read->next;
	while (curr != buffer.read) {
		temp = curr->next;
		kfree(curr);
		curr = temp;
	}
	// Free the final node.
	kfree(curr);
	curr = NULL;
	// Reset the buffer.
	buffer.read = NULL;
	buffer.write = NULL;
	buffer.length = 0;
	return 0;
}
