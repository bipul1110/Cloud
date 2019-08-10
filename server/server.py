import socket
import sys
import os


if len(sys.argv) != 2:
    print >> sys.stderr, "Usage: python %s <port>" % sys.argv[0]
    sys.exit()

port = sys.argv[1]

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the port
server_address = (str(socket.INADDR_ANY), int(port))
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)

# Listen for incoming connections
sock.listen(1)

while True:
    # Wait for a connection
    print >>sys.stderr, 'waiting for a connection'
    # TODO: need to perform blocking accept call on new thread
    connection, client_address = sock.accept()

    is_client = os.fork()
    
    if is_client == 0:
        try:
            print >>sys.stderr, 'connection from', client_address

            # Receive the data in small chunks and retransmit it
            # Modify to receive files
            while True:
                data = connection.recv(16)
                pid = os.getpid()
                print >>sys.stderr, 'received: %s from client with pid %d' % (data, pid)
                if data:
                    print >>sys.stderr, 'sending data back to the client'
                    connection.sendall(data)
                else:
                    print >>sys.stderr, 'no more data from', client_address
                    break
                
        finally:
            # Clean up the connection
            print 'closing connection'
            connection.close()
            sys.exit() 

sock.shutdown(socket.SHUT_RDWR)
sock.close()