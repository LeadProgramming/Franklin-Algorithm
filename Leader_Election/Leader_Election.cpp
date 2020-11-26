// Leader_Election.cpp : This file contains the 'main' function. Program execution begins and ends there.
/*
Write a distributed program in MPI that simulates one of the
leader election algorithms learnt in class for a ring topology
using a fixed number of processors.
*/

// TO RUN MY CODE: navigate to debug folder and run mpiexec -n 5 Leader_Election.exe
#include <iostream>
#include <mpi.h>
#include <stdlib.h> /* rand */
#include <string> /* stoi */
#include <vector>
using namespace std;

vector<int> randGenerator(size_t size) {
	vector<int> result;
	for (size_t i = 0; i < size; i++) {
		int rando = rand() % 1000 + 1;
		result.push_back(rando);
	}
	return result;
}
void displayList(vector<int> x) {
	cout << "Process 0: ";
	for (int i : x) {
		cout << i << " ";
	}
	cout << endl;
}

int main(int argc, char* argv[])
{
	size_t len = stoi(argv[1]);
	vector<int> datalist;
	int tag = 0;
	MPI_Init(&argc, &argv);
	int rank;
	int size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//MPI_Bcast(&datalist[0], datalist.size(), MPI_INT, 0, MPI_COMM_WORLD);
	if (rank == 0) {
		datalist = randGenerator(len);
		tag = 0;
		displayList(datalist);
	}
	//initial start we send.
	//if (tag == 0) {
	//	MPI_Send(&datalist[0], datalist.size(), MPI_INT, size, size, MPI_COMM_WORLD);
	//	MPI_Send(&datalist[0], datalist.size(), MPI_INT, 2, 1, MPI_COMM_WORLD);
	//}
	//MPI_Send(&datalist[tag], datalist.size(), MPI_INT , tag + 1, tag + 1, MPI_COMM_WORLD);
	//MPI_Send(&datalist[tag], datalist.size(), MPI_INT , tag - 1, tag - 1, MPI_COMM_WORLD);
	//else {
	//	MPI_Recv();
	//	MPI_Recv();
	//}

	//if(rank != 0){
	//	if (rank == size+1) {
	//		MPI_Send(&datalist[0], datalist.size(), MPI_INT, tag, MPI_COMM_WORLD);
	//	}
	//	else {
	//		MPI_Send(&datalist[rank-1], datalist.size(), MPI_INT, tag, MPI_COMM_WORLD);
	//	}
	//}
	//shared resource we need mutex

	tag++;
	if (tag > size) {
		tag = 0;
	}
	MPI_Finalize();

	return 0;
}

/*
*	MPI Basic (Blocking) Send
	MPI_Send(msg, count, datatype, dest, tag, comm);
	MPI Basic (Blocking) Receive
	MPI_Recv(msg, count, datatype, source, tag, comm, status);
*/