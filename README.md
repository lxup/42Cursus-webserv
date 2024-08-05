# Webserv 42


<aside>
⁉️ Creer un serveur web que l’on peut tester avec un navigateur web en cpp98

</aside>


# Todo pour le projet

## Divers

- [x]  faire un script qui simule x connection au server (avec x un argument qui represente le nombre de client) afin de voir a quel point notre server est efficace
    
    ⇒ done, ce script s’appelle manyConnections.py
    
- [ ]  voir la fonction fcntl() ce qu’elle fait et si elle peut etre utile sur linux ? ou alors c’est que pour mac ?

## Parsing

- [ ]  Parser le fichier de configuration pour recup les infos necessaire (hotes, port, path)
- [ ]  Faire un systeme de verification des erreurs de syntaxe et de logique.
- [ ]  Si error, ne pas lancer le server (logique batard)

## Initialisation et lancement du Server

- [ ]  Pour chaque bloc `server` creer un socket en le liant a l’adresse ip
- [ ]  Rendres les socket en mode non-bloquants
- [ ]  Ecouter les connexions avec listen() ?

## Gerer les requetes entrantes

- [ ]  faire une boucle avec epoll() pour gerer les evenement
    - [ ]  epoll_wait()
    - [ ]  accept()
    - [ ]  quand les sockets clients sont pret en lecture, parser la requete http
    - [ ]  gerer les erreurs, 404, methode non prise en charge, …

## Traiter les requetes (les insulter)

- [ ]  En fonction de l’url et de la methode http, faire ce qu’il faut
    - [ ]  GET: renvoi des fichiers statiques ou execute un CGI
    - [ ]  POST: gere les envois de donnees
    - [ ]  DELETE: supprime un truc sur le server
- [ ]  Pour les cgi:
    - [ ]  Utiliser fork() et exeve() (salut minishell)
    - [ ]  gerer les requete chunked en les dechunkant (ahah trop drole ce mot)
        
        → je fais pas ca ca a l’air trop dur
        

## Reponse HTTP

- [ ]  Construire la reponse http
- [ ]  utiliser send() ou write() ?
- [ ]  si erreur, envoyer les page personalise, 404, 500

## Explication Sujet

<aside>
❓ Checking the value of errno is strictly forbidden after a read or a write operation

</aside>

En gros comme il va y avoir beaucoup d’operation I/O simultanees sur mon server, verifier errno ca pose probleme ca sa valeur peut changer entre le moment ou un erreur est survenue et le moment ou je la verifie.

du coup apres chaque operation de lecture ou d’ecriture, il faut se reposer sur les resultat de poll (ou equivalent, epoll, select, …) pour savoir si un descripteur de fichier est pret pour le lecture ou l’ecriture.

<aside>
❓ You can’t use fork for something else than CGI (like PHP, or Python, and so forth).

</aside>

En gros un cgi est une norme qui permet au server web d’executer des script et de fournir la reponse au client. ils peuvent etre ecrit dans divers languages.

il est necessaire de fork afin d’isoler l’execution du script dans un processus a part pour proteger le processus principal du server web.

<aside>
❓ The first server for a host:port will be the default for this host:port (that means
it will answer to all the requests that don’t belong to an other server)

</aside>

En gros si plusieurs serveur ecoute sur le meme port mais sur des server_name different, et qu’une requete arrive sur ce port ne correspondant a aucun server_name, la requete est redirige vers le premier bloc server declarer

<aside>
❓ Setup the server_names or not

</aside>

si le server_names n’est pas declare, le bloc server en question va gerer toutes les requete qui arrive sur ce port

<aside>
❓ Turn on or off directory listing.

</aside>

Il faut determiner si la requete du client pointe vers un dossier, si c’est le cas, il faut afficher une page a la main avec tout les fichiers/dossiers present dans ce dossier

## Fonctions utilise

- **socket()**
    
    ```cpp
    int socket(int domain, int type, int protocol);
    ```
    
    retourne le fd creer, ou alors -1 pour une erreur
    
    `domain` refers to the protocol the socket will use for communication. Some possible values are:
    
    - `AF_UNIX, AF_LOCAL` - Local communication
    - `AF_INET` - IPv4 Internet protocols
    - `AF_INET6` - IPv6 Internet protocols
    - `AF_IPX` - IPX Novell protocols
    
    `type` specifies if the communication will be conectionless, or persistent. Not all `types` are compatible with all `domains`. Some examples are:
    
    - `SOCK_STREAM` - Two-way reliable communication (TCP)
    - `SOCK_DGRAM` - Connectionless, unreliable (UDP)
    
    Normally there is only one `protocol` available for each `type`, so the value `0` can be used.
    
- **Quelles sont les différences entre `select()`, `poll()`, et `epoll()` et `kqueue()` ?**
    
    poll ⇒ fonction bloquante/non-bloquante **(dépend des flags)**, surveille des fds définis par des **structures de données**.
    
    select ⇒ surveille les fds, bloque jusqu’à ce qu’un fd soit prêt pour une opération.
    
    epoll ⇒ pour linux, plus efficace pour un grand nombre de fds, sépare les fds en interest list et ready list.
    
    kqueue ⇒ pour mac, similaire a epoll()
    
    - gpt
        
        ### `select()`
        
        - **Pro:** Facile à comprendre et à utiliser.
        - **Con:** Limité par la taille maximale de `fd_set`, souvent 1024. Peut devenir inefficace avec un grand nombre de descripteurs de fichiers à surveiller, car il doit vérifier tous les descripteurs à chaque appel.
        - **Utilisation:** Bien pour un nombre limité de connexions.
        
        ### `poll()`
        
        - **Pro:** Pas de limite fixe sur le nombre de descripteurs de fichiers. Utilise une structure de tableau qui peut être plus flexible que `fd_set`.
        - **Con:** Comme `select()`, il doit vérifier tous les descripteurs à chaque appel, ce qui peut être inefficace pour un grand nombre de connexions.
        - **Utilisation:** Meilleur que `select()` pour un nombre plus important de connexions.
        
        ### `epoll()` (spécifique à Linux)
        
        - **Pro:** Très efficace pour un grand nombre de descripteurs. Au lieu de vérifier tous les descripteurs à chaque appel, il utilise un mécanisme de notification pour suivre les descripteurs actifs.
        - **Con:** Spécifique à Linux, donc pas portable.
        - **Utilisation:** Idéal pour des serveurs web à grande échelle sous Linux.
        
        ### `kqueue` (spécifique à BSD et macOS)
        
        - **Pro:** Efficace et flexible, similaire à `epoll()`. Permet de surveiller différents types d'événements, pas seulement les descripteurs de fichiers.
        - **Con:** Spécifique à BSD et macOS, donc pas portable.
        - **Utilisation:** Idéal pour des serveurs web à grande échelle sous BSD ou macOS.
- **bind()**
    
    ```cpp
    int bind(int sockfd, const sockaddr *addr, socklen_t addrlen);
    ```
    
    sert a associe un port a un socket, en gros ca dit au pc “hey, cette socket va ecouter sur cette adresse”
    
    Similar to `socket`, the function returns -1 in case of error. In case of success, it returns 0.
    
    `sockfd` ⇒ c’est le fd du socket creer
    
    `addr` is a 
    struct used to specify the address we want to assign to the socket. The 
    exact struct that needs to be used to define the address, varies by 
    protocol. Since we are going to use IP for this server, we will use [sockaddr_in](http://man7.org/linux/man-pages/man7/ip.7.html):
    ``
    
    ```cpp
    struct sockaddr_in {
       sa_family_t    sin_family; /* address family: AF_INET */
       in_port_t      sin_port;   /* port in network byte order */
       struct in_addr sin_addr;   /* internet address */
    };
    ```
    
    `addrlen` is just the `size()` of `addr`.
    
- **listen**
    
    marque un socket comme passive, cela signifie qu’il va accepter des connections
    
    ```cpp
    int listen(int sockfd, int backlog)
    ```
    
    Returns -1 in case of error. In case of success, it returns 0.
    
    `sockfd` fd du socket
    
    `backlog` is the maximum number of connections that will be queued before connections start being refused.
    
- **accept**
    
    [accept](http://man7.org/linux/man-pages/man2/accept.2.html) creer un un nouveau socket avec la connection entrante. du coup ca renvoit un autre fd de la nouvelle connection creer. 
    
    c’est avec ce fd que l’on va communiquer avec le client.
    
    ```cpp
    int accept(int sockfd, sockaddr *addr, socklen_t *addrlen);
    ```
    
    The function will return -1 if there is an error. On success, it will return a file descriptor for the connection.
    
    les arguments c’est les meme que pour la fonction bind, sauf
    
    `addrlen` qui est la valeur du result. ca attend un pointer vers un int qui va etre initialise a la taille d’addr. 
    

## Principale METHODE HTTP

### GET

**GET** tu demande une ressources au server et il te la renvoit

**Exemple** :

- Tu envoies : "Bonjour, quelle est la couleur de ton vélo ?"
- Tu reçois : "Mon vélo est rouge."

### POST

**POST** tu envois une ressource au server et il la stocke

**Exemple** :

- Tu envoies : "Bonjour, voici un dessin que j'ai fait pour toi."
- Ton ami reçoit le dessin et il te remercie.

### DELETE

**DELETE** tu supprimes une ressource que tu as precedement envoye ou alors qu’y est present sur le server

**Exemple** :

- Tu envoies : "Bonjour, peux-tu s'il te plaît supprimer le dessin que je t'avais envoyé hier ?"
- La boîte aux lettres magique le supprime.

### PUT

**PUT** tu met a jour une ressources que tu as envoye

**Exemple** :

- Tu envoies : "Bonjour, voici la nouvelle version de ma lettre avec les bonnes informations."
- La boîte aux lettres magique remplace l'ancienne lettre par la nouvelle.

## URL and URI

une url est un type specifique d’uri

![Untitled](images/Untitled.png)

## Nginx

<aside>
💡 Serveur web performant open source. Sa capacite a gerer de nombreuse connexions en simultanees le rend tres performant. **C’est un reverse proxy**

</aside>

Souvent utilise pour:

- Servir des fichiers statiques (HTML, images, vidéos, etc.).
- Agir comme un proxy inverse pour équilibrer la charge entre différents serveurs backend.
- Gérer des connexions sécurisées (SSL/TLS).
- Fournir des fonctions avancées comme la mise en cache et la compression

⇒ le fichier de configuration de nginx se trouve generalement dans: **/etc/nginx/nginx.conf**

⇒ Ce fichier contient des ligne (directive) de key/value pair, suivi d’un ‘;’

 

⇒ les bloc `main`, `http`, `events`, `server`, `location` sont appele des **context**

- le bloc `main` englobe tout.
- `events` et `http` sont dans le `main`
- `server` est dans `http`
- `location` dans `server`

`root`  c’est l’endroit ou nginx va chercher les fichiers a servir quand le bloc location correspond  

### Myme type

`myme type` c’est pour  que le css fonctionne. C’est pour que le header de la requete html contienne text/css plutot que text/plain

pour ca, on peut mapper un par un les fichier envers leur valeur des headers

```bash
type {
	text/css css;
	text/html;
}
```

mais la meilleur pratique c’est de faire

```bash
include mime.types;
```

c’est le fichiers qui contient tous les valeur remapper pour tout les fichiers differents

### Servir du contenu static

generalement, le fichier nginx.conf contient plusieur bloc server, qui ecoute tous sur des ports differents

Une fois que nginx a decide quel `server` servir, il test l’uri specifie dans le header de la requete avec le bon bloc location.

par exemple, si la requete est: 

```bash
http://localhost/images/example.png
```

ca va chercher le path le plus long qui correspond a cette localisation:

donc si on a une config comme cela:

```bash
server {
    location / {
        root /data/www;
    }

    location /images/ {
        root /data;
    }
}
```

le server va chercher le fichier example.png dans /data/images/

- exemple 2
    
    alias modifie les dossiers dans lequel on va chercher le contenu
    
    ```bash
    location /fruits {
    	root /data/www/
    }
    
    location /carbs {
    	alias /data/www/fruits; // ca va pas rajouter /carbs a la fin pour
    													// la recherche mais /fruits 
    													// pas besoin d'avoir un /carbs directory
    }
    ```
    

### Configurer nginx comme un reverse proxy

mais l’utilisation principale de nginx c’est de servir de proxy server, c’est a dire qu’il va transmettre la requette a un autre proxy, recuperer la reponse puis la renvoyer au client

par exemple pour un nginx.conf pour le proxied server:

```bash
server {
    listen 8080;
    root /data/up1;

    location / {
    }
}
// il ecoute sur le port 8080
```

si il n’y a pas de root dans le bloc location, le server va utiliser le root du bloc server

et le fichier de conf pour le server proxy

```bash
server {
    location / {
        proxy_pass http://localhost:8080/;
    }

    location ~ \.(gif|jpg|png)$ { // selectionne le format des image avec un regex
        root /data/images;
    }
}

```

### Load Balancer

c’est le fait de repartir la charge des requete entre les differents server du meme site

l’algorithme utilise est l’algorithm de round robin.

```bash
http{

	upstream backendserver{
		server 127.0.0.1:1111;
		server 127.0.0.1:2222;
		server 127.0.0.1:3333;
		server 127.0.0.1:4444;
	}
	
	
	server{
		// toute les requete vers / vont etre redirige vers une 
		// des quatre ip definie dans upstream
		location / {
			proxy_pass http://backendserver/;
		}
	}
}
```

## RFC 2616

C’est la doc qui definie le protocol http/1.1

http/1.1 c’est une version du protocol http, utilise pour la communication sur le web.

les principales features de http:1.1:

- **Connexion Persistante** : Les connexions peuvent rester ouvertes après une requête, permettant de multiples requêtes/réponses sur une seule connexion TCP, ce qui réduit la latence.
- **Pipeline de Requêtes** : Les clients peuvent envoyer plusieurs requêtes sans attendre les réponses correspondantes, optimisant ainsi le temps de chargement.
- **Gestion Améliorée des Erreurs** : Plus de codes d'état et une meilleure gestion des erreurs, ce qui permet une communication plus robuste.
- **Cache** : Meilleure gestion du cache pour améliorer les performances et réduire la bande passante.
- **Chunked Transfer Encoding** : Permet l'envoi de réponses en morceaux, ce qui est utile pour les réponses de taille inconnue au départ

## Proxy vs reverse Proxy

⇒ **Un proxy** est un “middleman” entre un client et un server, il masque donc l’adress ip du client et seulement l’adress ip du proxy est visible. Il permet donc de bypass des restrictions present sur certains pays sur internet.

- Schema proxy
    
    ![Untitled](images/Untitled%201.png)
    

Un proxy peut aussi permettre de filtrer des choses sur internet. Par exemple dans une ecole, on pourrait connecter tout le reseau internet a un proxy pour filtrer quels site on a le droit d’aller ou pas. 

⇒ **Un reverse Proxy** permet de rediriger des connections qui proviennent d’internet vers des servers assigne dans un reseau protege.

le reverse proxy peut donc par exemple repartir la charge correctement les requete vers un meme site sur plusieurs serveur.

- Schema reverse Proxy
    
    ![Untitled](images/Untitled%202.png)
    

## Proxy vs VPN

<aside>
💡 les deux servent a masquer ton adress ip d’internet en faisant un middleman entre le client et le server. Et c’est tout, les similarites s’arrete la.

</aside>

| Aspect | Proxy | VPN |
| --- | --- | --- |
| Sécurité | Faible, pas de chiffrement | Élevée, chiffrement des données |
| Anonymat | Masque l'IP | Masque l'IP et chiffre les données |
| Performance | Généralement rapide | Peut ralentir la connexion |
| Configuration | Facile, souvent intégré dans les navigateurs | Plus complexe, nécessite un logiciel ou une application |
| Utilisation | Contrôle d'accès, anonymat basique | Sécurité en ligne, accès global |

# Outils

---

## Telnet

Telnet c’est un protocol qui se base sur http. il permet de se connecter de facon non securise (en comparaison a ssh) a un server a distance. 

pour se connecter

```cpp
telnet <hostname> <port>
// du coup pour se co a notre server

telnet localhost 4321

```

une fois connecter, on peut faire des requete au server:

```cpp
GET / HTTP/1.1

// et la on obtient la reponse du server
```

et on peut aussi le faire sur le protocol smtp

```cpp
telnet smtp.example.com 25
```

et apres on peut envoye un mail:

```cpp
HELO example.com
MAIL FROM:<your-email@example.com>
RCPT TO:<recipient@example.com>
DATA
Subject: Test Email

This is a test email sent via Telnet.
.
QUIT
```

## CURL command

### Qu'est-ce que `curl` ?

`curl` est un outil en ligne de commande utilisé pour transférer des données depuis ou vers un serveur en utilisant des protocoles comme HTTP, HTTPS, FTP, etc. C'est un outil puissant pour interagir avec les API, télécharger des fichiers et tester des connexions réseau.

### Utilisation de base de `curl`

La syntaxe de base de `curl` est :

```bash
curl <URL>

```

### Options importantes de `curl`

### 1. **Faire une requête GET**

La commande par défaut de `curl` est une requête GET :

```bash
curl <http://example.com>

```

### 2. **Faire une requête POST**

Pour envoyer des données avec une requête POST :

```bash
curl -X POST -d "param1=value1&param2=value2" <http://example.com/resource>

```

ou avec un fichier JSON :

```bash
curl -X POST -H "Content-Type: application/json" -d '{"key1":"value1", "key2":"value2"}' <http://example.com/resource>

```

### 3. **Ajouter des en-têtes HTTP**

Pour ajouter des en-têtes HTTP à la requête :

```bash
curl -H "Authorization: Bearer <token>" -H "Content-Type: application/json" <http://example.com/resource>

```

### 4. **Télécharger un fichier**

Pour télécharger un fichier et l'enregistrer avec un nom spécifique :

```bash
curl -o myfile.txt <http://example.com/file.txt>

```

ou pour conserver le nom du fichier original :

```bash
curl -O <http://example.com/file.txt>

```


# Code

---

## Server simple qui renvoit une simple page html

```cpp
int raf()
{
	int port = 1234;
	int fdSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (!fdSocket)
	{
		perror("Error with creating socket");
		exit(EXIT_FAILURE);
	}

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	if (bind(fdSocket, (const sockaddr *)(&address), sizeof(address)) < 0)
	{
		perror("Error with Bind function");
		exit(EXIT_FAILURE);
	}

	if (listen(fdSocket, 10000) < 0)
	{
		perror("Error with listen function");
		exit(EXIT_FAILURE);
	}

	bool active = true;
	int connection;
	while (active)
	{
		unsigned long resultLen = sizeof(sockaddr);
		Logger::log(Logger::INFO, "Listening on Port: %d", port);
		if ((connection = accept(fdSocket, (struct sockaddr *)(&address), (socklen_t *)&resultLen)) < 0)
		{
			perror("Error with accept function");
			exit(EXIT_FAILURE);
		}

		char buffer[100];
		ssize_t bytesRead = read(connection, buffer, 100);
		Logger::log(Logger::INFO, "Le message fait: %d", bytesRead);
		Logger::log(Logger::INFO, buffer);

		// std::string response = "Salut du mini Server de Raf\n";
		const std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: \n\n<h1>Hello, World bonjour louis!</h1>";
		if (send(connection, response.c_str(), response.size(), 0));
		close(connection);
	}

	close(fdSocket);

	return (EXIT_SUCCESS);
}

```

## Comment gerer plusieurs connection en meme temps ?

- **Maniere 1** en utilisant des thread (pas safe)
    
    Le but d’un serveur web est de pouvoir servir une ressource à plusieurs clients en même temps. Pour cela, il faut implémenter du multithreading en utilisant des fonctions telles que `poll()` mentionnées dans le sujet.
    
    Plutôt que de créer un thread par connexion entrante (ce qui n'est pas optimal), on va utiliser un pool de threads. Par exemple, on peut créer un pool de 20 threads qui seront utilisés successivement pour chaque nouvelle connexion.
    
    Pour cela, on crée une `deque` de taille maximale 20 pour représenter les connexions.
    
    1. Le serveur attend une connexion.
    2. Lorsqu'une connexion est reçue, elle est ajoutée dans la `deque` et traitée par un thread du pool.
    3. Une fois la connexion traitée, le thread est libéré.
    
    Le problème de cette approche est que si aucune connexion n'est présente, le serveur effectue du “busy waiting” (attente active) en vérifiant constamment l'arrivée de nouvelles connexions, ce qui est très consommateur en énergie pour le CPU.
    
    Pour contrer cela, on utilise une **condition variable** qui permet de mettre les threads en attente jusqu'à ce qu'un événement se produise. Cela réduit considérablement la consommation de ressources CPU.
    

## Probleme 1: Plusieurs clients qui mettent longtemps a envoyer leur request

Si le nombre de clients N est supérieur ou égal au nombre de threads dans mon pool, tous les threads seront utilisés, et mon serveur ne pourra plus traiter de nouvelles connexions.

Les fonctions d’input/output (I/O) sont presque toutes bloquantes, ce qui pose problème dans ce contexte.

Nous avons besoin de fonctions asynchrones qui permettent de faire une requête, de continuer d'autres tâches, puis de revenir à la requête une fois qu'elle est terminée. Cela permet de gérer efficacement les entrées/sorties sans bloquer les threads et d'assurer que le serveur reste réactif même sous forte charge.

**Pour resoudre ce probleme**

- Event driven programming
- Asynchronous input/output
    
    
    ### Select()
    
    Cette fonction va prendre un groupe de fd a surveiller et une fois qu’un des fd est dispo a lire ou a ecrire, elle le signal
    
    ```cpp
    //prototype
    int select(int nb_max de fd, void *les fd a surveiller pour le reading,
    void *les fds a surveiller pour le writing, void *les erreur, 
    int TIMEOUT == optionnal pour attentdre un temps donnes les changements)
    ```
    
    ```cpp
    // fonction qui initialise le server et qui renvoit le socket associe
    int server_socket = setup_server()
    
    fd_set curent_sockets, ready_sockets;
    
    //initialize my current set 
    FD_ZERO(&current_sockets);
    FD_SET(server_socket, &current_sockets);
    
    // boucle qui va attendre toute les connections
    while (true){
    	// sauvegarde parce que select() va me niquer current_sockets;
    	ready_sockets = current_sockets;
    	
    	if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0){
    		handle_error()
    	}
    	
    	log("Waiting for connections...\n");
    	for (int i = 0; i < FD_SETSIZE; i++){
    		if (FD_ISSET(i, &ready_sockets){
    			if (i == server_socket){
    				// this is a new connection
    				int client_socket = accept_new_connection(server_socket);
    				FD_SET(client_socket, &current_sockets);
    			}else{
    				// do whatever we do with connections.
    				handle_connection(i);
    				FD_CLR(i, &current_sockets);
    			}
    		}
    	}
    
    	handle_connection(client_socket);
    
    }
    	
    ```
    

# Divers

---

## Question/Reponse principales

- **c’est quoi un fd ?**
    
    un fd c’est un int positif qui reference un fichier ouvert dans un process.
    
    par default, ils heritent de 3 fds, 0, 1, 2, stdin, stdout et stderr.
    
- **Qu'est-ce que le protocole HTTP et quelle est sa fonction principale ?**
    
    HTTP se base **sur TCP**. **Sa fonction principale est** de transférer des **ressources** d’un serveur à un client.
    
- **Comment le serveur web traite-t-il une requête HTTP d'un client ?**
    
     il récupère **et analyse** le header de la requête, puis répond avec les fichiers demandés **ou une réponse appropriée** selon la méthode.
    
- **Qu'est-ce qu'un fichier de configuration et quel est son rôle dans un serveur web ?**
    
    Le fichier de configuration définit les différents **serveurs virtuels** et **paramètres de comportement** du serveur web.
    
- **Qu'est-ce que le polling et pourquoi est-il utilisé dans ce projet ?**
    
    c’est le fait de gérer plusieurs connexions simultanément sans bloquer le serveur.
    
- **Pourquoi est-il important que ton serveur soit non-bloquant ?**
    
    Pour pouvoir gerer plusieur client en meme temps
    
- **Qu'est-ce qu'un CGI (Common Gateway Interface) et comment fonctionne-t-il ?**
    
     c’est une interface pour exécuter des scripts côté serveur, renvoyant la sortie au client en réponse HTTP.
    
- **Pourquoi et comment utiliserais-tu des extensions comme `.php` ou `.py` dans ton serveur ?**
    
    Pour exécuter des scripts CGI et générer du contenu dynamique en réponse aux requêtes HTTP.
    
- **Quels sont les défis liés à la gestion des requêtes chunked et comment les surmonter ?**
    
    c’est de gérer un message en plusieurs morceaux, il faut assembler les chunks et traiter le tout comme un message complet.
    
- **Quels mécanismes peux-tu mettre en place pour t'assurer que ton serveur ne plante jamais ?**
    
    utiliser des try-catch pour gérer les exceptions, des tests de charge, surveiller et redémarrer automatiquement le serveur en cas de crash.
    
- **Quelle fonction utiliser pour ecouter les fds ?**
    - **Si tu es sous Linux** :
        - Utilise `epoll()`. C'est plus performant que `poll()` pour gérer un grand nombre de descripteurs de fichiers.
        - Ex : `epoll_create1()`, `epoll_ctl()`, `epoll_wait()`.
    - **Si tu es sous macOS ou BSD** :
        - Utilise `kqueue()`. Il est conçu pour être performant et flexible sur ces systèmes.
        - Ex : `kqueue()`, `kevent()`.
    - **Si tu veux du code portable** :
        - Utilise `poll()`. Bien qu'il soit moins performant pour un grand nombre de descripteurs de fichiers, il est disponible sur presque tous les systèmes Unix/Linux.
        - Ex : `poll()`.
