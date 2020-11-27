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
	//token
	int count = 0;

	int rank;
	int size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Status status;
	MPI_Request req;
	//consumer	
	if (rank == 0) {
		vector<int> selected;
		displayList(datalist);

		for (int i = 0; i < count; i++) {
			MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			if (status.MPI_ERROR) {
				char error_str[512];
				int len;
				MPI_Error_string(status.MPI_ERROR, error_str, &len);
				cout << error_str << endl;
			}
			else {
				cout << "CONSUMER SOURCE RECEIVED: " << status.MPI_SOURCE << endl;
				MPI_Recv(&datalist[status.MPI_SOURCE - 1], 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, &status);
				count++;
				MPI_Wait(&req, &status);

				cout << "Consumer Received: Rank = " << status.MPI_SOURCE << " : " << datalist[status.MPI_SOURCE - 1] << endl;
				selected.push_back(datalist[status.MPI_SOURCE - 1]);
			}
		}

	}
	//producer
	else {
		MPI_Status status;
		cout << "Rank: " << rank << endl;
		int tmp = datalist[rank - 1];

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
		MPI_Isend(&datalist[rank - 1], 1, MPI_INT, left, 0, MPI_COMM_WORLD,&req);
		MPI_Isend(&datalist[rank - 1], 1, MPI_INT, right, 0, MPI_COMM_WORLD, &req);
		MPI_Wait(&req, &status);

		MPI_Irecv(&datalist[rank - 1], 1, MPI_INT, left, 0, MPI_COMM_WORLD, &req);
		if (datalist[rank - 1] > tmp) {
			tmp = datalist[rank - 1];
		}
		MPI_Irecv(&datalist[rank - 1], 1, MPI_INT, right, 0, MPI_COMM_WORLD, &req);

		if (datalist[rank - 1] > tmp) {
			tmp = datalist[rank - 1];
		}
		//we only want the heavy influencers.
		if (tmp != datalist[rank - 1]) {
			cout << "Provider Collected: " << rank << " : " << tmp << endl;
			MPI_Send(&tmp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
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