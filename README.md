Label Replacer
===
The label replacer for the Nibble Knowledge CPU

LR4
---

LR4 is the label replacer written in C99 for the Nibble Knowledge CPU. It is under 500 lines of executable code, and simply replaces labels in an [AS4](http://github.com/Nibble-Knowledge/cpu-assembler "AS4") assembly file based on a simple definition file.

### Useage ###
There is only one invocation of lr4:
* ./lr4 DEFINITION_FILE INPUT_ASSEMBLY OUTPUT_ASSEMBLY

Where DEFINITION_FILE is the label replacement definition file, INPUT_ASSEMBLY is the [AS4](http://github.com/Nibble-Knowledge/cpu-assembler "AS4") assembly file that requires replacement, and OUTPUT_ASSEMBLY is the output assembly file.

### Definition Files ###
Definition files have a very simple format. There are only 2 allowed constructs:
* Comments: prefixed with either ";" or "#" as in [AS4](http://github.com/Nibble-Knowledge/cpu-assembler "AS4").
* Replacements: these are in the form LABEL_NAME: ADDRESS. Every instance of LABEL_NAME in the input assembly file will be replaced with ADDRESS, optionally with an additional offset if LABEL_NAME is used in the form LABEL_NAME[OFFSET] in the input assembly file. 
* ADDRESS and OFFSET may be in hexadecimal, decimal, octal or binary. However, the rules slightly differ for each case - in the scase of ADDRESS, hexadecimal must be prefixed with "0x", and decimal is either without prefix or with "0d". In the case of OFFSET, hexadecimal may be without prefix or with "0x", but decimal requires a "0d" prefix. This is to accomodate the macro assembler. Octal always requires a prefix, either "0" or "0o"; and binary always requires the prefix "0b".
