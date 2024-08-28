#!/usr/bin/env python3

import os
import urllib.parse

# En-têtes HTTP requis
print("Content-Type: text/plain; charset=utf-8\r\n\r\n")

upload_dir = './www/main/cgi-bin/uploads/'

# Vérifier la méthode de requête
if os.environ['REQUEST_METHOD'] == 'DELETE':
    # Extraire la chaîne de requête de l'URL
    query_string = os.environ.get('QUERY_STRING', '')
    params = urllib.parse.parse_qs(query_string)
    resource = params.get('resource', [''])[0]  # Récupère la première valeur de 'resource'

    file_path = os.path.join(upload_dir, resource)
    #print(f"file_path: {file_path}")
    #print(f"resource: {resource}")

    # Vérifier que la ressource est valide
    if resource and os.path.exists(file_path):
        try:
            os.remove(file_path)
            print(f"Resource '{resource}' has been deleted successfully.")
        except Exception as e:
            print(f"Failed to delete the resource '{resource}': {e}")
    else:
        print(f"Resource '{resource}' does not exist.")
else:
    print("Invalid request method. Use DELETE.")
