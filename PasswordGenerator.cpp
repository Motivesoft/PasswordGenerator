// PasswordGenerator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

typedef struct
{
    unsigned short length;
    unsigned short count;
    bool allowLowercase;
    bool allowUppercase;
    bool allowDigits;
    bool allowSimilar;
    bool allowSpecial;
    bool allowDuplicate;
    bool saveOnExit;
} Configuration;

std::string generatePassword( const Configuration* configuration )
{
    std::string charsetUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string charsetLower = "abcdefghijklmnopqrstuvwxyz";
    std::string charsetDigits = "0123456789";
    std::string charsetSpecial = "!@#$%^&*()_+{}|:<>?-=[];',./";
    std::string charsetSimilar = "iloILO01!|/";

    std::string charset;

    if ( configuration->allowUppercase )
    {
        charset += charsetUpper;
    }
    if ( configuration->allowLowercase )
    {
        charset += charsetLower;
    }
    if ( configuration->allowDigits )
    {
        charset += charsetDigits;
    }
    if ( configuration->allowSpecial )
    {
        charset += charsetSpecial;
    }

    if ( !configuration->allowSimilar )
    {
        // Filter similar characters unless we are only using one charset
        int charsetCount =
            ( configuration->allowUppercase ? 1 : 0 ) +
            ( configuration->allowLowercase ? 1 : 0 ) +
            ( configuration->allowDigits ? 1 : 0 ) +
            ( configuration->allowSpecial ? 1 : 0 );

        if ( charsetCount > 1 )
        {
            // Remove similar characters
            for ( std::string::iterator it = charsetSimilar.begin(); it != charsetSimilar.end(); it++ )
            {
                std::string::iterator found = std::find( charset.begin(), charset.end(), *it );

                if ( found != charset.end() )
                {
                    charset.erase( found );
                }
            }
        }
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

    std::string password;

    for ( int i = 0; i < configuration->length; i++ )
    {
        char c = charset[ rand() % ( charset.length() - 1 ) ];

        if ( !configuration->allowDuplicate )
        {
            // Each character can only be used once in the password - erase them from the charset as they get used
            charset.erase( std::find( charset.begin(), charset.end(), c ) );
        }

        password += c;
    }

    return password;
}

void processConfigurationItem( std::string configurationItem, Configuration* configuration )
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
        else if ( argument == "allow-numbers" )
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
    }
    else
    {
        std::cerr << "Invalid option: " << configurationItem << std::endl;

        // Don't save if there is a problem with the configuration
        configuration->saveOnExit = false;
    }
}

int main( int argc, char** argv )
{
    // Seed the random number generator
    srand( static_cast<unsigned int>( time( 0 ) ) );

    // Configuration filename is 
    // - Windows: the executable filename with a ".cfg" extension     "PasswordGenerator.cfg"
    // - Other:   the executable filename with a dot and no extension ".PasswordGenerator"
    std::filesystem::path executable( argv[ 0 ] );
#ifdef _WIN32
    std::string configFile = executable.filename().replace_extension( ".cfg" ).string();
#else // e.g. Linux or Apple
    std::string configFile = "." + executable.filename().replace_extension("").string();
#endif // _WIN32 or _WIN64

    Configuration configuration;

    // Default values
    configuration.length = 12;
    configuration.count = 1;
    configuration.saveOnExit = false;

    configuration.allowUppercase = true;
    configuration.allowLowercase = true;
    configuration.allowDigits = true;
    configuration.allowSpecial = true;
    configuration.allowSimilar = false;
    configuration.allowDuplicate = true;

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

        if ( argument == "-help" || argument == "--help" )
        {
            // Ignore for now and process later
            continue;
        }

        processConfigurationItem( argument, &configuration );
    }

    // Look for command line configuration overrides
    for ( int loop = 1; loop < argc; loop++ )
    {
        std::string argument = argv[ loop ];

        if ( argument == "-help" || argument == "--help" )
        {
            std::cout << std::endl;
            std::cout << "Password Generator" << std::endl;
            std::cout << std::endl;
            std::cout << "Command line options:" << std::endl;
            std::cout << std::endl;
            std::cout << "-length:N             Generate passwords of N characters long (" << configuration.length << ")" << std::endl;
            std::cout << "-count:N              Generate N passwords (" << configuration.count << ")" << std::endl;
            std::cout << std::endl;
            std::cout << "Use the options below with either + or - to enable/disable" << std::endl;
            std::cout << std::endl;
            std::cout << "+/-allow-uppercase    Include uppercase characters in a password (" << (configuration.allowUppercase ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << "+/-allow-lowercase    Include lowercase characters in a password (" << (configuration.allowLowercase ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << "+/-allow-digits       Include numeric digits in a password       (" << (configuration.allowDigits ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << "+/-allow-special      Include special characters in a password   (" << (configuration.allowSpecial ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << "+/-allow-similar      Include similar characters in a password   (" << (configuration.allowSimilar ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << "+/-allow-duplicate    Allow duplicated characters in a password  (" << (configuration.allowDuplicate ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << std::endl;
            std::cout << "+/-save               Save the provided configuration as default (" << (configuration.saveOnExit ? "enabled" : "disabled") << ")" << std::endl;
            std::cout << std::endl;
            std::cout << "Example:" << std::endl;
            std::cout << "    PasswordGenerator.exe +allow-uppercase -allow-lowercase -length:16" << std::endl;

            return -1;
        }
        else
        {
            // Nothing else to do - we've already processed this
            continue;
        }
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
        file << ( configuration.allowUppercase ? "+" : "-" ) << "allow-uppercase" << std::endl;
        file << ( configuration.allowLowercase ? "+" : "-" ) << "allow-lowercase" << std::endl;
        file << ( configuration.allowDigits ? "+" : "-" ) << "allow-digits" << std::endl;
        file << ( configuration.allowSpecial ? "+" : "-" ) << "allow-special" << std::endl;
        file << ( configuration.allowSimilar ? "+" : "-" ) << "allow-similar" << std::endl;
        file << ( configuration.allowDuplicate ? "+" : "-" ) << "allow-duplicate" << std::endl;

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
    // - numeric characters                 on                  +/-allow-numbers
    // - special characters                 on                  +/-allow-special
    // - similar characters                 on                  +/-allow-similar
    // - duplicated characters              on                  +/-allow-duplicate
    // ? start with a letter                on                  +/-start-with-letter
    // ? avoid sequences (123, abc)         on                  +/-avoid-sequences
    // ? save this configuration            off                 +/-save

    return 0;
}
