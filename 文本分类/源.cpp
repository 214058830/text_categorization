#include "cppjieba/Jieba.hpp"
#include"TextSimilarity.h"
using namespace std;

int main(int argc, char** argv) {
	TextSimilarity s("./1.txt", "./2.txt");
	//TextSimilarity s(argv[1], argv[2]);

	return 0;
}
