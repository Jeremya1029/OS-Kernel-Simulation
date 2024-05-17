#include <iostream>
#include "SimOS.h"




// base + logical address = physical address
// logical / page size = page num
// logical % pagesize = offset
// page size = page num & offset
// page num -> page table -> frame num -> frame address + offset
//


    SimOS::SimOS(int numOfDisks, unsigned long long numRAM, unsigned int sizeOfPage)
    {
        // The parameters specify number of hard diskQueues in the simulated computer, amount of memory, and page size.
        // Disks, frame, and page enumerations all start from 0.
        numberOfDisks = numOfDisks;
        amountOfRAM = numRAM;
        pageSize = sizeOfPage;
        UpdatePID(NO_PROCESS);

        std::deque<FileReadRequest> diskQueue;
        FileReadRequest noRequest;
        for (int i = 0; i < numOfDisks; i++)
        {
            diskQueues.push_back(diskQueue);
            disks.push_back(noRequest);
            
        }
        

    }
    Process SimOS::CreateProcess(int pid, int parentPID){   
        pidCounter++;
        std::vector<int> children;
    Process newPro{
        pid,
        parentPID, 
        READY, 
        children
    };  
//TODO: FIX
        processVector.push_back(newPro);

        return newPro;
    }
    void SimOS::IdleCPUCheck(){
        if(GetCPU() == NO_PROCESS)
        {
        UpdatePID(GetReadyQueue().front());  //add to cpu
        GetReadyQueue().pop_front();    //remove from queue
        processVector[GetCPU()].state = RUNNING; // set running state
        }
    }

    void SimOS::NewProcess()
    {
        //  Creates a new process in the simulated system. The new process takes place in the ready-queue or immediately starts using the CPU.
        //  Every process in the simulated system has a PID. Your simulation assigns PIDs to new processes starting from 1 and increments it
        //  by one for each new process. Do not reuse PIDs of the terminated processes.
        Process newProcess = CreateProcess(pidCounter, NO_PROCESS);
        
        GetReadyQueue().push_back(newProcess.PID);
        

        IdleCPUCheck();
    }

    void SimOS::SimFork()
    {
        // TODO
        //  The currently running process forks a child. The child is placed in the end of the ready-queue.
        int currentPID = GetCPU();

        Process childProcess = CreateProcess(pidCounter, currentPID);
        processVector[currentPID].children.push_back(childProcess.PID);

        processVector[childProcess.PID] = childProcess;
        GetReadyQueue().push_back(childProcess.PID);
    }
    void SimOS::SimExit()
    {
        // TODO
        //  The process that is currently using the CPU terminates. 
        //  Make sure you release the memory used by this process immediately.
        //  If its parent is already waiting, the process terminates immediately and the parent becomes runnable (goes to the ready-queue).
        //  If its parent hasn't called wait yet, the process turns into zombie.
        //  To avoid the appearance of the orphans, the system implements the cascading termination.
        //  Cascading termination means that if a process terminates, all its descendants terminate with it.
        int currentProcess = GetCPU();
        if(processVector[currentProcess].parentPID != NO_PROCESS)
        {
            int parentID = processVector[currentProcess].parentPID;
            if(processVector[parentID].state == WAITING)
            {
                GetReadyQueue().push_back(parentID);
                processVector[parentID].state = READY;
            }
            else{
                processVector[currentProcess].state = ZOMBIE;
            }   
        }


        if(processVector[currentProcess].children.size() != 0)
        {
        while (processVector[currentProcess].children.size() != 0)
        {
            int childPID = processVector[currentProcess].children.back();
            processVector[childPID].state = ZOMBIE;
            processVector[currentProcess].children.pop_back();
            processVector.erase(processVector.begin() + childPID);  
        }
        }
            
        



        for(unsigned int i = 0; i < memory.size(); i++)
        {
            if(memory[i].PID == currentProcess)
            {
                memory.erase(memory.begin() + i);
            }
        }

    }
    void SimOS::SimWait()
    {
        // TODO
        //  The process wants to pause and wait for any of its child processes to terminate.
        //  Once the wait is over, the process goes to the end of the ready-queue or the CPU. If the zombie-child already exists,
        //  the process proceeds right away (keeps using the CPU) and the zombie-child disappears. If more than one zombie-child exists,
        //  the system uses one of them (any!) to immediately resumes the parent, while other zombies keep waiting for the next wait from the parent.
        int currentProcess = GetCPU();
        processVector[currentProcess].state = WAITING;
        int amtOfChildren = processVector[currentProcess].children.size();
        
        if(amtOfChildren != 0){
        for(int i = 0; i < amtOfChildren; i++)
        {           
            int childPID = processVector[currentProcess].children[i];

            if(processVector[childPID].state == ZOMBIE)
            {
                processVector[currentProcess].state = READY;
                processVector[currentProcess].children.erase(processVector[currentProcess].children.begin() + i);
                processVector.erase(processVector.begin() + childPID);
                 
                return; // continue running program by skipping pop below
            }
        }
        }

        if(GetReadyQueue().front())
        {
        UpdatePID(GetReadyQueue().front()); 
        GetReadyQueue().pop_front();
        processVector[GetCPU()].state = RUNNING;
        }
    }
    void SimOS::TimerInterrupt()
    {
        // DONE: Interrupt arrives from the timer signaling that the time slice of the currently running process is over.
        if (GetCPU() != NO_PROCESS)
        {
            GetReadyQueue().push_back(GetCPU());
            processVector[GetCPU()].state = READY;
            IdleCPUCheck();
        }
    }
    void SimOS::DiskReadRequest(int diskNumber, std::string fileName)
    {
       
        if(GetCPU() == NO_PROCESS)
            return;

        FileReadRequest processRequest{GetCPU(), fileName};  //created read request

        int currentProcess = GetCPU();
        int nextProcess = NO_PROCESS;
        if(!GetReadyQueue().empty()){ //check if ready queue is empty to pop front and add next in line to cpu
            nextProcess = GetReadyQueue().front();
            GetReadyQueue().pop_front();
        }
        
        UpdatePID(nextProcess);
        processVector[currentProcess].state = WAITING;

        if(disks[diskNumber].PID == NO_PROCESS)
        {
            disks[diskNumber].PID = processRequest.PID;
            disks[diskNumber].fileName = processRequest.fileName;
        }else{
            diskQueues[diskNumber].emplace_back(processRequest);
        }

        
        
        //GetDiskQueue(diskNumber).push_back(processRequest);//add read request to the disk queue
        //  Currently running process requests to read the specified file from the disk with a given number.
        //  The process issuing disk reading requests immediately stops using the CPU, even if the ready-queue is empty.
    }
    void SimOS::DiskJobCompleted(int diskNumber)  
    {
        // DONE: A disk with a specified number reports that a single job is completed. The served process should return to the ready-queue.
        int finishedPID = disks[diskNumber].PID;
        processVector[finishedPID].state = READY;
        GetReadyQueue().push_back(processVector[finishedPID].PID);   //returned served process      
        IdleCPUCheck();   

        FileReadRequest newRequest;

        if(GetDiskQueue(diskNumber).size() > 0)
        {
            newRequest = GetDiskQueue(diskNumber).front();
            GetDiskQueue(diskNumber).pop_front();
        }
        disks[diskNumber] = newRequest;

    }
    void SimOS::AccessMemoryAddress(unsigned long long address)
    {
        // TODO

        //  Currently running process wants to access the specified logical memory address.
        //  System makes sure the corresponding page is loaded in the RAM.
        //  If the corresponding page is already in the RAM, its “recently used” information is updated.

        MemoryItem newItem;
        newItem.PID = GetCPU();
        newItem.pageNumber = address / pageSize;
        memory.push_back(newItem); 

    }
    int SimOS::GetCPU()
    {
        // TODO: GetCPU returns the PID of the process currently using the CPU. If CPU is idle it returns NO_PROCESS (see the supplied definitions above).

        if (cpu.PID == NO_PROCESS)
            return NO_PROCESS;
        else
            return cpu.PID;
    }
    std::deque<int> SimOS::GetReadyQueue()
    {
        // DONE: GetReadyQueue returns the std::deque with PIDs of processes in the ready-queue where element in front corresponds start of the ready-queue.

        return readyQueue;
    }

    MemoryUsage SimOS::GetMemory()
    {
        // GetMemory returns MemoryUsage vector describing all currently used frames of RAM. Remember,
        // Terminated “zombie” processes don’t use memory, so they don’t contribute to memory usage.
        // MemoryItems appear in the MemoryUsage vector in the order they appear in memory (from low addresses to high).
        

        return memory;
    }
    FileReadRequest SimOS::GetDisk(int diskNumber)
    {
        /*TODO: GetDisk returns an object with PID of the process served by specified disk and the name of the file read for that process.
        If the disk is idle, GetDisk returns the default FileReadRequest object (with PID 0 and empty string in fileName) */
        FileReadRequest defaultFileReadReq;
        // TODO: If a disk with the requested number doesn’t exist throw std::out_of_range exception.
    
            
            if (diskNumber <= -1 || diskNumber > numberOfDisks)
                throw std::out_of_range("disk with the requested number doesn’t exist"); // If a disk with the requested number doesn’t exist throw std::out_of_range exception.

            if(disks[diskNumber].PID == NO_PROCESS) // If the disk is idle, GetDisk returns the default FileReadRequest object (with PID 0 and empty string in fileName)
                return defaultFileReadReq;
                   //std::cout << "disk file req "<< diskNumber << " fileName = " <<  GetDiskQueue(diskNumber)[0].fileName << std::endl;
        return disks[diskNumber]; 
            
    }

    std::deque<FileReadRequest> SimOS::GetDiskQueue(int diskNumber)
    {
        // DONE: GetDiskQueue returns the I/O-queue of the specified disk starting from the “next to be served” process.
        //std::cout << "disk "<< diskNumber << " size = " << diskQueues[diskNumber].size() << std::endl;
        return diskQueues[diskNumber];
    
    }

void SimOS::UpdatePID(int pid){
    cpu.PID = pid;
}