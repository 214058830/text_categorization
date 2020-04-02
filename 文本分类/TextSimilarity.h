#pragma once
#include <cppjieba/jieba.hpp>
class TextSimilarity
{
public:
	typedef std::unordered_map<std::string, int> wordFreq;
	typedef std::unordered_set<std::string> wordSet;
	TextSimilarity(const char* file1, const char* file2);
	void getTextSimilarity(const char* file1, const char* file2);
private:
	wordSet getSumWordSet(wordSet& first, wordSet& second);
	void getStopWordTable(const char* stopWordFile);
	wordFreq getWordFreq(const char* file);
	std::string UTF8ToGBK(std::string str);
	std::string GBKToUTF8(std::string str);
	std::vector<std::pair<std::string, int>> sortByValueReverse(wordFreq& wf);
	void selectAimWords(std::vector<std::pair<std::string, int>> wfvec, wordSet& wset, int num = 10);
	std::vector<int> getOneHot(wordSet& wset, wordFreq& wf);
	double cosine(std::vector<int>& oneHot1, std::vector<int>& oneHot2);

	std::string DICT;
	std::string DICT_PATH;
	std::string HMM_PATH;
	std::string USER_DICT_PATH;
	std::string IDF_PATH;
	std::string STOP_WORD_PATH;
	cppjieba::Jieba _jieba;

	wordSet _stopWordSet;  
	int _maxWordNumber;
};