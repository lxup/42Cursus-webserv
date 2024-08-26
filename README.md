
# Fichier de Configuration du Serveur Web 🛜

Ce document décrit les directives et les règles à suivre pour configurer votre serveur web en utilisant le fichier de configuration approprié.

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