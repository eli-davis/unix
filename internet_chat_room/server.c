#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <fcntl.h>
#include <string.h>

const int max_users = 100;

// user socket and initials
struct client_data
{ char initial_one;
  char initial_two;
  int room_socket; };

// for debugging
void print_clients(struct client_data * * client_list);

int create_server()
{ unsigned int port_number = 3201;
  struct hostent * current_working_host = gethostbyname("rabbit.eng.miami.edu");
  if (!current_working_host)
  { herror("get current host failed");
    exit(1); }
  struct in_addr * address = (struct in_addr *)current_working_host->h_addr;
  //printf("IP address: %s\n", inet_ntoa(*address));
  struct sockaddr_in server_info;
  server_info.sin_addr.s_addr = htonl(INADDR_ANY);
  server_info.sin_port = htons(port_number);
  server_info.sin_len = sizeof(server_info);
  server_info.sin_family = AF_INET;
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0)
  { perror("create server socket failed");
    exit(1); }
  int bind_socket_to_server = bind(server_socket, (struct sockaddr *) &server_info, sizeof(server_info));
  if (bind_socket_to_server < 0)
  { perror("failed binding socket to server");
    exit(1); }
  //turn on server
  int listen_on_port = listen(server_socket, 3);
  if (listen_on_port < 0)
  { perror("listen failed, unable to turn on server");
    exit(1); }
  printf("server online\n");
  //set the socket to be non-blocking
  fcntl(server_socket, F_SETFL, O_NONBLOCK);
  return server_socket; }

int accept_client_connection(int server_socket)
{ //printf("accept client\n");
  struct sockaddr_in client_info;
  unsigned int client_info_size = sizeof(client_info);
  int room_socket = accept(server_socket, (struct sockaddr *) &client_info, &client_info_size);
  if (room_socket <= 0)
  { perror("accept failed");
    exit(1); }
  char * client_address = inet_ntoa(client_info.sin_addr);
  printf("\nserver connected to client: %s; room socket: %d\n", client_address, room_socket);
  return room_socket; }

void print_online_users(int room_socket, struct client_data * * client_list)
{ int i, index, end_of_message_index, number_of_users = 0;
  char message[max_users * 4];
  for (i = 0; i < max_users; i += 1)
    if (client_list[i]->room_socket != 0)
    { index = number_of_users * 4;
      message[index]     = client_list[i]->initial_one;
      message[index + 1] = client_list[i]->initial_two;
      message[index + 2] = ',';
      message[index + 3] = ' ';
      number_of_users += 1; }
  if (number_of_users > 0)
  { end_of_message_index = number_of_users * 4 - 1;
    message[end_of_message_index - 1] = '.';
    message[end_of_message_index] = 0; }
  FILE * f = fdopen(room_socket, "w");
  fprintf(f, "Hello!\n");
  if (number_of_users == 0)
    fprintf(f, "no one else is online\n\n");
  else if (number_of_users == 1)
    fprintf(f, "one other person is online:\n%s\n", message);
  else
    fprintf(f, "there are %d other people online:\n%s\n", number_of_users, message);
  fflush(f); }

void get_user_initials(int room_socket, int room_number, struct client_data * * client_list)
{ char buffer[32];
  FILE * f_write = fdopen(room_socket, "w");
  fprintf(f_write, "Enter your two initials to chat\n");
  fflush(f_write);
  while(1)
  { int r = read(room_socket, buffer, 31);
    if (r == 3)
    { fprintf(f_write, "Thanks !\n\n");
      fflush(f_write);
      client_list[room_number]->initial_one = buffer[0];
      client_list[room_number]->initial_two = buffer[1];
      client_list[room_number]->room_socket = room_socket;
      break; }
    else if (r > 0)
    { fprintf(f_write, "Invalid entry. Please enter two initals \"JD\"\n");
      fflush(f_write); } } }

int send_outgoing_message(int room_socket, int room_number, struct client_data * * client_list)
{ //print_clients(client_list);
  int i, r, current_socket;
  char buffer[512];
  r = read(room_socket, buffer, 512);
  buffer[r] = 0;
  if (r <= 0)
  { close(room_socket);
    printf("socket %d closed\n", room_socket);
    return 0; }
  else
  { char initial_one = client_list[room_number]->initial_one;
    char initial_two = client_list[room_number]->initial_two;
    for (i = 0; i < max_users; i += 1)
    { current_socket = client_list[i]->room_socket;
      if (current_socket && current_socket != room_socket)
      { FILE * f_write = fdopen(current_socket, "w");
        fprintf(f_write, "%c%c: %s", initial_one, initial_two, buffer);
        fflush(f_write); } }
    return 1; } }

void print_user_logout(int room_socket, int room_number, struct client_data * * client_list)
{ int i, current_socket;
  FILE * f_write, * f = fdopen(room_socket, "w");
  char message[32];
  message[0] = client_list[room_number]->initial_one;
  message[1] = client_list[room_number]->initial_two;
  char logoff_event[12] = " logged out.";
  strncat(message, logoff_event, 12);
  for (i = 0; i < max_users; i += 1)
  { current_socket = client_list[i]->room_socket;
    if (current_socket && current_socket != room_socket)
    { f_write = fdopen(current_socket, "w");
      fprintf(f_write, "%s\n", message);
      fflush(f_write); } } }

void manage_clients(int server_socket, struct client_data * * client_list)
{ int i, room_number = 0;
  struct pollfd open_room[max_users + 1];
  // creates poll field for each client
  for (i = 0; i < max_users; i += 1)
  { open_room[i].fd = 0;
    open_room[i].events = 0;
    open_room[i].revents = 0; }
  //creates poll field for server
  open_room[max_users].fd = server_socket;
  open_room[max_users].events = POLLRDNORM;
  open_room[max_users].revents = 0;
  while (1)
  { // indicates wait 200ms between calls to poll
    int p = poll(open_room, max_users + 1, 200);
    if (p < 0)
    { perror("poll error");
      usleep(100000);
      continue; }
    if (open_room[max_users].revents && POLLRDNORM)
    { int room_socket = accept_client_connection(server_socket);
      if (room_number < max_users)
      { open_room[room_number].fd = room_socket;
        open_room[room_number].events = POLLRDNORM || POLLERR;
        open_room[room_number].revents = 0;
        print_online_users(room_socket, client_list);
        get_user_initials(room_socket, room_number, client_list);
        room_number += 1; } }
    for (i = 0; i < max_users; i += 1)
    { if (open_room[i].revents && POLLRDNORM)
      { printf("\nreceived data from %d\n", i);
        int s = send_outgoing_message(open_room[i].fd, i, client_list);
        if (!s)
        { print_user_logout(open_room[i].fd, i, client_list);
          close(open_room[i].fd);
          open_room[i].fd = 0;
          open_room[i].events = 0;
          open_room[i].revents = 0;
          client_list[i]->room_socket = 0; } } }
  }
}

// debugging
void print_clients(struct client_data * * client_list)
{ int i;
  for (i = 0; i < max_users; i += 1)
  { printf("%c%c:", client_list[i]->initial_one, client_list[i]->initial_two);
    printf("%d|", client_list[i]->room_socket); }
  printf("\n"); }

int main()
{ int i;
  int server_socket = create_server();
  struct client_data * * client_list = malloc(max_users * sizeof(*client_list));
  for (i = 0; i < max_users; i += 1)
    client_list[i] = malloc(sizeof(struct client_data));
  for (i = 0; i < max_users; i += 1)
  { client_list[i]->initial_one = '0';
    client_list[i]->initial_two = '0';
    client_list[i]->room_socket = 0; }
  manage_clients(server_socket, client_list); }
