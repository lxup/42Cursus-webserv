#!/usr/bin/env python3

import os
import cgitb
import uuid
import cgi
import time

isNewCLient = False
# En-tête HTTP pour indiquer que le contenu qui suit est du HTML
print("Content-Type: text/html; charset=utf-8")  # En-tête HTTP

# Activer le mode débogage pour afficher les erreurs sur la page
cgitb.enable()

def get_new_user_theme(user_id):
    form = cgi.FieldStorage()
    newTheme = form.getvalue("theme")
    if (newTheme is None):
        newTheme = get_user_theme(user_id)
    try: # Écrire le thème utilisateur dans le fichier
        with open(f"./www/cookie/database/{user_id}.txt", "w") as file:
            file.write(newTheme)
    except Exception as e:
        print(f"Erreur lors de l'écriture du fichier de thème : {e}")
    return newTheme 

# Fonction pour récupérer le thème de l'utilisateur depuis le fichier
def get_user_theme(user_id):
    try:
        with open(f"./www/cookie/database/{user_id}.txt", "r") as file:  # Chemin vers le répertoire des thèmes utilisateur
            theme = file.read().strip()
            return theme if theme in ["light", "dark"] else "light"
    except FileNotFoundError:
        return "light"  # Valeur par défaut si le fichier n'existe pas
    except Exception as e:
        print(f"Erreur lors de la lecture du fichier de thème : {e}")
        return "light"

# Fonction qui génère un ID unique
def generateId():
    return str(uuid.uuid4())

def generateExpirationDate():
    # Calculer l'heure d'expiration : 30 jours (30 * 24 * 60 * 60 secondes)
    expiration_time = time.time() + 60 * 60 * 24 * 30
    # Formater la date d'expiration en format GMT
    formatted_time = time.strftime("%a, %d-%b-%Y %H:%M:%S GMT", time.gmtime(expiration_time))
    #print(f"Expires: {formatted_time}\r\n")
    return formatted_time

def createNewCookie():
    user_id = generateId()
    expiration_date = generateExpirationDate()
    # En-tête Set-Cookie correctement formaté
    cookieLine = "Set-Cookie: session_id=" + user_id + "; Expires=" + expiration_date + "; Path=/\r\n"
    print(cookieLine)
    #print(cookieLine)
    with open(f"./www/cookie/database/{user_id}.txt", "w") as file:
        file.write("light")
    return user_id

# Récupérer les cookies de la variable d'environnement
cookies = os.environ.get('HTTP_COOKIE', '')
user_id = None

# Si le cookie est vide, on crée un cookie et un fichier pour stocker le thème
if cookies == "":
    user_id = createNewCookie()
else:
    # Analyser les cookies pour trouver l'ID de session
    for cookie in cookies.split(';'):
        name, value = cookie.strip().split('=', 1)
        if name == "session_id":
            user_id = value
            break

# Si l'ID utilisateur n'est pas trouvé, initialiser avec une valeur par défaut
if user_id is None:
    user_id = createNewCookie()

# Récupérer le thème de l'utilisateur
#user_theme = get_user_theme(user_id)

#changer le thème si l'utilisateur a soumis le formulaire
user_theme = get_new_user_theme(user_id)

# Définir le style en fonction du thème
if user_theme == "dark":
    background_color = "#333"
    text_color = "#f0f0f0"
    buttonColor = "white"
else:
    background_color = "#f0f0f0"
    text_color = "#333"
    buttonColor = "black"
    


light_selected = "selected" if user_theme == "light" else ""
dark_selected = "selected" if user_theme == "dark" else ""


# Contenu HTML avec le thème utilisateur
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
            background-color: {background_color};
            color: {text_color};
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
        }}
        .container {{
            text-align: center;
        }}
        
        .button{{
        margin-top: 2rem;
        border: none;
        padding: 1rem 3rem;
        background-color: {buttonColor};
        border-radius: 15px;
        font-weight: semibold;
        color: {background_color};
      }}
            
       form{{
        display: flex;
        flex-direction: column;
        align-items: center;
      }}
      
      #theme {{
				margin-top: 1rem;
				padding: 5px 20px;
        background-color: {text_color};
        border-radius: 3px;
        font-weight: semibold;
        color: {background_color};
			}}
      
      #theme option {{
				display: flex;
      }}    
    </style>
</head>
<body>
    <div class="container">
        <h2>Cookie Page</h2>
        <form action="/cookie/cookie.py" method="post">
            <label for="theme">Choisissez un thème :</label>
            <select name="theme" id="theme">
                <option {light_selected} value="light">Light</option>
                <option {dark_selected} value="dark">Dark</option>
            </select>
            <button class="button" type="submit" >Save</button>
        </form>
    </div>
</body>
<script src="https://cdn.jsdelivr.net/npm/js-confetti@latest/dist/js-confetti.browser.js"></script>
</html>
"""
if (isNewCLient == False):
  print("\r\n")
print(html_content)
