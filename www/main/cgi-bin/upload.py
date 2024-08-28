#!/usr/bin/env python3

import cgi
import os
import cgitb

# Activer le mode débogage pour afficher les erreurs sur la page
cgitb.enable()

# Définir le répertoire où les fichiers téléchargés seront enregistrés
upload_dir = "./www/main/cgi-bin/uploads/"

# Créer une instance FieldStorage pour traiter les données du formulaire
form = cgi.FieldStorage()

# Obtenir le champ du fichier téléchargé
file_item = form['file']

# En-tête HTTP pour indiquer que le contenu qui suit est du HTML
print("Content-Type: text/html; charset=utf-8\r\n\r\n")  # En-tête HTTP

# Vérifier si un fichier a été téléchargé
if file_item.filename:
    # Créer un chemin sécurisé pour enregistrer le fichier
    filename = os.path.basename(file_item.filename)
    filepath = os.path.join(upload_dir, filename)

    try:
        # Lire le fichier téléchargé en morceaux et l'écrire dans le fichier de destination
        with open(filepath, 'wb') as output_file:
            while True:
                chunk = file_item.file.read(1024)
                if not chunk:
                    break
                output_file.write(chunk)

        # Message de succès
        value = f"'{filename}' a été uploader avec succès et enregistré à '{upload_dir}'"
    except Exception as e:
        # Message d'erreur en cas de problème lors de l'enregistrement du fichier
        value = f"Erreur lors de l'enregistrement du fichier : {e}"
else:
    # Message d'erreur si aucun fichier n'a été téléchargé
    value = "Aucun fichier n'a été téléchargé."



html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Upload</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&display=swap');

        body {{
            font-family: 'Inter', sans-serif;
            background-color: #f0f0f0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            color: #333;
        }}
        .container {{
            text-align: center;
        }}
    </style>
</head>
<body>
    <div class="container">
				<h2> {value} </h2>
				<a href="/">Back home</a>
    </div>
</body>
</html>
"""


print(html_content)