#ifdef _OPENMP
#include <omp.h>
#endif
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "DESAlgorithm.cpp"
using namespace std;
using namespace chrono;

#define nToCrack 10

void testCrack(vector<string> pwdList, vector<string> pwdToCrack);
void sequentialCrack(vector<string> pwdList, vector<string> pwdToCrack);
void parallelCrack(vector<string> pwdList, vector<string> pwdToCrack, int nThreads);

int main()
{
	string line;
	ifstream file("text_gen/words.txt");
	vector<string> pwdList = {};
	while (getline(file, line))
	{
		pwdList.push_back(line);
	}
	file.close();

	vector<string> pwdToCrack = {};
	while (pwdToCrack.size() < nToCrack)
	{
		auto newEl = pwdList[rand() % pwdList.size()];
		if (find(pwdToCrack.begin(), pwdToCrack.end(), newEl) == pwdToCrack.end())
			pwdToCrack.push_back(newEl);
	}

	testCrack(pwdList, pwdToCrack);
}

void testCrack(vector<string> pwdList, vector<string> pwdToCrack)
{
	cout << "--------- PWD TO CRACK SIZE = " << nToCrack << " ---------" << endl;

	// SEQUENTIAL
	auto start = system_clock::now();
	sequentialCrack(pwdList, pwdToCrack);
	auto end = system_clock::now();
	auto seqElapsed = duration_cast<milliseconds>(end - start);
	cout << "Sequential: " << seqElapsed.count() << "ms" << endl;
	cout << "-----------------------------------------" << endl;

	vector<int> threadTests = {1, 2, 4, 8, 16, 32, 64};
	/* #ifdef _OPENMP
		for (int i = 0; pow(2, i) <= omp_get_max_threads(); i++)
		{
			threadTests.push_back(pow(2, i));
		}
	#endif */
	vector<float> speedups = {};

	for (int i = 0; i < threadTests.size(); i++)
	{
		start = system_clock::now();
		parallelCrack(pwdList, pwdToCrack, threadTests[i]);
		end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start);
		cout << "Parallel [t=" << threadTests[i] << "]: " << elapsed.count() << "ms" << endl;
		cout << "Speedup: " << (float)seqElapsed.count() / elapsed.count() << "x" << endl;
		cout << "-----------------------------------------" << endl;
		speedups.push_back((float)seqElapsed.count() / elapsed.count());
	}

	for (int i = 0; i < speedups.size(); i++)
	{
		if (i == 0)
			cout << "Speedups: [";
		cout << speedups[i];
		if (i != speedups.size() - 1)
			cout << ", ";
		else
			cout << "]" << endl;
	}
}

void sequentialCrack(vector<string> pwdList, vector<string> pwdToCrack)
{
	DESAlgorithm des;

	for (string &encrypted : pwdToCrack)
	{
		// cout << "Password to crack: " << encrypted << endl;
		encrypted = des.DES(des.stringToBin(encrypted));
		for (string &pwd : pwdList)
		{
			string pwdEncrypted = des.DES(des.stringToBin(pwd));

			if (encrypted == pwdEncrypted)
			{
				cout << "Password found: " << pwd << endl;
				break;
			}
		}
	}
}

void parallelCrack(vector<string> pwdList, vector<string> pwdToCrack, int nThreads)
{
#ifdef _OPENMP
	omp_set_num_threads(nThreads);
#endif
	DESAlgorithm des;

	for (string &encrypted : pwdToCrack)
	{
		// cout << "Password to crack: " << encrypted << endl;
		encrypted = des.DES(des.stringToBin(encrypted));
		volatile bool found = false;
		int workloadThread = static_cast<int>(ceil((double)pwdList.size() / (double)nThreads));

#pragma omp parallel shared(des, encrypted, found)
		{
#ifdef _OPENMP
			int tid = omp_get_thread_num();
			for (int i = tid * workloadThread; i < (tid + 1) * workloadThread && !found; i++)
			{
				string pwdEncrypted = des.DES(des.stringToBin(pwdList[i]));

				if (encrypted == pwdEncrypted)
				{
					cout << "Password found: " << pwdList[i] << endl;
					found = true;
				}
			}
#endif
		}
	}
}
