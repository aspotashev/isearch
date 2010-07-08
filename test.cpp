#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

vector<pair<string, int> > db;

int main()
{
	string input;

	ifstream fin("input");
	while (!fin.eof())
	{
		string tmp;
		fin >> tmp;
		input += tmp;
	}
	fin.close();

	cout << "Input size: " << input.size() / 1000000.0 << " MB" << endl;

	for (int i = 0; i < 100000 /*((int)input.size() - 1)*/; i ++)
	{
		string s = input.substr(i, 20);
		db.push_back(make_pair(s, i));
	}

//	map<string, int>::iterator iter = db.lower_bound(string("Hello"));
//	for (int i = 0; i < 5; i ++, iter ++)
//	{
//		cout << "key: " << iter->first << endl;
//	}

	return 0;
}

