#include <cstring>
#include <chrono>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h>
#include <cmath>

#define MAXFILE 500
using namespace std;
using namespace std::chrono;
int main (int argc, char **argv)
{
  if (argc < 4)
  {
    std :: cerr << "Usage client <hostname> <port number> <timestamp> <filename> " << std:: endl;
    exit(1);
  }
  string buffer;
  char buffer1[MAXFILE];
  int x=0;
  char ch;
  char *filename;
  filename = argv[4];
  fstream fp3;
  string s[MAXFILE];
  char buff[MAXFILE];
  char buffer2[MAXFILE];
  const char* y;//==>

  //open the filename received from the command line
  auto start =high_resolution_clock::now();
  fp3.open(filename, ios::in | ios::out);
  int portno;
  struct hostent *server;
  double sec;
  portno = atoi(argv[2]);
  sec = atof(argv[3]);
  server = gethostbyname(argv[1]);
  if (server == NULL)
  {
    std :: cerr << "No such host exists" << std :: endl;
    exit(1);
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,(char *)&server_addr.sin_addr.s_addr,server->h_length);
  server_addr.sin_port = htons(portno);

  int socketf3 = socket (AF_INET, SOCK_STREAM, 0);

  if (socketf3 < 0)
  {
    std :: cerr << "Unable to form a socket for client" << std :: endl;
    exit(0);
  }
  //cout << socketf3 << endl;
  double time;
  double first;

  //Connecting the client to socketf1
  if(connect(socketf3, (struct sockaddr *) &server_addr, sizeof(server_addr))<0)
  {
    std :: cerr << "Unable to connect to server" << std :: endl;
  }

  x++; //--
  int size =0; //==>
  int tx = 0;

  //getting each transaction and send it to server
  while (getline(fp3,buffer))
  {
    auto start =high_resolution_clock::now();
    first +=1;
    y=buffer.c_str();
    strcpy(buffer1,y);
    for (tx=0;tx<strlen(buffer1);tx ++)
      buffer2[tx] = buffer1[tx];
    char *p;
    p = strtok(buffer2," ");
    if(p)
    {
      time = atoi(p);
    }
    //Creating a delay based on the timestamp
    while (first != time)
    {
      if(first  > time)
      {
        sleep((first-time)*sec);
        first +=1;
      }
      else
      {
        sleep((time-first)*sec);
        first=first+1;
      }
    }
    int n;
    if (first==time)
    {
      cout << "Sending data to the bank server : " << " " << buffer1 << endl;
      n= write(socketf3,buffer1,100);
      if (n<0)
      {
        std :: cerr << "Error writing to the socket" << std::endl;
        exit(1);
      }
      bzero(buffer1, MAXFILE);
      //Reading  the acknowledgment received from the server
      n = read(socketf3,buff,MAXFILE);
      cout << "Acknowledgement received : " << buff << endl;
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop-start);
    cout << duration.count() << endl;
  }
  close(socketf3);
  fp3.close();
  exit(0);
  return 0;
}
