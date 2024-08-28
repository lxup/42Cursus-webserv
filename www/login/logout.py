#!/usr/bin/env python3

import cgitb
import os

# Activer le mode débogage pour afficher les erreurs sur la page
cgitb.enable()

# En-tête HTTP pour indiquer que le contenu qui suit est du HTML
print("Content-Type: text/html; charset=utf-8")

def getIdClient():
    cookies = os.environ.get('HTTP_COOKIE', '')
    user_id = None

    for cookie in cookies.split(';'):
        parts = cookie.strip().split('=', 1)
        if len(parts) == 2:
            name, value = parts
            if name == "id":
                user_id = value
                break
    return user_id

user_id = getIdClient()

if user_id is not None:
    if os.path.exists(f"./www/login/database/{user_id}.txt"):
      os.remove(f"./www/login/database/{user_id}.txt")
    print("Set-Cookie: id=; Expires=Thu, 01 Jan 1970 00:00:00 GMT; Path=/\r\n")
    message = "Your account has been successfully deleted."
else:
    message = "An error occurred while logging out."

page = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Logout Page</title>
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
            font-size: 1.6rem;
            font-weight: 600;
            color: #333;
            text-align: center;
            margin-bottom: 1rem;
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

        .button a{{
            text-decoration: none;
            color: #f0f0f0;
        }}
    </style>
</head>
<body>
    <div class="container">
        <h1>{message} </h1>
        <a href="/"><div class="button">Home</div></a>
    </div>
</body>
</html>
"""

print(page)
