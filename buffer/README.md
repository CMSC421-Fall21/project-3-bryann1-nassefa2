Author: Bryan Nguyen & Naod Assefa
Email(s): bryann1@umbc.edu & nassefa2@umbc.edu
Date: 11/26/2021 CMSC 421 Project 3

/**************** README ****************/
- This is the Project 3 producer-consumer program 
- We are demonstrating the producer consumer problem in a Linux kernel.
- The prerequisites are:
  - Having Debian 11 on your virtual machine
  - A clean copy of Linux kernel 5.10.70 
  - The project 3 files

- To compile it use the command "gcc test.c -o test -lpthread -lrt"
- We have both userspace and kernel space implementations which have their
  own test files. 
  - buffer_user.c -> userTest.c & buffer.c -> test.c

- The producer will:
  - Write to consecutive blocks of characters from 0-9 in order.
  - Make sure we are only adding one digit we used the corresponding decimal
    value to the chars '0'-'9' according to the ASCII table then type cast to 
    a char.
  - Use memset to conveniently copy the char to data block and all of 
    its 1204 bytes.
  - Null terminate each block to prevent read size errors and such.
  - Wait between 0-10ms randomly before enqueue.
- The consumer will:
  - Get a block from the buffer.
  - Wait a random amount of time between 0-10 ms before dequeue.
- For the sleep functions we used usleep and multiplied it by 1000 in order to
    convert from microseconds (original unit of usleep) to milliseconds.

- Enqueue:
  - Will hold when buffer is full and will release when size is no longer 
    full/dequeue is called.
  - Increase size.
- Dequeue:
  - Vice versa of the above.^ 
- Mutual exclusion (mutex binary semaphore) will make sure enqueue 
  and dequeue do not interfere with each other. The fill count and 
  empty count will basically tell you the size of the buffer and also 
  take care of the edge case of dequeueing on an empty buffer or enqueueing 
  on a full buffer.

- In the buffer.c we did our initialization, deletion, enqueue and dequeue. 
- We also initialized our semaphores in init_buffer_421().
- In test.c we tested for the edge cases such as calling the enqueue, 
    dequeue, and delete on a nonexistent buffer. 
- We also initialized our pthreads and calls for the producer and consumer 
    functions. The aforementioned producer and consumer functions will call
    enqueue and dequeue in a loop 100k times respectively. 
- Then we finally join the threads to end the program.
