all:
	g++ *.h *.cpp -o Pricer

run:
	./Pricer 200 feed.txt

clean:
	rm -f *.gch *~
