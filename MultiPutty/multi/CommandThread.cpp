#include "stdafx.h"
#include "MainFrame.h"

#define BUFLEN 512  //Max length of buffer
#define PORT 4578   //The port on which to listen for incoming data
//echo - n "hello" > / dev / udp / 10.1.2.130 / 4578
CommandThread::CommandThread()
{
  struct sockaddr_in server;

  //Create a socket
  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
  {
    stdlog("Could not create socket : %d", WSAGetLastError());
    goto err;
  }

  int opt = 1;
  if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
  {
    stdlog("setsockopt failed\n");
    goto err;
  }

  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  //server.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  server.sin_port = htons(PORT);

  //Bind
  if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
  {
    stdlog("Bind failed with error code : %d", WSAGetLastError());
    goto err;
  }
  return;
err:
  if (s != INVALID_SOCKET)
    closesocket(s);
  s = INVALID_SOCKET;
}


void CommandThread::Terminate()
{
  SOCKET s0 = s;
  s = INVALID_SOCKET;
  if (s0 != INVALID_SOCKET)
    closesocket(s0);
}

void CommandThread::Work(LPVOID pWorkParam)
{
  char buf[BUFLEN];
  int slen, recv_len;
  struct sockaddr_in si_other;

  slen = sizeof(si_other);

  //keep listening for data
  while (1)
  {
    //clear the buffer by filling null, it might have previously received data
    memset(buf, '\0', BUFLEN);

    //try to receive some data, this is a blocking call
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
    {
      if (s != INVALID_SOCKET)
        stdlog("recvfrom() failed with error code : %d", WSAGetLastError());
      break;
    }

    //print details of the client/peer and the data received
    pWndMain->BrodcastUdpCommand(buf);
    //stdlog("Data: %s\n", buf);
  }

  if (s != INVALID_SOCKET)
    closesocket(s);
  s = INVALID_SOCKET;
}