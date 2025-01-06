// PasswordGenerator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

#define VERSION "1.0.0.5"

typedef struct
{
    unsigned short length;
    unsigned short count;
    unsigned short maxSpecial;
    bool allowLowercase;
    bool allowUppercase;
    bool allowDigits;
    bool allowSimilar;
    bool allowSpecial;
    bool allowDuplicate;
    bool startWithLetter;
    bool saveOnExit;
} Configuration;

std::string filter( std::string charset, std::string filter )
{
    std::string filtered = charset;

    // Remove undesirable characters from 'filtered'
    for ( std::string::iterator it = filter.begin(); it != filter.end(); it++ )
    {
        std::string::iterator found = std::find( filtered.begin(), filtered.end(), *it );

        if ( found != filtered.end() )
        {
            filtered.erase( found );
        }
    }

    return filtered;
}

std::string generatePassword( const Configuration* configuration )
{
    std::string charsetUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string charsetLower = "abcdefghijklmnopqrstuvwxyz";
    std::string charsetDigits = "0123456789";
    std::string charsetSpecial = "!@#$%^&*()_+{}|:<>?-=[];',./";
    std::string charsetSimilar = "iloILO01!|/";

    std::string charset;

    // Randomizer function
    std::random_device rd;
    std::mt19937 g( rd() );

    std::string password;

    short specialCount = 0;

    if ( !configuration->allowSimilar )
    {
        // Remove similar characters
        charsetUpper = filter( charsetUpper, charsetSimilar );
        charsetLower = filter( charsetLower, charsetSimilar );
        charsetDigits = filter( charsetDigits, charsetSimilar );
        charsetSpecial = filter( charsetSpecial, charsetSimilar );
    }

    // Make sure it has one of each requested type
    if ( configuration->allowUppercase )
    {
        charset += charsetUpper;

        password += charsetUpper[ rand() % ( charsetUpper.length() - 1 )];
    }
    if ( configuration->allowLowercase )
    {
        charset += charsetLower;

        password += charsetLower[ rand() % ( charsetLower.length() - 1 ) ];
    }
    if ( configuration->allowDigits )
    {
        charset += charsetDigits;

        password += charsetDigits[ rand() % ( charsetDigits.length() - 1 ) ];
    }
    if ( configuration->allowSpecial )
    {
        // If we are only allowed one special character, don't add others to the main charset
        if ( configuration->maxSpecial != 1 )
        {
            charset += charsetSpecial;
        }

        password += charsetSpecial[ rand() % ( charsetSpecial.length() - 1 ) ];
        
        // Allow the number of special characters to be limited
        specialCount++;
    }

    // Impossible
    if ( charset.length() == 0 )
    {
        std::cerr << "No character sets selected" << std::endl;

        return "";
    }

    // Impossible
    if ( !configuration->allowDuplicate && charset.length() < configuration->length )
    {
        std::cerr << "Insufficient characters available for requested configuration" << std::endl;

        return "";
    }

    // If we must start with a letter, then use what we've already added to the string
    if ( configuration->startWithLetter )
    {
        if ( !configuration->allowLowercase && !configuration->allowUppercase )
        {
            std::cerr << "Cannot start password with a letter" << std::endl;

            return "";
        }
        else if ( configuration->allowLowercase && configuration->allowUppercase )
        {
            // First two characters should be an upper and a lower by this point, so shuffle them
            std::string::iterator it1 = password.begin();
            std::string::iterator it2 = password.begin();
            it2++;
            it2++;

            std::shuffle( it1, it2, g );
        }
        // else only of of uppercase or lowercase allowed, so our prototype password already starts correctly
    }

    if ( !configuration->allowDuplicate )
    {
        // Each character can only be used once in the password - erase any already selected from the charset
        for ( std::string::const_iterator it = password.cbegin(); it != password.cend(); it++ )
        {
            charset.erase( std::find( charset.begin(), charset.end(), *it ) );
        }
    }

    // Fill out the rest of the password
    while ( password.length() < configuration->length )
    {
        char c = charset[ rand() % ( charset.length() - 1 ) ];

        if ( !configuration->allowDuplicate )
        {
            // Each character can only be used once in the password - erase them from the charset as they get used
            charset.erase( std::find( charset.begin(), charset.end(), c ) );
        }

        // Keep track of special characters?
        if ( configuration->maxSpecial > 0 )
        {
            if ( std::find( charsetSpecial.begin(), charsetSpecial.end(), c ) != charsetSpecial.end() )
            {
                specialCount++;
                if ( specialCount >= configuration->maxSpecial )
                {
                    // No more after this one - erase all of the specials from the main charset
                    for ( std::string::const_iterator it = charsetSpecial.cbegin(); it != charsetSpecial.cend(); it++ )
                    {
                        charset.erase( std::find( charset.begin(), charset.end(), *it ) );
                    }
                }
            }
        }

        password += c;
    }

    // A final shuffle of the password
    if ( configuration->startWithLetter )
    {
        // Leave the first character unmoved as we have already made sure it is a letter
        std::string::iterator it1 = password.begin();
        std::string::iterator it2 = password.end();
        it1++;

        std::shuffle( it1, it2, g );
    }
    else // Shuffle the whole thing
    {
        std::shuffle( password.begin(), password.end(), g );
    }

    // Clip to length if required (unexpected unless length is a very small number)
    if ( password.length() > configuration->length )
    {
        password = password.substr( 0, configuration->length );
    }

    return password;
}

bool processConfigurationItem( std::string configurationItem, Configuration* configuration )
{
    // Assume everything starts with a + or -
    if ( configurationItem[ 0 ] == '+' || configurationItem[ 0 ] == '-' )
    {
        bool flag = configurationItem[ 0 ] == '+';

        std::string argument = configurationItem.substr( 1 );

        if ( argument == "allow-uppercase" )
        {
            configuration->allowUppercase = flag;
        }
        else if ( argument == "allow-lowercase" )
        {
            configuration->allowLowercase = flag;
        }
        else if ( argument == "allow-digits" )
        {
            configuration->allowDigits = flag;
        }
        else if ( argument == "allow-special" )
        {
            configuration->allowSpecial = flag;
        }
        else if ( argument == "allow-similar" )
        {
            configuration->allowSimilar = flag;
        }
        else if ( argument == "allow-duplicate" )
        {
            configuration->allowDuplicate = flag;
        }
        else if ( argument == "start-with-letter" )
        {
            configuration->startWithLetter = flag;
        }
        else if ( argument.substr( 0, 6 ) == "save" )
        {
            configuration->saveOnExit = flag;
        }
        else if ( argument.substr( 0, 7 ) == "length:" )
        {
            configuration->length = atoi( argument.substr( 7 ).c_str() );
        }
        else if ( argument.substr( 0, 6 ) == "count:" )
        {
            configuration->count = atoi( argument.substr( 6 ).c_str() );
        }
        else if ( argument.substr( 0, 12 ) == "max-special:" )
        {
            configuration->maxSpecial = atoi( argument.substr( 12 ).c_str() );
        }
        else
        {
            std::cerr << "Invalid option: " << configurationItem << std::endl;

            // Don't save if there is a problem with the configuration
            configuration->saveOnExit = false;
            return false;
        }
    }
    else
    {
        std::cerr << "Invalid command line argument: " << configurationItem << std::endl;

        // Don't save if there is a problem with the configuration
        configuration->saveOnExit = false;
        return false;
    }

    return true;
}

// Read the settings from a file into the configuration.
// If the file does not exit, that's OK.
// For the benefit of the caller, return true if a configuration file was found
bool processConfigurationFile( std::string filename, Configuration* configuration ) 
{
    std::ifstream file( filename );

    if ( file.is_open() )
    {
        std::string item;
        while ( file >> item )
        {
            processConfigurationItem( item, configuration );
        }

        file.close();
        return true;
    }

    return false;
}

int main( int argc, char** argv )
{
    // Seed the random number generator
    srand( static_cast<unsigned int>( time( 0 ) ) );

    Configuration configuration;

    // Default values
    configuration.length = 12;
    configuration.count = 1;
    configuration.maxSpecial = 0;
    configuration.saveOnExit = false;

    configuration.allowUppercase = true;
    configuration.allowLowercase = true;
    configuration.allowDigits = true;
    configuration.allowSpecial = true;
    configuration.allowSimilar = false;
    configuration.allowDuplicate = true;
    configuration.startWithLetter = false;

    // Configuration filename is 
    // - Windows: the executable filename with a ".cfg" extension     "PasswordGenerator.cfg"
    // - Other:   the executable filename with a dot and no extension ".PasswordGenerator"
    std::filesystem::path executable( argv[ 0 ] );
#ifdef _WIN32
    std::string helpOption = "-help";
    std::string versionOption = "-version";

    // Look for a config file in the current directory
    std::string configFile = executable.filename().replace_extension( ".cfg" ).string();
    processConfigurationFile( configFile, &configuration );
#else // e.g. Linux or Apple
    std::string helpOption = "--help";
    std::string versionOption = "--version";

    // Look for a config file in the current directory. If none found, try the user's home directory
    std::string configFile = "." + executable.filename().replace_extension("").string();
    if ( !processConfigurationFile( configFile, &configuration ) )
    {
        const char* homeDir = std::getenv("HOME");
        if ( homeDir )
        {
            processConfigurationFile( std::string( homeDir ) + "/" + configFile, &configuration );
        } 
    }
#endif // _WIN32 or _WIN64

    // Load configuration values from file, if present
    std::ifstream file( configFile );

    if ( file.is_open() )
    {
        std::string item;
        while ( file >> item )
        {
            processConfigurationItem( item, &configuration );
        }
    }

    file.close();

    // Look for command line configuration overrides
    for ( int loop = 1; loop < argc; loop++ )
    {
        std::string argument = argv[ loop ];

        if ( argument == helpOption || argument == versionOption )
        {
            // Ignore for now and process later
            continue;
        }

        if ( !processConfigurationItem( argument, &configuration ) )
        {
            return -5;
        }
    }

    // Look for command line questions
    bool keepRunning = true;
    for ( int loop = 1; loop < argc; loop++ )
    {
        std::string argument = argv[ loop ];

        if ( argument == versionOption )
        {
            std::cout << "Password Generator " << VERSION << std::endl;

            keepRunning = false;
            break;
        }
        else if ( argument == helpOption )
        {
            std::cout << "Password Generator " << VERSION << std::endl;
            std::cout << std::endl;
            std::cout << "Command line options:" << std::endl;
            std::cout << std::endl;
            std::cout << "-length:N             Generate passwords of N characters long (" << configuration.length << ")" << std::endl;
            std::cout << "-count:N              Generate N passwords (" << configuration.count << ")" << std::endl;
            if ( configuration.maxSpecial == 0 )
            {
                std::cout << "-max-special:N        Include at most N special characters (unlimited)" << std::endl;
            }
            else
            {
                std::cout << "-max-special:N        Include at most N special characters (" << configuration.maxSpecial << ")" << std::endl;
            }
            std::cout << std::endl;
            std::cout << "Use the options below with either + or - to enable/disable" << std::endl;
            std::cout << std::endl;
            std::cout << "+/-allow-uppercase    Include uppercase characters in a password (" << (configuration.allowUppercase ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << "+/-allow-lowercase    Include lowercase characters in a password (" << (configuration.allowLowercase ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << "+/-allow-digits       Include numeric digits in a password       (" << (configuration.allowDigits ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << "+/-allow-special      Include special characters in a password   (" << (configuration.allowSpecial ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << "+/-allow-similar      Include similar characters in a password   (" << (configuration.allowSimilar ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << "+/-allow-duplicate    Allow duplicated characters in a password  (" << (configuration.allowDuplicate ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << "+/-start-with-letter  Ensure the first character is a letter     (" << ( configuration.allowDuplicate ? "enabled" : "disabled" ) << ")" << std::endl;
            std::cout << std::endl;
            std::cout << "+/-save               Save the provided configuration as default (" << (configuration.saveOnExit ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << std::endl;
            std::cout << "Example:" << std::endl;
            std::cout << "    PasswordGenerator.exe +allow-uppercase -allow-lowercase -length:16" << std::endl;

            keepRunning = false;
            break;
        }
        else
        {
            // Nothing else to do - we've already processed this
            continue;
        }
    }

    // If we showed help or version, don't continue
    if ( !keepRunning )
    {
        return - 1;
    }

    if ( configuration.count == 0 )
    {
        std::cerr << "Nothing to do" << std::endl;

        return -3;
    }

    if ( configuration.length == 0 )
    {
        std::cerr << "Password cannot be zero length" << std::endl;

        return -4;
    }

    for ( short count = 0; count < configuration.count; count++ )
    {
        std::string password = generatePassword( &configuration );

        if ( password.length() > 0 )
        {
            std::cout << generatePassword( &configuration ) << std::endl;
        }
        else
        {
            // Don't save the configuration if this caused an error
            return -2;
        }
    }

    if ( configuration.saveOnExit )
    {
        std::ofstream file( configFile );

        file << "-length:" << configuration.length << std::endl;
        file << "-count:" << configuration.count << std::endl;
        file << "-max-special:" << configuration.maxSpecial << std::endl;
        file << ( configuration.allowUppercase ? "+" : "-" ) << "allow-uppercase" << std::endl;
        file << ( configuration.allowLowercase ? "+" : "-" ) << "allow-lowercase" << std::endl;
        file << ( configuration.allowDigits ? "+" : "-" ) << "allow-digits" << std::endl;
        file << ( configuration.allowSpecial ? "+" : "-" ) << "allow-special" << std::endl;
        file << ( configuration.allowSimilar ? "+" : "-" ) << "allow-similar" << std::endl;
        file << ( configuration.allowDuplicate ? "+" : "-" ) << "allow-duplicate" << std::endl;
        file << ( configuration.startWithLetter ? "+" : "-" ) << "start-with-letter" << std::endl;

        file.close();
    }

    // Command line configuration with a default setup
    // 
    // Arguments with value:                Default:            Option:
    // - password length                    - default 16        -length=[0-9]+
    // - number of passwords to generate    - default 1         -count=[0-9]+
    // 
    // Flags (on/off):                      Default:            Option:
    // - uppercase characters               on                  +/-allow-uppercase
    // - lowercase characters               on                  +/-allow-lowercase
    // - numeric characters                 on                  +/-allow-digits
    // - special characters                 on                  +/-allow-special
    // - similar characters                 on                  +/-allow-similar
    // - duplicated characters              on                  +/-allow-duplicate
    // - save this configuration            off                 +/-save
    // ? start with a letter                on                  +/-start-with-letter
    // ? avoid sequences (123, abc)         on                  +/-avoid-sequences

    return 0;
}
