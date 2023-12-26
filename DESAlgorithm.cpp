#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>
using namespace std;

class DESAlgorithm
{
public:
    string roundKeys[16];
    int expansionTable[48];
    int substitutionBoxes[8][4][16];

    DESAlgorithm()
    {
        defineTables();
        generateKeys();
    }

    ~DESAlgorithm() {}

    string stringToBin(string s)
    {
        unordered_map<char, string> mp = {
            {'0', "00000000"},
            {'1', "00000001"},
            {'2', "00000010"},
            {'3', "00000011"},
            {'4', "00000100"},
            {'5', "00000101"},
            {'6', "00000110"},
            {'7', "00000111"},
            {'8', "00001000"},
            {'9', "00001001"},
            {'a', "00001010"},
            {'b', "00001011"},
            {'c', "00001100"},
            {'d', "00001101"},
            {'e', "00001110"},
            {'f', "00001111"},
            {'g', "00010000"},
            {'h', "00010001"},
            {'i', "00010010"},
            {'j', "00010011"},
            {'k', "00010100"},
            {'l', "00010101"},
            {'m', "00010110"},
            {'n', "00010111"},
            {'o', "00011000"},
            {'p', "00011001"},
            {'q', "00011010"},
            {'r', "00011011"},
            {'s', "00011100"},
            {'t', "00011101"},
            {'u', "00011110"},
            {'v', "00011111"},
            {'w', "00100001"},
            {'x', "00100010"},
            {'y', "00100011"},
            {'z', "00100100"},
            {'A', "00100101"},
            {'B', "00100110"},
            {'C', "00100111"},
            {'D', "00101000"},
            {'E', "00101001"},
            {'F', "00101010"},
            {'G', "00101011"},
            {'H', "00101100"},
            {'I', "00101101"},
            {'J', "00101110"},
            {'K', "00101111"},
            {'L', "00110000"},
            {'M', "00110001"},
            {'N', "00110010"},
            {'O', "00110011"},
            {'P', "00110100"},
            {'Q', "00110101"},
            {'R', "00110110"},
            {'S', "00110111"},
            {'T', "00111000"},
            {'U', "00111001"},
            {'V', "00111010"},
            {'W', "00111011"},
            {'X', "00111100"},
            {'Y', "00111101"},
            {'Z', "00111110"},
            {'.', "00111111"},
            {'/', "01000000"}};

        string bin = "";
        for (char i : s)
        {
            bin += mp[i];
        }
        return bin;
    }

    string binToString(string s)
    {
        unordered_map<string, char> mp = {
            {"00000000", '0'},
            {"00000001", '1'},
            {"00000010", '2'},
            {"00000011", '3'},
            {"00000100", '4'},
            {"00000101", '5'},
            {"00000110", '6'},
            {"00000111", '7'},
            {"00001000", '8'},
            {"00001001", '9'},
            {"00001010", 'a'},
            {"00001011", 'b'},
            {"00001100", 'c'},
            {"00001101", 'd'},
            {"00001110", 'e'},
            {"00001111", 'f'},
            {"00010000", 'g'},
            {"00010001", 'h'},
            {"00010010", 'i'},
            {"00010011", 'j'},
            {"00010100", 'k'},
            {"00010101", 'l'},
            {"00010110", 'm'},
            {"00010111", 'n'},
            {"00011000", 'o'},
            {"00011001", 'p'},
            {"00011010", 'q'},
            {"00011011", 'r'},
            {"00011100", 's'},
            {"00011101", 't'},
            {"00011110", 'u'},
            {"00011111", 'v'},
            {"00100001", 'w'},
            {"00100010", 'x'},
            {"00100011", 'y'},
            {"00100100", 'z'},
            {"00100101", 'A'},
            {"00100110", 'B'},
            {"00100111", 'C'},
            {"00101000", 'D'},
            {"00101001", 'E'},
            {"00101010", 'F'},
            {"00101011", 'G'},
            {"00101100", 'H'},
            {"00101101", 'I'},
            {"00101110", 'J'},
            {"00101111", 'K'},
            {"00110000", 'L'},
            {"00110001", 'M'},
            {"00110010", 'N'},
            {"00110011", 'O'},
            {"00110100", 'P'},
            {"00110101", 'Q'},
            {"00110110", 'R'},
            {"00110111", 'S'},
            {"00111000", 'T'},
            {"00111001", 'U'},
            {"00111010", 'V'},
            {"00111011", 'W'},
            {"00111100", 'X'},
            {"00111101", 'Y'},
            {"00111110", 'Z'},
            {"00111111", '.'},
            {"01000000", '/'}};

        string txt = "";
        for (int i = 0; i < s.length(); i += 8)
        {
            string ch = "";
            ch += s[i];
            ch += s[i + 1];
            ch += s[i + 2];
            ch += s[i + 3];
            ch += s[i + 4];
            ch += s[i + 5];
            ch += s[i + 6];
            ch += s[i + 7];
            txt += mp[ch];
        }
        return txt;
    }

    void generateKeys()
    {
        for (int i = 0; i < 16; i++)
        {
            string roundKey = "";
            srand((unsigned)time(NULL) * (i + 1));
            int random;
            for (int j = 0; j < 48; j++)
            {
                random = rand();
                if ((random - j) % 2 == 0)
                {
                    roundKey += "0";
                }
                else
                {
                    roundKey += "1";
                }
            }
            roundKeys[i] = roundKey;
        }
    }

    void reverseKeys()
    {
        int i = 15;
        int j = 0;
        while (i > j)
        {
            string temp = roundKeys[i];
            roundKeys[i] = roundKeys[j];
            roundKeys[j] = temp;
            i--;
            j++;
        }
    }

    string DES(string text)
    {
        string left = text.substr(0, 32);
        string right = text.substr(32, 32);

        for (int i = 0; i < 16; i++)
        {
            string right_expanded = "";
            for (int j : expansionTable)
            {
                right_expanded += right[j - 1];
            }

            string xored = XOR(roundKeys[i], right_expanded);
            string res = "";
            for (int k = 0; k < 8; k++)
            {
                int row = binToDec(xored.substr(k * 6, 1) + xored.substr(k * 6 + 5, 1));
                int col = binToDec(xored.substr(k * 6 + 1, 1) + xored.substr(k * 6 + 2, 1) +
                                   xored.substr(k * 6 + 3, 1) + xored.substr(k * 6 + 4, 1));
                res += decToBin(substitutionBoxes[k][row][col]);
            }

            xored = XOR(res, left);
            left = xored;
            if (i < 15)
            {
                string temp = right;
                right = xored;
                left = temp;
            }
        }

        return left + right;
    }

private:
    string decToBin(int dec)
    {
        string bin;
        while (dec != 0)
        {
            bin = (dec % 2 == 0 ? "0" : "1") + bin;
            dec = dec / 2;
        }
        while (bin.length() < 4)
        {
            bin = "0" + bin;
        }
        return bin;
    }

    int binToDec(string bin)
    {
        int dec = 0;
        int counter = 0;
        for (int i = bin.length() - 1; i >= 0; i--)
        {
            if (bin[i] == '1')
            {
                dec += pow(2, counter);
            }
            counter++;
        }
        return dec;
    }

    string XOR(string bin1, string bin2)
    {
        string result = "";
        for (int i = 0; i < bin2.size(); i++)
        {
            if (bin1[i] != bin2[i])
            {
                result += "1";
            }
            else
            {
                result += "0";
            }
        }
        return result;
    }

    void defineTables()
    {
        for (int &i : expansionTable)
        {
            i = rand() % 32 + 1;
        }

        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                for (int k = 0; k < 16; k++)
                {
                    substitutionBoxes[i][j][k] = rand() % 32;
                }
            }
        }
    }
};