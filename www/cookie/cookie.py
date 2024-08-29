#!/usr/bin/env python3

import os
import cgitb
import uuid
import cgi
import time

print("Content-Type: text/html; charset=utf-8")

cgitb.enable()

def get_new_user_theme(user_id):
    form = cgi.FieldStorage()
    newTheme = form.getvalue("theme")
    if (newTheme is None):
        newTheme = get_user_theme(user_id)
    try:
        with open(f"./www/cookie/database/{user_id}.txt", "w") as file:
            file.write(newTheme)
    except Exception as e:
        print(f"Erreur lors de l'écriture du fichier de thème : {e}")
    return newTheme 

def get_user_theme(user_id):
    try:
        with open(f"./www/cookie/database/{user_id}.txt", "r") as file:
            theme = file.read().strip()
            return theme if theme in ["light", "dark"] else "light"
    except FileNotFoundError:
        return "light"
    except Exception as e:
        print(f"Erreur lors de la lecture du fichier de thème : {e}")
        return "light"

def generateId():
    return str(uuid.uuid4())

def generateExpirationDate():
    expiration_time = time.time() + 60 * 60 * 24 * 30
    formatted_time = time.strftime("%a, %d-%b-%Y %H:%M:%S GMT", time.gmtime(expiration_time))
    return formatted_time

def createNewCookie():
    user_id = generateId()
    expiration_date = generateExpirationDate()
    cookieLine = "Set-Cookie: session_id=" + user_id + "; Expires=" + expiration_date + "; Path=/\r\n"
    print(cookieLine)
    with open(f"./www/cookie/database/{user_id}.txt", "w") as file:
        file.write("light")
    return user_id

cookies = os.environ.get('HTTP_COOKIE', '')
user_id = None

if cookies == "":
    user_id = createNewCookie()
else:
    for cookie in cookies.split(';'):
        name, value = cookie.strip().split('=', 1)
        if name == "session_id":
            user_id = value
            break

if user_id is None:
    user_id = createNewCookie()


user_theme = get_new_user_theme(user_id)

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
<script>
				const jsConfetti = new JSConfetti()
                jsConfetti.addConfetti({{ confettiNumber: 1 }})

</script>
</html>
"""

print(html_content)
