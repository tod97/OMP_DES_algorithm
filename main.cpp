#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "DESAlgorithm.cpp"
using namespace std;
using namespace chrono;

#define nWords 10000
#define partialWords 5000

void sequentialDES(vector<string> lines, int size)
{
	DESAlgorithm *des = new DESAlgorithm();

	for (int j = 0; j < size; j++)
	{
		string text = lines[j];

		string encryptedText = des->DES(des->stringToBin(text));
		des->reverseKeys();
		string decryptedText = des->DES(encryptedText);

		string result = des->binToString(decryptedText);
	}
}

int main()
{
	ifstream file("text_gen/words.txt");
	vector<string> lines(nWords);
	for (int j = 0; j < nWords; ++j)
		getline(file, lines[j]);

	// SEQUENTIAL
	auto start = system_clock::now();
	sequentialDES(lines, partialWords);
	auto end = system_clock::now();
	auto elapsed = duration_cast<milliseconds>(end - start);
	cout << "seq DES (n=" << partialWords << "): " << elapsed.count() << "ms" << endl;
}