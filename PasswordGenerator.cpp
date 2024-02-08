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
    srand( static_cast<unsigned int>( time( 0 ) ) );

    if ( configuration->allowDuplicate )
    {
        for ( int i = 0; i < configuration->length; i++ )
        {
            password += charset[ rand() % ( charset.length() - 1 ) ];
        }
    }
    else
    {
        // A bit more logic if we are to avoid duplicated characters
        for ( int i = 0; i < configuration->length; i++ )
        {
            char c = charset[ rand() % ( charset.length() - 1 ) ];

            // Character is no longer available
            charset.erase( std::find( charset.begin(), charset.end(), c ) );

            password += c;
        }
    }

    return password;
}

int main( int argc, char** argv )
{
    std::cout << "Password Generator\n";

    Configuration configuration;

    // Default values - TODO read from a dot file if present
    configuration.length = 12;
    configuration.count = 1;
    configuration.saveOnExit = false;

    configuration.allowUppercase = false;
    configuration.allowLowercase = false;
    configuration.allowDigit = true;
    configuration.allowSpecial = false;
    configuration.allowSimilar = false;
    configuration.allowDuplicate = false;

    // Look for configuration overrides
    for ( int loop = 1; loop < argc; loop++ )
    {
        std::string argument = argv[ loop ];

        if ( argument[ 0 ] == '+' || argument[ 0 ] == '-' )
        {
            bool on = argument[ 0 ] == '+';

            argument = argument.substr( 1 );
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

    // Arguments with value:                Default:            Option:
    // - password length                    - default 16        -length=[0-9]+
    // - number of passwords to generate    - default 1         -count=[0-9]+
    // 
    // Flags (on/off):                      Default:            Option:
    // - special characters                 on                  +/-allow-special-characters
    // - similar characters                 on                  +/-allow-similar-characters
    // - duplicated characters              on                  +/-allow-duplicated-characters
    // - uppercase characters               on                  +/-allow-uppercase
    // - lowercase characters               on                  +/-allow-lowercase
    // - numeric characters                 on                  +/-allow-numbers
    // - start with a letter                on                  +/-start-with-letter
    // - avoid sequences (123, abc)         on                  +/-avoid-sequences
    // - save this configuration            off                 +/-save
}
