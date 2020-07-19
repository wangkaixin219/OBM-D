OBJ = main.o data.o cclo.o opt.o q_learning.o mmdh.o hst.o restricted_q_learning.o

all: $(OBJ)
	g++ -g -O3 -Wall -o OBM-D $(OBJ)

main.o: main.cpp data.h q_learning.h opt.h cclo.h mmdh.h restricted_q_learning.h
	g++ -g -c main.cpp

data.o: data.cpp data.h def.h
	g++ -g -c data.cpp

cclo.o: cclo.cpp cclo.h def.h
	g++ -g -c cclo.cpp

opt.o: opt.cpp opt.h def.h
	g++ -g -c opt.cpp

q_learning.o: q_learning.cpp q_learning.h def.h
	g++ -g -c q_learning.cpp

restricted_q_learning.o: restricted_q_learning.cpp opt.h def.h
	g++ -g -c restricted_q_learning.cpp

mmdh.o: mmdh.cpp mmdh.h hst.h def.h
	g++ -g -c mmdh.cpp

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
