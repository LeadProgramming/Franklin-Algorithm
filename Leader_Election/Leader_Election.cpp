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
	srand(time(NULL));
	size_t len = stoi(argv[1]);

	vector<int> datalist = randGenerator(len);

	MPI_Init(&argc, &argv);
	int rank;
	int size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	MPI_Status status;
	MPI_Request req;

	vector<int> maybes;

	//consumer	
	if (rank == 0) {
		displayList(datalist);
		MPI_Barrier(MPI_COMM_WORLD);
		//raymond algorithm 
		MPI_Recv(&maybes[0], 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, &status);
		if (status.MPI_SOURCE == size - 1) {
			cout << maybes.size() << endl;
			for (int i = 0; i < maybes.size(); i++) {
				cout << "Consumer Received: Rank = " << status.MPI_SOURCE << " : " << maybes[i] << endl;
			}
		}

	}
	//producer
	else {
		int tmp = datalist[rank - 1];
		MPI_Status status;
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
		MPI_Irecv(&datalist[rank - 1], 1, MPI_INT, left, 0, MPI_COMM_WORLD, &req);
		if (datalist[rank - 1] > tmp) {
			tmp = datalist[rank - 1];
		}
		MPI_Wait(&req, &status);

		MPI_Send(&datalist[rank - 1], 1, MPI_INT, right, 0, MPI_COMM_WORLD);
		MPI_Irecv(&datalist[rank - 1], 1, MPI_INT, right, 0, MPI_COMM_WORLD, &req);
		if (datalist[rank - 1] > tmp) {
			tmp = datalist[rank - 1];
		}
		MPI_Wait(&req, &status);
		MPI_Barrier(MPI_COMM_WORLD);
		//we only want the heavy influencers.
		if (maybes.size() > 0) {
			MPI_Recv(&maybes[0], maybes.size(), MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
		}
		if (tmp == datalist[rank - 1]) {
			cout << "Provider Sending: " << rank << " : " << tmp << endl;
			maybes.push_back(tmp);
			for (int i : maybes) {
				cout << i << " ";
			}
			cout << endl;
		}
		if (maybes.size() > 0) {
			if (rank == size - 1) {
				MPI_Ssend(&maybes[0], maybes.size(), MPI_INT, 0, 0, MPI_COMM_WORLD);
			}
			else {
				MPI_Ssend(&maybes[0], maybes.size(), MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
			}
		}

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