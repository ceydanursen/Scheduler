# Scheduler

## __Abstract:__

The project I developed is a scheduler in C. In this project, the main implementation

method for scheduling is priority based, also it has a distinct feature: when more than

one processes with the same priority, the scheduler uses Round Robin approach to

decide which one gets into the CPU first and switch between them according to their

special time quantums.The aim of this project is to decide on the order of execution

of the processes coming into CPU.

## __Explanation of the general methods used:__

The project uses some methods. First of all it reads the necessary files to create

processes class and makes the processes ready for decision. The other method

used is creating a function that handles almost all scheduling parts. This function is

called priority_scheduler and it uses a main while loop that starts and does not finish

until all the processes are done.

I created some other helper methods for decision making. While this function

schedules the processes correctly, some values are stored to calculate average

waiting time and turnaround time for processes. I did not use queue data structure

for this project but tried a new creative approach. Creating my own functions for

sorting the processes and implementing round robin mechanisms is some of the

approaches that I used. Priority_scheduler function first looks at the processes

priorities, types, arrival times and other criteria to decide on which process should

got into CPU for execution in a for loop. After deciding on the process, it starts

executing the process in a while loop depending on whether it should be executed in

Round Robin scheduling or not. The loop continues until some higher priority

process comes into play or the process is not completed. Also, it cheks type

transformations and time quantums for processes and decides on the upcoming

process. The main aim of the helper functions are generally for round robin

mechanisms.

## __Installation__

Before running the below commands, make sure you have

+ GCC compiler
- Linux OS (prefereably Ubuntu)
  
Run the following commands to start the shell.

    $make

    $./scheduler
