/*Heather Bullard 
 * Project: Multi-Thread Processor
 * Date Due: 2/25/2024
 * Description: Write a program that creates 4 threads to process input from standard input as follows
 *                -Thread 1, called the Input Thread, reads in lines of characters from the standard input.
 *                -Thread 2, called the Line Separator Thread, replaces every line separator in the input by a space.
 *                -Thread, 3 called the Plus Sign thread, replaces every pair of plus signs, i.e., "++", by a "^".
 *                -Thread 4, called the Output Thread, write this processed data to standard output as lines of exactly 80 characters.
 *                -Furthermore, in your program these 4 threads must communicate with each other using the Producer-Consumer approach.
 * Sources: Module 6: Concurrency & Thread
 *          Linux Programming Interface
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

#define MAX_CHAR 1000
#define MAX_LINES 50
#define MAX_LINE_LEN 80

int length = 0;

char buffer_1[MAX_LINES][MAX_CHAR];					//shared resource between input and line separator
int count_1 = 0;						//number of items in the buffer
int prod_idx_1 = 0;						//index where the input thread will put the next item
int con_idx_1 = 0;						//index where the line separator will put the next item
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;		//initialize the mutex for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;		//initialize the condition variable for buffer 1

char buffer_2[MAX_LINES][MAX_CHAR];					//shared resource between line separator and plus sign 
int count_2 = 0;						//number of items in the buffer
int prod_idx_2 = 0;						//index where the line separator thread will put the next item
int con_idx_2 = 0;						//index where the plus sign thread will put the next item
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;		//initialize the mutex for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;		//initialize the condition variable for buffer 2

char buffer_3[MAX_LINES][MAX_CHAR];					//shared resource between plus sign and output thread
int count_3 = 0;						//number of items in the buffer
int prod_idx_3 = 0;						//index where the plus sign thread will put the next item
int con_idx_3 = 0;						//index where the output thread will put the next item
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;		//initialize the mutex for buffer 3
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;		//initialize the condition variable for buffer 3


/* -------InputThread-------
 *  Function that the input thread will run.
 *  Get input from the user.
 *  Put the item in the buffer shared with the square_root thread.
*/

void *inputThread(void *args) {
  char line[MAX_CHAR];
  for(;;) {

    fgets(line, MAX_CHAR, stdin);
    fflush(stdin);
    if (strstr(line, "STOP\n") != NULL) {
      line[0] = '\t';
      line[1] = '\0';
    }
	  pthread_mutex_lock(&mutex_1);                                      // Lock the mutex before putting the item in the buffer
	  strcpy(buffer_1[prod_idx_1], line);                                       // Put the item in the buffer
	  prod_idx_1 = prod_idx_1 + 1;                                       // Increment the index where the next item will be put.
	  count_1++;
	  pthread_cond_signal(&full_1);                                      // Signal to the consumer that the buffer is no longer empty
	  pthread_mutex_unlock(&mutex_1);                                    // Unlock the mutex

    length++;
    if (line[0] == '\t') {
      close(fileno(stdin));
      break;
    }
  }
  return NULL;
}

/* ------LineSeparator-------
 * LineSeparator puts buffer1 into line2 
 * result changes all the \n to spaces
 * line2 is copied to buffer2
 * */
void *lineSeparator(void *args) {
  char line2[MAX_CHAR];
 // char newLine = '\n';
  char *result;
 // char space = ' ';
  for (;;) {
    pthread_mutex_lock(&mutex_1);
	  while (count_1 == 0) pthread_cond_wait(&full_1, &mutex_1);
  	strcpy(line2, buffer_1[con_idx_1]);
  	con_idx_1 = con_idx_1 + 1;
	  count_1--;
	  pthread_mutex_unlock(&mutex_1);
    
    while ((result = strstr(line2, "\n")) != NULL) {
      result[0] = ' ';
      
    }
    pthread_mutex_lock(&mutex_2);
	  strcpy(buffer_2[prod_idx_2], line2);
	  prod_idx_2 = prod_idx_2 + 1;
    count_2++;
	  pthread_cond_signal(&full_2);
	  pthread_mutex_unlock(&mutex_2);;
   
   if (line2[0] == '\t') break;
  }
  return NULL;
}

/* ------replacePlusSign------
 * replacePlusSign puts buffer2 into line3
 * result changes each occurrence of '++' to a caret and '~' to maintain memory
 * line3 is copied into buffer3
 * */

void *replacePlusSign(void *args) {
  char line3[MAX_CHAR];
//  char plus = '+';
 // char caret = '^';
  char *result;
  for (int i = 0; i < MAX_LINES; ++i) {  
   	pthread_mutex_lock(&mutex_2);
	  while (count_2 == 0) pthread_cond_wait(&full_2, &mutex_2);
	  strcpy(line3, buffer_2[con_idx_2]);
	  con_idx_2 = con_idx_2 + 1;
	  count_2--;
	  pthread_mutex_unlock(&mutex_2); 

    
    while ((result = strstr(line3, "++")) != NULL) {
      result[0] = '^';
      result[1] = '~';
      result++;
    }
	  pthread_mutex_lock(&mutex_3);
	  strcpy(buffer_3[prod_idx_3], line3);
	  prod_idx_3 = prod_idx_3 + 1;
	  count_3++;
	  pthread_cond_signal(&full_3);
	  pthread_mutex_unlock(&mutex_3);
    if (line3[0] == '\t') break;
  }
  return NULL;
} 


/* ------write_output------
 * write_out puts buffer_3 into line4
 * Once 80 characters is available it immediately writes to stdout
 * '~' is removed in the process, if less than 80 characters then
 * then those characters are not printed.
 * */
void *writeOutput(void *args) {
  int op = 0;
  char printer[81];
  char line4[MAX_CHAR];
 // char plug = '~';
  
  for (;;) {
    pthread_mutex_lock(&mutex_3);
    while(count_3 == 0) pthread_cond_wait(&full_3, &mutex_3);
    strcpy(line4, buffer_3[con_idx_3]);
    con_idx_3 = con_idx_3 +1;
    count_3--;
    pthread_mutex_unlock(&mutex_3);
       
    if (line4[0] == '\t') break;
    int len2 = strlen(line4);
    for (int i = 0; i < len2; ++i) {
      
      if (line4[i] == '~') i++;
      printer[op] = line4[i];
      op++;
      if (op == MAX_LINE_LEN) {
        fwrite(printer, 1, 80, stdout);
        putchar('\n');
        fflush(stdout);
        op = 0;
      }
     // printer[op] = line4[i];
     // op++;
     
    }
  }
  return NULL;
}

int main(void) {
  pthread_t input_t, lineSeparator_t, plusSign_t, output_t;
  
  // Create the threads
  pthread_create(&input_t, NULL, inputThread, NULL);
  pthread_create(&lineSeparator_t, NULL, lineSeparator, NULL);
  pthread_create(&plusSign_t, NULL, replacePlusSign, NULL);
  pthread_create(&output_t, NULL, writeOutput, NULL);
  
  // Wait for the threads to terminate
  pthread_join(input_t, NULL);
  pthread_join(lineSeparator_t, NULL);
  pthread_join(plusSign_t, NULL);
  pthread_join(output_t, NULL);

  return EXIT_SUCCESS;
}


