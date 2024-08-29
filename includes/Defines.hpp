#ifndef DEFINE_HPP
#define DEFINE_HPP

/* OS */
#if defined(__linux__)
#define LINUX 1
#elif defined(__APPLE__)
#define MAC 1
#endif

/* COLORS */
#define C_BLACK "\001\033[30m\002"		/* - Black color */
#define C_RED "\001\033[31m\002"		/* - Red color */
#define C_GREEN "\001\033[32m\002"		/* - Green color */
#define C_YELLOW "\001\033[33m\002"		/* - Yellow color */
#define C_BLUE "\001\033[34m\002"		/* Blue - color */
#define C_MAGENTA "\001\033[35m\002"	/* - Magenta color */
#define C_CYAN "\001\033[36m\002"		/* - Cyan color */
#define C_WHITE "\001\033[37m\002"		/* - White color */
#define C_GRAY "\001\033[90m\002"		/* - Gray color */
#define C_LIGHT_GRAY "\001\033[37m\002" /* - Light gray color */
#define C_BEIGE "\001\033[93m\002"		/* - Beige color */
#define C_RESET "\001\033[0m\002"		/* - Reset color */

/* BACKGROUND COLORS */
#define BG_BLACK "\001\033[40m\002"		  /* - Black background */
#define BG_RED "\001\033[41m\002"		  /* - Red background */
#define BG_GREEN "\001\033[42m\002"		  /* - Green background */
#define BG_YELLOW "\001\033[43m\002"	  /* - Yellow background */
#define BG_BLUE "\001\033[44m\002"		  /* - Blue background */
#define BG_MAGENTA "\001\033[45m\002"	  /* - Magenta background */
#define BG_CYAN "\001\033[46m\002"		  /* - Cyan background */
#define BG_WHITE "\001\033[47m\002"		  /* - White background */
#define BG_TRANSPARENT "\001\033[49m\002" /* - Transparent background */


#define REQUEST_FLAGS EPOLLIN | EPOLLRDHUP | EPOLLERR // Quand on attend une requete
#define RESPONSE_FLAGS EPOLLIN | EPOLLRDHUP | EPOLLERR | EPOLLOUT // Quand la reponse est prete est que l'on a quelque chose a envoyer dans le socket
#define MAX_EVENTS 100

#endif // DEFINE_HPP
