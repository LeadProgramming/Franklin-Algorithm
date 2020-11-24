// Leader_Election.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <mpi.h>
using namespace std;
int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    cout << "Hello World!" << endl;
    MPI_Finalize();
    return 0;
}

