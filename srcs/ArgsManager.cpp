
#include "ArgsManager.hpp"

/*
 * @brief Constructor by default
 *
 * @param argc : number of arguments
 * @param argv : arguments
 */
ArgsManager::ArgsManager(int ac, char **av) : _ac(ac), _av(av), _configFilePath(DEFAULT_CONFIG_FILE_PATH), _options(_generateOptions()), _state(EXIT_SUCCESS)
{
	_parse();
	if (isOption("--quiet"))
		Logger::setLogState(false);
	if (isOption("--log"))
		Logger::setLogFileState(true);
	if (isOption("--debug"))
		Logger::setLogDebugState(true);
}

/*
 * @brief Destructor by default
 */
ArgsManager::~ArgsManager(void) {}

/*  */
/* ---------------------------------- UTILS --------------------------------- */
/*  */

/*
 * @brief Generate the options map
 *
 * @return std::map<std::string, bool> : The generated options map
 */
std::map<std::string, bool> ArgsManager::_generateOptions(void)
{
	std::map<std::string, bool> options;

	// Help
	options["--help"] = false;
	// Logs in the console
	options["--quiet"] = false;
	// Logs in a file
	options["--log"] = false;
	// Debug mode
	options["--debug"] = false;

	return (options);
}

/*
 * @brief Get the long option
 *
 * @param option : The option to get
 *
 * @return std::string : The long option
 */
std::string ArgsManager::_convertToLongOption(std::string option)
{
	if (option == "-h")
		return ("--help");
	if (option == "-q")
		return ("--quiet");
	if (option == "-l")
		return ("--log");
	if (option == "-d")
		return ("--debug");
	return (option);
}

/*
 * @brief Get the short option
 *
 * @param option : The option to get
 *
 * @return std::string : The short option
 */
std::string ArgsManager::_convertToShortOption(std::string option)
{
	if (option == "--help")
		return ("-h");
	if (option == "--quiet")
		return ("-q");
	if (option == "--log")
		return ("-l");
	if (option == "--debug")
		return ("-d");
	return (option);
}
/*  */
/* ---------------------------------- MAIN ---------------------------------- */
/*  */

/*
 * @brief Parse the arguments
 */
void ArgsManager::_parse(void)
{
	try
	{
		for (int i = 1; i < _ac; i++)
		{
			std::string arg = _av[i];

			if ((arg.size() == 2 && arg[0] == '-') || (arg.size() > 2 && arg.substr(0, 2) == "--"))
			{
				if (arg == "-h" || arg == "--help")
					_options["--help"] = true;
				else if (arg == "-q" || arg == "--quiet")
					_options["--quiet"] = true;
				else if (arg == "-l" || arg == "--log")
					_options["--log"] = true;
				else if (arg == "-d" || arg == "--debug")
					_options["--debug"] = true;
				else
					throw WebservException(0, "illegal option -- %s", arg.substr(2).c_str());
				// throw std::invalid_argument("illegal option -- " + arg.substr(2));
			}
			else
			{
				if (_configFilePath == "./config/default.conf")
					_configFilePath = arg;
				else
					throw WebservException(0, "invalid argument -- %s (config file already set: \"%s\")", arg.c_str(), _configFilePath.c_str());
				// throw WebservException(0, "invalid argument -- %s", arg.c_str());
				// throw std::invalid_argument("invalid argument -- " + arg);
			}
		}
	}
	catch (const std::exception &e)
	{
		printMsg(std::cerr, "%s : %s\n", _av[0], e.what());
		// std::cerr << _av[0] << " : " << e.what() << std::endl << std::endl;
		_options["--help"] = true;
		_state = EXIT_FAILURE;
	}
}

/*
 * @brief Check if the option is enabled
 *
 * @param option : The option to check
 *
 * @return bool : True if the option is enabled, false otherwise
 */
bool ArgsManager::isOption(std::string option)
{
	std::string opt = _convertToLongOption(option);

	if (_options.find(opt) != _options.end() && _options[opt] == true)
		return (true);

	return (false);
}

/*
 * @brief Display the help
 */
void ArgsManager::help(void)
{
	/*
	std::string	help;

	help += "Usage: ";
	help += _av[0];
	help += " [options] [config_file]\n\n";
	help += "  Options:\n"
		"    -h, --help\t\tDisplay this information\n"
		"    -q, --quiet\t\tDisable the logs in the console\n"
		"    -l, --log\t\tEnable the logs in a file\n"
		"    -d, --debug\t\tEnable the debug mode\n"
		"\n";
	help += "  Config file:\n"
		"    The path to the configuration file\n"
		"    Default: ./config/default.conf\n";
	*/

	std::cout << "Usage: " << _av[0] << " [options] [config_file]" << std::endl
			  << std::endl
			  << "  Options:" << std::endl
			  << "    -h, --help\t\tDisplay this information" << std::endl
			  << "    -q, --quiet\t\tDisable the logs in the console" << std::endl
			  << "    -l, --log\t\tEnable the logs in a file" << std::endl
			  << "    -d, --debug\t\tEnable the debug mode" << std::endl
			  << std::endl
			  << "  Config file:" << std::endl
			  << "    The path to the configuration file" << std::endl
			  << "    Default: ./config/default.conf" << std::endl;
}

/*
 * @brief Display the summary
 */
void ArgsManager::summary(void)
{
	std::cout << "Summary:" << std::endl;
	std::cout << "  - Config file path: " << _configFilePath << std::endl;
	std::cout << "  - Options:" << std::endl;
	for (std::map<std::string, bool>::iterator it = _options.begin(); it != _options.end(); ++it)
	{
		std::cout << "    " << it->first << ": " << it->second << std::endl;
	}
}

/*  */
/* --------------------------------- GETTERS -------------------------------- */
/*  */

/*
 * @brief Get the state
 *
 * @return int : The state
 */
int ArgsManager::getState(void) const
{
	return (_state);
}

/*
 * @brief Get the config file path
 *
 * @return std::string : The config file path
 */
std::string ArgsManager::getConfigFilePath(void) const
{
	return (_configFilePath);
}
