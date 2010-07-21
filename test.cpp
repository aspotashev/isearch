#include "lib.cpp"

using namespace std;

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
	printf("     %s", search_run(index));

	printf("\n");
}

int main()
{
	init_search();


	for (int i = 0; i < input_sz; i ++)
	{
		print_unicode(input + db[i], 20);
	}

	return 0;
}

