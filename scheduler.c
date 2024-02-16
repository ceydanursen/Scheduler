#include <stdio.h>   // For standard input/output operations
#include <stdlib.h>  // For memory allocation and other utility functions
#include <stdbool.h> 
#include <string.h>
#include<limits.h>
#define MAX_LINE_LENGTH 100
#define NUMBER_INST_MAX 21
#define FILE_NUM 10
int same_priority_count = 0;
//instruction struct is used when reading instructions.txt
struct Instruction {
    char name[MAX_LINE_LENGTH];
    int time;//execution duration
    bool is_complete;
};
struct InstructionDictionary {
    struct Instruction instructions[NUMBER_INST_MAX];
    int count;
   
};
void initializeDictionary(struct InstructionDictionary *dict) {
    dict->count = 0;
}

void addToDictionary(struct InstructionDictionary *dict, const char *name, int time) {
    if (dict->count < NUMBER_INST_MAX) {
        strcpy(dict->instructions[dict->count].name, name);
        dict->instructions[dict->count].time = time;
        dict->instructions[dict->count].is_complete = false;
        dict->count++;
    } else {
        //printf("Dictionary full, cannot add more instructions.\n");
    }
}
//i used dictionary for instructions
enum Types{
    GOLD = 0,
    SILVER = 1,
    PLATINUM=2
};
//to define types of the processes enum is used
struct Process {
    char name[MAX_LINE_LENGTH];//name of the process
    int priority;//priority of the process
    int line_where_left;//to get current instruction after preemption
    struct Instruction processes_instructions[21];//the instructions for the process are stored here
    int num_instructions;//i am checking if the line_where_left is eqaul to num_instruciton to get if the process is done.
    enum Types type_of_process;//type of the process
    int arrival_time;//arrival time for the process
    int time_quantum;
    bool completed;//checks whether the process is completed or not
    int burst_count;//when burst count is reached type transformation occurs
    int time_of_completion;//i used time_of_completion at the end of the main calculating turnaround and waiting time
    int burst_time;//i used burst_time at the end of the main calculating turnaround and waiting time
    int control;//to check the quantum is reached
    int time_selected;//time selected used in RR part.
    int first_CPU;//it stores the time that process comes into CPU first time
    int into_CPU;//into and out CPU is used when i deciding round robin
    int out_CPU;
};
enum Types getProcessType(const char *process, struct Process *process_object){
    if (strcmp(process, "GOLD") == 0) {
        process_object->time_quantum = 120;
        return GOLD;
    } else if (strcmp(process, "SILVER") == 0) {
        process_object->time_quantum = 80;
        return SILVER;
    } else if (strcmp(process, "PLATINUM") == 0) {
        process_object->time_quantum=120;
        return PLATINUM;
    } else {
        process_object->time_quantum=-1;
        return -1; // Indicate an error or unknown type
    }
}
struct Process processes[FILE_NUM];
//at the beginning creating 10 processes and initializing their fields accordingly
void create_10_processes(){
    for(int p =0;p<10;p++){
        struct Process process;
        char process_name[MAX_LINE_LENGTH];
        sprintf(process_name, "P%d", p + 1); // Generating process name "P1", "P2", ..., "P10"
        strcpy(process.name, process_name);
        processes[p] = process;
        processes[p].priority=-1;
        processes[p].arrival_time=-1;
        processes[p].type_of_process=-1;
        processes[p].completed=false;
        processes[p].line_where_left=0;
        processes[p].burst_count=0;
        processes[p].time_of_completion=0;
        processes[p].burst_time=0;
        processes[p].control=0;
        processes[p].time_selected=0;
        processes[p].first_CPU=0;
        processes[p].into_CPU=0;
        processes[p].out_CPU=0;
    }
};
int index_same;//index_same is an important variable it is used in round-robin part.
int numberOfProcesses=0;//it is used in file reading in the main
//prior_process is the function that checks whether a higher priority process comes to the CPU
int prior_process(struct Process processes_to_execute[], int line_count,int current_time, struct Process *selected_process){
    int next_process_index= -1;
    for (int i = 0; i < line_count; i++) {
        //cheking PLAT processes and others in different conditions
        
        if(processes_to_execute[i].arrival_time <= current_time &&
            processes_to_execute[i].type_of_process == PLATINUM && !processes_to_execute[i].completed){
            next_process_index = i; // Higher priority process found, preempt the current process
            selected_process = &processes_to_execute[next_process_index];
            break;  

            }
        else if (processes_to_execute[i].arrival_time <= current_time &&
            processes_to_execute[i].priority > selected_process->priority && !processes_to_execute[i].completed) {
            next_process_index = i; // Higher priority process found, preempt the current process
            selected_process = &processes_to_execute[next_process_index];
            //if such a process is found then break the for loop and return its index.
            break;
                        }
            }
    return (next_process_index);

}
//same_priority_count_change is yet crucial function that cheks how many process have the same priority. //i am using its return value as a decision
//variable to implement round_robin or not.
int same_priority_count_change(struct Process processes_to_execute[], int line_count,int current_time, struct Process *selected_process){
    int increase = 0;
    //exculuding Plat processes counting the processes that have the same priority, are arrived and not completed processes
    for (int i = 0; i < line_count; i++) {
        if (processes_to_execute[i].arrival_time <= current_time && processes_to_execute[i].type_of_process != PLATINUM &&
            processes_to_execute[i].priority == selected_process->priority && !processes_to_execute[i].completed) {
            increase++; // Higher priority process found, preempt the current process
            
            
            }
            }
    same_priority_count=increase;

    return (increase);

}
//same_pr_index is used when a process completes its time quantum this function finds the next process available to execute
int same_pr_index(int next_process_index, struct Process processes_to_execute[], int line_count, int current_time, struct Process *selected_process) {
    int temp = next_process_index;
    if(selected_process->type_of_process==PLATINUM){
        //since the plat processes are executed first, in this if statement i am preventing a low priority eqauls to plat process' priority to be choosen
        //cheking the whole processes_to_execute array to find if a higher priority process is in that array.
        for(int h = 0;h<line_count;h++){
            if(processes_to_execute[h].priority > selected_process->priority && !processes_to_execute[h].completed && processes_to_execute[h].type_of_process!=PLATINUM
            &&processes_to_execute[h].arrival_time <= current_time){
                next_process_index=h;
            }
        }
    }
    else{
        while(true){
            //until finding a process with the same priority ,not completed not having the same name the while loop executes
        next_process_index = ((next_process_index + 1) % line_count);
        if(processes_to_execute[next_process_index].priority == selected_process->priority &&
        processes_to_execute[next_process_index].arrival_time <= current_time&&strcmp(selected_process->name ,processes_to_execute[next_process_index].name)!=0 && processes_to_execute[next_process_index].completed==false 
                            ){
                                break;
                            }
        else if(temp==next_process_index){
            //if we checked the whole process and turn back to the beginning then break the loop we could not find such a process 
            break;
        }

    }

    }
    index_same = next_process_index;//index_same is a global variable //it is used when deciding the next process
    return next_process_index;
}

 //priority scheduler is the function that handles almost all things to be done in this project,it gets an array of processes and number of lines in
 //other words its size   
void priority_scheduler(struct Process processes_to_execute[], int line_count) {
    int current_time = 0;
    //starting the current time
    while (true) {
        bool processes_completed = true;
        // Check if any processes are still pending for execution
        for (int i = 0; i < line_count; i++) {
            
            if (!processes_to_execute[i].completed) {
                processes_completed = false;
                break;
            }
        }

        if (processes_completed) {
            break; // All processes have completed execution
        }

        // Select the process with the highest priority and is ready for execution
        int process_index = -1;
        int highest_priority = INT_MIN; // Initialize with min value
        bool platinum_found=false;
        bool found_index_same_change=false;
        for (int i = 0; i < line_count; i++) {
            //we are searching whole array starting from index 0
            if (!processes_to_execute[i].completed && processes_to_execute[i].arrival_time <= current_time) {
                //if the process is not completed and it is arrived before current time we start to examine
                if(processes_to_execute[i].type_of_process == PLATINUM){
                    //if this process is a Plat process we should implement the algorithm somewhat different
                    platinum_found=true;
                    if(i==0){
                        highest_priority=processes_to_execute[i].priority;
                        process_index=i;
                    }
                    else if(processes_to_execute[i].priority == processes_to_execute[process_index].priority && processes_to_execute[process_index].type_of_process==PLATINUM){
                        //if there are more than one Plat process then we are deciding according to their names
                        //smaller name comes first P1 comes before P2 for example
                        if(strcmp(processes_to_execute[i].name,processes_to_execute[process_index].name)<0){
                            highest_priority=processes_to_execute[i].priority;
                            process_index=i;
                        }
                    }
                    else if(processes_to_execute[i].priority == highest_priority){
                        highest_priority=processes_to_execute[i].priority;
                        process_index=i;
                    }
                    else{
                        //in that else part if the chosen plat process should be replaced by the other plat process. it occurs if there is a plat 
                        //process with higher priority
                        if (processes_to_execute[i].priority > processes_to_execute[process_index].priority && processes_to_execute[process_index].type_of_process==PLATINUM) {
                        
                            highest_priority = processes_to_execute[i].priority;
                            process_index = i;
                        }
                        else if(processes_to_execute[i].priority < processes_to_execute[process_index].priority && processes_to_execute[process_index].type_of_process==PLATINUM){
                            ;
                        }
                        
                        else{
                            process_index=i;
                        }
                    }
                }
                //thia else if part is simply implementing ROUND ROBIN logic 
                //if sam_priority_count is bigger than one,it means we have processes with the same priority and some special conditions are also met
                else if(same_priority_count>1 && platinum_found == false){
                    //same_pr_index function founds a process that is available but here i am cheking whether this process to be executed first

                    if(processes_to_execute[i].time_selected < processes_to_execute[index_same].time_selected && processes_to_execute[i].priority == processes_to_execute[index_same].priority
                    &&processes_to_execute[i].into_CPU<processes_to_execute[index_same].out_CPU){
                        process_index=i;
                        index_same=i;
                    }
                    

                    else if (processes_to_execute[i].arrival_time <= processes_to_execute[index_same].arrival_time&&
                        
                        processes_to_execute[i].time_selected <= processes_to_execute[index_same].time_selected && processes_to_execute[i].priority == processes_to_execute[index_same].priority) {
                            if(processes_to_execute[i].arrival_time < processes_to_execute[index_same].arrival_time && !found_index_same_change){
                                found_index_same_change=true;
                                process_index=i;
                                index_same=i;
                            }
                            //the process with the smallest name chosen
                            else if(strcmp(processes_to_execute[i].name,processes_to_execute[index_same].name)<0){
                                index_same=i;
                                highest_priority=processes_to_execute[i].priority;
                                process_index=i;
                            } 
                            else{
                                //there is no change
                                process_index=index_same;
                            }

                        }
                        else{
                            //there is no change
                            process_index=index_same;
                        }

                }   
                //in that part if there is no other process with the same priority check whether a higher priority is found
                else if (processes_to_execute[i].priority > highest_priority && platinum_found==false) {
                    highest_priority = processes_to_execute[i].priority;
                    process_index = i;
                }
                //if the found priority is eqaul to the process which is being examined then choose the one with smaller arrival time
                else if(processes_to_execute[i].priority == highest_priority && platinum_found==false){
                    
                        //smaller the arrival time should come first
                        //also cheking the name priority
                        if(strcmp(processes_to_execute[i].name,processes_to_execute[process_index].name)<0 && processes_to_execute[i].arrival_time<processes_to_execute[process_index].arrival_time){
                            
                            highest_priority=processes_to_execute[i].priority;
                            process_index=i;
                        } 
                        else if(processes_to_execute[i].arrival_time<processes_to_execute[process_index].arrival_time) {
                           
                        process_index=i;
                        }
                    
                }
            }
        }
        //now one process is ready to execute//above we determine the process index
        if (process_index != -1) {
            //process_index == -1 means we should increase the current time until one process available is found
            // Execute the selected process based on its type and time quantum
            struct Process *selected_process = &processes_to_execute[process_index];
            //creating selected process struct which reffering to the process with the process index found above
            if(selected_process->time_selected==0){
                //first_CPU field is set
                selected_process->first_CPU=current_time;
            }
            selected_process->time_selected=selected_process->time_selected+1;
            //because the process is selected we are incrementing this field. it basicly stores the information that how many times the process got into the CPU
            selected_process->into_CPU=current_time;
           

            // Handle execution based on process type and rules
            int remaining_time = selected_process->processes_instructions[selected_process->line_where_left].time;

            if (selected_process->type_of_process == PLATINUM) {
                // Execute Platinum process immediately without preemption
                //stariting from the line where left we are executing plat process
                for(int y = selected_process->line_where_left ;y<selected_process->num_instructions;y++){
                    current_time=current_time+selected_process->processes_instructions[y].time;
                    selected_process->processes_instructions[y].is_complete=true;
                }
                selected_process->completed=true;
                //it is completed
                selected_process->time_of_completion=current_time;
                //completion time is also recorded
                //in the part below i am cheking if the all processes are completed//if not than we can context switch by adding 10 ms to the current time
                bool all_completed = true;
                for (int i = 0; i < line_count; i++) {
                    if (!processes_to_execute[i].completed) {
                        all_completed = false;
                        break;
                    }
                }
                if (all_completed) {
                    break;
                }
                else{
                  current_time=current_time+10;  
                }
                //after finishing executing a process call those functions to update global variables they are setting
                (same_priority_count_change(processes_to_execute, line_count,current_time, selected_process));
                same_pr_index(process_index,processes_to_execute,line_count, current_time, selected_process);
            } else if (selected_process->type_of_process == GOLD || selected_process->type_of_process == SILVER) {
                // Execute Gold/Silver process with priority handling
                // Execute using round-robin if there are multiple processes with the same priority
                if ((same_priority_count_change(processes_to_execute, line_count,current_time, selected_process)) > 1) {
                    int process_quantum;
                    // Determine the time quantum based on the process type
                    if (selected_process->type_of_process == GOLD) {
                        
                        process_quantum = 120; 
                    } else if (selected_process->type_of_process == SILVER) {
                        process_quantum = 80; 
                    } 
                    int p =0;
                   
                    bool check_flag = false;
                    bool flag_1=false;
                    bool another_flag=false;
                    //these flags are used for determining if 10 ms context switch should be added to the current time
                    //for example if one process is turning into plat than i should break the loop without adding context switch time and 
                    //execute plat process above
                    while (!selected_process->completed && prior_process(processes_to_execute, line_count, (current_time), selected_process 
                    ) == -1 && (same_priority_count_change(processes_to_execute, line_count,current_time, selected_process)) > 1) {
                        //loop continues until there is a process with a higher priority to be executed, not finished and there are processes to 
                        //round robin between them
                        int total_time_spent=0;
                        current_time += selected_process->processes_instructions[selected_process->line_where_left].time;
                        selected_process->burst_count= selected_process->burst_count+ selected_process->processes_instructions[selected_process->line_where_left].time;
                        selected_process->processes_instructions[selected_process->line_where_left].is_complete=true;
                        selected_process->control = selected_process->control + selected_process->processes_instructions[selected_process->line_where_left].time;
                        //control is used for type transformation so i am adding the instruciton time to it
                        selected_process->line_where_left++;//line where left incremented
                        
                        p++;
                        //in this if statement we are cheking whether the process is done with its instructions totally
                        if (selected_process->line_where_left >= selected_process->num_instructions) {
                            selected_process->time_of_completion=current_time;
                            bool all_completed = true;
                            for (int i = 0; i < line_count; i++) {
                                if (!processes_to_execute[i].completed) {
                                    all_completed = false;
                                    break;
                                }
                            }
                            if (all_completed) {
                                break;
                            }
                            else{
                            current_time=current_time+10;  
                            }
                            
                            
                            
                            selected_process->completed = true;
                            //to update global varaibles i am calling these functions after completion
                            (same_priority_count_change(processes_to_execute, line_count,current_time, selected_process));
                            same_pr_index(process_index,processes_to_execute,line_count, current_time, selected_process);
                            
                            break;
                            
                            
                            
                        }
                         
                        if(selected_process->type_of_process == GOLD && selected_process->burst_count >600 ){
                            //this part checks type transformation
                            selected_process->type_of_process = PLATINUM;
                            selected_process->burst_count = 0;
                            check_flag=true;
                            flag_1= true;
                            break;
                        }
                        else if(selected_process->type_of_process == SILVER && selected_process->burst_count > 240){
                            //this part cheks type transformation
                            selected_process->type_of_process = GOLD;
                            selected_process->burst_count = 0;
                            check_flag=true;
                        }
                    
                        if(selected_process->control>=process_quantum){
                           //this part cheks whether a process exceeds its time quantum and break
                            another_flag=true;
                            selected_process->control = 0;
                            current_time=current_time+10;
                           
                            break;

                        }
                        
                    }
                    
                   
                     // Update the process index for Round Robin continuation
                     // Check for higher-priority processes
                    selected_process->out_CPU=current_time;
                    int next_process_index_f = prior_process(processes_to_execute, line_count, (current_time), selected_process);
                    
                    // If a higher-priority process is found, switch to that process
                    if(flag_1){
                       
                        selected_process=&processes_to_execute[process_index];
                       
                        
                    }
                    else{
                         if (next_process_index_f != -1) {
                        
                        if(!flag_1 && !another_flag){
                           current_time=current_time+10; 
                        }
                        
                        selected_process = &processes_to_execute[next_process_index_f];
                           
                    }
                    else{
                        int next_process_index=-1;
                        for(int h = 0; h< line_count; h++){
                            if(strcmp(selected_process->name, processes_to_execute[h].name)==0){
                                next_process_index=h;
                                break;
                            }
                        }
                        

                        //this part decides the next process to be executed in round robin
                        int rr = same_pr_index(next_process_index,processes_to_execute,line_count, current_time, selected_process);
                        if(rr != -1 && (same_priority_count_change(processes_to_execute, line_count,current_time, selected_process)) > 1){
                        selected_process = &processes_to_execute[rr]; 
                        }

                        }

                    }
                    
                } else {
                   //else part handles if there are no processes with the same priority
                   //its logic simpler
                    int next_process_index = -1;
                    bool flag = false;
                    int over_quantum=0;
                    // Execute the remaining portion of the process without preemption????
                    while (!selected_process->completed && prior_process(processes_to_execute, line_count, (current_time), selected_process) == -1)
                     {
                        current_time += selected_process->processes_instructions[selected_process->line_where_left].time;
                        selected_process->burst_count= selected_process->burst_count+ selected_process->processes_instructions[selected_process->line_where_left].time;
                        over_quantum = over_quantum + selected_process->processes_instructions[selected_process->line_where_left].time;
                        selected_process->line_where_left++;
                        if (selected_process->line_where_left >= selected_process->num_instructions) {
                            selected_process->completed = true;
                            (same_priority_count_change(processes_to_execute, line_count,current_time, selected_process));
                            same_pr_index(process_index,processes_to_execute,line_count, current_time, selected_process);
                            selected_process->time_of_completion=current_time;
                            bool all_completed = true;
                            for (int i = 0; i < line_count; i++) {
                                if (!processes_to_execute[i].completed) {
                                    all_completed = false;
                                    break;
                                }
                            }
                            if (all_completed) {
                                break;
                            }
                            else{
                                current_time=current_time+10;  
                            }
                           
                        }
                        
                        //Above i am cheking type transformations and if while executing the process there is a new process with the same priority
                        //then it turns into round robin
                        if(selected_process->type_of_process == GOLD && selected_process->burst_count > 600 ){
                            selected_process->type_of_process = PLATINUM;
                            selected_process->burst_count = 0;
                            flag = true;//platinum transformation happened then do not increase current time//do not context switch
                            break;
                    }
                        else if(selected_process->type_of_process == SILVER && selected_process->burst_count > 240){
                            selected_process->type_of_process = GOLD;
                            selected_process->burst_count = 0;
                            
                        }
                        //turning into round robin if more than one process are found with the same priority
                        if((same_priority_count_change(processes_to_execute, line_count,current_time, selected_process)) > 1){
                            if(selected_process->type_of_process == SILVER && over_quantum >=80){
                                current_time=current_time+10;
                                break;
                            }
                            else if(selected_process->type_of_process == GOLD && over_quantum >= 120){
                                current_time=current_time+10;
                                break;
                            }
                        }

                        
                    }
                    // Check for higher-priority processes
                    selected_process->out_CPU=current_time;
                    next_process_index = prior_process(processes_to_execute, line_count, (current_time), selected_process);
                    
                    // If a higher-priority process is found, switch to that process
                    if (next_process_index != -1) {
                        if(!flag){
                           current_time=current_time+10; 
                        }
                        
                        selected_process = &processes_to_execute[next_process_index];
                           
                    }
   
                }
            }  
        }
        else{
            //if there is no process ready in the current time than increase the time until there is a process is ready
            //to execute
            current_time++;
        }
        bool all_completed = true;
        for (int i = 0; i < line_count; i++) {
            if (!processes_to_execute[i].completed) {
                all_completed = false;
                break;
            }
        }
        if (all_completed) {
            break;
        }
        
    }
}


int main() {
    //calling necessary functions
    create_10_processes();//creating 10 process
    struct InstructionDictionary instructionDict;//instruction dictionary is initialized
    
    initializeDictionary(&instructionDict);
    
    //start to read files first instructions file
    FILE *file = fopen("instructions.txt", "r");
    if (file == NULL) {
        //printf("Error opening file.\n");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    
    while (fgets(line, sizeof(line), file) != NULL) {
        // Parsing line into instruction name and time using sscanf
        char *name;
        int time;
        //strtok is used to split the line
        name = strtok(line, " ");
        if(name==NULL){
            //error handling
            //printf("Invalid format in file.\n");
            return 1;
        }

        // Read the second part (time)
        char *timeStr = strtok(NULL, " ");
        if (timeStr == NULL) {
            //printf("Invalid format in file.\n");
            return 1;
        }

        time = atoi(timeStr);//converting the time (read as string) into int
        addToDictionary(&instructionDict, name, time);
    }
    fclose(file);//close instruction file
    //process files are read and process struct is created in this part of the code
    FILE *file1;
    char name_Of_file[10];
    for(int i = 0; i< FILE_NUM;i++){
        int count =0;
        int instruction_count=0;
        int burst_time_process = 0;
        sprintf(name_Of_file, "P%d.txt",i+1);
        file1 = fopen(name_Of_file,"r");
        if(file1 == NULL){
            return 1;
        }
        struct Process proc;

        char line[MAX_LINE_LENGTH];
        while (fgets(line, sizeof(line), file1) != NULL) {
            instruction_count++;
            if (count > 20) {
                break; // Exit the loop if the array limit is reached
            }

            if (line[strlen(line) - 1] == '\n') {
                line[strlen(line) - 1] = '\0'; // Remove the newline character
            }

            for (int k = 0; k < NUMBER_INST_MAX; k++) {
                if (strcmp(instructionDict.instructions[k].name, line) == 0) {
                    if (count < 21 && i < FILE_NUM) {
                        processes[i].processes_instructions[count] = instructionDict.instructions[k];
                        burst_time_process=burst_time_process+instructionDict.instructions[k].time;
                        count++; // Increment count
                    } else {
                        if (count >= 21) {
                        }
                        if (i > FILE_NUM) {
                        }
                        
                    }   
                }
            }
        processes[i].burst_time=burst_time_process;
        processes[i].num_instructions=instruction_count;
        }
        
    
    fclose(file1);

    }
    //definiton txt is read here
    struct Process processes_to_execute[FILE_NUM];
    int execute_count=0;
    FILE *file_2 = fopen("definition.txt", "r");
    if (file_2 == NULL) {
        //printf("Error opening file.\n");
        return 1;
    }
    int line_count=0;
    char line_definition[MAX_LINE_LENGTH];
    while (fgets(line_definition, sizeof(line_definition), file_2) != NULL) {
        // Parsing line into instruction name and time using sscanf
        line_count++;
        size_t length = strlen(line_definition);
        if (length > 0 && line_definition[length - 1] == '\n') {
            line_definition[length - 1] = '\0';
        }
        char *name;//splitting the line into name, priority, arrival time, and type of the process
        int priority;
        int arrival_time;
        enum Types type;
        
        
        //strtok is used to split the line
        name = strtok(line_definition, " ");
        char *priorityStr = strtok(NULL, " ");
        char *arrivalStr = strtok(NULL, " ");
        char *typeStr= strtok(NULL, " ");
        if (priorityStr == NULL || arrivalStr==NULL|| typeStr==NULL) {
            return 1;
        }

        priority = atoi(priorityStr);
        arrival_time= atoi(arrivalStr);
        
        
        
        for(int m = 0; m<FILE_NUM;m++){
            if(strcmp(processes[m].name,name)==0){
                type = getProcessType(typeStr, &processes[m]);
                numberOfProcesses++;
                processes[m].priority=priority;
                processes[m].arrival_time=arrival_time;
                processes[m].type_of_process=type;
                processes[m].into_CPU=arrival_time;
                
            }
            //process that are initalized are put into the process to execute array//later used in almost all parts of this project
            if(strcmp(processes[m].name,name)==0 && processes[m].name != NULL && processes[m].arrival_time!= -1){
                processes_to_execute[execute_count]=processes[m];
                execute_count++;
            }

        }
  
    }
    fclose(file_2);//close instruction file
    //in that part waiting and turnaround times are calculated.
    priority_scheduler(processes_to_execute,execute_count);
    int time_completion_total=0;
    int waiting_time=0;
    for(int k = 0; k<line_count;k++){
        time_completion_total= 10+time_completion_total+ processes_to_execute[k].time_of_completion - processes_to_execute[k].arrival_time;
        waiting_time=waiting_time-processes_to_execute[k].burst_time;
    }
    
    float average_waiting_time= ((float)(time_completion_total+waiting_time))/(line_count);
    float average_turnaround_time= ((float)time_completion_total)/(line_count);
    if (average_waiting_time == (int)average_waiting_time) {
        printf("%d\n", (int)average_waiting_time);  // If result is an integer, print without decimal part
    } else {
        printf("%.1f\n", average_waiting_time);  // If result is not an integer, print with one digit after the decimal point
    }
    if (average_turnaround_time == (int)average_turnaround_time) {
        printf("%d\n", (int)average_turnaround_time);  // If result is an integer, print without decimal part
    } else {
        printf("%.1f\n", average_turnaround_time);  // If result is not an integer, print with one digit after the decimal point
    }
    return 0;
}