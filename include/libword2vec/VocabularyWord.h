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
            std::string d_word;
            std::array<char, MAX_CODE_LENGTH> d_code;
            std::array<int, MAX_CODE_LENGTH> d_point;
            size_t d_codelen;


        public:
            VocabularyWord(std::string const &word)
            :
                d_cn(0),
                d_word(word),
                d_codelen(MAX_CODE_LENGTH)
            {}
            
            VocabularyWord(VocabularyWord const &rhs)
            :
                d_cn(rhs.d_cn),
                d_word(rhs.d_word),
                d_code(rhs.d_code),
                d_point(rhs.d_point),
                d_codelen(rhs.d_codelen)
            {}

            VocabularyWord(VocabularyWord &&rhs)
            :
                d_cn(rhs.d_cn),
                d_word(std::move(rhs.d_word)),
                d_code(std::move(rhs.d_code)),
                d_point(std::move(rhs.d_point)),
                d_codelen(rhs.d_codelen)
            {}

            VocabularyWord &operator=(VocabularyWord &&rhs)
            {
                d_cn = rhs.d_cn;
                d_point = std::move(rhs.d_point);
                d_word = std::move(rhs.d_word);
                d_code = std::move(rhs.d_code);
                d_codelen = rhs.d_codelen;
                return *this;
            }
                
            ~VocabularyWord()
            {}

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
                return d_word.c_str();
            }

            inline char codeAt(int pos) const
            {
                return d_code[pos];
            }

            inline void setCodeAt(size_t index, char code)
            {
                d_code[index] = code;
            }

            inline void setCodeLen(size_t codelen)
            {
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
