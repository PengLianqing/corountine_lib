
TARGET = test
CC =g++ -L. -o3 -g -Wall -std=c++11
INCLUDE =$(shell find ./ -name "*.h")
SOURCE =$(shell find ./ -name "*.cc")
OBJS =$(SOURCE:%.cc=%.o)

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)
	
%.o: %.cc $(INCLUDE)
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)
	
all:$(TARGET)

clean:
	rm -rf $(OBJS) $(TARGET)
