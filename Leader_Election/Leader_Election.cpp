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
#include <string>	/* stoi */
#include <vector>
using namespace std;
struct Proc {
	int id;
	int val;
};
vector<Proc> randGenerator(size_t size)
{
	vector<Proc> result;
	for (size_t i = 0; i < size; i++)
	{
		Proc p;
		p.id = i + 1;
		p.val = rand() % 1000 + 1;
		result.push_back(p);
	}
	return result;
}
void displayList(vector<Proc> x)
{
	cout << "Process 0: ";
	for (Proc i : x)
	{
		cout << i.val << " ";
	}
	cout << endl;
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	size_t len = stoi(argv[1]);

	vector<Proc> datalist = randGenerator(len);
	MPI_Init(&argc, &argv);
	int rnk;
	int sze;
	MPI_Comm_rank(MPI_COMM_WORLD, &rnk);
	MPI_Comm_size(MPI_COMM_WORLD, &sze);
	MPI_Datatype proc_type;
	int lengths[2] = { 1, 1 };
	const MPI_Aint displacements[2] = { 0, sizeof(int) };
	MPI_Datatype types[2] = { MPI_INT, MPI_INT };
	MPI_Type_create_struct(2, lengths, displacements, types, &proc_type);
	MPI_Type_commit(&proc_type);
	MPI_Comm comm_world = MPI_COMM_WORLD;
	bool active = true;
	while (datalist.size() > 1)
	{
		MPI_Status status;
		MPI_Request req;
		int count = 0;
		int idx = 0;
		Proc p;
		p.val = 0;
		vector<Proc> valueList;
		//consumer
		if (rnk == 0)
		{
			//sending the data to every existing processes.
			for (int i = 0; i < datalist.size(); i++) {
				MPI_Send(datalist.data(), datalist.size(), proc_type, datalist[i].id, i, comm_world);
				cout << "ID: " << datalist[i].id << endl;
				cout << "Value: " << datalist[i].val << endl;
			}
			//now we need to send the index of the array to every existing processes. 
			MPI_Barrier(comm_world);

			displayList(datalist);
			MPI_Barrier(comm_world);
			MPI_Barrier(comm_world);
			MPI_Barrier(comm_world);
			//raymond algorithm
			cout << "SEND: " << datalist[0].id << endl;
			cout << "FINAL: " << datalist[datalist.size() - 1].id << endl;
			MPI_Ssend(&count, 1, MPI_INT, 1, 0, comm_world);
			MPI_Recv(&count, 1, MPI_INT, datalist[datalist.size() - 1].id, 0, comm_world, &status);
			cout << "COUNT: " << count << endl;
			cout << "RCV" << endl;
			MPI_Barrier(comm_world);
			cout << "RESULT: ";
			for (int i = 0; i < count; i++)
			{
				if (count == 1) {
					cout << "Our leader: ";
				}
				MPI_Recv(&p, 1, proc_type, MPI_ANY_SOURCE, 0, comm_world, &status);
				//sorted insert
				if (!valueList.empty()) {
					if (p.id > valueList.front().id) {
						valueList.push_back(p);
					}
					else {
						valueList.insert(valueList.begin(),p);
					}
				}
				else {
					valueList.push_back(p);
				}
				cout << p.val << " ";
			}
			cout << endl;
			datalist = valueList;
			for (Proc i : datalist) {
				cout << i.val << " ";
			}
			cout << endl;

		}
		//producer
		else
		{
			if (active) {
				cout << "Rank: " << rnk << endl;
				MPI_Barrier(comm_world);
				MPI_Probe(0, MPI_ANY_TAG, comm_world, &status);
				idx = status.MPI_TAG;
				cout << "IDX: " << idx << endl;
				int dl_size = 0;
				MPI_Get_count(&status, proc_type, &dl_size);

				MPI_Recv(datalist.data(), dl_size, proc_type, 0, idx, comm_world, &status);
				for (int i = 0; i < dl_size; i++) {
					cout << "VAL: " << datalist[i].val << " ";
					cout << "ID: " << datalist[i].id << " ";
				}
				cout << endl;

				p = datalist[idx];
				vector<Proc> comp;
				comp.push_back(datalist[idx]);
				MPI_Barrier(comm_world);
				int left = 0;
				int right = 0;
				if (idx == 0)
				{
					left = datalist[dl_size - 1].id;
					right = datalist[idx + 1].id;
				}
				else if (idx == dl_size - 1)
				{
					left = datalist[idx - 1].id;
					right = datalist[0].id;
				}
				else {
					left = datalist[idx - 1].id;
					right = datalist[idx + 1].id;
				}
				MPI_Send(&datalist[idx], 1, proc_type, left, 0, comm_world);
				MPI_Send(&datalist[idx], 1, proc_type, right, 0, comm_world);
				MPI_Barrier(comm_world);

				MPI_Irecv(&datalist[idx], 1, proc_type, left, 0, comm_world, &req);
				comp.push_back(datalist[idx]);
				MPI_Wait(&req, &status);
				MPI_Irecv(&datalist[idx], 1, proc_type, right, 0, comm_world, &req);
				comp.push_back(datalist[idx]);
				MPI_Wait(&req, &status);

				MPI_Barrier(comm_world);

				int l, r;
				if (idx == 0) {
					l = 0;
					r = datalist[idx].id;
				}
				else if (idx == dl_size - 1) {

					l = datalist[idx - 1].id;
					r = 0;
				}
				else {
					l = datalist[idx - 1].id;
					r = datalist[idx].id;
				}
				cout << "RNK: " << rnk << " LEFT: " << l;
				cout << "RIGHT: " << r << endl;
				MPI_Recv(&count, 1, MPI_INT, l, 0, comm_world, &status);
				//we only want the heavy influencers.
				if (comp[0].val > comp[1].val && comp[0].val > comp[2].val)
				{
					count++;
				}
				cout << "SENDING: " << datalist[idx].id << endl;

				MPI_Ssend(&count, 1, MPI_INT, r, 0, comm_world);
				MPI_Barrier(comm_world);
				if (comp[0].val > comp[1].val && comp[0].val > comp[2].val)
				{
					cout << "PID: " << p.id << endl;
					cout << "Provider Sending: " << rnk << " : " << p.val << endl;
					MPI_Send(&p, 1, proc_type, 0, 0, comm_world);
				}
				else {
					active = false;
				}

			}
			else {
				MPI_Barrier(comm_world);
				MPI_Barrier(comm_world);
				MPI_Barrier(comm_world);
				MPI_Barrier(comm_world);
				MPI_Barrier(comm_world);
			}
		}
	}
	MPI_Finalize();
	//cout << "Our leader: " << datalist.front().val << endl;

	return 0;
}
