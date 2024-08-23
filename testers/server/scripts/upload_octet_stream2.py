import socket
import os

def upload_file(host, port, file_path):
    # Créer un socket TCP
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        # Connexion au serveur
        sock.connect((host, port))
        
        # Préparer la requête HTTP
        file_size = os.path.getsize(file_path)
        request_headers = [
            f"POST /upload/ HTTP/1.1",
            f"Host: {host}",
            f"Content-Type: application/octet-stream",
            "",
            ""  # Fin des en-têtes
        ]
        request_header = "\r\n".join(request_headers)
        
        # Lire le contenu du fichier
        with open(file_path, 'rb') as file:
            file_content = file.read()

        # Envoyer les en-têtes suivis du contenu du fichier
        sock.sendall(request_header.encode('utf-8'))
        sock.sendall(file_content)
        
        # Recevoir la réponse du serveur
        response = b""
        while True:
            chunk = sock.recv(4096)
            if not chunk:
                break
            response += chunk
        
        # Afficher la réponse du serveur
        print(response.decode('utf-8'))

if __name__ == "__main__":
    server_host = '10.211.55.5'  # Remplace par l'hôte de ton serveur
    server_port = 8080             # Remplace par le port de ton serveur
    file_to_upload = '/Users/loup/Projects/DEV/42/42Cursus-webserv/testers/server/scripts/bible_small.txt'  # Remplace par le chemin vers le fichier que tu veux uploader
    upload_file(server_host, server_port, file_to_upload)