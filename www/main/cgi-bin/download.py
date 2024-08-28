#!/usr/bin/env python3

import os
import cgi
import cgitb
import sys

# Enable debugging
cgitb.enable()

# Define the path where files are located
FILE_DIRECTORY = "./www/main/cgi-bin/uploads/"

def generate_html_message(message):
    return f"""
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>File Download</title>
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
            <h2>{message}</h2>
        </div>
    </body>
    </html>
    """

def main():
    # Parse form data
    form = cgi.FieldStorage()

    # Get the 'resource' parameter from the form
    resource = form.getvalue('resource')

    # Construct the full file path
    file_path = os.path.join(FILE_DIRECTORY, resource)

    # Check if the file exists
    if resource and os.path.isfile(file_path):
        # Send response headers to initiate a file download
        print(f"Content-Type: application/octet-stream")
        print(f"Content-Disposition: attachment; filename=\"{resource}\"")
        print(f"Content-Length: {os.path.getsize(file_path)}") # Optional
        print() # End of headers
        sys.stdout.flush()

        # Read the file and send it to the client
        # with open(file_path, 'rb') as file:
        #     print(file.read().decode('utf-8'))
        # Force the headers to be flushed

        # Read and send the file in chunks
        with open(file_path, 'rb') as file:
            chunk_size = 8192  # 8KB per chunk
            while chunk := file.read(chunk_size):
                sys.stdout.buffer.write(chunk)
                sys.stdout.buffer.flush()  # Ensure the data is sent immediately
    else:
        # If file does not exist, send an error message in HTML format
        print("Content-Type: text/html")
        print()  # End of headers
        print(generate_html_message("Erreur: Le fichier demandé n'existe pas."))

if __name__ == "__main__":
    main()
