#include <iostream>
#include <string>
#include "DESAlgorithm.cpp"
using namespace std;

int main()
{
	DESAlgorithm *des = new DESAlgorithm();

	string text = "azAZ09./";
	text = des->stringToBin(text);

	string encryptedText = des->DES(text);
	cout << "Encrypted Text: " << encryptedText << endl;
	des->reverseKeys();
	string decryptedText = des->DES(encryptedText);
	cout << "Decrypted Text: " << des->binToString(decryptedText) << endl;
}