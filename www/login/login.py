#!/usr/bin/env python3

import os
import cgitb
import uuid
import cgi
import time
import urllib.parse

isNewClient = False

cgitb.enable()

print("Content-Type: text/html; charset=utf-8")

# ========= CREATE =========
def generateId():
    return str(uuid.uuid4())

def generateExpirationDate():
    expiration_time = time.time() + 60 * 60 * 24 * 30
    formatted_time = time.strftime("%a, %d-%b-%Y %H:%M:%S GMT", time.gmtime(expiration_time))
    return formatted_time

def createNewCookie():
    global isNewClient
    isNewClient = True
    form = cgi.FieldStorage()
    name = form.getvalue("username")
    password = form.getvalue("password")

    if name is None or password is None:
        return None

    isNewClient = False
    user_id = generateId()
    expiration_date = generateExpirationDate()
    print(f"Set-Cookie: id={user_id}; Expires={expiration_date}; Path=/\r\n")
    if not os.path.exists("./www/login/database"):
        os.makedirs("./www/login/database")
    with open(f"./www/login/database/{user_id}.txt", "w") as file:
        file.write(f"{name}\n{password}")
    return user_id

# ========= READ =========

def saveNote():
    form = cgi.FieldStorage()
    note = form.getvalue("note")
    
    if note is None:
        return
    
    user_id = getUserIdFromCookie()
    if user_id is None:
        return
    
    file_path = f"./www/login/database/{user_id}.txt"
    if not os.path.exists(file_path):
        return
    
    with open(file_path, "r") as file:
        lines = file.readlines()
        if len(lines) < 2:
            return
        username = lines[0].strip()
        password = lines[1].strip()
    
    with open(file_path, "w") as file:
        file.write(f"{username}\n{password}\n{note}")

def getUserIdFromCookie():
    cookies = os.environ.get('HTTP_COOKIE', '')
    user_id = None
    if cookies:
        for cookie in cookies.split(';'):
            parts = cookie.strip().split('=', 1)
            if len(parts) == 2:
                name, value = parts
                if name == "id":
                    user_id = value
                    break
    return user_id

def getUserInfo(user_id):
    saveNote()
    file_path = f"./www/login/database/{user_id}.txt"
    
    if not os.path.exists(file_path):
        print("<html><body><h1>Erreur: utilisateur non trouvé</h1></body></html>")
        return None
    
    with open(file_path, "r") as file:
        lines = file.readlines()
        if len(lines) < 2:
            print("<html><body><h1>Erreur: données utilisateur incorrectes</h1></body></html>")
            return None
        username = lines[0].strip()
        password = lines[1].strip()
        if len(lines) > 2:
            note = lines[2].strip()
        else:
            note = ""
    return {"username": username, "password": password, "note": note}

# Code principal
user_id = getUserIdFromCookie()
userInfo = {"username": "", "password": "", "note": ""}
if user_id is None:
    user_id = createNewCookie()
else:
    userInfo = getUserInfo(user_id)
    if userInfo is None:
        isNewClient = True


loginPage = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login Page</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&display=swap');

        .container {{
            --max-width: 400px;
            --padding: 2rem;
            width: min(var(--max-width), 100% - (var(--padding) * 1.2));
            margin-inline: auto;
            background-color: white;
            padding: 2rem;
            border-radius: 15px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }}
        body {{
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            font-family: 'Inter', sans-serif;
            background-color: #f0f0f0;
            margin: 0;
        }}
        h1 {{
            font-size: 2rem;
            font-weight: 600;
            color: #333;
            text-align: center;
            margin-bottom: 1rem;
        }}
        form {{
            display: flex;
            flex-direction: column;
        }}
        label {{
            font-size: 1rem;
            margin-bottom: 0.5rem;
            color: #333;
        }}
        input[type="text"],
        input[type="password"] {{
            padding: 0.8rem;
            font-size: 1rem;
            margin-bottom: 1rem;
            border-radius: 10px;
            border: 1px solid #ccc;
        }}
        .button {{
            margin-top: 1rem;
            border: none;
            padding: 1rem;
            background-color: #333;
            border-radius: 15px;
            font-weight: semibold;
            color: #f0f0f0;
            cursor: pointer;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>Login</h1>
        <form action="./" method="post">
            <label for="username">Username</label>
            <input type="text" id="username" name="username" required>
            <label for="password">Password</label>
            <input type="password" id="password" name="password" required>
            <button class="button" type="submit">Login</button>
        </form>
    </div>
</body>
</html>
"""


resultPage = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login Page</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&display=swap');

        .container {{
            --max-width: 400px;
            --padding: 2rem;
            width: min(var(--max-width), 100% - (var(--padding) * 1.2));
            margin-inline: auto;
            background-color: white;
            padding: 2rem;
            border-radius: 15px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            display: flex;
            flex-direction: column;
            align-items: center;
        }}
        body {{
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            font-family: 'Inter', sans-serif;
            background-color: #f0f0f0;
            margin: 0;
        }}
        h1 {{
            font-size: 2rem;
            font-weight: 600;
            color: #333;
            text-align: center;
            margin-bottom: 1rem;
        }}
        .button {{
            font-size: .8rem;
            margin-top: 1rem;
            border: none;
            padding: 1rem;
            background-color: #333;
            border-radius: 15px;
            font-weight: semibold;
            color: #f0f0f0;
            cursor: pointer;
        }}

        .button:hover {{
          opacity: 0.9;
        }}


        .delete {{
          background-color: #dc2626;
        }}

        .buttons {{
          display: flex;
          justify-content: center;
          gap: 1rem;
        }}

        .link{{
            text-decoration: none;
            color: #f0f0f0;
        }}

    </style>
</head>
<body>
    <div class="container">
        <h1>Welcome {userInfo['username']}</h1>
        <p>Your password is: {userInfo['password']}</p>
        <textarea id="note" name="note" rows="4" cols="50">{userInfo['note']}</textarea>
        <div class="buttons">
          <button class="button" onclick="saveNote()" >Save notes</button>
          <a class="link" href="/"><div class="button">Home</div></a>
          <button class="button delete" onclick="window.location.href = './logout.py'">Delete Acount</button>      
        </div>
    </div>
</body>
<script>
function saveNote() {{
    let note = document.getElementById("note").value;
    
    fetch("./login.py", {{
        method: "POST",
        headers: {{
            'Content-Type': 'application/x-www-form-urlencoded'
        }},
        body: new URLSearchParams({{
            'note': note
        }})
    }})
    .then(response => response.text())
    .then(data => {{
        console.log("Note saved:", data);
        alert("Note saved successfully!");
    }})
    .catch(error => {{
        console.error("Error saving note:", error);
        alert("Failed to save note.");
    }});
}}
</script>
</html>
"""

if (isNewClient == True):
  print("\r\n")
  print(loginPage)
else:
  print(resultPage)
