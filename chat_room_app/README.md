This program uses TCP and poll() to create an internet chat service. 

server.c opens a port and accepts connections from clients. 
It can service a number of clients; I've tested using up to eight clients at once.

client.c connects to the server and receives word which users are online (using the user's initials "JD")
when a user sends a line of text to the server, the server sends that users initials and the message
to the other clients. Likewise when a user recieves a line of text, the client displays it for the user to see.

when a client logs off or disconnects, the other users are notified.
