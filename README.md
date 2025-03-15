# suji command - A version string generator

## What is sujico?

sujico (suji command: すじこ) is a version string generator.
`sujico` builds a string from numbers and strings written in a configuration file and Git or Mercurial commit logs.
Additionally, you can modify the values in the configuration file using specified options.

For example, you can automatically generate a version string at build time.

## Usage

### Simple example

An example that outputs the value written in the configuration file and the format to stdout.

1. Write the following text in a text editor:

    ```toml
    Major = 1
    Minor = 0
    format = '''${Major}.${02:Minor}'''
    ```

1. Save it as `.sujiconf.toml`
1. Run `suji`. The output will be:

    ```plain
    1.00
    ```

### Embedding in a C language project

An example that writes C code in the configuration file and actually compiles and runs with a C compiler.

1. Write the following text in a text editor:

    ```toml
    Major = 1
    Minor = 0
    format = '''#define VERSION "${Major}.${02:Minor}"'''
    ```

1. Save it as `.sujiconf.toml`
1. Redirect the output to `version.h` and run `suji`
1. Write the following code in an editor:
  
    ```c
    #include <stdio.h>
    #include "version.h"

    int main(int argc, char* argv[])
    {
      printf("%s\n", VERSION);
      return 0;
    }
    ```

1. Save it as `test.c`, compile it, and it will output the version string.

### Example for build number

An example that increments the build number.

1. Write the following text in a text editor:

    ```toml
    Major = 1
    Minor = 0
    Build = 100
    format = '''${Major}.${02:Minor}.${03:Build}'''
    ```

1. Save it as `.sujiconf.toml`
1. Run `suji`. The output will be:

    ```c
    #define VERSION "1.00.100"
    ```

1. Redirect the output to `version.h` and run `suji` again
1. Write the following code in an editor:
  
    ```c
    #include <stdio.h>
    #include "version.h"

    int main(int argc, char* argv[])
    {
      printf("%s\n", VERSION);
      return 0;
    }
    ```

1. Save it as `test.c`, compile it, and it will output the version string.
1. Run `suji` and increment the value in `.sujiconf.toml`

    * At the point, `.sujiconf.toml` is overwritten

    ```sh
    suji -i Build -w > version.h
    ```

1. Recompile `test.c` and run it. Output the version string. The build number has increased by 1

    ```plain
    1.00.101
    ```

## Supported variables

### Legend

* [ ] ... Not supported (planned)
* [x] ... Supported

### Environment

* [ ] OS version
* [ ] Compiler version
* [ ] Built datetime
* [ ] User name
* [ ] Build path

### CVS

#### Git

* [x] Hash
* [x] Commit author
* [x] Commit author's email
* [x] Committed datetime
* [ ] Tag
* [ ] Branch
* [ ] Remote address

#### Mercurial

* [x] Changeset
* [x] Hash
* [x] Tag
* [x] Committed user
* [x] Committed user's email address
* [x] Committed datetime
* [ ] Remote address

### Operation

* [x] Substitution
* [x] Increment
  * [x] Integer
  * [x] Character
  * [ ] String
* [ ] Four arithmetic operations
* [x] Format string

## configuration reference

### Basic syntax

* Configuration file syntax is following  [TOML v0.5.0 (github)](https://github.com/toml-lang/toml/blob/master/README.md)
* Write the  substitution expression in configure file
  * e.g., `Major = 1`
* `format` key is special. This value is treated as a format. See also "format key" section below

### format key

* You can use variables in the `format` key's value. The variables name is written key in configuration file. If use the variables in `format` key, write `${key}`
  * e.g., `format = '''${Major}.${02:Minor}'''`
* You can include constant string
  * e.g., `format = '''Version: ${Major}'''`
* You can include the format string. See also "Format and internal variables" section below

### Format and internal variables

Split the format string and variable name by `:` in the format string. Supported variables are following:

|Category|Value|Behavior|Note|
|---|---|---|---|
|Format|`0N (N: digit count)`|Zero fill|Only integer|
|CVS|git|Reference the internal Git information|See also "CVS>Git" section below|
||hg|Reference the internal Mercurial information|See also "CVS>Mercurial" section below|

### CVS

If running directory is under CVS management, you can get the commit log. `sujico` is parsing the commit log and store it internally.

#### Git

|Name|Content|Note|
|---|---|---|
|`longhash`|Long hash for latest commit||
|`hash`|Short hash for latest commit|7 chars|
|`tag`|Tag for latest commit||
|`user`|Committed user for latest commit||
|`date`|Committed date for latest commit||
|`time`|Committed time for latest commit||
|`utcdate`|Committed UTC date for latest commit||
|`utctime`|Committed UTC time for latest commit||

### Mercurial

|Name|Content|Note|
|---|---|---|
|`changeset`|Changeset for latest commit||
|`hash`|Hash for latest commit|12 chars|
|`tag`|Tag for latest commit||
|`user`|Committed user for latest commit||
|`date`|Committed date for latest commit||
|`time`|Committed time for latest commit||
|`utcdate`|Committed UTC date for latest commit||
|`utctime`|Committed UTC time for latest commit||

## Command line options

### -i

Increment the specified value.

* Long option
  * `--increment`
* Support type
  * Integer
* Example
  * `-i Major`
* Result
  * Written the `Major` value incremented by 1 in the configuration file
    * e.g., `Major` value is 1. after running the command, the value is 2
* Error
  * Value is integer and value is greater equal `18446744073709551615` (2^64^ - 1), this operation fails

### -s

Set the specified value to the key

* Long option
  * `--set`
* Support type
  * Integer, String
* Example
  1. `-s Major=1`
  1. `-s Major=a`
* Result
  * Update the `Major` value to 1 in the configuration file
* Error
  * If the key type is integer and set the string value, this operation fails

### -f

Specified file treating the configuration file.

* Long option
  * `--file`
* Default
  * `.sujiconf.toml`
* Example
  * `-f conf/myconf.toml`
* Result
  * Load the `conf/myconf.toml` as the configuration file
* Error
  * The specified file doesn't exist or access denied, this operation fails

### -w

Specific file or specified by `-f` option, save the current content.

* Long option
  * `--write`
* Default
  * `-f` option specified file (if not specified, default of `-f` option)
* Example
    1. `-w`
    1. `-w newconf.toml`
    1. `-f conf/myconf.toml -w`
* Result
    1. Write to `.sujiconf.toml`
    1. Write to `newconf.toml`
    1. Write to `conf/myconf.toml`
* Error
  * Failed to write the file, this operation fails
* Note
  * Written content is not keep the order of the original content

### -h

Show help.

* Long option
  * `--help`

### -l

Show the license. This doesn't show sujico's own license.

* Long option
  * `--license`

### -v

Show version.

* Long option
  * `--version`

### --verbose

Show debug information.

[!CAUTION]
Do not compile or run the generated output while this option is enabled.

### --VERBOSE

Show more debug information.

[!CAUTION]
Do not compile or run the generated output while this option is enabled.

## Build sujico

## Dependencies

* Boost >= 1.70.0
  * program_options
  * date_time
  * regex
* toml11
* C++ compiler with C++ 17 support
  * g++ 8
  * Visual C++ 2019

## License

MIT
