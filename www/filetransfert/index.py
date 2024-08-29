#!/usr/bin/env python3

import cgi
import time
import os

download_path = "./www/filetransfert/uploads/"
upload_url = "/filetransfert/uploads/"

# make function to load file list
def load_files():
	links = ""
	for file in os.listdir(download_path):
		if os.path.isfile(os.path.join(download_path, file)):
				if file[0] != "." and file != "index.py":
					links += f"""
						<li class="row" id="file-{file}">
                            <div class="content upload">
								<i class="fas fa-file-alt"></i>
								<div class="details">
								<span class="name">{file}</span>
								<span class="size">{os.path.getsize(download_path + file)} bytes</span>
								</div>
							</div>
							<div>
								<a href="{upload_url}{file}" class="link-button" download><i class="fas fa-download"></i></a>
								<i class="fas fa-trash" onclick="handleDelete('{file}')"></i>
							</div>
						</li>
					"""
	return links

def main():
	links = load_files()
	html_content = f"""
	<!DOCTYPE html>
	<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<title>Informations de l'Utilisateur</title>
		<link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css"/>
		<style>
			/* Import Google font - Poppins */
			@import url('https://fonts.googleapis.com/css2?family=Poppins:wght@400;500;600&display=swap');
			*{{
			margin: 0;
			padding: 0;
			box-sizing: border-box;
			font-family: "Poppins", sans-serif;
			}}
			body{{
			display: flex;
			align-items: center;
			justify-content: center;
			min-height: 100vh;
			background: #f0f0f0;
			}}

			::selection{{
			color: #fff;
			background: #6990F2;
			}}
			.wrapper{{
			width: 430px;
			background: #1E1E1E;
			border-radius: 20px;
			padding: 30px;
			box-shadow: 7px 7px 12px rgba(0,0,0,0.05);
			}}
			.wrapper header{{
			color: white;
			font-size: 27px;
			font-weight: 600;
			text-align: center;
			}}
			.wrapper form{{
			height: 167px;
			display: flex;
			cursor: pointer;
			margin: 30px 0;
			align-items: center;
			justify-content: center;
			flex-direction: column;
			border-radius: 5px;
			border: 2px dashed white;
			}}
			form :where(i, p){{
			color: white;
			}}
			form i{{
			font-size: 50px;
			}}
			form p{{
			margin-top: 15px;
			font-size: 16px;
			}}

			section .row{{
			margin-bottom: 10px;
			background: #E9F0FF;
			list-style: none;
			padding: 15px 20px;
			border-radius: 5px;
			display: flex;
			align-items: center;
			justify-content: space-between;
			}}
			section .row i{{
			color: #6990F2;
			font-size: 30px;
			}}
			section .details span{{
			font-size: 14px;
			}}
			.progress-area .row .content{{
			width: 100%;
			margin-left: 15px;
			}}
			.progress-area .details{{
			display: flex;
			align-items: center;
			margin-bottom: 7px;
			justify-content: space-between;
			}}
			.progress-area .content .progress-bar{{
			height: 6px;
			width: 100%;
			margin-bottom: 4px;
			background: #fff;
			border-radius: 30px;
			}}
			.content .progress-bar .progress{{
			height: 100%;
			width: 0%;
			background: #6990F2;
			border-radius: inherit;
			}}
			.file-area{{
			max-height: 232px;
			overflow-y: scroll;
			}}
			.file-area.onprogress{{
			max-height: 150px;
			}}
			.file-area::-webkit-scrollbar{{
			width: 0px;
			}}
			.file-area .row .content{{
			display: flex;
			align-items: center;
			}}
			.file-area .row .details{{
			display: flex;
			margin-left: 15px;
			flex-direction: column;
			}}
			.file-area .row .details .size{{
			color: #404040;
			font-size: 11px;
			}}
			.file-area i.fa-trash, .file-area i.fa-download{{
				font-size: 20px;
				cursor: pointer;
			}}
			.file-area i.fa-trash{{
				color: #700000;
			}}
			.file-area i.fa-download{{
				color: #1d8038;
			}}
		</style>
		<script>
			function handleDelete(filename) {{
				var xhr = new XMLHttpRequest();
				xhr.open('DELETE', '{upload_url}' + filename, true);

				// Event listener for completion
				xhr.onload = function() {{
					if (xhr.status === 200) {{
						document.getElementById('file-' + filename).remove();
					}} else {{
						alert('Failed to delete file');
					}}
				}};
				xhr.send();
			}}

			function handleFileSelect() {{
				const fileInput = document.getElementById('fileInput');
				if (fileInput.files.length) {{
					let filename = fileInput.files[0].name;
					uploadFile(filename);
				}}
			}}

			function uploadFile(name){{
				progressArea = document.querySelector(".progress-area");
				uploadedArea = document.querySelector(".file-area");
				let xhr = new XMLHttpRequest();
				xhr.open('POST', '{upload_url}' + name, true);
				xhr.setRequestHeader("Filename", name);
				xhr.setRequestHeader("Content-Type", "application/octet-stream");

				xhr.upload.onprogress = function(event){{
					if (event.lengthComputable){{
						var previousRow = document.getElementById("upload-file-" + name);
						if (previousRow){{
							previousRow.remove();
						}}

						let row = document.createElement("li");
						row.classList.add("row");
						row.id = "upload-file-" + name;
						row.innerHTML = `
							<div class="content upload">
								<i class="fas fa-file-alt"></i>
								<div class="details">
									<span class="name">${{name}}</span>
									<span class="size">${{fileInput.files[0].size}} bytes</span>
								</div>
							</div>
							<div class="progress-area">
								<div class="content">
									<div class="details">
										<span class="name">${{name}}</span>
										<span class="size">${{fileInput.files[0].size}} bytes</span>
									</div>
									<div class="progress-bar">
										<div class="progress" style="width: ${{(event.loaded / event.total) * 100}}%"></div>
									</div>
								</div>
							</div>
						`;
						progressArea.appendChild(row);
					}}
				}};

				xhr.onload = function(){{
					if (xhr.status === 200){{
						let row = document.createElement("li");
						row.classList.add("row");
						row.id = "file-" + name;
						row.innerHTML = `
							<div class="content upload">
								<i class="fas fa-file-alt"></i>
								<div class="details">
									<span class="name">${{name}}</span>
									<span class="size">${{fileInput.files[0].size}} bytes</span>
								</div>
							</div>
							<div>
								<a href="{upload_url}${{name}}" class="link-button" download><i class="fas fa-download"></i></a>
								<i class="fas fa-trash" onclick="handleDelete('${{name}}')"></i>
							</div>
						`;
						uploadedArea.appendChild(row);
						progressArea.innerHTML = "";
					}}
					else{{
						alert('Failed to upload file');
						var previousRow = document.getElementById("upload-file-" + name);
						if (previousRow){{
							previousRow.remove();
						}}
					}}
				}};

				xhr.onerror = function(){{
					alert('Failed to upload file');
					var previousRow = document.getElementById("upload-file-" + name);
					if (previousRow){{
						previousRow.remove();
					}}
				}};

				xhr.send(fileInput.files[0]);
				}}
        </script>
	</head>
	<body>
		<div class="wrapper">
			<header>FileTransfert</header>
			<form action="#" id="uploadForm" onclick="document.getElementById('fileInput').click()">
			<input id="fileInput" class="file-input" type="file" name="file" hidden onchange="handleFileSelect()">
			<i class="fas fa-cloud-upload-alt"></i>
			<p>Browse File to Upload</p>
			</form>
			<section class="progress-area"></section>
			<section class="file-area">
				{links}
			</section>
		</div>
	</body>
	</html>
	"""
	
	print("Content-Type: text/html\r\n\r\n")
	print(html_content)

if __name__ == "__main__":
	main()