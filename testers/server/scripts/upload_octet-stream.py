import requests

def upload_file(url, file_path):
    headers = {
        'Content-Type': 'application/octet-stream'
    }
    
    # Ouvre le fichier en mode binaire pour lecture
    with open(file_path, 'rb') as file:
        # Envoie le fichier via une requête POST
        response = requests.post(url, headers=headers, data=file)

    # Affiche la réponse du serveur
    print(f"Status Code: {response.status_code}")
    print(f"Response Body: {response.text}")

if __name__ == "__main__":
    upload_url = 'http://10.211.55.5:8080'  # Remplace par l'URL de ton serveur
    file_to_upload = './bible.txt'   # Remplace par le chemin vers le fichier que tu veux uploader
    upload_file(upload_url, file_to_upload)