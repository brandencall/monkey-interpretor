CXX = g++
CXXFLAGS = -I$(PWD)/include
LDFLAGS = -lgtest -lgtest_main -pthread
OBJECTS = main.o token.o lexer.o 
TARGET = final 
TEST_LEXER_OBJECTS = test_lexer.o token.o lexer.o 
TEST_LEXER_BIN = test_lexer

all: $(TARGET)

test_lexer: $(TEST_LEXER_BIN)
	./$(TEST_LEXER_BIN)

token.o: src/token/token.cpp include/token.h
	$(CXX) $(CXXFLAGS) -c src/token/token.cpp -o token.o

lexer.o: src/lexer/lexer.cpp include/lexer.h include/token.h
	$(CXX) $(CXXFLAGS) -c src/lexer/lexer.cpp -o lexer.o

main.o: src/main.cpp 
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o main.o

test_lexer.o: src/test/test_lexer.cpp include/lexer.h include/token.h
	$(CXX) $(CXXFLAGS) -c src/test/test_lexer.cpp -o test_lexer.o

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

$(TEST_LEXER_BIN): $(TEST_LEXER_OBJECTS)
	$(CXX) $(TEST_LEXER_OBJECTS) $(LDFLAGS) -o $(TEST_LEXER_BIN)

clean:
	rm -f $(OBJECTS) $(TEST_LEXER_OBJECTS) $(TARGET) $(TEST_LEXER_BIN)

.PHONY: all clean test_lexer
