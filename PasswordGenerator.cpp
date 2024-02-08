// PasswordGenerator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>

typedef struct
{
    unsigned short length;
    unsigned short count;
    bool allowLowercase;
    bool allowUppercase;
    bool allowDigit;
    bool allowSimilar;
    bool allowSpecial;
    bool allowDuplicate;
    bool saveOnExit;
} Configuration;

std::string generatePassword( const Configuration* configuration )
{
    std::string charsetUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string charsetLower = "abcdefghijklmnopqrstuvwxyz";
    std::string charsetDigit = "0123456789";
    std::string charsetSpecial = "!@#$%^&*()_+{}|:<>?-=[];',./";
    std::string charsetSimilar = "iloILO01!|";

    std::string charset;

    if ( configuration->allowUppercase )
    {
        charset += charsetUpper;
    }
    if ( configuration->allowLowercase )
    {
        charset += charsetLower;
    }
    if ( configuration->allowDigit )
    {
        charset += charsetDigit;
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
            ( configuration->allowDigit ? 1 : 0 ) +
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

int main( int argc, char** argv )
{
    std::cout << "Password Generator\n";

    // Seed the random number generator
    srand( static_cast<unsigned int>( time( 0 ) ) );

    Configuration configuration;

    // Default values - TODO read from a dot file if present
    configuration.length = 12;
    configuration.count = 1;
    configuration.saveOnExit = false;

    configuration.allowUppercase = true;
    configuration.allowLowercase = true;
    configuration.allowDigit = true;
    configuration.allowSpecial = true;
    configuration.allowSimilar = false;
    configuration.allowDuplicate = true;

    // Look for configuration overrides
    for ( int loop = 1; loop < argc; loop++ )
    {
        std::string argument = argv[ loop ];

        if ( argument[ 0 ] == '+' || argument[ 0 ] == '-' )
        {
            bool flag = argument[ 0 ] == '+';

            argument = argument.substr( 1 );

            if ( argument == "allow-uppercase" )
            {
                configuration.allowUppercase = flag;
            }
            else if ( argument == "allow-lowercase" )
            {
                configuration.allowLowercase = flag;
            }
            else if ( argument == "allow-numbers" )
            {
                configuration.allowDigit = flag;
            }
            else if ( argument == "allow-special" )
            {
                configuration.allowSpecial = flag;
            }
            else if ( argument == "allow-similar" )
            {
                configuration.allowSimilar = flag;
            }
            else if ( argument == "allow-duplicate" )
            {
                configuration.allowDuplicate = flag;
            }
            else if ( argument.substr( 0, 7 ) == "length:" )
            {
                configuration.length = atoi( argument.substr( 7 ).c_str() );
            }
            else if ( argument.substr( 0, 6 ) == "count:" )
            {
                configuration.count = atoi( argument.substr( 6 ).c_str() );
            }
        }
        else
        {
            std::cerr << "Invalid option: " << argument << std::endl;

            // Don't save a syntax error to the standard configuration
            configuration.saveOnExit = false;
        }
    }

    // TODO generate and print password(s)
    for ( short count = 0; count < configuration.count; count++ )
    {
        std::string password = generatePassword( &configuration );

        if ( password.length() > 0 )
        {
            std::cout << generatePassword( &configuration ) << std::endl;
        }
        else
        {
            break;
        }
    }

    // TODO if configured, attempt to save configuration
    
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
}
