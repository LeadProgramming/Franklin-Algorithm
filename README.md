# 474-Project-2
Project 2 for CPSC 474.01

Group members:

David Nguyen vietcloudx@csu.fullerton.edu

*This project has a different folder structure compared to my first project. I used Visual Studio Community.

To run my file:

1. Navigate into the Leader_Election folder.

2. Navigate into the Debug folder. 

3. Open up a CLI and run this command to execute my program.

mpiexec -n 6 ./Leader_Election.exe 5

*Note: 6 can be replaced by any # of processes you want. Just add an extra process for the root process. The 5 is actually the # of processes you want to randomly generate without the root process.  