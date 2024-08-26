import socket

# Connexion à ton serveur
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(('localhost', 1313))

# Construction de la requête chunked
request = (
    "POST       /      HTTP/1.1   \r\n"
    "Host:10.211.55.5\r\n"
	"Content-Length: 25\r\n"
    "\n"
    "Bonjour je suis Wikipedia\r\n"
)

# Envoi de la requête
sock.sendall(request.encode('utf-8'))

# Réception de la réponse du serveur
response = sock.recv(4096)
print(response.decode('utf-8'))

# Fermeture de la connexion
sock.close()