#!/usr/bin/env python3

import cgi

# En-têtes HTTP requis
print("Content-Type: text/html\n")

# Récupérer les données du formulaire
form = cgi.FieldStorage()
name = form.getvalue("name")
age = form.getvalue("age")

# Générer le contenu HTML avec les informations de l'utilisateur
html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Informations de l'Utilisateur</title>
    <style>
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
        <h1>Bienvenue, {name}!</h1>
        <p>Votre âge est {age} ans.</p>
    </div>
</body>
</html>
"""

print(html_content)
