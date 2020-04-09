/**
 * 	Generates the same "grid" every time.
 */
 
#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;

int main() {
	
	ofstream of;
	of.open("grid.txt");
	
	int num = 0;
	for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100; j++) {
			num = rand() % 30;
			of << num << endl;
		}
	}

	of.close();	
	cout<<"Output Done"<<endl;
	
	return 0;
}
