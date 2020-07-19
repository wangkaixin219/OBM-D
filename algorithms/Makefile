OBJ = main.o data.o fixed-h.o opt.o adaptive-h.o variable-h.o hst.o rql-adapt.o

all: $(OBJ)
	g++ -g -O3 -Wall -o OBM-D $(OBJ)

main.o: main.cpp data.h adaptive-h.h opt.h fixed-h.h variable-h.h rql-adapt.h
	g++ -g -c main.cpp

data.o: data.cpp data.h def.h
	g++ -g -c data.cpp

fixed-h.o: fixed-h.cpp fixed-h.h def.h
	g++ -g -c fixed-h.cpp

opt.o: opt.cpp opt.h def.h
	g++ -g -c opt.cpp

adaptive-h.o: adaptive-h.cpp adaptive-h.h def.h
	g++ -g -c adaptive-h.cpp

rql-adapt.o: rql-adapt.cpp rql-adapt.h opt.h def.h
	g++ -g -c rql-adapt.cpp

variable-h.o: variable-h.cpp variable-h.h hst.h def.h
	g++ -g -c variable-h.cpp

hst.o: hst.cpp hst.h
	g++ -g -c hst.cpp


.PHONY: didi
didi: $(OBJ)
	g++ -g -O3 -Wall -D DIDI -o OBM-D $(OBJ)

.PHONY: olist
olist: $(OBJ)
	g++ -g -O3 -Wall -D OLIST -o OBM-D $(OBJ)

.PHONY: clean
clean:
	rm OBM-D $(OBJ)
