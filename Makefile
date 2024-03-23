CXX = g++
CXXFLAGS = -std=c++20 -Iredis/include
LDFLAGS = -lboost_context -lboost_coroutine -lboost_system -lssl -lcrypto

all: popper pusher

popper: main.cc popper.cc
	$(CXX) $(CXXFLAGS) -o popper main.cc popper.cc $(LDFLAGS)

pusher: main.cc pusher.cc
	$(CXX) $(CXXFLAGS) -o pusher main.cc pusher.cc $(LDFLAGS)

clean:
	rm -f popper pusher