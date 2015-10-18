.PHONY: build clean rebuild
SDir = ./src
IDir = ./include
BDir = ./build
Flags = -std=c++11
Libs = -pthread
Objects = \
	$(BDir)/message.o\
	$(BDir)/client.o\
	$(BDir)/server.o\
	$(BDir)/UDPsocket.o\
	$(BDir)/worker.o\

first: build

App-Client: $(Objects) $(BDir)/Main-client.o
	$(CXX) $(Objects) $(BDir)/Main-client.o $(Libs) -o App-Client $(Flags)

App-Server: $(Objects) $(BDir)/Main-server.o
	$(CXX) $(Objects) $(BDir)/Main-server.o $(Libs) -o App-Server $(Flags)

$(BDir)/%.o: $(SDir)/%.cpp
	$(CXX) -c $< -o $@ $(Flags) -I$(IDir)

build: App-Client App-Server

rebuild: clean build

clean:
	rm $(BDir)/*.o *.log App-* -f
