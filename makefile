CC = gcc
#The -Ofast might not work with older versions of gcc; in that case, use -O2
CFLAGS = -lm -pthread -Ofast -march=native -Wall  -Wno-unused-result -funroll-loops -g
#

all: word2gram word2vec word2phrase distance word-analogy compute-accuracy compute-accuracy-syntax 

word2gram : word2gram.c
	$(CC) word2gram.c vocab.c trainingThread.c  ngram_tools.c -o word2gram $(CFLAGS)
word2vec : word2vec.c
	$(CC) word2vec.c vocab.c trainingThread.c -o word2vec $(CFLAGS)
word2phrase : word2phrase.c
	$(CC) word2phrase.c -o word2phrase $(CFLAGS)
distance : distance.c
	$(CC) distance.c -o distance $(CFLAGS)
word-analogy : word-analogy.c
	$(CC) word-analogy.c -o word-analogy $(CFLAGS)
compute-accuracy : compute-accuracy.c
	$(CC) compute-accuracy.c -o compute-accuracy $(CFLAGS)
compute-accuracy-syntax : compute-accuracy-syntax.c
	$(CC) compute-accuracy-syntax.c -o compute-accuracy-syntax $(CFLAGS)
	chmod +x *.sh

clean:
	rm -rf word2vec word2gram word2phrase distance word-analogy compute-accuracy compute-accuracy-syntax