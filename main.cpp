#include<bits/stdc++.h>
// #include<iostream.h>
#include "parser.h"
using namespace std;

#define all(v) v.begin(), v.end()

const string TRACE = "trace";
const string SHOW_STATISTICS = "stats";
const string ALGORITHMS[9] = {"", "FCFS", "RR-", "SPN", "SRT", "HRRN", "FB-1", "FB-2i", "AGING"};



bool sortByArrivalTime(const tuple < string , int , int > &a ,const tuple <string , int , int > &b){
    return (get<1>(a) < get<1>(b));
}              

bool sortByServiceTime(const tuple < string ,int ,int > &a,const tuple < string , int ,int > &b){
    return (get<2>(a) < get<2>(b));
}

bool sortDecending_by_response_ratio(const tuple < string , double , int >&a , const tuple <string, double , int > &b){

    return (get<1>(a) > get<1>(b));         
}

bool sortDecending_by_priorityLevel(const tuple <int ,int ,int > &a ,const tuple <int ,int ,int >&b){

    if(get<0>(a) == get<0>(b))
    {
        return get<2>(a) > get<2>(b);
    }
        return get<0>(a) > get<0>(b);
}

void clear_timeline(){

    for(int i = 0; i < last_instant ; i++)
        {
            for(int j = 0; j < process_count; j++)
                {
                        timeline[i][j] = ' ';
                }
        }
}

string getProcessName(tuple< string, int, int> &a)
{
    return get<0>(a);
}

int getArrivalTime(tuple< string, int, int > &a)
{
    return get<1>(a);
}

int getServiceTime(tuple<string, int, int> &a)
{
    return get<2>(a);
}

int getPriorityLevel(tuple<string, int, int> &a)
{
    return get<2>(a);
}

double calculate_response_ratio(int wait_time , int service_time){
    
    double response_ratio;
     
    response_ratio = (double)(wait_time + service_time) / (1.0*service_time);
    
    return response_ratio;

}

void fillInWaitTime(){
    for (int i = 0; i < process_count; i++)
    {
        int arrivalTime = getArrivalTime(processes[i]);
        for (int k = arrivalTime; k < finishTime[i]; k++)
        {
            if (timeline[k][i] != '*')
                timeline[k][i] = '.';
        }
    }
}

void firstComeFirstServe(){

    int startTime = getArrivalTime(processes[0]);

    for(int i = 0 ; i < processes.size(); i++){

        int processIndex = i;
        int arrivalTime = getArrivalTime(processes[processIndex]);
        int serviceTime = getServiceTime(processes[processIndex]);

        finishTime[processIndex] = (startTime + serviceTime);
        turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
        normTurn[processIndex]  = (turnAroundTime[processIndex] * 1.0 / serviceTime);

        for(int j = startTime; j < finishTime[processIndex]; j++)
        {
            timeline[j][processIndex] = '*';
        }

        for(int j = arrivalTime; j<startTime; j++)
        {
            timeline[j][processIndex] = '.';
        }

        startTime+=serviceTime;

    }
}

void roundRobin(int originalQuantum){

    int j = 0;  
    // used to for count the number of process so we will able to know that certain process are yet to come
    
    queue<pair<int,int>>processQueue; 

    if(getArrivalTime(processes[j])==0){
        processQueue.push({j,getServiceTime(processes[j])});
        j++;
    }

    int currentQuantum = originalQuantum;

    for(int current_instant = 0; current_instant < last_instant;current_instant++){
        if(!processQueue.empty()){

            int processIndex = processQueue.front().first;
            processQueue.front().second = processQueue.front().second-1;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            int remainingServiceTime =(processQueue.front().second);
            currentQuantum--;
            timeline[current_instant][processIndex]='*';

            // if new process is arrived then push into the queue before adding current process{if remaining} in queue

            while(j<process_count && getArrivalTime(processes[j])==current_instant+1){
                processQueue.push({j,getServiceTime(processes[j])});
                j++;
            }
        
        //checking the condition after completing a single time unit

        //    1.both quantum and process is finished
        //    2.quantum is finished but process is remaining
        //    3.quantum is not finished but process is finished

         if(currentQuantum==0 && remainingServiceTime==0)
           {
                finishTime[processIndex]=current_instant+1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
                currentQuantum=originalQuantum;
                processQueue.pop();
           }
         else if(currentQuantum==0 && remainingServiceTime!=0)
            {
                processQueue.pop();
                processQueue.push({processIndex,remainingServiceTime});
                currentQuantum=originalQuantum;
            }
        else if(currentQuantum!=0 && remainingServiceTime==0)
            {
                finishTime[processIndex]=current_instant+1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
                processQueue.pop();
                currentQuantum=originalQuantum;
            }
        }

        while(j<process_count && getArrivalTime(processes[j])==current_instant+1)
        {
            processQueue.push({j,getServiceTime(processes[j])});
            j++;
        }   

    }

    fillInWaitTime();
}

void shortestProcessNext()
{
    // stores the {serviceTime,index}  sorted based on increasing order of serviceTime
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> processQueue; // pair of service time and index

    // process Index
    int j = 0;
    for (int current_instant = 0; current_instant < last_instant; current_instant++)
    {   
        // pushing the process in queue
        while(j<process_count && getArrivalTime(processes[j]) <= current_instant){
            processQueue.push({getServiceTime(processes[j]), j});
            j++;
        }
        if (!processQueue.empty())
        {   
            int processIndex = processQueue.top().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            processQueue.pop();

            // mark all the values from arrival_time to current time as waiting time

            int temp = arrivalTime;
            for (;temp < current_instant; temp++){
                timeline[temp][processIndex] = '.';
            }

            // from current instant to service time mark the process execution

            temp = current_instant;
            for (; temp < current_instant + serviceTime; temp++)
                timeline[temp][processIndex] = '*';

            // calculating the stats of process
            
            finishTime[processIndex] = (current_instant + serviceTime);
            turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
            normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);

            // need to some debugging test for this condition

            current_instant = temp - 1;
        }
    }
}

void shortestRemainingTime(){
     
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> processQueue; // pair of service time and index

    int j = 0;

    for(int current_instant = 0; current_instant< last_instant; current_instant++){

        while(j < process_count && getArrivalTime(processes[j]) == current_instant){
            processQueue.push({getServiceTime(processes[j]),j});
            j++;
        }
        if(!processQueue.empty()){
        
        int processIndex = processQueue.top().second;
        int remainingTime = processQueue.top().first;
        processQueue.pop();
        int arrivalTime = getArrivalTime(processes[processIndex]);
        int serviceTime = getServiceTime(processes[processIndex]);

        timeline[current_instant][processIndex] = '*';
         
        // process will be completed here
        if(remainingTime == 1){

            finishTime[processIndex] = current_instant + 1;
            turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
            normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
            
        }
        else{
            processQueue.push({remainingTime-1,processIndex});
        }

    }
    }
    fillInWaitTime();
}

void highestResponseRatioNext(){

    vector<tuple<string,double,int>>present_processes;
    // stores {processname,responseratio,time in service}
    int j = 0;

    for(int current_instant = 0; current_instant < last_instant; current_instant++){

        // pushing the all the processes in vector till current Instant
        
        while(j < process_count && getArrivalTime(processes[j])<=current_instant){
            present_processes.push_back(make_tuple(getProcessName(processes[j]),1.0,0));
            j++;
        }

        // finding out the Response Ratio of each Process present in vector

        for( auto&process : present_processes ){
            string process_name = get<0>(process);
            int process_index = processToIndex[process_name];
            int wait_time = current_instant - getArrivalTime(processes[process_index]);
            int service_time = getServiceTime(processes[process_index]);
            get<1>(process) = calculate_response_ratio(wait_time, service_time);
        }

        // sort all process by decreasing Order of Response Ratio

        sort(all(present_processes),sortDecending_by_response_ratio);

        // serve the process 

        if(!present_processes.empty()){

            int process_index = processToIndex[get<0>(present_processes[0])];
            int serviceTime = getServiceTime(processes[process_index]);
            while(current_instant < last_instant && get<2>(present_processes[0]) != serviceTime){
                timeline[current_instant][process_index]='*';
                current_instant++;
                get<2>(present_processes[0])++;
            }
            current_instant--;
            present_processes.erase(present_processes.begin());

        //  calculating and storing the statistics of process

            finishTime[process_index] = current_instant + 1;
            turnAroundTime[process_index] = finishTime[process_index] - getArrivalTime(processes[process_index]);
            normTurn[process_index] = (turnAroundTime[process_index] * 1.0 / getServiceTime(processes[process_index]));
        }

        }

        fillInWaitTime(); 
    }

void feedbackQ1(){

    priority_queue<pair<int,int> , vector<pair<int,int>>, greater<pair<int, int>>>processQueue;
    // used to store the pair {priorityLevel,processIndex}

    unordered_map<int,int>remainingServiceTime;
    // used to store the {key-value} => {processIndex,remainingServiceTime}
    int j = 0;
    if(j < process_count && getArrivalTime(processes[j])==0){
        processQueue.push({0,j});
        remainingServiceTime[j] = getServiceTime(processes[j]);
        j++;
    }

    for(int current_instant = 0 ; current_instant < last_instant ; current_instant++){
        if(!processQueue.empty()){
            int processIndex = processQueue.top().second;
            int priorityLevel = processQueue.top().first;
            remainingServiceTime[processIndex]--;
            timeline[current_instant][processIndex]='*';
            processQueue.pop();

            // if new Process is arrived then pushed into priority queue

            while(j<process_count && getArrivalTime(processes[j])==current_instant+1){
                processQueue.push({0,j});
                remainingServiceTime[j]=getServiceTime(processes[j]);
                j++;
            }

            // if process is completed then calcutate statistics and store it 
            
            if(remainingServiceTime[processIndex]==0){
                finishTime[processIndex] = current_instant + 1;
                turnAroundTime[processIndex] = finishTime[processIndex] - getArrivalTime(processes[processIndex]);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / getServiceTime(processes[processIndex]));
            }
            else if(processQueue.size()>=1){
                // if more than 1 processes are present then update prirityLevel

                processQueue.push({priorityLevel + 1,processIndex});
            }
            else{
                // if only 1 process is availble then there no need to update the priorty level

                processQueue.push({priorityLevel,processIndex});
            }   
        }
        //  pushing process if pq is empty at any instant of time
         while(j<process_count && getArrivalTime(processes[j])==current_instant+1){
            processQueue.push(make_pair(0,j));
            remainingServiceTime[j]=getServiceTime(processes[j]);
            j++;
        }

    }
    fillInWaitTime();
}

void feedbackQ2i(){
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>>processQueue;
    unordered_map<int,int>remainingServiceTime;
    
    int j = 0;

    if(j < process_count && getArrivalTime(processes[j])==0){
        processQueue.push({0,j});
        remainingServiceTime[j]=getServiceTime(processes[j]);
        j++;
    }

    for(int current_instant = 0; current_instant < last_instant ; current_instant++){

        if(!processQueue.empty()){
            int processIndex = processQueue.top().second;
            int priorityLevel = processQueue.top().first;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            processQueue.pop();

            while(j < process_count && getArrivalTime(processes[j]) <= current_instant+1){
                processQueue.push({0,j});
                remainingServiceTime[j]=getServiceTime(processes[j]);
                j++;    
            }

            int currentQuantum = pow(2,priorityLevel);
            int temp = current_instant;

            while(currentQuantum && remainingServiceTime[processIndex]){
                currentQuantum--;
                remainingServiceTime[processIndex]--;
                timeline[temp++][processIndex]='*';
            }

            if(remainingServiceTime[processIndex]==0){
                finishTime[processIndex]=temp;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
            }else{
                if(processQueue.size() >= 1)
                    processQueue.push({priorityLevel+1,processIndex});
                else
                    processQueue.push({priorityLevel,processIndex});
            }
            current_instant = temp-1;
        }
        
        while(j<process_count && getArrivalTime(processes[j]) <= current_instant+1)
        {
            processQueue.push({0,j});
            remainingServiceTime[j]=getServiceTime(processes[j]);
            j++;
        }

    }
    fillInWaitTime();

}

void aging(int originalQuantum){

    vector<tuple<int,int,int>>present_processes;
    // stores(priorityLevel,processInde,waiting time)
    int j = 0;
    int currentProcess = -1;
    for(int current_instant = 0; current_instant < last_instant; current_instant++){
        
        // push the process till current_instant

          while(j<process_count &&  getArrivalTime(processes[j]) <= current_instant){
               present_processes.push_back(make_tuple(getPriorityLevel(processes[j]),j,0));
               j++;
          }
        //   make executed process with intial configuration
        //   increase PriorityLevel and Waitingtime for all other Processes(aging)

            for(int i = 0; i < present_processes.size(); i++){
                if(get<1>(present_processes[i]) == currentProcess){
                    get<2>(present_processes[i]) = 0;
                    get<0>(present_processes[i]) = getPriorityLevel(processes[currentProcess]);
                }
                else{
                    get<0>(present_processes[i])++;
                    get<2>(present_processes[i])++;
                }
            }

            sort(all(present_processes),sortDecending_by_priorityLevel);

            currentProcess=get<1>(present_processes[0]);
            int currentQuantum = originalQuantum;

                while(currentQuantum-- && current_instant < last_instant){
                    timeline[current_instant][currentProcess]='*';
                    current_instant++;
                }
           finishTime[currentProcess]=current_instant;
           current_instant--;
    }

    fillInWaitTime();
}
void printAlgorithm(int algorithm_index)
{
    int algorithm_id = algorithms[algorithm_index].first - '0';

    // if RR then Print Quantum also
    if(algorithm_id==2)
        cout << ALGORITHMS[algorithm_id]<<algorithms[algorithm_index].second<<"  " <<endl;
    else
        cout << ALGORITHMS[algorithm_id]<<"  "<< endl;
}
void printProcesses()
{
    cout << "Process    ";
    for (int i = 0; i < process_count; i++)
        cout << "|  " << getProcessName(processes[i]) << "  ";
    cout << "|\n";
}

void printArrivalTime()
{
    cout << "Arrival    ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ",getArrivalTime(processes[i]));
    cout<<"|\n";
}

void printServiceTime()
{
    cout << "Service    ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ",getServiceTime(processes[i]));
    cout << "| Mean |\n";
}

void printFinishTime()
{
    cout << "Finish     ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ",finishTime[i]);
    cout << "| ---- |\n";
}


void printTurnAroundTime()
{
    cout << "Turnaround ";
    int sum = 0;
    for (int i = 0; i < process_count; i++)
    {
        printf("|%3d  ",turnAroundTime[i]);
        sum += turnAroundTime[i];
    }
    if((1.0 * sum / turnAroundTime.size())>=10)
		printf("| %2.2f|\n",(1.0 * sum / turnAroundTime.size()));
    else
	 	printf("|  %2.2f|\n",(1.0 * sum / turnAroundTime.size()));
}

void printNormTurn()
{
    cout << "NormTurn   |";
    float sum = 0;
    for (int i = 0; i < process_count; i++)
    {
        if( normTurn[i]>=10 )
            printf("%2.2f|",normTurn[i]);
        else
            printf(" %2.2f|",normTurn[i]);
        sum += normTurn[i];
    }

    if( (1.0 * sum / normTurn.size()) >=10 )
        printf(" %2.2f|\n",(1.0 * sum / normTurn.size()));
	else
        printf("  %2.2f|\n",(1.0 * sum / normTurn.size()));
}


void printStats(int algorithm_index)
{
    printAlgorithm(algorithm_index);
    printProcesses();
    printArrivalTime();
    printServiceTime();
    printFinishTime();
    printTurnAroundTime();
    printNormTurn();
}

void printTimeline(int algorithm_index)
{
    for (int i = 0; i <= last_instant; i++)
        cout << i % 10<<" ";
    cout <<"\n";
    cout << "------------------------------------------------\n";
    for (int i = 0; i < process_count; i++)
    {
        cout << getProcessName(processes[i]) << "     |";
        for (int j = 0; j < last_instant; j++)
        {
            cout << timeline[j][i]<<"|";
        }
        cout << " \n";
    }
    cout << "------------------------------------------------\n";
}

void execute_algorithm(char algorithm_id , int quantum, string operation){

    switch (algorithm_id)
    {
    case '1':
        if(operation==TRACE)cout<<"FCFS  ";
        firstComeFirstServe();
        break;
    case '2':
        if(operation==TRACE)cout<<"RR-"<<quantum<<"  ";
        roundRobin(quantum);
        break;
    case '3':
        if(operation==TRACE)cout<<"SPN   ";
        shortestProcessNext();
        break;
    case '4':
        if(operation==TRACE)cout<<"SRT   ";
        shortestRemainingTime();
        break;
    case '5':
        if(operation==TRACE)cout<<"HRRN  ";
        highestResponseRatioNext();
        break;
    case '6':
        if(operation==TRACE)cout<<"FB-1  ";
        feedbackQ1();
        break;
    case '7':
        if(operation==TRACE)cout<<"FB-2i ";
        feedbackQ2i();
        break;
    case '8':
        if(operation==TRACE)cout<<"Aging ";
        aging(quantum);
        break;
    default:
        break;
    
        }
    }

int main(){
    parse();
    for(int i = 0; i < algorithms.size(); i++)
    {

        // clearTimeline vector before Executing
        clear_timeline();

        // extract algorithm id and quantum {if possible}
        execute_algorithm(algorithms[i].first,algorithms[i].second,operation);

       // print the tracing or statistics of algorithm
        if(operation == TRACE){
             printTimeline(i);
        }
        else if(operation == SHOW_STATISTICS){
            printStats(i);
        }
        cout<<endl;
    }
     
    return 0;
}