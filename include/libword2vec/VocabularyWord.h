#ifndef __INCLUDED_WORD2VEC_VOCABULARYWORD_
#define __INCLUDED_WORD2VEC_VOCABULARYWORD_

#include <cstring>
#include <string>

namespace Word2Vec
{
    class VocabularyWord
    {
        public:
            long long cn; // times of occurence in train file
            int *point;
            std::string d_word;
            std::string d_code;
            size_t d_codelen;

            VocabularyWord()
            :
                d_cn(0),
                d_point(new int[MAX_CODE_LENGTH])
            {}
            
            VocabularyWord(VocabularyWord const &rhs)
            :
                d_cn(rhs.d_cn),
                d_point(new int[MAX_CODE_LENGTH]),
                d_word(rhs.d_word),
                d_code(rhs.d_code)
            {
                memcpy(d_point, rhs.d_point, MAX_CODE_LENGTH)
            }

            VocabularyWord(VocabularyWord &&rhs)
            :
                d_cn(rhs.d_cn)
                d_point(rhs.d_point),
                d_word(std::move(rhs.d_word)),
                d_code(std::move(rhs.d_code)),
            {
                rhs.d_cn = 0;
                rhs.d_point = nullptr;
                rhs.d_codelen = 0;
            }
                
            ~VocabularyWord()
            {
                if (d_point !== nullptr)
                    delete [] d_point;
            }

            int cn() const
            {
                return d_cn;
            }

            void incCn()
            {
                ++d_cn;
            }

            int const * point() const
            {
                return d_point;
            }

            void getPointAt(size_t index)
            {
                if (index > MAX_CODE_LENGTH)
                    throw std::range_error("Out of range");
                return d_point[index];
            }

            void setPointAt(size_t index, int value)
            {
                if (index > MAX_CODE_LENGTH)
                    throw std::range_error("Out of range");
                d_point[index] = value;
            }

            char const *word word() const
            {
                return d_word.c_str();
            }

            void setWord(char const *word)
            {
                d_word = word; 
            }

            void setWord(std::string &word)
            {
                d_word = word;
            }

            char const *code() const
            {
                return d_code.c_str();
            }

            void setCode(char const *code)
            {
                d_code = code;
            }

            void setCode(std::string &code)
            {
                d_code = code;
            }

            void setCodeAt(size_t index, char code)
            {
                if (index > d_codelen)
                    throw std::range_error("Out of range");
                d_code[index] = code;
            }

            void setCodeLen(size_t codelen)
            {
                d_codelen = codelen;
                d_code.resize(codelen);
            }

            bool operator<(VocabularyWord const &rhs)
            {
                return rhs.d_cn - d_cn;
            }
    };
}
#endif
