#include <assert.h>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include <iconv.h>

using namespace std;

vector<int> db;

typedef short int char_t;

//bool myfunction(int i, int j)
//{
//	cout << "i = " << i << ", j = " << j << endl;
//	return input.substr(i, 20) < string("hello");
//}

char_t *input;
int input_sz;
map<int, string> pos_to_src;

void read_dump()
{
	FILE *f = fopen("../dump.dat", "rb");

	fseek(f, 0, SEEK_END);
	long sz = ftell(f);

	assert(sz % 2 == 0);
	input_sz = (int)(sz / 2);

	rewind(f);
	input = new char_t[input_sz];
	fread(input, 1, sz, f);

	fclose(f);
}

void read_dump_map()
{
	FILE *f = fopen("../dump-map.txt", "r");

	int pos;
	char s[200];
	while(1)
	{
		fscanf(f, "%d %s", &pos, s);
		if (feof(f))
			break;

		pos_to_src[pos] = string(s);
	}

	fclose(f);
}

bool compare_input_substrings(int i, int j)
{
	if (i == j)
		return false;

	if (input[i] < input[j])
	{
		return true;
	}
	else if (input[i] > input[j])
	{
		return false;
	}
	else
	{
		return compare_input_substrings(i + 1, j + 1);
	}

//	return wcscmp(input + i, input + j) < 0;
}

void print_unicode(void *s, int chars)
{
	char buf[200];

	size_t inbytesleft = 2*chars;
	size_t outbytesleft = 200;
	char *input_part = (char *)s;
	char *output_part = buf;

	iconv_t cd = iconv_open("UTF-8", "UCS-2");
	iconv(cd, &input_part, &inbytesleft, &output_part, &outbytesleft);
	iconv_close(cd);

	*output_part = '\0';

	printf("TEXT: %s", buf);
	printf("     ");
	for (int i = 0; i < 10; i ++)
	{
		printf("%04x ", ((char_t *)s)[i]);
	}

	int index = (char_t *)s - input;
	map<int, string>::iterator it = pos_to_src.upper_bound(index);
	printf("     %d, %s", it->first, it->second.c_str());

	printf("\n");
}

int main()
{
	read_dump();

	read_dump_map();

	cout << "Input size: " << input_sz << " chars" << endl;

	for (int i = 0; i < input_sz; i ++)
	{
		db.push_back(i);
	}

	sort(db.begin(), db.end(), compare_input_substrings);
	for (int i = 0; i < input_sz - 1; i ++)
	{
//		assert(!compare_input_substrings(i+1, i));
	}

	for (int i = 0; i < input_sz; i ++)
	{
		print_unicode(input + db[i], 20);
	}

//	vector<int>::iterator iter = lower_bound(db_i.begin(), db_i.end(), -1, myfunction);
//	for (int i = 0; i < 5; i ++, iter ++)
//	{
//		cout << "res: " << *iter << "      ---> " << input.substr(*iter, 20) << endl;
//	}

//	map<string, int>::iterator iter = db.lower_bound(string("Hello"));
//	for (int i = 0; i < 5; i ++, iter ++)
//	{
//		cout << "key: " << iter->first << endl;
//	}

	return 0;
}

