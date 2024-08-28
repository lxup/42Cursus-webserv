#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <iostream>

// Fonction pour préparer le tableau des variables d'environnement
char** prepareEnv(const std::string& cookies) {
    char** env = new char*[2]; // 1 variable + 1 NULL
    std::string http_cookie = "HTTP_COOKIE=" + cookies;

    // Allocation et copie de la chaîne
    env[0] = new char[http_cookie.size() + 1];
    std::strcpy(env[0], http_cookie.c_str());

    env[1] = nullptr; // Terminateur NULL pour le tableau d'environnements
    return env;
}

int main() {
    // Chemin vers ton exécutable CGI compilé
    const char* cgi_path = "/Users/toukoum/42/webserv/www/cookie/cookie.out";
    const char* argv1 = "./www/cookie/cookie.out";
    // Préparation des arguments pour execve
    char* const argv[] = {const_cast<char*>(cgi_path), const_cast<char*>(argv1), nullptr};  // Pas d'arguments supplémentaires
    
    // Exemple de cookie récupéré, pourrait être lu depuis la requête HTTP
    std::string cookies = "session_id=12345";
    
    // Préparation des variables d'environnement
    char** env = prepareEnv(cookies);

    // Appel de execve pour exécuter le CGI
    if (execve(cgi_path, argv, env) == -1) {
        perror("execve");
        return EXIT_FAILURE;
    }

    // Libération de la mémoire
    delete[] env[0];
    delete[] env;
    
    return 0;
}
