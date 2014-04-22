# ArgumentParser

## Description

Yet another argument parser, written and accessible in c++. Enables short and long keys, callbacks and value targets.

## Installation

    ./configure
    make
    make install

## Usage

See include/ArgumentParser.h for all available functions.

ArgumentParser is divided into `Key/Value pairs`, `Standalones` and `Callbacks`.

### Key/Value pairs

Keys are alphanumeric keywords which reference a value of any of the following data types:

* bool (Bool)
* int (Int)
* unsigned int (UInt)
* cstring (String)
* double (Double)

Complete Example:

    #include <ArgumentParser.h>

    int main (int argc, char **argv) {
      ArgumentParser args("myprog");

      bool mybool;

      args.Bool("mykey", false, "This is a comment", 'b', &mybool);
      args.UInt("myuint", 23, "An arbitrary unsigned int", 'u');
      args.String("mystring", "", "Finally, a string", 's');

      args.parseArgs(argc, argv);

      unsigned int myuint = args.getUInt("myuint");
      char mystring[1024];
      if (args.wasValueSet("mystring")) {
        args.getString("mystring", mystring);
        const char *mycstring = args.getCString("mystring");
      }
    }

Output of `./myprog --help`:

    usage: myprog [options]

    Options:
    -h, --help   (default = false):
            display help message and exit
    -b, --mykey   (default = false):
            This is a comment
    -s, --mystring  [string]:
            Finally, a string
    -u, --myuint  [uint]   (default = 23):
            An arbitrary unsigned int

In the example, there are the following definition functions, which define a key for further use:

    args.Bool()
    args.Int()
    args.UInt()
    args.Double()
    args.String()

Each function takes the following arguments, in order:

* longkey (required): an alphanumeric long key. It produces `--longkey` options
* defaultValue (optional): a default value
* comment (recommended): a short explanation of the command line switch
* shortKey (optional): a single-character alternative for the long key. Produces `-k` options
* target (optional): a pointer at which to write the value when set so you don't have to read it manually

From your program, the keys can also be set using the `set()` functions:

    args.set("mystring", "some content");
    args.set("mybool", true);

You can read the values using args.getXXX functions:

    bool mybool = args.getBool("mybool");
    int myint = args.getInt("myint");

Strings are always an exception in C++, so there are two functions:
One for strcpy-like behavior and one for retrieving the cstring directly.

    // direct pointer
    const char *cstring = args.getCString("mystring");

    // strcpy-like behavior
    char mystring[1024];
    args.getString("mystring", mystring);

There's also a set of functions meant for validation:

    args.keyExists("longkey");
    args.wasValueSet("longkey", false);
    args.shortKeyExists('k');
    args.allValuesSet("'%s' hasn't been set");
    char output[1024];
    args.getLongKey('k', output);


### Standalones

Standalones are string which aren't referenced by a keyword. A typical example would be the file names in `mv source.txt target.txt`.

Standalones can be mixed with other options.

mv-inspired example:

    #include <ArgumentParser.h>

    void moveFile(const char *src, const char *trg);

    int main(int argc, char **argv) {
      ArgumentParser args("mv");

      args.Standalones(-1, "file/directory", "path of a file or a directory");

      args.parseArgs(argc, argv);

      if (args.getStandaloneCount() < 2) {
        return 1;
      }

      moveFile(args.getCStandalone(0), args.getCStandalone(1));

      return 0;
    }

args.Standalone() takes the following arguments:

* maximum: max number of standalone arguments. -1 for unlimited. Default: 0
* helpKey: A single word that describes the standalone argument, e.g. "file", "name", ...
* comment: A comment for the help page

### Callbacks

Callbacks can be registered to every _Key_. Standalone callbacks aren't planned as of yet.

    #include <ArgumentParser.h>
    #include <iostream>

    void myCallback(void *data) {
      static int i = 0;
      std::cout << "data: " << *((int*)data) << " " << ++i << std::endl;
    }

    int main(int argc, char **argv) {
      ArgumentParser args("myprog");

      args.Bool("mybool", "a boolean value that triggers the callback");

      int someData = 5;
      
      args.registerCallback("myvalue", myCallback, &someData);

      args.parseArgs(argc, argv);

      return 0;
    }

Output of `./myprog --mybool -b --mybool`:

    5 1
    5 2
    5 3

### File I/O

Key/Value pairs can be read from and written to files. Files follow a simplistic format:

    mybool    = false
    myuint    = 23
    mystring  = lorem ipsum dolor sit amet
    mypath    = /home/user/dir/file.ext

    include   = another/file.cfg

`include` is not special in any sense, but you can define it to read key/value pairs from another file:

    args.File("include", "read options from file", 'f');

You can also write every defined key/value pair to a file as follows. This excludes unset values and completely ignores any callback magic and standalones.

    args.writeFile("dir/file.cfg");


