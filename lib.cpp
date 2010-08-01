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

bool compare_unicode_strings(const char_t *a, const char_t *b)
{
	if (*a < *b)
	{
		return true;
	}
	else if (*b > *a)
	{
		return false;
	}
	else
	{
		return compare_unicode_strings(a + 1, b + 1);
	}

//	return wcscmp(input + i, input + j) < 0;
}

bool compare_input_substrings(int i, int j)
{
	if (i == j)
		return false;

	return compare_unicode_strings(input + i, input + j);
}

void init_search()
{
	read_dump();
	read_dump_map();

	for (int i = 0; i < input_sz; i ++)
	{
		db.push_back(i);
	}

	sort(db.begin(), db.end(), compare_input_substrings);

	for (int i = 0; i < input_sz - 1; i ++)
	{
//		assert(!compare_input_substrings(i+1, i));
	}
}

const char *index_to_string_id(int index)
{
	std::map<int, std::string>::iterator it = pos_to_src.upper_bound(index);
	it --;
	return it->second.c_str();
}

