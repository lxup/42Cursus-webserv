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

- [x]  Parser le fichier de configuration pour recup les infos necessaire (hotes, port, path)
- [x]  Faire un systeme de verification des erreurs de syntaxe et de logique.
- [x]  Si error, ne pas lancer le server (logique batard)
- [ ]  reste le cgi a gerer
- [ ]  reste le bloc server par default a gerer

## Initialisation et lancement du Server

- [ ]  Pour chaque groupe de bloc `server` (pair ip/port) creer un socket en le liant a l’adresse ip
    - [ ] Avec socket(), bind(), listen()
- [ ]  Rendres les socket en mode non-bloquants
- [ ]  Ecouter les connexions avec listen() ?

## Gerer les requetes entrantes

- [ ]  faire une boucle avec epoll() pour gerer les evenement (on surveille les sockets) a la fois les socket d'ecoute ip:port et a la fois les socket de conexion client
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

