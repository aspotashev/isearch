#include <assert.h>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include <iconv.h>
#include <sys/mman.h>
#include <fcntl.h>


typedef unsigned short int char_t;


class CmpUnicodeBase
{
protected:
	// true  -- a < b
	// false -- a > b
	bool compare_unicode_strings(const char_t *a, const char_t *b, int max_length)
	{
		int i = 0;

		if (max_length > 200) // for safety
			max_length = 200;

		for (i = 0; i < max_length; i ++)
		{
			if (a[i] != b[i]) break;
		}

		if (i == max_length)
		{
			// Compare pointers. By doing so, a string in
			// the end of the input becomes "smaller" that
			// the one in the middle of the input.
			// It imitates a zero character at the end
			// of the input.
			return b < a;
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
};


class CmpForBinarySearch : public CmpUnicodeBase
{
public:
	CmpForBinarySearch(char_t *input, int input_sz, char_t *search_pattern_string):
		m_input(input),
		m_input_sz(input_sz),
		m_search_pattern_string(search_pattern_string)
	{
	}

	int search_pattern_length()
	{
		int len = 0;

		while (m_search_pattern_string[len] != 0)
			len ++;

		return len;
	}

	bool operator() (int i, int j)
	{
		assert(j == -1);

		return compare_unicode_strings(m_input + i, m_search_pattern_string,
			std::min(search_pattern_length(), m_input_sz - i));
	}

private:
	char_t *m_input;
	int m_input_sz; // in characters
	char_t *m_search_pattern_string;
};

class CmpInputSubstrings : public CmpUnicodeBase
{
public:
	CmpInputSubstrings(char_t *input, int input_sz):
		m_input(input),
		m_input_sz(input_sz)
	{
	}

	bool operator() (int i, int j)
	{
	//	printf("i = %d, j = %d\n", i, j);

		if (!(i >= 0 && i < m_input_sz))
		{
			printf("------------------\n");
			assert(0);
		}
		if (!(j >= 0 && j < m_input_sz))
		{
			printf("------------------\n");
			assert(0);
		}


		if (i == j)
			return false;

		return compare_unicode_strings(m_input + i, m_input + j,
			std::min(m_input_sz - i, m_input_sz - j));
	}

private:
	char_t *m_input;
	int m_input_sz; // in characters
};

class ISearch
{
public:

	void init_search(const char *f_dump, const char *f_index, const char *f_map);
	const char *find_string_id_by_str(char *s);
	int get_internal_index_by_string(char *s);
	const char *get_msg_id_by_internal_index(int index);

private:
	long get_file_size(const char *filename);
	void read_dump(const char *f_dump);
	void read_dump_map(const char *f_map);
	const char *index_to_string_id(int index);
	int find_index_by_string(char *s);

	void check_search_index_correctness(int *check_db);
	bool file_exists(const char *filename);
	void create_search_index(const char *f_index);
	void *map_file(const char *filename);
	void setup_search_index(const char *f_index);

private:
	char_t *input;
	int input_sz; // in characters

	int *db; // main index
	std::map<int, std::string> pos_to_src;
};


//------------------- ISearch: methods ----------------

long ISearch::get_file_size(const char *filename)
{
	FILE *f = fopen(filename, "rb");
	assert(f);

	fseek(f, 0, SEEK_END);
	long res = ftell(f);

	fclose(f);


	return res;
}

void ISearch::read_dump(const char *f_dump)
{
	long sz = get_file_size(f_dump);
	assert(sz % 2 == 0);
	input_sz = (int)(sz / 2);


//	FILE *f = fopen(f_dump, "rb");
//	assert(f);
//
//	input = new char_t[input_sz];
//	assert(fread(input, 1, sz, f) == sz);
//
//	fclose(f);


	// use mmap instead of reading the file
	input = (char_t *)map_file(f_dump);
}

void ISearch::read_dump_map(const char *f_map)
{
	FILE *f = fopen(f_map, "r");

	int pos;
	char s[200];
	while(1)
	{
		int n_read = fscanf(f, "%d %s", &pos, s);
		if (feof(f))
			break;
		assert(n_read == 2);

		pos_to_src[pos] = std::string(s);
	}

	fclose(f);
}

// check that the substrings go in alphabetical order
void ISearch::check_search_index_correctness(int *check_db = NULL)
{
	if (check_db == NULL)
	{
		check_db = db; // workaround for default argument
	}

	CmpInputSubstrings comparator(input, input_sz);
	for (int i = 0; i < input_sz - 1; i ++)
	{
		if (comparator(check_db[i+1], check_db[i]))
		{
			printf("i = %d, i+1 = %d, check_db[i] = %d, check_db[i+1] = %d, first_char(i) = %04x, first_char(i+1) = %04x\n",
				i, i+1,
				check_db[i], check_db[i+1],
				input[check_db[i]], input[check_db[i+1]]);
			assert(0);
		}
	}
}

bool ISearch::file_exists(const char *filename)
{
	FILE *f = fopen(filename, "r");
	if (f)
	{
		fclose(f);
		return true;
	}
	else
	{
		return false;
	}
}

void ISearch::create_search_index(const char *f_index)
{
	int *temp_db = new int [input_sz];

	for (int i = 0; i < input_sz; i ++)
	{
		temp_db[i] = i;
	}

	std::sort(temp_db, temp_db + input_sz, CmpInputSubstrings(input, input_sz));

	check_search_index_correctness(temp_db);

	// write dump index to file
	FILE *f_w = fopen(f_index, "wb");
	assert(fwrite(temp_db, sizeof(int), input_sz, f_w) == input_sz);
	fclose(f_w);

	delete [] temp_db;
}

void *ISearch::map_file(const char *filename)
{
	int file_size = get_file_size(filename);

	int fd = open(filename, O_RDONLY);
	assert(fd != -1);
	return mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
}

void ISearch::setup_search_index(const char *f_index)
{
	if (!file_exists(f_index))
	{
		create_search_index(f_index);
	}

//	// read dump index from file
//	FILE *f = fopen(f_index, "rb");
//
//	db = new int [input_sz];
//	assert(fread(db, sizeof(int), input_sz, f) == input_sz);
//
//	fclose(f);


	// use mmap instead of reading the file
	db = (int *)map_file(f_index);


	check_search_index_correctness();
}

void ISearch::init_search(const char *f_dump, const char *f_index, const char *f_map)
{
	read_dump(f_dump);
	read_dump_map(f_map);

	setup_search_index(f_index);
}

const char *ISearch::index_to_string_id(int index)
{
	std::map<int, std::string>::iterator it = pos_to_src.upper_bound(index);
	it --;
	return it->second.c_str();
}

int ISearch::get_internal_index_by_string(char *s)
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

	int *iter = std::lower_bound(db, db + input_sz, -1, CmpForBinarySearch(input, input_sz, s_unicode));
	return iter - db;
}

const char *ISearch::get_msg_id_by_internal_index(int index)
{
	return index_to_string_id(db[index]);
}

int ISearch::find_index_by_string(char *s)
{
	return db[get_internal_index_by_string(s)];
}

const char *ISearch::find_string_id_by_str(char *s)
{
	return index_to_string_id(find_index_by_string(s));
}

//------------------- simple functions -----------------

ISearch isearch_instance;

void init_search(const char *f_dump, const char *f_index, const char *f_map)
{
	isearch_instance.init_search(f_dump, f_index, f_map);
}

const char *find_string_id_by_str(char *s)
{
	return isearch_instance.find_string_id_by_str(s);
}

int get_internal_index_by_string(char *s)
{
	return isearch_instance.get_internal_index_by_string(s);
}

const char *get_msg_id_by_internal_index(int index)
{
	return isearch_instance.get_msg_id_by_internal_index(index);
}

