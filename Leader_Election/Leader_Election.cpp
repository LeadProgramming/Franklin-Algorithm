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
	vector<int> datalist = randGenerator(len);

	MPI_Init(&argc, &argv);
	int rank;
	int size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	//token
	int control[2] = { 0, 0 };

	MPI_Status status;
	MPI_Request req;
	//consumer	
	if (rank == 0) {
		vector<int> selected;
		displayList(datalist);
		//raymond algorithm 
		MPI_Send(&control[0], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(&control[0], 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, &status);
		if (status.MPI_SOURCE == size - 1) {
			cout << control[0] << endl;
			for (int i = 0; i < control[0]; i++) {
				MPI_Recv(&control[1], 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
				cout << "Consumer Received: Rank = " << status.MPI_SOURCE << " : " << datalist[status.MPI_SOURCE - 1] << endl;
				selected.push_back(control[1]);
			}
			for (int i : selected) {
				cout << i << " ";
			}
		}

	}
	//producer
	else {

		control[1] = datalist[rank - 1];
		MPI_Status status;
		MPI_Request req;
		cout << "Rank: " << rank << endl;

		int left = rank - 1;
		int right = rank + 1;
		if (rank == 1) {
			left = size - 1;
			right = rank + 1;
		}
		else if (rank == size - 1) {
			left = 1;
			right = rank - 1;
		}
		MPI_Send(&datalist[rank - 1], 1, MPI_INT, left, 0, MPI_COMM_WORLD);
		MPI_Send(&datalist[rank - 1], 1, MPI_INT, right, 0, MPI_COMM_WORLD);
		MPI_Recv(&datalist[rank - 1], 1, MPI_INT, left, 0, MPI_COMM_WORLD, &status);
		if (datalist[rank - 1] > control[1]) {
			control[1] = datalist[rank - 1];
		}
		MPI_Recv(&datalist[rank - 1], 1, MPI_INT, right, 0, MPI_COMM_WORLD, &status);
		if (datalist[rank - 1] > control[1]) {
			control[1] = datalist[rank - 1];
		}
		MPI_Recv(&control[0], 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
		//we only want the heavy influencers.
		if (control[1] != datalist[rank - 1]) {
			cout << "Provider Sending: " << rank << " : " << control[1] << endl;
			MPI_Send(&control[1], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			control[0]++;
		}
	}
	if (rank == size - 1) {
		MPI_Send(&control[0], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	else {
		MPI_Send(&control[0], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
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