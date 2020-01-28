#include <cstring>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sys/socket.h>
#include <chrono>
#include <netinet/in.h>
#include <unistd.h>
#include <cmath>
#include <pthread.h>
#include </usr/include/semaphore.h>
#define MAXSIZE 256
void *operhandle(void *);
void *updateinterests(void *);
using namespace std;
using namespace std::chrono;
string buffer;
char *p;
int balance[MAXSIZE];
char *name[MAXSIZE];
string c_name[MAXSIZE];
fstream fp;
char* buffer1[MAXSIZE];
string s[MAXSIZE];
int done=0;

int i=0,j=0;
int u;

int id[MAXSIZE];
char re[MAXSIZE];
char contents[MAXSIZE];

pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;
void thr_exit()
{
  cout << "Exiting the update interests thread"<<endl;
  pthread_mutex_lock(&mutex4);
  done = 1;
  pthread_cond_signal(&c);
  pthread_mutex_unlock(&mutex4);
}
void thr_join()
{
  pthread_mutex_lock(&mutex4);
  while (done==0)
    pthread_cond_wait(&c,&mutex4);
  pthread_mutex_unlock(&mutex4);
}
int accid, amount;
char op[2];
size_t len=0;
int x=0;

int socketf1,socketf2, portno, *new_sock;
struct sockaddr_in serveradd, clientaddr;

int main(int argc, char **argv)
{
  if (argc<2)
  {
    std:: cerr << "<number>usage: server <port>" << std:: endl;
    exit(1);
  }

  //Reading from Records.txt
  fp.open("Records.txt", ios::in | ios::out);
  while(getline(fp, buffer))
  {
    strcpy(re,buffer.c_str());
    p=strtok(re," ");
    if (p)
    {
      id[i]=atoi(p);
    }
    p= strtok(NULL," ");
    if (p)
    {
      name[i]=p;
      c_name[i] = name[i];
    }
    p= strtok(NULL," ");
    balance[i]=atoi(p);
    i++;
  }
  j=i;
  fp.clear();
  fp.seekg(0,ios::beg);
  socketf1=socket(AF_INET, SOCK_STREAM,0);
  if (socketf1<0)
    std:: cerr  << "Server error No 1: Unable to establish a server socket" << " " << std:: endl;
  //Initializing the socketf1
  bzero((char*)&serveradd, sizeof(serveradd));

  portno= atoi(argv[1]);
  serveradd.sin_addr.s_addr= INADDR_ANY;
  serveradd.sin_port=htons(portno);

  //Binding the host address
  if (bind(socketf1, (struct sockaddr *)&serveradd,sizeof(serveradd))<0)
  {
    std:: cerr<< "Server error No 2: Unable to bind the server socket" << endl;
  }

  //Listen to clients

  listen(socketf1,5);
  socklen_t clientlen = sizeof(clientaddr);

  //Initializing a mutex
  pthread_mutex_init(&mutex3,NULL);

  //

  //While loop to accept connections from multiple clients
  cout << "Waiting for client connection" << endl;
  cout << "==========================================Waiting=========================================="<<endl;
  while (socketf2=accept(socketf1,(struct sockaddr*)&clientaddr, &clientlen))
  {
    cout << "==========================================Waiting=========================================="<<endl;
    if (socketf2<0)
    {
      std::cerr<< "Server error No 3: Unable to connect to client" << endl;
    }
    else
    {
      cout << "Client is connected" << endl;
    }
    pthread_t p_thread;
    
    int *new_sock;
    new_sock = new int;
    *new_sock=socketf2;

    if (x==100)
    {
      cout << "Running the thread to update interests" << endl;
      pthread_t xyz;
      pthread_create(&xyz,NULL,updateinterests,NULL);
      thr_join();
      return 0;
    }


    //Creating thread for each client
    //auto start = high_resolution_clock::now();
    if (pthread_create(&p_thread, NULL, operhandle, (void*)new_sock)<0)
    {
      cout << "Server error No 4 : Could not create a thread\n";
      return 1;
    }
  }
  // close the socket
  close (socketf2);
  pthread_exit(NULL);

  //closing the filename
  fp.close();

  //closing the server socketfd1
  close (socketf1);
  return 0;
}

//called for each client or Thread
void *operhandle (void *sock_test)
{
  //Getting the socket descriptor
  x++;
  cout << "Transaction no:\t" << " " << x << endl;
  bzero(buffer1,MAXSIZE);
  int newsocket= *(int *)sock_test;
  //Receiving data from clients
  cout << "On thread no:\t" << " " << pthread_self() << endl;
  bzero(contents, MAXSIZE);
  int n=0;
  while(n=read(newsocket,contents,MAXSIZE))
  {
    if(n<0)
    {
      std:: cerr << "Server error no 5 : Server error in receiving data" << std:: endl;
      exit(1);
    }
    if(strlen(contents)==0)
    {
      continue;
    }
    cout<<"---------------------------------------------------------------------------------"<<endl;
    cout << "Data received from client:\t" << " " << contents<< endl;
    //separating ACCOUNT ID and transaction to be performed and the amount from the received data
    char *t;
    p=strtok(contents," ");
    p=strtok(NULL," ");

    if(p)
    {
      accid = atoi(p);
    }

    p=strtok(NULL, " ");

    if (p)
    {
      strcpy(op,p);
    }

    p=strtok(NULL," ");

    if (p)
    {
      amount= atoi(p);
    }

    int size = 0;
    int exist = 0;

    //Check if the ID received from the client exists in the Records.txt
    for (i=0;i<j; i++)
    {
      if (accid==id[i])
      {
        u=i;
        pthread_mutex_lock(&mutex3);
        cout << "This account ID exists in our records /t" << endl;
        if(strcmp(op,"w")==0)
        {
          cout << "Withdrawal is in progress........." << endl;
          cout << "Checking for sufficient balance in the account no:" << " " << accid <<endl;
          if (balance[i]-amount>0)
          {
            balance[i]=balance[i]- amount;
            cout << "Withdrawal successful...." << endl;
            cout << "Present balance" << " " << balance[i] << endl;
            n=write(newsocket, "ACK: Amount withdrawn",50);
            if(n<0)
              cerr << "Server error no 6: Server error sending response to the client" << endl;
            cout << "ACK sent" << endl;
            cout << "Writing the newly calculated balance back to our records" << endl;
            if (u==0)
            {
              fp.clear();
              fp.seekg(0,ios::beg);
              size = 0;
              fp << id[u] << " " << c_name[u] << " " << balance[u]<< endl;
              fp.clear();
              fp.seekg(0,ios::beg);
            }
            else
            {
              for (i=0;i<=u-1;i++)
              {
                size = size + (floor(log10(abs(id[i])))+1) + strlen(c_name[i].c_str()) + (floor(log10(abs(balance[i])))+1) + 3;
              }
              fp.clear();
              fp.seekg(0,ios::beg);
              fp.clear();
              fp.seekg(size, ios::beg);
              fp << id[u] << " " << c_name[u] << " " << balance[u]<<endl;
            }
            cout << "Account updated...."<<endl;
            //fp.clear();
          }
          else
          {
            //Insufficient Balance
            cout << "Insufficient balance" << balance[i]<<endl;
            cout << "Acknowledgement sent" << endl;
            n=write(newsocket, "Insufficient balance\n", 50);
            if (n<0)
              cerr << "Server error no 7: Server error sending response to the client" << endl;
          }
        }
        else if (strcmp(op,"d")==0)
        {
          cout << "Depositing amount:" << " " << amount <<endl;
          cout << "Present Balance" << " " << balance[i] << endl;
          balance[i] = balance[i]+amount;
          cout << "Updated balance" << " " << balance[i] << endl;
          n=write(newsocket, "ACK: Amount Deposited",50);
          if(n<0)
            cerr << "Server error no 8 : Server error sending response to the client" << endl;
          cout << "ACK sent" << endl;
          cout << "Writing the newly calculated balance back to our records" << endl;
          if (u==0)
          {
            fp.clear();
            fp.seekg(0,ios::beg);
            size = 0;
            fp << id[u] << " " << c_name[u] << " " << balance[u]<< endl;
            fp.clear();
            fp.seekg(0,ios::beg);
          }
          else
          {
            for (i=0;i<=u-1;i++)
            {
              size = size + (floor(log10(abs(id[i])))+1) + strlen(c_name[i].c_str()) + (floor(log10(abs(balance[i])))+1) + 3;
            }
            fp.clear();
            fp.seekg(0,ios::beg);
            fp.clear();
            fp.seekg(size, ios::beg);
            fp << id[u] << " " << c_name[u] << " " << balance[u]<<endl;
          }
        }
        else
        {
          cout << "Invalid transaction type" << endl;
          n=write(newsocket, "Invalid transaction type\n",50);
          if (n<0)
            cerr << "Server error no 9: Server error sending response to the client" << endl;
        }
        exist = 1;
        //Release the pthread_mutex_lock
        pthread_mutex_unlock(&mutex3);
        break;
      }
    }
    if (exist==0)
    {
      //Invalid account ID
      cout<< " This account ID doesn't exist" << " " << endl;
      n=write(newsocket, "NACK: This account ID doesn't exist", 4);
      if (n<0)
      cerr<<  "Server error no 10: Server error writing to socket" << endl;
    }
  }
  return 0;
}
void *updateinterests(void *arg)
{
  cout << "Updating interests on balances in the record" << endl;
  int rate=10;
  int period = 2;
  cout << "Balances before updating the interests" << endl;
  for (int k=0;k<j;k++)
  {
    cout << "Account:" << k << "balance is" << balance[k]<< endl;
  }
  for (int k=0;k<j;k++)
  {
    balance[k]=balance[k]+(balance[k]*period*10)/100;
  }
  cout << "Balances after updating the interests" << endl;
  for (int k=0;k<j;k++)
  {
    cout << "Account:" << k << "balance is" << balance[k]<<endl;
  }
  cout << "Writing the updated balances to the Records" << endl;
  fp.clear();
  fp.seekg(0,ios::beg);
  for (i=0;i<j;i++)
  {
    fp << id[i] << " " << c_name[i] << " " << balance[i]<<endl;
    //size = size + (floor(log10(abs(id[i])))+1) + strlen(c_name[i].c_str()) + (floor(log10(abs(balance[i])))+1) + 3;
  }
  fp.clear();
  fp.seekg(0, ios::beg);
  thr_exit();
  return NULL;
}
