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
	//MPI_Request* initialSend = new MPI_Request[size*2];
	int count = 0;
	int tmp = 0;
	//consumer	
	if (rank == 0) {
		displayList(datalist);
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Barrier(MPI_COMM_WORLD);
		//raymond algorithm 
		MPI_Ssend(&count, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Recv(&count, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, &status);
		MPI_Barrier(MPI_COMM_WORLD);
		if (status.MPI_SOURCE == size - 1) {
			cout << count << endl;
			vector<int> tmpList;
			for (int i = 0; i < count; i++) {
				MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
				cout << tmp << " ";
				tmpList.push_back(tmp);
			}
			cout << endl;
			if (tmpList.size() == 1) {
				cout << "Our leader: " << tmpList.front() << endl;
			}
			else {
				datalist = tmpList;
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
	//producer
	else {
		tmp = datalist[rank - 1];
		cout << "Rank: " << rank << endl;
		vector<int> comp;
		comp.push_back(datalist[rank - 1]);
		MPI_Barrier(MPI_COMM_WORLD);
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
		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Irecv(&datalist[rank - 1], 1, MPI_INT, left, 0, MPI_COMM_WORLD, &req);
		comp.push_back(datalist[rank - 1]);
		MPI_Wait(&req, &status);
		MPI_Irecv(&datalist[rank - 1], 1, MPI_INT, right, 0, MPI_COMM_WORLD, &req);
		comp.push_back(datalist[rank - 1]);
		MPI_Wait(&req, &status);
		MPI_Barrier(MPI_COMM_WORLD);
		//we only want the heavy influencers.
		MPI_Recv(&count, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
		if (comp[0] > comp[1] && comp[0] > comp[2]) {
			count++;
		}
		if (rank == size - 1) {
			MPI_Ssend(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
		else {
			MPI_Ssend(&count, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
		}
		MPI_Barrier(MPI_COMM_WORLD);
		cout << "Rank: " << rank << " ";
		for (int i : comp) {
			cout << i << " ";
		}
		cout << endl;
		if (comp[0] > comp[1] && comp[0] > comp[2]) {
			tmp = comp[0];
			cout << "Provider Sending: " << rank << " : " << tmp << endl;
			MPI_Send(&tmp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
	//delete[] initialSend;

	MPI_Finalize();
	return 0;
}

/*
*	MPI Basic (Blocking) Send
	MPI_Send(msg, count, datatype, dest, tag, comm);
	MPI Basic (Blocking) Receive
	MPI_Recv(msg, count, datatype, source, tag, comm, status);
*/