#!/usr/bin/env python3

import cgi
import time
import os

# En-têtes HTTP requis
print("Content-Type: text/html\r\n\r\n")
# Récupérer les données du formulaire


download_path = "./www/main/cgi-bin/uploads/"
path = "../cgi-bin/uploads/"
links = ""
for file in os.listdir(download_path):
		if os.path.isfile(os.path.join(download_path, file)):
				# si le file ne commence pas par un point, on le rajoute
				if file[0] != ".":
					links += f'<a href={path}{file} download><div class="link">{file}</div></a>'


# Générer le contenu HTML avec les informations de l'utilisateur
html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Informations de l'Utilisateur</title>
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

				.links a {{
					color: white;
					text-decoration: none;
					background-color: #121212;
				}}

				.link {{
						padding: 5px 10px;
						background-color: #121212;
						margin: 5px;
						border-radius: 5px;
				}}

				.link:hover {{
					cursor: pointer;
					opacity: 0.9;
				}}

    </style>
</head>
<body>
    <div class="container">
        <h2>Download the file you want</h2>
				<div class="links">
					{links}
				</div>
    </div>
</body>
</html>
"""

print(html_content)
