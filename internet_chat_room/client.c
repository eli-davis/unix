#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

const int max_message_length = 512;

int create_client()
{ unsigned short int port_number = 3201;
  struct hostent * current_working_host = gethostbyname("rabbit.eng.miami.edu");
  if (!current_working_host)
  { herror("get current host failed; try pinging rabbit.eng.miami.edu");
    exit(1); }
  struct in_addr * address = (struct in_addr *)current_working_host->h_addr;
  //printf("Host IP address: %s\n", inet_ntoa(*address));
  // create a sockaddr_in to describe the server
  struct sockaddr_in server_info;
  server_info.sin_addr = *address;
  server_info.sin_port = htons(port_number);
  server_info.sin_len = sizeof(server_info);
  server_info.sin_family = AF_INET;
  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket < 0)
  { perror("create client socket failed");
    exit(1); }
  int connect_to_server = connect(client_socket, (struct sockaddr *) &server_info, sizeof(server_info));
  if (connect_to_server < 0)
  { perror("failed connecting socket to server");
    exit(1); }
  return client_socket; }

int read_user_message_from_keyboard(char * buffer)
{ int i;
  for (i = 0; 1; i += 1)
  { buffer[i] = getchar();
    if (buffer[i] == '\n' || i == max_message_length-2)
      break; }
  buffer[i+1] = '\0';
  if (i == 0)
    return 0;
  return 1; }

void display_incoming_messages(int client_socket)
{ if (fork() == 0)
  { FILE * f_read = fdopen(client_socket, "r");
    FILE * f_write = fdopen(1, "w");
    char buffer[512], * s;
    while(1)
    { s = fgets(buffer, 512, f_read);
      if (!s)
        break;
      fprintf(f_write, "%s", buffer);
      fflush(f_write);  }
  printf("\nDisconnected from server\n");
  fclose(f_read);
  fclose(f_write);
  exit(0); } }

void join_chat_room()
{ int n, client_socket = create_client();
  display_incoming_messages(client_socket);
  FILE * f_write = fdopen(client_socket, "w");
  char buffer[512];
  while (1)
  { n = read_user_message_from_keyboard(buffer);
    if (n)
      fprintf(f_write, "%s", buffer);
    fflush(f_write); } }

int main()
{ join_chat_room(); }
