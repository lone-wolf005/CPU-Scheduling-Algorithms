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

           current_instant--;
    }
    fillInWaitTime();