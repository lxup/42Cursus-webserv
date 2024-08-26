#!/usr/bin/python3
import os
import sys
import urllib.parse

# make infinite loop
# while True:
# 	pass

# Lire le corps de la requête depuis l'entrée standard (stdin)
content_length = int(os.environ.get('CONTENT_LENGTH', 0))
# post_data = sys.stdin.read(content_length)
query_string = os.environ.get('QUERY_STRING', '')

# Parse le corps de la requête
# params = urllib.parse.parse_qs(post_data)
params = urllib.parse.parse_qs(query_string)
name = params.get('name', [''])[0].capitalize() # Rendre la première lettre en majuscule
age = params.get('age', [''])[0]

# Générer la réponse HTML
print("Status: 201\r\n")
print("Content-Type: text/html\r\n\r\n")
print(f"<html><body><h1>Bonjour {name}, tu as {age} ans!</h1></body></html>")
# print(f"Status: 404")