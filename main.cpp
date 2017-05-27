#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unordered_map>

#define MAX_ITEMS 200000

typedef struct{
	size_t key;
	void* value;
}Pair; 

void printStats(std::unordered_map<size_t, void*> &map){
	printf("T: %zu\n", map.bucket_count());
	printf("N: %zu\n", map.size());
	printf("Load Factor: %f\n", map.load_factor());
}

void randTest(){
	srand(19);
	
	Pair items[MAX_ITEMS];
	int i;
	std::unordered_map<size_t, void*> map;
	
	for (i=0; i<MAX_ITEMS; i++){
		items[i].key = (size_t)(i*10 + rand()%10);
		items[i].value = (void*)rand();
		map.emplace(items[i].key, items[i].value);
	}

	//printStats(map);

	for (i=0; i<MAX_ITEMS/2; i++){
		int index = rand()%MAX_ITEMS;
		map.erase(items[index].key);
		items[index].value = NULL;
	}

	for (i=0; i<MAX_ITEMS; i++){
		if (items[i].value == NULL) continue;
		void* val = map.at(items[i].key);
		if (val != items[i].value){
			printf("Lookup FAILURE: <%zu, %p> != <%zu, %p>\n",
				items[i].key, items[i].value, items[i].key, val);
		}
	}
}

void stringTest(){
	static Pair tests[] = {
		{(size_t)285909807262152150, (void*)"__add__"},
		{(size_t)-7432373563400207827, (void*)"__class__"},
		{(size_t)-3864244867600433497, (void*)"__contains__"},
		{(size_t)-21206928619717391, (void*)"__delattr__"},
		{(size_t)-8518757509529533123, (void*)"__doc__"},
		{(size_t)2177041475305762140, (void*)"__eq__"},
		{(size_t)8362317231626074027, (void*)"__format__"},
		{(size_t)177011475287762302, (void*)"__ge__"},
		{(size_t)-4016074044355204212, (void*)"__getattribute__"},
		{(size_t)-2585418108064572858, (void*)"__getitem__"},
		{(size_t)7834477386492485275, (void*)"__getnewargs__"},
		{(size_t)4753113843515500806, (void*)"__getslice__"},
		{(size_t)177028475415762519, (void*)"__gt__"},
		{(size_t)5317119094262461816, (void*)"__hash__"},
		{(size_t)243017754987892126, (void*)"__init__"},
		{(size_t)9177214476136763251, (void*)"__le__"},
		{(size_t)5277902763571881278, (void*)"__len__"},
		{(size_t)9177197476008762906, (void*)"__lt__"},
		{(size_t)7048710602605022567, (void*)"__mod__"},
		{(size_t)-5397995471050529035, (void*)"__mul__"},
		{(size_t)7177204476118763201, (void*)"__ne__"},
		{(size_t)3717045426703639461, (void*)"__new__"},
		{(size_t)8370432368919977356, (void*)"__reduce__"},
		{(size_t)-8813356612065597533, (void*)"__reduce_ex__"},
		{(size_t)7914004857907398901, (void*)"__repr__"},
		{(size_t)8224150437717880858, (void*)"__rmod__"},
		{(size_t)4670584510141430808, (void*)"__rmul__"},
		{(size_t)1862642071337639586, (void*)"__setattr__"},
		{(size_t)7819592757661739418, (void*)"__sizeof__"},
		{(size_t)3406273213427680266, (void*)"__str__"},
		{(size_t)4590790060814328915, (void*)"__subclasshook__"},
		{(size_t)2870999192938818550, (void*)"_formatter_field_name_split"},
		{(size_t)-1544660614358752326, (void*)"_formatter_parser"},
		{(size_t)1399668705107177948, (void*)"capitalize"},
		{(size_t)-1095339294798586395, (void*)"center"},
		{(size_t)7943899258573201248, (void*)"count"},
		{(size_t)2194059052396695344, (void*)"decode"},
		{(size_t)4408087139288664376, (void*)"encode"},
		{(size_t)7717545679810809234, (void*)"endswith"},
		{(size_t)-2162370607129298578, (void*)"expandtabs"},
		{(size_t)-509531383349783523, (void*)"find"},
		{(size_t)2880483376040711751, (void*)"format"},
		{(size_t)-8139544016919160671, (void*)"index"},
		{(size_t)-1891891588800347354, (void*)"isalnum"},
		{(size_t)-1891913589055347917, (void*)"isalpha"},
		{(size_t)-1094508681242454120, (void*)"isdigit"},
		{(size_t)6662203971650579192, (void*)"islower"},
		{(size_t)-3264133018777589699, (void*)"isspace"},
		{(size_t)4865563698166163645, (void*)"istitle"},
		{(size_t)6296712317920652027, (void*)"isupper"},
		{(size_t)-2338040935105240732, (void*)"join"},
		{(size_t)756173091232232219, (void*)"ljust"},
		{(size_t)-2243981909596768798, (void*)"lower"},
		{(size_t)5246082140489123976, (void*)"lstrip"},
		{(size_t)1893173574489465835, (void*)"partition"},
		{(size_t)-475611394924150513, (void*)"replace"},
		{(size_t)8566273963665974510, (void*)"rfind"},
		{(size_t)-295205302757070552, (void*)"rindex"},
		{(size_t)2119545889811422497, (void*)"rjust"},
		{(size_t)-7632506038852954866, (void*)"rpartition"},
		{(size_t)-7478859825809313188, (void*)"rsplit"},
		{(size_t)6967846247828238390, (void*)"rstrip"},
		{(size_t)-1666158750398020171, (void*)"split"},
		{(size_t)6545242454319875859, (void*)"splitlines"},
		{(size_t)-9054588884789670847, (void*)"startswith"},
		{(size_t)2334087250953981719, (void*)"strip"},
		{(size_t)-5944333514074386483, (void*)"swapcase"},
		{(size_t)-9092798511318848473, (void*)"title"},
		{(size_t)-4075548806044035269, (void*)"translate"},
		{(size_t)121890850537711745, (void*)"upper"},
		{(size_t)7057264582011549012, (void*)"zfill"},
	};
	int i;
	std::unordered_map<size_t, void*> map;
	for (i=0; i<sizeof(tests)/sizeof(Pair); i++){
		map.emplace(tests[i].key, tests[i].value);
	}

	printStats(map);
	
	for (i=0; i<sizeof(tests)/sizeof(Pair); i++){
		if (tests[i].value == NULL) continue;
		void* val = map.at(tests[i].key);
		if (val != tests[i].value){
			printf("Lookup FAILURE: <%zu, %p> != <%zu, %p>\n",
				tests[i].key, tests[i].value, tests[i].key, val);
		}
	}
	
	for (i=0; i<sizeof(tests)/sizeof(Pair); i++){
		map.erase(tests[i].key);
		tests[i].value = NULL;
	}
}

int main(int argc, char** argv){
	//stringTest();
	randTest();
	return 0;
}
