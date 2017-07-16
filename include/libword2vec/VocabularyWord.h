#ifndef __INCLUDED_WORD2VEC_VOCABULARYWORD_
#define __INCLUDED_WORD2VEC_VOCABULARYWORD_

#include <cstring>
#include <string>

#include "Parameters.h"

namespace Word2Vec
{
    class VocabularyWord
    {
        private:
            size_t d_cn; // times of occurence in train file
            int *d_point;
            char *d_word;
            char *d_code;
            size_t d_codelen;

            bool ma = false;
            bool mc = false;

        public:
            VocabularyWord(char const *word)
            :
                d_cn(0),
                d_point(new int[MAX_CODE_LENGTH]),
                d_code(nullptr),
                d_codelen(MAX_CODE_LENGTH)
            {
                unsigned l = strlen(word) + 1;
                if (l > MAX_STRING)
                    l = MAX_STRING;
                d_word = new char[l];
                strncpy(d_word, word, l);
                d_word[l - 1] = 0;
            }
            
            VocabularyWord(VocabularyWord const &rhs)
            :
                d_cn(rhs.d_cn),
                d_point(new int[MAX_CODE_LENGTH]),
                d_code(rhs.d_code)
            {
                memcpy(d_point, rhs.d_point, MAX_CODE_LENGTH);
                unsigned l = strlen(rhs.d_word) + 1;
                d_word = new char[l];
                std::strncpy(d_word, rhs.d_word, l);
                d_word[l - 1] = 0;
            }

            VocabularyWord(VocabularyWord &&rhs)
            :
                d_cn(rhs.d_cn),
                d_point(rhs.d_point),
                d_word(rhs.d_word),
                d_code(rhs.d_code)
            {
                rhs.d_cn = 0;
                rhs.d_word = nullptr;
                rhs.d_code = nullptr;
                rhs.d_point = nullptr;
                rhs.d_codelen = 0;
                rhs.mc = true;
            }

            VocabularyWord &operator=(VocabularyWord &&rhs)
            {
                d_cn = rhs.d_cn;
                d_point = rhs.d_point;
                rhs.d_point = nullptr;
                d_word = rhs.d_word;
                rhs.d_word = nullptr;
                d_code = rhs.d_code;
                rhs.d_code = nullptr;
                d_codelen = rhs.d_codelen;
                rhs.ma = true;
                return *this;
            }
                
            ~VocabularyWord()
            {
                if (d_point != nullptr)
                    delete [] d_point;
                if (d_word != nullptr)
                    delete [] d_word;
                if (d_code != nullptr)
                    delete [] d_code;
            }

            inline int cn() const
            {
                return d_cn;
            }

            inline void incCn()
            {
                ++d_cn;
            }

            inline void setCn(size_t cn)
            {
                d_cn = cn;
            }

            inline int const *point() const
            {
                return d_point;
            }

            inline int pointAt(size_t index)
            {
                return d_point[index];
            }

            inline void setPointAt(size_t index, int value)
            {
                d_point[index] = value;
            }

            inline char const *word() const
            {
                return d_word;
            }

            inline char const *code() const
            {
                return d_code;
            }

            inline char codeAt(int pos) const
            {
                return d_code[pos];
            }

            inline void setCode(char const *code)
            {
                if (d_code == nullptr)
                    d_code = new char[d_codelen];
                strncpy(d_code, code, d_codelen);
            }

            inline void setCodeAt(size_t index, char code)
            {
                if (d_code == nullptr)
                    d_code = new char[d_codelen];
                d_code[index] = code;
            }

            inline void setCodeLen(size_t codelen)
            {
                if (codelen != d_codelen && d_code != nullptr)
                {
                    char *new_ptr = new char[codelen];
                    strncpy(new_ptr, d_code, d_codelen);
                    delete [] d_code;
                    d_code = new_ptr;
                }
                else
                    d_code = new char[codelen];

                d_codelen = codelen;
            }

            inline size_t codeLen() const
            {
                return d_codelen;
            }

            inline bool operator<(VocabularyWord const &rhs) const
            {
                return rhs.d_cn < d_cn;
            }
    };
}
#endif
