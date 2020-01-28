compile :
	g++ -o server qserver.cpp -lm -lpthread
	g++ -o client strcli.cpp -lm -lpthread

runserver :
	./server 8787

runclient1 :
	./client 127.0.0.1 8787 2 Transactions.txt

runclientmulti:
	chmod 755 script.sh
	./script.sh

clean:
	rm server client
