#include "lib.h"

int main()
{
	init_search("../dump.dat", "../dump-map.txt");


//	for (int i = 0; i < input_sz; i ++)
//	{
//		print_unicode(input + db[i], 20);
//	}

	printf("start...\n");
	for (int i = 0; i < 1000 * 1000; i ++)
	{
		find_string_id_by_str("hofdskfj");
	}
	printf("...finish\n");

	for (int i = 0; i < 20; i ++)
	{
		printf("%s -> %s\n", "hofdskfj", find_string_id_by_str("hofdskfj"));
	}

	return 0;
}

