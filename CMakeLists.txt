cmake_minimum_required(VERSION 2.8.3)
project(libword2vec)

add_definitions("-std=c++11 -O3 -s -Wall")
set(CMAKE_BUILD_TYPE Debug)

find_package(Boost 1.58.0)

include_directories(include)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(word2vec src/word2vec.cc)
add_executable(distance src/distance.cc)
add_executable(word2phrase src/word2phrase.cc)
add_executable(DistanceServer src/Server/DistanceServer.cc)

target_link_libraries(word2vec
    WordModel.o
    TrainingThread.o
    Vocabulary.o
    NGrams.o
    -pthread
)

target_link_libraries(word2phrase
    Vocabulary.o
    -pthread
)

target_link_libraries(distance
    WordModel.o
    Vocabulary.o
)

target_link_libraries(DistanceServer
    HttpServer.o
    WordModel.o
    Vocabulary.o
    -lboost_system
    -pthread
)

###
# Old makefile follows
# SCRIPTS_DIR=../scripts
# CC = gcc
# #The -Ofast might not work with older versions of gcc; in that case, use -O2
# CFLAGS = -lm -pthread -march=native -Wall  -Wno-unused-result -funroll-loops -Ofast
# BIN_DIR=../bin
# 
# all: word2gram word2vec word2phrase distance word-analogy compute-accuracy compute-accuracy-syntax cleano
# 
# vocab.o : vocab.c vocab.h
# 	$(CC) -c vocab.c $(CFLAGS)
# ngram_tools.o : ngram_tools.c ngram_tools.h
# 	$(CC) -c ngram_tools.c $(CFLAGS)
# trainingThread.o : trainingThread.c trainingThread.h
# 	$(CC) -c trainingThread.c $(CFLAGS)
# word2gram : word2gram.c vocab.o ngram_tools.o trainingThread.o
# 	$(CC) word2gram.c vocab.o ngram_tools.o trainingThread.o -o ${BIN_DIR}/word2gram $(CFLAGS)
# word2vec : word2vec.c vocab.o ngram_tools.o trainingThread.o
# 	$(CC) word2vec.c vocab.o ngram_tools.o trainingThread.o -o ${BIN_DIR}/word2vec $(CFLAGS)
# word2phrase : word2phrase.c
# 	$(CC) word2phrase.c -o ${BIN_DIR}/word2phrase $(CFLAGS)
# distance : distance.c
# 	$(CC) distance.c -o ${BIN_DIR}/distance $(CFLAGS)
# word-analogy : word-analogy.c
# 	$(CC) word-analogy.c -o ${BIN_DIR}/word-analogy $(CFLAGS)
# compute-accuracy : compute-accuracy.c
# 	$(CC) compute-accuracy.c -o ${BIN_DIR}/compute-accuracy $(CFLAGS)
# compute-accuracy-syntax : compute-accuracy-syntax.c
# 	$(CC) compute-accuracy-syntax.c -o ${BIN_DIR}/compute-accuracy-syntax $(CFLAGS)
# 	chmod +x ${SCRIPTS_DIR}/*.sh
# 
# cleano: word2vec word2gram
# 	rm *.o
# 
# clean:
# 	pushd ${BIN_DIR} && rm -rf word2vec word2phrase distance word-analogy compute-accuracy; popd

add_library(
    NGrams.o
    src/NGram/addGramPosition.cc
    src/NGram/getGrams.cc
    src/NGram/writeGramVocabularyToWordVectors.cc
    src/NGram/minMaxGram.cc
    src/NGram/sumFreqGram.cc
    src/NGram/sumGram.cc
    src/NGram/truncGram.cc
    src/NGram/writeGrams.cc
)

add_library(
    TrainingThread.o
    src/TrainingThread/trainCBOWModel.cc
    src/TrainingThread/trainCBOWModelGram.cc
    src/TrainingThread/trainSKIPModel.cc
    src/TrainingThread/trainSKIPModelGram.cc
)

add_library(
    Vocabulary.o
    src/Vocabulary/addWord.cc
    src/Vocabulary/createBinaryTree.cc
    src/Vocabulary/destructor.cc
    src/Vocabulary/getWordHash.cc
    src/Vocabulary/readTrainFileNgram.cc
    src/Vocabulary/readTrainFile.cc
    src/Vocabulary/readVocabularyFile.cc
    src/Vocabulary/readWord.cc
    src/Vocabulary/readWordIndex.cc
    src/Vocabulary/reduce.cc
    src/Vocabulary/save.cc
    src/Vocabulary/searchAndAdd.cc
    src/Vocabulary/search.cc
    src/Vocabulary/sort.cc
    src/Vocabulary/Vocabulary.cc
)

add_library(
    WordModel.o
    src/WordModel/destructor.cc
    src/WordModel/initUnigramTable.cc
    src/WordModel/train.cc
    src/WordModel/WordModel.cc
    src/WordModel/saveTrainResults.cc
    src/WordModel/saveTrainResultsNgrams.cc
    src/WordModel/readWordModels.cc
    src/WordModel/findWords.cc
)

add_library(
    HttpServer.o
    src/Server/RequestHandler/constructor.cc
    src/Server/RequestHandler/handleRequest.cc
    src/Server/RequestHandler/url_decode.cc
    src/Server/RequestHandler/RequestHandler.ih
    src/Server/RequestParser/is_ctl.cc
    src/Server/RequestParser/constructor.cc
    src/Server/RequestParser/reset.cc
    src/Server/RequestParser/is_char.cc
    src/Server/RequestParser/is_digit.cc
    src/Server/RequestParser/consume.cc
    src/Server/RequestParser/is_tspecial.cc
    src/Server/Reply/Reply.cc
    src/Server/ConnectionManager/start.cc
    src/Server/ConnectionManager/stop.cc
    src/Server/ConnectionManager/constructor.cc
    src/Server/ConnectionManager/stopAll.cc
    src/Server/Connection/start.cc
    src/Server/Connection/stop.cc
    src/Server/Connection/constructor.cc
    src/Server/Connection/doWrite.cc
    src/Server/Connection/Connection.ih
    src/Server/Connection/doRead.cc
    src/Server/MimeTypes/extension_to_type.cc
    src/Server/Server/constructor.cc
    src/Server/Server/doAccept.cc
    src/Server/Server/run.cc
    src/Server/Server/doAwaitStop.cc
)
