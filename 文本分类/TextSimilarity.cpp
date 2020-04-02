#include "TextSimilarity.h"
#include <Windows.h>
#include<fstream>
#include <string>
#include<vector>
#include <unordered_map>
#include <unordered_set>
#include<algorithm>

TextSimilarity::TextSimilarity(const char* file1, const char* file2)
	:_maxWordNumber(10),
	DICT("../cppjieba/dict"),
	DICT_PATH("../cppjieba/dict/jieba.dict.utf8"),
	HMM_PATH("../cppjieba/dict/hmm_model.utf8"),
	USER_DICT_PATH("../cppjieba/dict/user.dict.utf8"),
	IDF_PATH("../cppjieba/dict/idf.utf8"),
	STOP_WORD_PATH("../cppjieba/dict/stop_words.utf8"),
	_jieba(DICT_PATH,
	HMM_PATH,
	USER_DICT_PATH,
	IDF_PATH,
	STOP_WORD_PATH){

	std::vector<cppjieba::Word> jiebawords;
	getTextSimilarity(file1, file2);
}

std::string TextSimilarity::GBKToUTF8(std::string str) {
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t* res = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, res, len);
	len = WideCharToMultiByte(CP_UTF8, 0, res, -1, NULL, 0, NULL, NULL);
	char* cstr = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, res, -1, cstr, len, NULL, NULL);
	std::string s(cstr);
	delete[] cstr;
	return s;
}

std::string TextSimilarity::UTF8ToGBK(std::string str) {
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t* res = new wchar_t[len];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, res, len);
	len = WideCharToMultiByte(CP_ACP, 0, res, -1, NULL, 0, NULL, NULL);
	char* cstr = new char[len];
	WideCharToMultiByte(CP_ACP, 0, res, -1, cstr, len, NULL, NULL);
	std::string s(cstr);
	delete[] cstr;
	return s;
}

void TextSimilarity::getStopWordTable(const char* stopWordFile) {
	std::fstream out;
	out.open(stopWordFile);
	while (!out.eof())
	{
		std::string s;
		out >> s;
		_stopWordSet.insert(s);
	}
	out.close();
}
TextSimilarity::wordFreq TextSimilarity::getWordFreq(const char* file) {
	wordFreq freq;
	std::string s;
	std::fstream out;
	out.open(file);
	while (!out.eof())
	{
		out >> s;
		//std::cout << s <<  std::endl;
		std::vector<std::string> words;
		_jieba.Cut(GBKToUTF8(s), words, true);
		for (auto& e : words) {
			if (_stopWordSet.find(e) == _stopWordSet.end() && (UTF8ToGBK(e) != " ")) {
				if (freq.count(e) >= 1) {
					freq[e]++;
				}
				else {
					freq[e] = 1;
				}
			}
		}
	}
	out.close();
	return freq;
}
bool cmp(std::pair<std::string, int> par1, std::pair<std::string, int> par2) {
	if (par1.second < par2.second) {
		return false;
	}
	return true;
}
std::vector<std::pair<std::string, int>> TextSimilarity::sortByValueReverse(TextSimilarity::wordFreq& wf) {
	std::vector<std::pair<std::string, int>> s(wf.begin(), wf.end());
	sort(s.begin(), s.end(), cmp);
	return s;
}
void TextSimilarity::selectAimWords(std::vector<std::pair<std::string, int>> wfvec, TextSimilarity::wordSet& wset, int num) {
	//assert(wfvec.size() >= num);
	wfvec.resize(num);
	for (auto&e : wfvec) {
		wset.insert(e.first);
	}
}
TextSimilarity::wordSet TextSimilarity::getSumWordSet(TextSimilarity::wordSet& first, TextSimilarity::wordSet& second) {
	TextSimilarity::wordSet ret(first);
	ret.insert(second.begin(), second.end());
	return ret;
}
std::vector<int> TextSimilarity::getOneHot(TextSimilarity::wordSet& wset, TextSimilarity::wordFreq& wf) {
	std::vector<int> v;
	TextSimilarity::wordFreq::iterator pos;
	for (auto& e : wset) {
		if ((pos = wf.find(e)) != wf.end()) {
			v.push_back(pos->second);
		}
		else v.push_back(0);
	}
	return v;
}
double TextSimilarity::cosine(std::vector<int>& oneHot1, std::vector<int>& oneHot2) {
	double m = 0;
	double n1 = 0, n2 = 0;
	for (int pos = 0; pos != oneHot1.size(); ++pos) {
		m += (double)oneHot1[pos] * (double)oneHot2[pos];
		n1 += (double)oneHot1[pos] * (double)oneHot1[pos];
		n2 += (double)oneHot2[pos] * (double)oneHot2[pos];
	}
	return m/(sqrt(n1)*sqrt(n2));
}
void TextSimilarity::getTextSimilarity(const char* file1, const char* file2){
	getStopWordTable(STOP_WORD_PATH.c_str());
	wordFreq first = getWordFreq(file1);
	wordFreq second = getWordFreq(file2);
	std::vector<std::pair<std::string, int>> m = sortByValueReverse(first);
	std::vector<std::pair<std::string, int>> n = sortByValueReverse(second);
	wordSet first1;
	wordSet second2;
	selectAimWords(m, first1);
	selectAimWords(n, second2);
	wordSet third = getSumWordSet(first1, second2);
	while (m.size() > 15 && n.size() > 15 && third.size() < 20) {
		selectAimWords(m, first1, _maxWordNumber);
		selectAimWords(n, second2, _maxWordNumber);
		wordSet third = getSumWordSet(first1, second2);
		_maxWordNumber += 5;
	}
	std::vector<int> v1 = getOneHot(third, first);
	std::vector<int> v2 = getOneHot(third, second);

	//for (auto&e : third) {
	//	std::cout << TextSimilarity::UTF8ToGBK(e) << " "<< std::endl;
	//}
	//std::cout << std::endl;
	//for (auto&e : v1) {
	//	std::cout << e << " ";
	//}
	//std::cout << std::endl;
	//for (auto&e : v2) {
	//	std::cout << e << " ";
	//}
	//std::cout << std::endl;

	std::cout << "文本相似度为：" << cosine(v1, v2)*100 << "%" << std::endl;
}