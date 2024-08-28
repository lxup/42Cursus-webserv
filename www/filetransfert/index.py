#!/usr/bin/env python3

import cgi
import time
import os

download_path = "./www/filetransfert/"

# make function to load file list
def load_files():
	links = ""
	for file in os.listdir(download_path):
		if os.path.isfile(os.path.join(download_path, file)):
				if file[0] != "." and file != "index.py":
					links += f"""
						<div id="file-{file}" class="link">
							<div>{file}</div>

							<div class="link-buttons">
								<a href={file} class="link-button" download>
									Download
								</a>
								<form id="delete-{file}" onsubmit="deleteFile(event)">
									<button type="submit" class="link-button">
										Delete
									</button>
								</form>
							</div>

						</div>
					"""
	return links

def main():
	links = load_files()
	html_content = f"""
	<!DOCTYPE html>
	<html lang="en">
	<head>
		<meta charset="UTF-8">
		<title>Informations de l'Utilisateur</title>
		<style>
			@import url('https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&display=swap');

			.container{{
				--max-width: 1000px;
				--padding: 1rem;

				width: min(var(--max-width), 100% - (var(--padding) * 1.2));
				margin-inline: auto;
			}}
			body {{
				display: flex;
				justify-content: center;
				align-items: center;
				height: 100vh;
				font-family: 'Inter', sans-serif;
				background-color: #f0f0f0;
			}}
			div {{
				# font-size: 1.5rem;
				# font-weight: semibold;
				# color: #333;
				# text-align: center;
			}}
			h2 {{
				font-size: 2rem;
				font-weight: semibold;
				color: #333;
				text-align: center;
			}}
			.button{{
				border: none;
				padding: 1rem 3rem;
				background-color: #333;
				border-radius: 15px;
				font-weight: semibold;
				color: #f0f0f0;
				cursor: pointer;
			}}
			.hidden-input {{
				display: none;
			}}
			.links {{
				display: flex;
				flex-wrap: wrap;
				justify-content: center;
				flex-direction: column;
				background-color: #dedcdc;
				
				border: 1rem solid #f0f0f0;
				border-radius: 30px;
				padding: 1rem;
			}}
			.link {{
				diplay: flex;
				justify-content: space-between;
				align-items: center;
				padding: 5px 10px;
				# background-color: #121212;
				border-radius: 5px;
				transition: background-color 0.3s;
			}}
			.link:hover {{
				cursor: pointer;
				background-color: #d3d3d3;
			}}
			.link-buttons {{
				display: flex;
				align-items: center;
			}}
			.link-button {{
				padding: 5px 10px;
				background-color: #121212;
				color: white;
				text-decoration: none;
				border-radius: 5px;
				transition: background-color 0.3s;
			}}

		</style>
		<script>
            function handleFileSelect() {{
				const fileInput = document.getElementById('fileInput');
				const filenameInput = document.getElementById('filenameInput');
				if (fileInput.files.length) {{
					filenameInput.value = fileInput.files[0].name;
				}}
			}}

			function deleteFile(event) {{
				event.stopPropagation();
				event.preventDefault();

				var form = event.target;
				var file = form.id.replace('delete-', '');

				fetch('/filetransfert/' + file, {{
					method: 'DELETE'
				}})
				.then(response => {{
					if (response.ok) {{
						document.getElementById('file-' + file).remove();
					}}
					else {{
						alert('Failed to delete file');
					}}
				}})
				.catch(error => {{
					alert('Failed to delete file');
				}});
			}}

			function handleUpload(event) {{
				event.stopPropagation();
				event.preventDefault();

				console.log('Uploading file...');

				var form = event.target;
				var fileInput = form.file;
				var filename = form.filename.value;
				var enctype = document.getElementById('enctypeSelect').value;
				var filenameInput = document.getElementById('filenameInput').value;

				if (filenameInput) {{
					filename = filenameInput;
				}}
				

				if (!fileInput.files.length) {{
					alert('Please select a file to upload');
					return;
				}}

				console.log('Filename:', filename);
				console.log('File:', fileInput.files[0]);
				console.log('Encoding:', enctype);

				if (enctype === 'application/octet-stream') {{
					fetch('/filetransfert', {{
						method: 'POST',
						headers: {{
							'Filename': filename,
							'Content-Type': 'application/octet-stream',
						}},
						body: fileInput.files[0]
					}})
					.then(response => {{
						if (response.ok) {{
							alert('File uploaded successfully');
							window.location.reload();
						}} else {{
							alert('Failed to upload file');
						}}
					}})
					.catch(error => {{
						alert('Failed to upload file');
					}});
				}} else {{
					alert('Only application/octet-stream encoding is supported for now');
					return;
				}}

			}}
        </script>
	</head>
	<body>
		<div class="container">
			<h2>FileTransfert</h2>
			<div>
				<form id="uploadForm" onsubmit="handleUpload(event)">
					<label for="filenameInput">File Name:</label>
					<input id="filenameInput" type="text" name="filename">

					<label for="enctypeSelect">Select Encoding:</label>
					<select id="enctypeSelect">
						<option value="application/octet-stream">application/octet-stream</option>
						<option value="multipart/form-data">multipart/form-data</option>
					</select>

					<input id="fileInput" type="file" name="file" class="hidden-input" onchange="handleFileSelect()">
					<button type="button" class="button" onclick="document.getElementById('fileInput').click();">Select File</button>
					
					<button type="submit" class="button">Upload File</button>
				</form>
            </div>
            <div class="links">
                {links}
            </div>
		</div>
	</body>
	</html>
	"""

	print("Content-Type: text/html\r\n\r\n")
	print(html_content)

if __name__ == "__main__":
	main()