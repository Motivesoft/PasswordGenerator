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

## Compilation
Use Visual Studio solution and project file in VS2022 or later for Windows.

For Linux/Mac, manually compile using GCC using the following:
```g++ -std=c++17 PasswordGenerator.cpp -o passwordgenerator```
