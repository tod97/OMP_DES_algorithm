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

#define nToCrack 5
#define nTests 10

void testCrack(vector<string> pwdList, vector<string> pwdToCrack);
void sequentialCrack(vector<string> pwdList, vector<string> pwdToCrack);
void parallelCrack(vector<string> pwdList, vector<string> pwdToCrack, int nThreads);
void testSmartCrack(vector<string> pwdList, vector<string> pwdToCrack);
void sequentialSmartCrack(vector<string> pwdList, vector<string> pwdToCrack);
void parallelSmartCrack(vector<string> pwdList, vector<string> pwdToCrack, int nThreads);
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

	// testCrack(pwdList, pwdToCrack);
	testSmartCrack(pwdList, pwdToCrack);
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
#endif
	DESAlgorithm des;

	for (string &encrypted : pwdToCrack)
	{
		// cout << "Password to crack: " << encrypted << endl;
		encrypted = des.DES(des.stringToBin(encrypted));
		volatile bool found = false;
		int splitDimension = static_cast<int>(ceil((double)pwdList.size() / (double)nThreads));

#pragma omp parallel shared(des, encrypted, found)
		{
#ifdef _OPENMP
			int tid = omp_get_thread_num();
			for (int i = tid * splitDimension; i < (tid + 1) * splitDimension && !found; i++)
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

void testSmartCrack(vector<string> pwdList, vector<string> pwdToCrack)
{
	vector<float> times = {};
	cout << "--------- PWD TO CRACK SMART SIZE = " << nToCrack << " ---------" << endl;

	// SEQUENTIAL
	auto start = system_clock::now();
	for (int i = 0; i < nTests; i++)
	{
		sequentialSmartCrack(pwdList, vector<string>(pwdToCrack.begin() + i * nToCrack, pwdToCrack.begin() + (i + 1) * nToCrack));
	}
	auto end = system_clock::now();
	auto seqElapsed = duration_cast<milliseconds>(end - start) / nTests;
	cout << "Sequential: " << seqElapsed.count() << "ms" << endl;
	cout << "-----------------------------------------" << endl;
	times.push_back(seqElapsed.count());

	vector<int> threadTests = {8};
	vector<float> speedups = {};

	for (int i = 0; i < threadTests.size(); i++)
	{
		start = system_clock::now();
		for (int j = 0; j < nTests; j++)
		{
			parallelSmartCrack(pwdList, vector<string>(pwdToCrack.begin() + j * nToCrack, pwdToCrack.begin() + (j + 1) * nToCrack), threadTests[i]);
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

void sequentialSmartCrack(vector<string> pwdList, vector<string> pwdToCrack)
{
	DESAlgorithm des;
	int pwdFound = 0;
	vector<string> pwdToCrackEncrypted = {};

	for (string &encrypted : pwdToCrack)
	{
		pwdToCrackEncrypted.push_back(des.DES(des.stringToBin(encrypted)));
	}

	for (string &pwd : pwdList)
	{
		string pwdEncrypted = des.DES(des.stringToBin(pwd));

		if (find(pwdToCrackEncrypted.begin(), pwdToCrackEncrypted.end(), pwdEncrypted) != pwdToCrackEncrypted.end())
		{
			cout << "Password found: " << pwd << endl;
			pwdFound++;
		}

		if (pwdFound == pwdToCrack.size())
		{
			break;
		}
	}
}

void parallelSmartCrack(vector<string> pwdList, vector<string> pwdToCrack, int nThreads)
{
#ifdef _OPENMP
	omp_set_num_threads(nThreads);
#endif
	DESAlgorithm des;
	volatile int pwdFound = 0;
	vector<string> pwdToCrackEncrypted = {};

	for (string &encrypted : pwdToCrack)
	{
		pwdToCrackEncrypted.push_back(des.DES(des.stringToBin(encrypted)));
	}

	int splitDimension = static_cast<int>(ceil((double)pwdList.size() / (double)nThreads));

#pragma omp parallel shared(des, pwdToCrackEncrypted, pwdFound)
	{
#ifdef _OPENMP
		int tid = omp_get_thread_num();
		for (int i = tid * splitDimension; i < (tid + 1) * splitDimension && pwdFound != pwdToCrack.size(); i++)
		{
			string pwdEncrypted = des.DES(des.stringToBin(pwdList[i]));

			if (find(pwdToCrackEncrypted.begin(), pwdToCrackEncrypted.end(), pwdEncrypted) != pwdToCrackEncrypted.end())
			{
				cout << "Password found: " << pwdList[i] << endl;
				pwdFound++;
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
