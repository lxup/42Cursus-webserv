#!/usr/bin/env python3

import cgi
import os

# Créer une instance FieldStorage
form = cgi.FieldStorage()

# Récupérer les données du formulaire
filename = form.getvalue("filename")
content = form.getvalue("content")

# Chemin où le fichier sera créé
filepath = f"/path/to/directory/{filename}"

print("Content-Type: text/html\r\n")  # En-tête HTTP

try:
    # Ouvrir le fichier en écriture et écrire le contenu
    with open(filepath, "w") as file:
        file.write(content)
    print(f"<html><body><h2>Le fichier '{filename}' a été créé avec succès !</h2></body></html>")
except Exception as e:
    print(f"<html><body><h2>Erreur : {e}</h2></body></html>")
