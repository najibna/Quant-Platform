CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -pthread
INCLUDES = -Icpp/include

BIN = bin
BUILD = build

ENGINE_SRC = \
	cpp/src/market_data.cpp \
	cpp/src/stats.cpp \
	cpp/src/latency.cpp \
	cpp/src/engine.cpp \
	cpp/src/main_engine.cpp

WORKER_SRC = \
	cpp/src/market_data.cpp \
	cpp/src/stats.cpp \
	cpp/src/latency.cpp \
	cpp/src/worker.cpp \
	cpp/src/main_worker.cpp

.PHONY: all clean dirs

all: dirs $(BIN)/engine $(BIN)/worker

dirs:
	mkdir -p $(BIN) $(BUILD) logs data/processed

$(BIN)/engine: $(ENGINE_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(ENGINE_SRC) -o $@

$(BIN)/worker: $(WORKER_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(WORKER_SRC) -o $@

clean:
	rm -rf $(BIN) $(BUILD)
