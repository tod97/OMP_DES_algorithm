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
            {'A', "00001010"},
            {'B', "00001011"},
            {'C', "00001100"},
            {'D', "00001101"},
            {'E', "00001110"},
            {'F', "00001111"},
            {'G', "00010000"},
            {'H', "00010001"},
            {'I', "00010010"},
            {'J', "00010011"},
            {'K', "00010100"},
            {'L', "00010101"},
            {'M', "00010110"},
            {'N', "00010111"},
            {'O', "00011000"},
            {'P', "00011001"},
            {'Q', "00011010"},
            {'R', "00011011"},
            {'S', "00011100"},
            {'T', "00011101"},
            {'U', "00011110"},
            {'V', "00011111"},
            {'W', "00100001"},
            {'X', "00100010"},
            {'Y', "00100011"},
            {'Z', "00100100"}};

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
            {"00001010", 'A'},
            {"00001011", 'B'},
            {"00001100", 'C'},
            {"00001101", 'D'},
            {"00001110", 'E'},
            {"00001111", 'F'},
            {"00010000", 'G'},
            {"00010001", 'H'},
            {"00010010", 'I'},
            {"00010011", 'J'},
            {"00010100", 'K'},
            {"00010101", 'L'},
            {"00010110", 'M'},
            {"00010111", 'N'},
            {"00011000", 'O'},
            {"00011001", 'P'},
            {"00011010", 'Q'},
            {"00011011", 'R'},
            {"00011100", 'S'},
            {"00011101", 'T'},
            {"00011110", 'U'},
            {"00011111", 'V'},
            {"00100001", 'W'},
            {"00100010", 'X'},
            {"00100011", 'Y'},
            {"00100100", 'Z'}};

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