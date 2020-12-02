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
#include "./CPSC-131-Data-Structures-code/CircularDummyNodeDLinkedList.hxx";
#include <iterator>
using namespace std;

struct Process {
	int index;
	int value;
};
DLinkedList<Process> randGenerator(size_t size) {
	DLinkedList<Process> generated;
	for (size_t i = 0; i < size; i++) {
		Process elem;
		elem.index = i;
		elem.value = rand() % 1000 + 1;
		generated.append(elem);
	}
	return generated;
}
void displayList(DLinkedList<Process> x) {
	cout << "Process 0: ";
	
	for (DLinkedList<Process>::Iterator i = x.begin(); i != x.end();i++) {
		cout << i->value << " ";
	}
	cout << endl;
}
int main(int argc, char* argv[])
{
	srand(time(NULL));
	size_t len = stoi(argv[1]);

	MPI_Init(&argc, &argv);
	MPI_Datatype node_type;
	int lengths[2] = { 1, 1 };
	const MPI_Aint displacements[2] = { 0, sizeof(int) };
	MPI_Datatype types[2] = { MPI_INT, MPI_INT };
	MPI_Type_create_struct(2, lengths, displacements, types, &node_type);
	MPI_Type_commit(&node_type);

	DLinkedList<Process> datalist = randGenerator(len);
	DLinkedList<Process>::Iterator curr = datalist.begin();
	int round = 0;
	while (datalist.size() > 1) {
		int rank;
		int size;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &size);

		MPI_Status status;
		MPI_Request req;
		int count = 0;
		struct Process tmp;
		tmp.index = 0;
		tmp.value = 0;
		//consumer	
		if (rank == 0) {
			displayList(datalist);
			for (curr = datalist.begin(); curr != datalist.end(); curr++) {
				//cout << curr->index << endl;
				MPI_Send(&*curr, 1, node_type, curr->index + 1, 0, MPI_COMM_WORLD);
			}
			//send first
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);
			//raymond algorithm 
			MPI_Ssend(&datalist.front(), datalist.size(), node_type, datalist.front().index + 1, 0, MPI_COMM_WORLD);
			MPI_Recv(&datalist.front(), datalist.size(), node_type, datalist.back().index + 1, 0, MPI_COMM_WORLD, &status);
			MPI_Barrier(MPI_COMM_WORLD);
			if (status.MPI_SOURCE) {
				cout << count << endl;
				DLinkedList<Process> tmpList;
				for (int i = 0; i < count; i++) {
					MPI_Recv(&tmp, 1, node_type, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
					cout << tmp.value << " ";
					tmpList.append(tmp);
				}
				cout << endl;
				//datalist = tmpList;
			}
			round++;
		}
		//producer
		else {
			//we need to send first
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Recv(&*curr, 1, node_type, 0, 0, MPI_COMM_WORLD, &status);
			cout << curr->value << endl;
			cout << datalist.front().value << endl;
			tmp = *curr;
			vector<Process> comp;
			comp.push_back(*curr);
			

			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Send(&*curr, 1, node_type, curr.prev(1)->index+1, 0, MPI_COMM_WORLD);
			MPI_Send(&*curr, 1, node_type, curr.next(1)->index+1, 0, MPI_COMM_WORLD);
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Irecv(&*curr, 1, node_type, curr.prev(1)->index+1, 0, MPI_COMM_WORLD, &req);
			comp.push_back(*curr);
			MPI_Wait(&req, &status);
			MPI_Irecv(&*curr, 1, node_type, curr.next(1)->index+1, 0, MPI_COMM_WORLD, &req);
			comp.push_back(*curr);
			MPI_Wait(&req, &status);
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Recv(&datalist.front(), 1, node_type, curr.prev(1)->index+1 , 0, MPI_COMM_WORLD, &status);
			if (status.MPI_ERROR) {
				cout << __LINE__ << endl;
			}
			////we only want the heavy influencers.
			//if (comp[0].value > comp[1].value && comp[0].value > comp[2].value) {
			//	count++;
			//}
			//if (rank == size - 1) {
			//	MPI_Ssend(datalist.data(), 1, node_type, datalist.begin()->index+1, 0, MPI_COMM_WORLD);
			//}
			//else {
			//	MPI_Ssend(datalist.data(), 1, node_type, next(curr,1)->index+1, 0, MPI_COMM_WORLD);
			//}
			//MPI_Barrier(MPI_COMM_WORLD);
			//if (comp[0].value > comp[1].value && comp[0].value > comp[2].value) {
			//	tmp = comp[0];
			//	cout << "Provider Sending: " << rank << " : " << tmp.value << endl;
			//	MPI_Send(&tmp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			//}
		}
		MPI_Finalize();
	}
	//cout << "Our leader: " << datalist.front() << endl;


	return 0;
}

/*
*	MPI Basic (Blocking) Send
	MPI_Send(msg, count, datatype, dest, tag, comm);
	MPI Basic (Blocking) Receive
	MPI_Recv(msg, count, datatype, source, tag, comm, status);
*/