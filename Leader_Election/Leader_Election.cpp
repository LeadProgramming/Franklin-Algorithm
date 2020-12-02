/// Leader_Election.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

struct Proc {
	int rank;
	int value;
};
vector<Proc> randGenerator(size_t size) {
	vector<Proc> result;
	for (size_t i = 0; i < size; i++) {
		Proc p;
		p.value = rand() % 1000 + 1;
		p.rank = i + 1;
		result.push_back(p);
	}
	return result;
}
void displayList(vector<Proc> x) {
	cout << "Process 0: ";
	for (auto i : x) {
		cout << i.value << " ";
	}
	cout << endl;
}
int main(int argc, char* argv[])
{
	srand(time(NULL));
	size_t len = stoi(argv[1]);

	vector<Proc> datalist = randGenerator(len);
	int round = 0;
	MPI_Init(&argc, &argv);
	
	MPI_Datatype proc_type;
	int lengths[2] = { 1, 1 };
	const MPI_Aint displacements[2] = { 0, sizeof(int) };
	MPI_Datatype types[3] = { MPI_INT, MPI_INT };
	MPI_Type_create_struct(2, lengths, displacements, types, &proc_type);
	MPI_Type_commit(&proc_type);

	MPI_Comm comm_world = MPI_COMM_WORLD;

	while (datalist.size() > 1) {
		int rank;
		int size;
		MPI_Comm_rank(comm_world, &rank);
		MPI_Comm_size(comm_world, &size);
		cout << rank << endl;

		struct Proc tmp;
		MPI_Status status;
		MPI_Request req;
		int count = 0;
		tmp.rank = rank;
		//consumer	
		if (rank == 0) {
			MPI_Group group_world;
			MPI_Comm_group(comm_world, &group_world);
			MPI_Comm_create(comm_world, group_world, &comm_world);
			int s;
			MPI_Group_size(group_world, &s);
			cout << "GROUP SIZE: " << s << endl;
			displayList(datalist);
			MPI_Barrier(comm_world);
			MPI_Barrier(comm_world);
			MPI_Barrier(comm_world);
			//raymond algorithm 
			MPI_Ssend(&count, 1, MPI_INT, 1, 0, comm_world);
			MPI_Recv(&count, 1, MPI_INT, size - 1, 0, comm_world, &status);
			MPI_Barrier(comm_world);
			if (status.MPI_SOURCE == size - 1) {
				cout << count << endl;
				vector<Proc> tmpList;
				for (int i = 0; i < count; i++) {
					MPI_Recv(&tmp, 1, proc_type, MPI_ANY_SOURCE, 0, comm_world, &status);
					cout << tmp.value << " ";
					tmpList.push_back(tmp);
				}
				cout << endl;
				datalist = tmpList;
				vector<int> ranks;
				for (auto i : datalist) {
					ranks.push_back(i.rank);
				}
				MPI_Group_incl(group_world, datalist.size(), ranks.data(), &group_world);
				//MPI_Group_free(&group_world);
			}
			round++;
			//MPI_Comm_free(&comm_world);
		}
		//producer
		else {
			tmp = datalist[rank - 1];
			cout << "Rank: " << rank << endl;
			vector<Proc> comp;
			comp.push_back(datalist[rank - 1]);
			MPI_Barrier(comm_world);
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
			MPI_Send(&datalist[rank - 1], 1, proc_type, left, 0, comm_world);
			MPI_Send(&datalist[rank - 1], 1, proc_type, right, 0, comm_world);
			MPI_Barrier(comm_world);
			MPI_Irecv(&datalist[rank - 1], 1, proc_type, left, 0, comm_world, &req);
			comp.push_back(datalist[rank - 1]);
			MPI_Wait(&req, &status);
			MPI_Irecv(&datalist[rank - 1], 1, proc_type, right, 0, comm_world, &req);
			comp.push_back(datalist[rank - 1]);
			MPI_Wait(&req, &status);
			MPI_Barrier(comm_world);
			//cout << "Rank: " << rank << " ";
			//for (int i : comp) {
			//	cout << i << " ";
			//}
			//cout << endl;
			MPI_Recv(&count, 1, MPI_INT, rank - 1, 0, comm_world, &status);
			//we only want the heavy influencers.
			if (comp[0].value > comp[1].value && comp[0].value > comp[2].value) {
				count++;
			}
			if (rank == size - 1) {
				MPI_Ssend(&count, 1, MPI_INT, 0, 0, comm_world);
			}
			else {
				MPI_Ssend(&count, 1, MPI_INT, rank + 1, 0, comm_world);
			}
			MPI_Barrier(comm_world);
			if (comp[0].value > comp[1].value && comp[0].value > comp[2].value) {
				tmp = comp[0];
				cout << "Provider Sending: " << rank << " : " << tmp.value << endl;
				MPI_Send(&tmp, 1, proc_type, 0, 0, comm_world);
			}
		}
	}
	MPI_Finalize();
	cout << "Our leader: " << datalist.front().value << endl;


	return 0;
}
