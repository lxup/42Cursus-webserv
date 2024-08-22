import socket

# Connexion à ton serveur
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(('10.211.55.5', 8080))

# Corps de la requête (plus long que Content-Length)
body = "Bonjour, ceci est un corps de requête plus long que spécifié."

# Construction de la requête HTTP
request = (
    "POST / HTTP/1.1\r\n"
    "Host: 10.211.55.5\r\n"
    f"Content-Length: 20\r\n"  # Content-Length inférieur à la taille réelle du body
    "Content-Type: text/plain\r\n"
    "\r\n"
    f"{body}\r\n"
)

# Envoi de la requête
sock.sendall(request.encode('utf-8'))

# Réception de la réponse du serveur
response = sock.recv(4096)
print(response.decode('utf-8'))

# Fermeture de la connexion
sock.close()
