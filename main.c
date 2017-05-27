#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "alsmap.h"

#define MAX_ITEMS 500000

typedef struct{
	size_t key;
	void* value;
}Pair;

void randTest(){
	srand(19);
	
	Pair items[MAX_ITEMS];
	int i;
	ALSHashMap* map = ALSHashMap_New();
	printf("Inserting\n");
	for (i=0; i<MAX_ITEMS; i++){
		items[i].key = (size_t)(i*10 + rand()%10);
		items[i].value = (void*)rand();
		ALSHashMap_Set(map, items[i].key, items[i].value);
	}

	ALSHashMap_PrintStats(map);
	printf("Deleting\n");
	for (i=0; i<MAX_ITEMS/2; i++){
		int index = rand()%MAX_ITEMS;
		if (index < 0) index*=-1;
		if (index == 0) index = 1;
		void* val = ALSHashMap_Del(map, items[index].key);
		if (val != items[index].value || ALSHashMap_Get(map, items[index].key) != NULL){
			printf("Deletion FAILURE: <%zu, %p> -> <%zu, %p>\n",
				items[index].key, items[index].value, items[index].key, val);
		}
		items[index].value = NULL;
	}
	printf("Searching\n");
	for (i=0; i<MAX_ITEMS; i++){
		if (items[i].value == NULL) continue;
		void* val = ALSHashMap_Get(map, items[i].key);
		if (val != items[i].value){
			printf("Lookup FAILURE: <%zu, %p> != <%zu, %p>\n",
				items[i].key, items[i].value, items[i].key, val);
		}
	}
	
	//ALSHashMap_PrintStats(map);
	ALSHashMap_Destroy(map);
}

void stringTest(){
	static Pair tests[] = {
		{285909807262152150, "__add__"},
		{-7432373563400207827, "__class__"},
		{-3864244867600433497, "__contains__"},
		{-21206928619717391, "__delattr__"},
		{-8518757509529533123, "__doc__"},
		{2177041475305762140, "__eq__"},
		{8362317231626074027, "__format__"},
		{177011475287762302, "__ge__"},
		{-4016074044355204212, "__getattribute__"},
		{-2585418108064572858, "__getitem__"},
		{7834477386492485275, "__getnewargs__"},
		{4753113843515500806, "__getslice__"},
		{177028475415762519, "__gt__"},
		{5317119094262461816, "__hash__"},
		{243017754987892126, "__init__"},
		{9177214476136763251, "__le__"},
		{5277902763571881278, "__len__"},
		{9177197476008762906, "__lt__"},
		{7048710602605022567, "__mod__"},
		{-5397995471050529035, "__mul__"},
		{7177204476118763201, "__ne__"},
		{3717045426703639461, "__new__"},
		{8370432368919977356, "__reduce__"},
		{-8813356612065597533, "__reduce_ex__"},
		{7914004857907398901, "__repr__"},
		{8224150437717880858, "__rmod__"},
		{4670584510141430808, "__rmul__"},
		{1862642071337639586, "__setattr__"},
		{7819592757661739418, "__sizeof__"},
		{3406273213427680266, "__str__"},
		{4590790060814328915, "__subclasshook__"},
		{2870999192938818550, "_formatter_field_name_split"},
		{-1544660614358752326, "_formatter_parser"},
		{1399668705107177948, "capitalize"},
		{-1095339294798586395, "center"},
		{7943899258573201248, "count"},
		{2194059052396695344, "decode"},
		{4408087139288664376, "encode"},
		{7717545679810809234, "endswith"},
		{-2162370607129298578, "expandtabs"},
		{-509531383349783523, "find"},
		{2880483376040711751, "format"},
		{-8139544016919160671, "index"},
		{-1891891588800347354, "isalnum"},
		{-1891913589055347917, "isalpha"},
		{-1094508681242454120, "isdigit"},
		{6662203971650579192, "islower"},
		{-3264133018777589699, "isspace"},
		{4865563698166163645, "istitle"},
		{6296712317920652027, "isupper"},
		{-2338040935105240732, "join"},
		{756173091232232219, "ljust"},
		{-2243981909596768798, "lower"},
		{5246082140489123976, "lstrip"},
		{1893173574489465835, "partition"},
		{-475611394924150513, "replace"},
		{8566273963665974510, "rfind"},
		{-295205302757070552, "rindex"},
		{2119545889811422497, "rjust"},
		{-7632506038852954866, "rpartition"},
		{-7478859825809313188, "rsplit"},
		{6967846247828238390, "rstrip"},
		{-1666158750398020171, "split"},
		{6545242454319875859, "splitlines"},
		{-9054588884789670847, "startswith"},
		{2334087250953981719, "strip"},
		{-5944333514074386483, "swapcase"},
		{-9092798511318848473, "title"},
		{-4075548806044035269, "translate"},
		{121890850537711745, "upper"},
		{7057264582011549012, "zfill"},
	};
	int i;
	ALSHashMap* map = ALSHashMap_New();
	printf("Inserting\n");
	for (i=0; i<sizeof(tests)/sizeof(Pair); i++){
		ALSHashMap_Set(map, tests[i].key, tests[i].value);
	}

	ALSHashMap_PrintStats(map);
	printf("Searching\n");
	for (i=0; i<sizeof(tests)/sizeof(Pair); i++){
		if (tests[i].value == NULL) continue;
		void* val = ALSHashMap_Get(map, tests[i].key);
		if (val != tests[i].value){
			printf("Lookup FAILURE: <%zu, %p> != <%zu, %p>\n",
				tests[i].key, tests[i].value, tests[i].key, val);
		}
	}
	printf("Deleting\n");
	for (i=0; i<sizeof(tests)/sizeof(Pair); i++){
		void* val = ALSHashMap_Del(map, tests[i].key);
		if (val != tests[i].value || ALSHashMap_Get(map, tests[i].key) != NULL){
			printf("Deletion FAILURE: <%zu, %p> -> <%zu, %p>\n",
				tests[i].key, tests[i].value, tests[i].key, val);
		}
		tests[i].value = NULL;
	}
	
	ALSHashMap_Destroy(map);
}

int main(int argc, char** argv){
	//stringTest();
	randTest();
	return 0;
}
