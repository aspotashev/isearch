#include <assert.h>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include <iconv.h>


typedef unsigned short int char_t;

std::vector<int> db; // main index
char_t *input;
int input_sz; // in characters
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
	int i = 0;
	for (i = 0; i < 200; i ++)
	{
		if (a[i] != b[i]) break;
	}

	if (a[i] < b[i])
	{
		return true;
	}
	else if (a[i] > b[i])
	{
		return false;
	}
	else
	{
		return false;
	}
}

bool compare_input_substrings(int i, int j)
{
	printf("i = %d, j = %d\n", i, j);

	if (!(i >= 0 && i < input_sz))
	{
		printf("------------------\n");
		assert(0);
	}
	if (!(j >= 0 && j < input_sz))
	{
		printf("------------------\n");
		assert(0);
	}
	fflush(stdout);


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

	printf("sz = %d\n", db.size());


	for (std::vector<int>::iterator i = db.begin(); i != db.end(); i ++)
	{
		assert(*i < 1000);
	}

	sort(db.begin(), db.end(), compare_input_substrings);

	for (int i = 0; i < input_sz - 1; i ++)
	{
		if (compare_input_substrings(db[i+1], db[i]))
		{
//			printf("i = %d, i+1 = %d, input[i] = %04x, input[i+1] = %04x\n", i, i+1, input[i], input[i+1]);
			assert(0);
		}
	}
}

const char *index_to_string_id(int index)
{
	std::map<int, std::string>::iterator it = pos_to_src.upper_bound(index);
	it --;
	return it->second.c_str();
}

//-------------------------------------------------------

char_t *search_pattern_string;

bool compare_for_binary_search(int i, int j)
{
	assert(j == -1);

	return compare_unicode_strings(input + i, search_pattern_string);
}

int find_index_by_string(char *s)
{
	char_t s_unicode[202];

// encode to UCS-2
	size_t inbytesleft = 10000000;
	size_t outbytesleft = 200;
	char *input_part = s;
	char *output_part = (char *)s_unicode;

	iconv_t cd = iconv_open("UCS-2", "UTF-8");
	iconv(cd, &input_part, &inbytesleft, &output_part, &outbytesleft);
	iconv_close(cd);

	*output_part = '\0';
//----------------

	search_pattern_string = s_unicode;
	std::vector<int>::iterator iter = lower_bound(db.begin(), db.end(), -1, compare_for_binary_search);
	return iter - db.begin();
}

