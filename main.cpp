/**
	TSP Sequential Genetic ALgorithm
**/

#include "mpi.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string.h>
using namespace std;

void prlst(int* l, int s) {
	if (s == 0 || l == 0) {
		cout << "[]" << '\n';
	}
	else {
		cout << "[";
		for (int i = 0; i < s; i++) {
			if (i != s-1) {
				cout << l[i] << ", ";
			}
			else {
				cout << l[i] << "]\n";
			}
		}
	}
}

void pr2dlst(int** l, int r, int c) {
	if (l == 0 || r == 0 || c == 0) {
		cout << "[]" << '\n';
	}
	else {
		for (int i = 0; i < r; i++) {
			cout<< "[";
			for (int j = 0; j < c; j++) {
				if (j != c-1) {
					cout << l[i][j] << ", ";
				}
				else {
					cout << l[i][j] << "]\n";
				}
			}
		}
	}
}

//generates valid permutations
void initPop(int** P, int m, int n) {

	int num = -1;

	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {

			num = rand() % n;

			for (int k = 0; k < j; k++) {

				if (P[i][k] == num) {

					k = -1;            // reset loop
					num = rand() % n; // gen new num
				}
			}
			P[i][j] = num;
		}
	}
}

//calcs fitness of an individual
int fitEval(int* D, int Dn, int* I, int In) {

	int x, y;
	int tot = 0;
	for (int i = 0; i < In; i++) {
		if (i != In-1) {
			x = I[i];
			y = I[i + 1];
			tot += *(D+x*Dn+y);
		}
		else {
			//going back to starting node
			x = I[i];
			y = I[0];
			tot += *(D+x*Dn+y);

			x = I[0];
			y = I[0];
			tot += *(D+x*Dn+y);
		}
	}
	return tot;
}

//creates the fitness number line
int* popFitEval(int** P, int m, int n, int* D, int Dn, int* &bestI, int &bestFit) {

	bestI = 0;
	bestFit = 9999;

	int* I = 0; //random indi
	int newFit = 9999;

	int* F = new int[m];
	int tot = 0;

	int f[m] = {};

	//calcing all fitness scores
	for (int i = 0; i < m; i++) {

		I = P[i];

		newFit = fitEval(D, Dn, I, n);

		tot += newFit;;
		F[i] = tot;

		f[i] = newFit;

		if (newFit < bestFit) {
			bestFit = newFit;
			bestI = I;
		}
	}

	return F;
}

//for adding to S
bool addIndi(int* I, int** S, int n, int* chosen, int s, int Sindex, int mi) {

	if (chosen != 0) {
		for (int i = 0; i < s; i++) {
			if (chosen[i] == mi)
				return false;
		}
	}

	for (int i = 0; i < n; i++) {
		S[Sindex][i] = I[i];
	}

	return true;
}

//completes right half of offsprings
void renumber(int* l, int n, int* missing, int mn) {

	int temp = -1;
	int ix = -1;
	int iy = -1;

	//shuffling missing list
	for (int i = 0; i < mn; i++) {

		ix = rand() % mn;
		iy = rand() % mn;

		temp = missing[ix];

		missing[ix] = missing[iy];
		missing[iy] = temp;
	}

	for (int i = 0; i < n; i++) {
		l[i] = missing[i];
	}
}

//mutates individual
void mutateIndi(int* l, int n) {

	int z = 0.05 * n;
	int temp = -1;
	int ix = -1;
	int iy = -1;

	for (int i = 0; i < z; i++) {

		ix = rand() % n;
		iy = rand() % n;

		temp = l[ix];

		l[ix] = l[iy];
		l[iy] = temp;
	}
}

//mutates random members of the population
void mutatePop(int** P, int m, int n) {

	int u = 0.1 * m;
	int I = -1;

	for (int i = 0; i < u; i++) {
		I = rand() % m;
		mutateIndi(P[I], n);
	}
}

//creates offsprings
void crossover(int** S, int s, int m, int n) {

	int* X = 0;
	int* Y = 0;
	int V[n] = {}; //left of x, right of y
	int W[n] = {};
	int notseenV[n] = {};
	int notseenW[n] = {};
	int vi = 0;
	int wi = 0;
	bool seenV = false;
	bool seenW = false;
	int added = s;

	for (int i = s; i < m; i++) {

		X = S[(rand() % s)];
		Y = S[(rand() % s)];

		//n is even
		if (n % 2 == 0) {
			for (int j = 0; j < n/2; j++) {
				//making V and W
				V[j] = X[j];
				V[j+(n/2)] = Y[j+(n/2)];

				W[j] = Y[j];
				W[j+(n/2)] = X[j+(n/2)];
			}
			
			//for each possible element
			for (int j = 0; j < n; j++) {

				//checking whats in left half
				for (int k = 0; k < n/2; k++) {

					if (V[k] == j) {
						seenV = true;
					}
					if (W[k] == j) {
						seenW = true;
					}
				}
				//if we didnt find an element in left half, add it to missing
				if (!seenV) {
					notseenV[vi++] = j;
				}
				if (!seenW) {
					notseenW[wi++] = j;
				}
				seenV = false;
				seenW = false;
			}

			//renumbering v and w
			renumber(&V[n/2], vi, &notseenV[0], vi);
			renumber(&W[n/2], wi, &notseenW[0], wi);
		}
		else {
			for (int j = 0; j < n/2; j++) {
				//making V and W
				V[j] = X[j];
				V[j+(n/2)] = Y[j+(n/2)];

				W[j] = Y[j];
				W[j+(n/2)] = X[j+(n/2)];
			}
			V[n-1] = Y[n-1];
			W[n-1] = X[n-1];

			//for each possible element
			for (int j = 0; j < n; j++) {

				//checking whats in left half
				for (int k = 0; k < n/2; k++) {

					if (V[k] == j) {
						seenV = true;
					}
					if (W[k] == j) {
						seenW = true;
					}
				}
				//if we didnt find an element in left half, add it to missing
				if (!seenV) {
					notseenV[vi++] = j;
				}
				if (!seenW) {
					notseenW[wi++] = j;
				}
				seenV = false;
				seenW = false;
			}

			//renumbering v and w
			renumber(&V[n/2], vi, &notseenV[0], vi);
			renumber(&W[n/2], wi, &notseenW[0], wi);
		}

		//add V and W to S
		if (added != m) {
			addIndi(&V[0], S, n, 0, added, added, 0);
			added++;
		}
		if(added != m) {
			addIndi(&W[0], S, n, 0, added, added, 0);
			added++;
		}

		//reset vars
		for (int i = 0; i < n; i++) {
			notseenV[i] = -1;
			notseenW[i] = -1;
			vi = 0;
			wi = 0;
		}
	}

	mutatePop(S, m, n);
}

//builds new population based on old one
int** selectPop(int** P, int m, int n, int s, int* D, int Dn, int* F) {

	int** S = new int*[m];
	for (int i = 0; i < m; i++) {
		S[i] = new int[n];
	}

	int tot = F[m-1];

	int y = -1; //rand fitness value

	int* I = 0; //rand indi selected

	//this is only for select pop, not crossover
	int chosen[s] = {};
	for (int i = 0; i < s; i++) {
		chosen[i] = -1;
	}
	int chi = 0;

	int added = 0; //num of indis added to S, and also the current index of S

	//trying 2s times to add indis to S
	for (int j = 0; j < 2*s; j++) {

		if (added == s){
			break;
		}

		y = (rand() % tot) + 1; //between 1 and total

		//finding I
		for (int i = 0; i < m; i++) {

			//first check
			if (y <= F[i]) {

				//F-1 = 0, for when i = 0
				if (i == 0) {

					//second check
					if (-1 < y) {
						I = P[i];

						if (addIndi(I, S, n, chosen, s, added, i)) {
							chosen[chi++] = i;
							added++;
						}
					}
				}
				//second check
				else if (F[i-1] < y) {
					I = P[i];

					if (addIndi(I, S, n, chosen, s, added, i)) {
						chosen[chi++] = i;
						added++;
					}
				}
			}
		}
	}
	crossover(S, added, m, n);
	return S;
}

//keeps a counter and checks if we term or not
bool term(int r, int &count, int* Iopt, int &cfit, int* newIopt, int nfit, int n) {

	if (nfit < cfit) {
		memcpy(Iopt, newIopt, sizeof(int)*n);
		cfit = nfit;
		count = 0;
		return false;
	}
	else {
		count++;
		if (count >= r) {
			return true;
		}
		return false;
	}
}

int main(int argc, char** argv) {

	int id;
	int p;

	MPI::Init(argc, argv); //  Initialize MPI.
	p = MPI::COMM_WORLD.Get_size(); //  Get the number of processes.
	id = MPI::COMM_WORLD.Get_rank(); //  Get the individual process ID.

	srand(time(NULL));

	int m = 10000;
	int n = 100;
	int s = 0.75 * m;
	int Dn = n;
	int k = 1000;
	int r = 20;
	int count = 0;
	int* Iopt = new int[n];
	int cfit = 9999;
	int* newIopt = 0;
	int nfit = 9999;

	//test graphs
	/*int d[5][5] = {{0, 12, 10, 19, 8},
		           {12, 0, 3, 7, 16},
		           {10, 3, 0, 6, 20},
		           {19, 7, 6, 0, 4},
		           {8, 2, 20, 4, 0}};

	int d1[5][5] = {{0,14,15,4,9},
					{14,0,18,5,4},
					{15,18,0,19,10},
					{4,5,19,0,12},
					{9,13,10,12,0}};

	int d2[n][n] = {{0,14,15,4},
					{14,0,18,5},
					{15,18,0,19},
					{4,5,19,0}};*/

	//reading from input file
	ifstream ifile;
	ifile.open("grid.txt");
	string line;
	int num = 0;
	int dist[100][100];

	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			getline(ifile, line);
			num = stoi(line);
			dist[i][j] = num;
		}
	}

	int* D = &dist[0][0];
	int* F = 0;
	int** S = 0;
	int** P = new int*[m];
	for (int i = 0; i < m; i++) {
		P[i] = new int[n];
	}

	//setup timer
	double wtime;
	wtime = MPI::Wtime();

	initPop(P, m, n);

	for (int i = 0; i < k; i++) {

		//check populations's fitness
		if (P != 0) {
			F = popFitEval(P, m, n, D, Dn, newIopt, nfit);
		}
		else {
			cout<< "P is null" <<endl;
			break;
		}

		//check term
		if (Iopt != 0) {
			if (term(r, count, Iopt, cfit, newIopt, nfit, n)) {
				cout<<'\n'<<"reached r"<<endl;
				cout<<"rounds: "<<i<<endl<<endl;
				break;
			}
		}
		else {
			memcpy(Iopt, newIopt, sizeof(int)*n);
		}

		if (P != 0) {
			if (F != 0) {
				S = selectPop(P, m, n, s, D, Dn, F);
			}
			else {
				cout<< "F is null" <<endl;
				break;
			}
		}
		else {
			cout<< "P is null" <<endl;
			break;
		}

		if (F != 0) {
			delete[] F;
			F = 0;
		}

		if (P != 0) {
			for (int i = 0; i< m; i++) {
				delete[] P[i];
			}
			delete[] P;
			P = 0;
		}
		P = S;
		S = 0;
	}

	if (id == 0) {
		wtime = MPI::Wtime() - wtime;
		cout << "Elapsed wall clock time = " << wtime << " seconds.\n";
	}

	prlst(Iopt, n);
	cout<<endl<<"shortest distance found: "<<cfit<<endl;

	if (F != 0) {
		delete[] F;
		F = 0;
	}

	if (P != 0) {
		for (int i = 0; i< m; i++) {
			delete[] P[i];
		}
		delete[] P;
		P = 0;
	}

	delete[] Iopt;

	MPI::Finalize();

	return 0;
}
