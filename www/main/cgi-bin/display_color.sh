#!/bin/bash

# Lire les données d'entrée de la requête POST
read -r input

# Extraire la valeur de la couleur à partir des données d'entrée
color=$(echo "$input" | sed -n 's/.*color=\([^&]*\).*/\1/p')

# Décode l'encodage URL (remplace les + par des espaces et les %xx par leur caractère ASCII)
color=$(echo "$color" | sed 's/+/ /g;s/%/\\x/g')

# En-tête HTTP

echo "Content-Type: text/html; charset=utf-8"
echo -e "\r\n\r\n"
echo ""

# Générer la page HTML avec la couleur de fond sélectionnée
cat << EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Selected Color: $color</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&display=swap');

        .container {
            --max-width: 1000px;
            --padding: 1rem;
            width: min(var(--max-width), 100% - (var(--padding) * 1.2));
            margin-inline: auto;
        }
        body {
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            font-family: 'Inter', sans-serif;
            background-color: $color;
        }
        div {
            font-size: 1.5rem;
            font-weight: semibold;
            color: #333;
            text-align: center;
        }
        .button {
            border: none;
            padding: 1rem 3rem;
            background-color: #333;
            border-radius: 15px;
            font-weight: semibold;
            color: #f0f0f0;
        }
    </style>
</head>
<body>
    <div class="container">
        <h2>The background color has been changed to $color!</h2>
        <form action="./display_color.sh" method="post">
            <label for="color">Choose a color:</label>
            <select id="color" name="color">
                <option value="red">Red</option>
                <option value="blue">Blue</option>
                <option value="green">Green</option>
                <option value="yellow">Yellow</option>
                <option value="purple">Purple</option>
            </select>
            <br><br>
            <input class="button" type="submit" value="Submit">
        </form>
    </div>
</body>
</html>
EOF
