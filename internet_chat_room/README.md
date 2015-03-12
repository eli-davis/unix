This program uses IP + TCP and poll() to create an internet chat service. 

server.c opens a port and accepts connections from clients. 
It can service a number of clients; I've tested it using up to several at once.

client.c connects to the server and receives word which users are online.
when a user sends a line of text to the server, the server sends the users initials and the message to the other clients ('ED: Hello'). Likewise when a user recieves a line of text from the server the client displays it for the user to see.

when a client logs off or disconnects, the other users are notified.
