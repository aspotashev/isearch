#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<pair<string, int> > db;
vector<int> db_i;

bool myfunction(int i, int j)
{
	cout << "i = " << i << ", j = " << j << endl;
	return true;
}

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

	for (int i = 0; i < 1*1000*1000 /*(int)input.size()*/; i ++)
	{
		string s = input.substr(i, 20);
		db.push_back(make_pair(s, i));
	}

	sort(db.begin(), db.end());
	for (int i = 0; i < (int)db.size(); i ++)
	{
		db_i.push_back(db[i].second);
	}

	vector<int>::iterator iter = lower_bound(db_i.begin(), db_i.end(), -1, myfunction);
	for (int i = 0; i < 5; i ++, iter ++)
	{
		cout << "res: " << *iter << "      ---> " << input.substr(*iter, 20) << endl;
	}

//	map<string, int>::iterator iter = db.lower_bound(string("Hello"));
//	for (int i = 0; i < 5; i ++, iter ++)
//	{
//		cout << "key: " << iter->first << endl;
//	}

	return 0;
}

