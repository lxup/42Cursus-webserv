

# Webserv - Projet de Serveur Web en C++98

## Introduction

Ce projet consiste à créer un serveur HTTP en C++98 que vous pouvez tester avec un navigateur web. Ce serveur doit être compatible avec le protocole HTTP/1.1, l'un des protocoles les plus utilisés sur Internet.

## Objectif

L'objectif est de comprendre et d'implémenter les fonctionnalités de base d'un serveur HTTP, de la réception de requêtes à la réponse appropriée, en passant par la gestion des différents codes de statut HTTP.

<details>
<summary>🔍 Qu'est-ce que HTTP?</summary>
HTTP (Hypertext Transfer Protocol) est le protocole utilisé pour la communication entre un client (navigateur web) et un serveur. Les documents hypertextes peuvent contenir des liens vers d'autres ressources, permettant à l'utilisateur d'y accéder facilement.
</details>

## Liens Utiles

- [Programmation Réseau via Sockets en C](https://www.codequoi.com/programmation-reseau-via-socket-en-c/) 🥇
- [The Method to epoll's Madness](https://copyconstruct.medium.com/the-method-to-epolls-madness-d9d2d6378642) 🥈
- [Tutoriel Vidéo les méthodes HTTP](https://www.youtube.com/watch?v=bs1WluLpLqE&t=1411s) 🥉

## Codes de Statut HTTP

Les codes de statut HTTP indiquent le résultat d'une requête HTTP.

- **1xx : Informational** - Requête reçue, traitement en cours.
- **2xx : Success** - Requête reçue, comprise et acceptée avec succès.
- **3xx : Redirect** - Une action supplémentaire doit être effectuée pour compléter la requête.
- **4xx : Client Error** - La requête contient une erreur qui empêche le serveur de la traiter.
- **5xx : Server Error** - Le serveur a échoué à traiter une requête apparemment valide.

## Gestion des BlocServers

![Schema de BlocServers](https://prod-files-secure.s3.us-west-2.amazonaws.com/c549902f-29ad-4132-b93c-511423de312f/439c10dc-6378-42e8-832c-e25a0e79a044/image.png)

Webserv écoute sur 3 ports différents: **80**, **1313** et **9090**.

Pour chaque requête entrante, Webserv doit déterminer quel bloc de configuration utiliser. Voici quelques exemples :

- `http://jul.com:80` ➔ Bloc Bleu (servername et port correspondants)
- `http://bob.com:1313` ➔ Bloc Rouge
- `http://existepas.com:1313` ➔ Bloc Vert (bloc par défaut)
- `http://nul.com:4321` ➔ Pas de réponse (port non écouté)
  
---


<details>
<summary>Structure d'une Requête HTTP</summary>

Une requête HTTP permet au client de demander une ressource au serveur.

1. **Ligne de Requête**  
   - **Méthode**: Action à réaliser sur le serveur (GET, POST, DELETE, etc.)
   - **URI**: Adresse de la ressource demandée sur le serveur.
   - **Version HTTP**: Version du protocole HTTP utilisée (HTTP/1.1, HTTP/1.0).

   Exemple:
   ```http
   GET /img/logo.jpg HTTP/1.0
   ```

2. **En-têtes de la Requête**  
   Paires clé-valeur fournissant des informations sur la requête ou le client.

   Exemple:
   ```http
   Host: abc.com
   Accept: text/html
   Cookie: _ga=GA1.2.407.167011
   ```

3. **Corps de la Requête** (pour POST et PUT uniquement)  
   Contient les données que le client souhaite transmettre au serveur.

   Exemple:
   ```http
   name=John+Doe&age=30&city=New+York
   ```
</details>



<details>
<summary> Structure d'une Réponse HTTP </summary>

La réponse HTTP est ce que le serveur renvoie après avoir reçu une requête.

1. **Ligne de Statut**  
   - **Version HTTP**
   - **Code de Statut**: Exemples courants :
     - `200 OK` : Requête traitée avec succès.
     - `404 Not Found` : Ressource introuvable.
     - `500 Internal Server Error` : Erreur interne du serveur.
   - **Message**: Phrase associée au code de statut.

   Exemple:
   ```http
   HTTP/1.1 200 OK
   ```

2. **En-têtes de Réponse**  
   Paires clé-valeur fournissant des informations sur la réponse ou le serveur.

   Exemple:
   ```http
   Content-Encoding: gzip
   Content-Length: 342
   Date: Sat, 08 Jan 2022 10:52:28 GMT
   ```

3. **Corps de la Réponse**  
   Contient la réponse elle-même, telle que la page HTML demandée.

</details>


<details>
<summary> Requêtes Chunked et Délimitation </summary>

Les requêtes chunked permettent d'envoyer des données en plusieurs morceaux de taille variable.

<details>
<summary>💡 Fonctionnement des Requêtes Chunked</summary>
Chaque chunk suit le format : `[taille du chunk en hexadécimal]\r\n[données du chunk]\r\n`. La fin de la transmission est indiquée par `0\r\n\r\n`.
</details>

Exemple de chunk:
```http
4\r\nWiki\r\n
```

### Délimitation des Requêtes HTTP

Une requête HTTP standard se termine soit :
- Par une ligne vide après les en-têtes (si aucun corps n'est présent).
- Par la réception de l'intégralité des données spécifiées par `Content-Length`.
- Par le chunk de fin (pour les requêtes chunked).

</details>


<details>
<summary>Fichier de Configuration du Serveur Web 🛜</summary>


Doc et règle pour le fichier de configuration

## Règles Générales ⚠️

- Les lignes commençant par `#` sont des commentaires. Les commentaires doivent être sur une ligne séparée et ne peuvent pas être mélangés avec une directive.
- Il est interdit d'avoir deux blocs `location` avec le même chemin (`path`) dans un bloc `server`.
- Un bloc `server` peut contenir plusieurs `server_name` et plusieurs `listen` (`ip:port`).
- Deux blocs `server` ne peuvent pas avoir le même `server_name`.
- Deux blocs `server` peuvent partager le même `listen` (`ip:port`).

## Structure des Blocs

```plaintext
server {
    ...
    location {
        ...
    }
    ...
}
```

## Détails des Directives ✅

Le tableau ci-dessous résume les directives disponibles dans le fichier de configuration, y compris leur duplicabilité, le nombre de paramètres autorisés, et leurs valeurs par défaut.

| **Directive**            | **Bloc**        | **Duplication** | **Nb Paramètres** | **Valeur par Défaut**       | **Description**                                                                                                                                                    | **Exemple**                                           |
|--------------------------|-----------------|-----------------|-------------------|-----------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------|-------------------------------------------------------|
| `server`                 | N/A             | DUP             | 0                 | none                        | Définit un bloc de configuration pour un serveur web virtuel.                                                                                                       | `server { ... }`                                      |
| `location`               | `server`        | DUP             | 1                 | none                        | Définit un bloc de configuration pour une URL spécifique.                                                                                                           | `location / { ... }`                                  |
| `listen`                 | `server`        | DUP             | 1                 | `ip: 0.0.0.0 port: 80`      | Définit l'adresse IP et le port sur lequel le serveur web doit écouter les requêtes.                                                                                 | `listen 80;`, `listen 127.0.0.1:8080;`                |
| `server_name`            | `server`        | DUP             | -1                | `localhost`                 | Définit le(s) nom(s) de domaine (host) sur lequel le serveur web doit répondre.                                                                                     | `server_name louis.com;`                              |
| `error_page`             | `server`        | DUP             | -1                | `/var/www/error.html`       | Définit les pages d'erreur personnalisées. La syntaxe est stricte : `error_page CODE /path/to/file;`.                                                               | `error_page 404 /404.html;`                           |
| `root`                   | `server`/`location` | NODUP        | 1                 | `/var/www/html`             | Définit le répertoire racine du serveur web (ou du bloc location). Dans un bloc `location`, cette directive surcharge celle du bloc `server` et ne peut pas être utilisée avec `alias`. | `root /var/www/html;`                                 |
| `index`                  | `server`/`location` | DUP          | -1                | `index.html`                | Définit les fichiers index par défaut (ou du bloc location). Dans un bloc `location`, cette directive surcharge celle du bloc `server`.                             | `index index.html index.htm;`                         |
| `client_max_body_size`   | `server`        | NODUP           | 1                 | `1` (Mo)                    | Définit la taille maximale des données que le serveur web peut recevoir (en Mo).                                                                                    | `client_max_body_size 10;`                            |
| `alias`                  | `location`      | NODUP           | 1                 | none                        | Définit un alias pour un répertoire. Ne peut pas être utilisé avec `root`.                                                                                          | `alias /var/www/images/;`                             |
| `return`                 | `location`      | DUP             | 2                 | none                        | Définit une règle de réécriture d'URL.                                                                                                                              | `return 301 https://github.com/toukoum;`              |
| `autoindex`              | `location`      | NODUP           | 1                 | `off`                       | Active ou désactive l'indexation automatique des répertoires. Ne doit pas coexister avec la directive `index` dans le bloc `location`.                               | `autoindex on;`                                       |
| `allow_methods`          | `location`      | NODUP           | 0..3              | `GET DELETE POST`           | Définit les méthodes HTTP autorisées.                                                                                                                               | `allow_methods GET DELETE POST;`                      |
| `cgi_extension`          | `location`      | DUP             | 2                 | none                        | Définit l'extension qui sera mappée à un script CGI.                                                                                                                | `cgi_extension .php /var/www/cgi-bin/php-cgi;`        |
| `upload_path`            | `location`      | NODUP           | 1                 | `/var/www/upload`           | Définit le répertoire de destination des fichiers uploadés.                                                                                                         | `upload_path /var/www/images;`                        |

## Exemples de Configuration

```plaintext
server {
    listen 80;
    server_name louis.com;
    root /var/www/html;
    index index.html index.htm;
    error_page 404 /404.html;

    location / {
        root /var/www/html;
        index index.html index.htm;
    }

    location /upload {
        allow_methods POST;
        upload_path /var/www/upload;
        cgi_extension .php /var/www/cgi-bin/php-cgi;
    }
}
```
</details>
