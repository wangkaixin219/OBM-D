OBJ = main.o data.o fixed-h.o opt.o adaptive-h.o variable-h.o hst.o rql-adapt.o
CC = gcc-10
CXX = g++-10
CFLAGS = -Wall -O3 -g 


all: $(OBJ)
	$(CXX) $(CFLAGS) -o OBM-D $(OBJ)

main.o: main.cpp data.h adaptive-h.h opt.h fixed-h.h variable-h.h rql-adapt.h
	$(CXX) $(CFLAGS) -c main.cpp

data.o: data.cpp data.h def.h
	$(CXX) $(CFLAGS) -c data.cpp

fixed-h.o: fixed-h.cpp fixed-h.h def.h
	$(CXX) $(CFLAGS) -c fixed-h.cpp

opt.o: opt.cpp opt.h def.h
	$(CXX) $(CFLAGS) -c opt.cpp

adaptive-h.o: adaptive-h.cpp adaptive-h.h def.h
	$(CXX) $(CFLAGS) -c adaptive-h.cpp

rql-adapt.o: rql-adapt.cpp rql-adapt.h opt.h def.h
	$(CXX) $(CFLAGS) -c rql-adapt.cpp

variable-h.o: variable-h.cpp variable-h.h hst.h def.h
	$(CXX) $(CFLAGS) -c variable-h.cpp

hst.o: hst.cpp hst.h
	$(CXX) $(CFLAGS) -c hst.cpp


.PHONY: clean
clean:
	rm OBM-D $(OBJ)
