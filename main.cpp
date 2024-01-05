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

#define nToCrack 1
#define nTests 10

void testCrack(vector<string> pwdList, vector<string> pwdToCrack);
void sequentialCrack(vector<string> pwdList, vector<string> pwdToCrack);
void parallelCrack(vector<string> pwdList, vector<string> pwdToCrack, int nThreads);
void printVector(vector<float> v, string name);

int main()
{
	string line;
	ifstream file("words.txt");
	vector<string> pwdList = {};
	while (getline(file, line))
	{
		pwdList.push_back(line);
	}
	file.close();

	vector<string> pwdToCrack = {};
	while (pwdToCrack.size() < nToCrack * nTests)
	{
		auto newEl = pwdList[rand() % pwdList.size()];
		if (find(pwdToCrack.begin(), pwdToCrack.end(), newEl) == pwdToCrack.end())
			pwdToCrack.push_back(newEl);
	}

	testCrack(pwdList, pwdToCrack);
}

void testCrack(vector<string> pwdList, vector<string> pwdToCrack)
{
	vector<float> times = {};
	cout << "--------- PWD TO CRACK SIZE = " << nToCrack << " ---------" << endl;

	// SEQUENTIAL
	auto start = system_clock::now();
	for (int i = 0; i < nTests; i++)
	{
		sequentialCrack(pwdList, vector<string>(pwdToCrack.begin() + i * nToCrack, pwdToCrack.begin() + (i + 1) * nToCrack));
	}
	auto end = system_clock::now();
	auto seqElapsed = duration_cast<milliseconds>(end - start) / nTests;
	cout << "Sequential: " << seqElapsed.count() << "ms" << endl;
	cout << "-----------------------------------------" << endl;
	times.push_back(seqElapsed.count());

	vector<int> threadTests = {2, 3, 4, 6, 8, 16, 32};
	/* #ifdef _OPENMP
		for (int i = 1; pow(2, i) <= omp_get_max_threads(); i++)
		{
			threadTests.push_back(pow(2, i));
		}
	#endif */
	vector<float> speedups = {};

	for (int i = 0; i < threadTests.size(); i++)
	{
		start = system_clock::now();
		for (int j = 0; j < nTests; j++)
		{
			parallelCrack(pwdList, vector<string>(pwdToCrack.begin() + j * nToCrack, pwdToCrack.begin() + (j + 1) * nToCrack), threadTests[i]);
		}
		end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start) / nTests;
		cout << "Parallel [t=" << threadTests[i] << "]: " << elapsed.count() << "ms" << endl;
		cout << "Speedup: " << (float)seqElapsed.count() / elapsed.count() << "x" << endl;
		cout << "-----------------------------------------" << endl;
		times.push_back(elapsed.count());
		speedups.push_back((float)seqElapsed.count() / elapsed.count());
	}

	printVector(times, "Times");
	printVector(speedups, "Speedups");
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
	cout << "Cancellation " << (omp_get_cancellation() == 1 ? "enabled" : "disabled. Set OMP_CANCELLATION=TRUE as env variable") << endl;
#endif
	DESAlgorithm des;

	for (string &encrypted : pwdToCrack)
	{
		// cout << "Password to crack: " << encrypted << endl;
		encrypted = des.DES(des.stringToBin(encrypted));
		vector<int> chunks;
		chunks.push_back(0);
		for (int i = 1; i < nThreads; ++i)
		{
			chunks.push_back(static_cast<int>(pwdList.size() / nThreads) * i);
		}
		chunks.push_back(pwdList.size());

#ifdef _OPENMP
#pragma omp parallel shared(des, encrypted, chunks)
		{
			int tid = omp_get_thread_num();
			for (int i = chunks[tid]; i < chunks[tid + 1]; i++)
			{
				string pwdEncrypted = des.DES(des.stringToBin(pwdList[i]));

				if (encrypted == pwdEncrypted)
				{
					cout << "Password found: " << pwdList[i] << endl;
#pragma omp cancel parallel
				}
#pragma omp cancellation point parallel
			}
		}
#endif
	}
}

void printVector(vector<float> v, string name)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (i == 0)
			cout << name << ": [";
		cout << v[i];
		if (i != v.size() - 1)
			cout << ", ";
		else
			cout << "]" << endl;
	}
}
