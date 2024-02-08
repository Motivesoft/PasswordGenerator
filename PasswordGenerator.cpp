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
    bool allowDigits;
    bool allowSimilar;
    bool allowSpecial;
    bool saveOnExit;
} Configuration;

std::string generatePassword( const Configuration* configuration )
{
    std::string charsetLower = "abcdefghijklmnopqrstuvwxyz";
    std::string charsetUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string charsetDigit = "0123456789";
    std::string charsetSpecial = "!@#$%^&*()_+{}|:<>?-=[];',./";

    std::string charset;

    charset = charsetLower + charsetUpper + charsetDigit + charsetSpecial;

    if ( !configuration->allowSimilar )
    {
        int charsetCount =
            configuration->allowUppercase ? 1 : 0 +
            configuration->allowLowercase ? 1 : 0 +
            configuration->allowDigits ? 1 : 0;

        if ( charsetCount > 1 )
        {
            std::string charsetSimilar = "iloILO01";

            // Remove similar characters
            for ( std::string::iterator it = charset.begin(); it != charset.end(); it++ )
            {
                if ( std::find( charsetSimilar.begin(), charsetSimilar.end(), *it ) != charsetSimilar.end() )
                {
                    it = charset.erase( it );
                }
            }
        }
    }

    std::string password;
    srand( static_cast<unsigned int>( time( 0 ) ) );
    for ( int i = 0; i < configuration->length; i++ )
    {
        password += charset[ rand() % ( sizeof( charset ) - 1 ) ];
    }
    return password;
}

int main( int argc, char** argv )
{
    std::cout << "Password Generator\n";

    Configuration configuration;

    // Default values - TODO read from a dot file if present
    configuration.length = 16;
    configuration.count = 1;
    configuration.saveOnExit = false;

    configuration.allowUppercase = false;
    configuration.allowLowercase = false;
    configuration.allowDigits = true;

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
        std::cout << generatePassword( &configuration ) << std::endl;
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
