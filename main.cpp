#ifdef _OPENMP
#include <omp.h>
#endif
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "DESAlgorithm.cpp"
using namespace std;
using namespace chrono;

#define nWords 10000
#define partialWords 5000

void testEncryptDecrypt(vector<string> lines);
void sequentialDES(vector<string> lines, int size);
void parallelDES(vector<string> lines, int size, int nThreads);

int main()
{
	ifstream file("text_gen/words.txt");
	vector<string> lines(nWords);
	for (int j = 0; j < nWords; ++j)
		getline(file, lines[j]);
	file.close();

	testEncryptDecrypt(lines);
}

void testEncryptDecrypt(vector<string> lines)
{
	// SEQUENTIAL
	auto start = system_clock::now();
	sequentialDES(lines, partialWords);
	auto end = system_clock::now();
	auto seqElapsed = duration_cast<milliseconds>(end - start);
	cout << "--------- SIZE = " << partialWords << " ---------" << endl;
	cout << "seq DES: " << seqElapsed.count() << "ms" << endl;
	cout << "------------------" << endl;

	// PARALLEL
	vector<int> nThreads = {};
#ifdef _OPENMP
	for (int i = 0; pow(2, i) <= omp_get_max_threads(); i++)
	{
		nThreads.push_back(pow(2, i));
	}
#endif
	vector<float> speedups = {};

	for (int i = 0; i < nThreads.size(); i++)
	{
		start = system_clock::now();
		parallelDES(lines, partialWords, nThreads[i]);
		end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start);
		cout << "par DES (t=" << nThreads[i] << "): " << elapsed.count() << "ms" << endl;
		cout << "Speedup: " << (float)seqElapsed.count() / elapsed.count() << "x" << endl;
		cout << "------------------" << endl;
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

void sequentialDES(vector<string> lines, int size)
{
	DESAlgorithm des;

	for (int j = 0; j < size; j++)
	{
		string text = lines[j];

		string encryptedText = des.DES(des.stringToBin(text));
		des.reverseKeys();
		string decryptedText = des.DES(encryptedText);

		string result = des.binToString(decryptedText);
		if (result != text)
		{
			cout << "Error: " << result << " != " << text << endl;
		}
	}
}

void parallelDES(vector<string> lines, int size, int nThreads)
{
#ifdef _OPENMP
	omp_set_num_threads(nThreads);
#endif

	DESAlgorithm des;

#pragma omp parallel for shared(lines, size) private(des)
	for (int j = 0; j < size; j++)
	{
		string text = lines[j];

		string encryptedText = des.DES(des.stringToBin(text));
		des.reverseKeys();
		string decryptedText = des.DES(encryptedText);

		string result = des.binToString(decryptedText);
		if (result != text)
		{
			cout << "Error: " << result << " != " << text << endl;
		}
	}
}
