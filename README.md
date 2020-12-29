# Franklin Algorithm - Leader Election 
Please visit my project report for more details! 

![mpiexec.gif](https://github.com/LeadProgramming/Lamport-Clock/blob/main/mpiexec.gif?raw=true)

## Summary

A decentralized algorithm that elects the leader based on whichever initiator (process) has the highest ID. Franklin Algorithm uses the ring topology. All initiators are active on the first round, they will transmit and receive values from their left neighbor and right neighbor. Then, the initiator will compare the values to see if it should stay passive or active. If the initiator has the highest value amongst its neighbors then it will stay active and the neighbors become passive. If the initiator has a neighbor who has the highest value then the initiator becomes passive. Instead of making the passive nodes pass on the message to the initiators, I decided to make them idle and have the initiators send the messages to each other. I turned this algorithm into a decrease-and-conquer style algorithm to optimize speed and memory.

## Installation

**Important**: due to the lack of hardware resources I am unable to get Linux to run smoothly on a virtual machine and I am stuck with Windows. Unfortunately, MPI on Cygwin is very buggy and I had to resort to Microsoft MPI. Sorry if you only use Linux.

### Window

Please read the Windows 10 MPI installation guide from *MPI_VisualStudio_Windows10_tutorial.pdf*

This will be convenient in terms of setting up your MPI environment for Windows.

After reading the tutorial, you can easily compile and run the project. 

To run my project:

1. Navigate into the Leader_Election folder.

2. Navigate into the Debug folder. 

3. Open up a CLI and run this command to execute my program.

```
mpiexec -n 6 ./Leader_Election.exe 5
```

**Note**: 6 can be replaced by any # of processes you want. Just add an extra process for the root process. The 5 is actually the # of processes you want to randomly generate without the root process.  

## Reference

I used my professor lectures to aid me in building this project.

https://mpitutorial.com/