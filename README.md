# Multi-ThreadProcessor

## Introduction
In this assignment, you'll write a program that will get you familiar with the use of threads, mutual exclusion and condition variables.

## Instructions
Write a program that creates 4 threads to process input from standard input as follows

1. Thread 1, called the Input Thread, reads in lines of characters from the standard input.
2. Thread 2, called the Line Separator Thread, replaces every line separator in the input by a space.
3. Thread, 3 called the Plus Sign thread, replaces every pair of plus signs, i.e., "++", by a "^".
4. Thread 4, called the Output Thread, write this processed data to standard output as lines of exactly 80 characters.

Furthermore, in your program these 4 threads must communicate with each other using the Producer-Consumer approach. 

## Input
- A “line of input” is
  - A sequence of the allowed characters (see below) that does not include a line separator
  - Followed by a line separator.
- Line separator
  - Since the program is going to be tested on os1, and hence Unix, for the purposes of this assignment line separator is the newline character, i.e., '\n'
  - Note that line separator may not be the newline character on Windows or Mac, but that's irrelevant for this assignment.
- Allowed characters
  - Other than the line separator, the input will only consist of ASCII characters from space (decimal 32) to tilde (decimal 126). These are sometimes termed printable characters.
- Stop-processing line
  - Your program must process input lines until it receives an input line that contains only the characters STOP, i.e., STOP followed immediately by the line separator.
  - Examples: The following input lines must not cause the program to stop processing the input
    - STOP!
    - ISTOP
    - stop
    - Stop
- If there are any more lines in the input after the stop-processing line, the program must not process them.
- The input will not contain any empty lines, i.e., lines that only have space characters or no characters except the line separator.
- An input line will never be longer than 1000 characters (including the line separator).
- The input for the program will never have more than 49 lines before the stop-processing line.
- Your program doesn't need to check the input for correctness.

## Output
- The “80 character line” to be written to standard output is defined as 80 non-line separator characters plus a line separator.
- Your program must not wait to produce the output only when the stop-processing line is received.
    - Whenever your program has sufficient data for an output line, the output line must be produced.
- After the program receives the stop-processing line and before it terminates, the program must produce all 80 character lines it can still produce based on the input lines which were received before the stop-processing line and which have not yet been processed to produce output.
- No part of the stop-processing line must be written to the output.
- In addition, your program must not output any user prompts, debugging information, status messages, etc.
- Your program must output only lines with 80 characters (with a line separator after each line).
- For the second replacement requirement, pairs of plus signs must be replaced as they are seen.
  - Examples:
    - The string “abc+++def” contains only one pair of plus signs and must be converted to the string "abc^+def".
    - The string “abc++++def” contains two pairs of plus signs and must be converted to the string "abc^^def".
  - Thus, whenever the Plus Sign thread replaces a pair of plus signs by a caret, the number of characters produced by the Plus Sign thread decreases by one compared to the number characters consumed by this thread.

## Example
1. We start the program, type 10 characters (not containing any ++) and then press enter. These characters should not be printed to standard output right away because we have 11 characters available to write (10 characters we typed and a space that replaced the line separator) and don't yet have the 80 characters needed to write one complete line.
2. Next we type 170 characters (not containing any ++) and then press enter. Now there are 182 characters available to write, 11 characters from the Step 1, 170 characters that we typed in step 2 and the space that replaced the line separator in step 2.
3. The program must write 2 lines with 80 characters each. There are still 22 characters available for output.
4. Next we type the stop-processing line. Since we only write complete lines with 80 characters, although there are 22 characters still available to write, the program terminates without writing these characters to the standard output.

## Multi-Threading Requirements


A pipeline of 4 threads. Input thread reads data from stdin and puts it in Buffer 1. Line Separator thread gets data from Buffer 1, processes it and puts it in Buffer 2. Plus Sign Thread reads data from Buffer 2, processes it and puts it in Buffer 3. Output Thread reads data from Buffer 3 and output lines to stdout.

### Pipeline of threads that gets data from stdin, processes it and displays it to stdout
- Each pair of communicating threads must be constructed as a producer/consumer system.
- If a thread T1 gets its input data from another thread T0, and T1 outputs data for use by another thread T2, then
    - T1 acts as a consumer with respect to T0 and T0 plays the role of T1’s producer
    - T1 acts as a producer with respect to T2 and T2 plays the role of T1’s consumer
- Thus each thread in the interior of the pipeline (i.e., the Line Separator and Plus Sign threads) will contain both producer code and consumer code.
- Each producer/consumer pair of threads will have its own shared buffer. Thus, there will be 3 of these buffers in your program, each one shared only by its producer and consumer.
- You must use condition variables for coordination.
- Your program must never sleep.
- If you size your buffers to hold 50 lines of 1000 characters each, you can model the problem as Producer-Consumer with unbounded buffers which will make your program simpler.
    - Recall that unbounded buffers are never full, though they can be empty.

## Hints & Resources
- Consider creating a single threaded implementation of the required processing to get the processing logic ironed out.
  - In the single-threaded version, put the processing that will eventually be done by different threads in separate functions. Then you can use those functions in your multi-threaded program.
  - When you create a multi-threaded implementation of your program, for the same input, the output of both implementations of the programs must be the same.
- For the multi-threaded implementation:
  - We have provided an example programLinks to an external site. that implements a pipeline of producers and consumers consisting of 3 threads.
  - Feel free to adapt this program for your implementation. As always, you are also allowed to use any code presented in the course explorations.
