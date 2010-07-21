#include <assert.h>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include <iconv.h>


typedef short int char_t;

std::vector<int> db;
char_t *input;
int input_sz;
std::map<int, std::string> pos_to_src;

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

		pos_to_src[pos] = std::string(s);
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

