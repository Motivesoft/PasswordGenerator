# PasswordGenerator
Simple offline password generator

## Usage
Run the utility on the command line and one or more passwords will be generated and written to the console.

Command line options allow the generate password to either include or exclude:
- uppercase characters
- lowercase characters
- numeric characters
- special characters

Other switches tell the generator whether to use similar or duplicate characters, to start passwords with a letter, and what length of password to generate.

Multiple passwords can be generated at the time and there is a command line option to update and save the default configuration.

Run the utility with ```-help``` (Windows) or ```--help``` (Linux/Mac) for more details, including the current configuration.

### Example
```PasswordGenerator -length:16 -allow-uppercase -allow-special +allow_lowercase +allow-digits -count:3```

## Configuration files
Configuration settings may be stored in a file called:
```script
# Windows
passwordgenerator.cfg

# Linux - conf file in current directory or '$HOME/.config'
passwordgenerator.conf
~/.config/passwordgenerator.conf
```

If the file exists in the current directory, it will be read to extract any configuration settings from it. In Linux, if the file is not present in the
current directory, it will be looked for in the user's home directory.

The configuration settings must be specified as one per line in the style shown by running the executable with the ```--help``` command line argument (```-help``` on Windows).

## Compilation
Use Visual Studio solution and project file in VS2022 or later for Windows.

For Linux/Mac, manually compile using GCC using the following:
```g++ -std=c++17 PasswordGenerator.cpp -o passwordgenerator```
