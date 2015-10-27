Label Replacer
===
The label replacer for the Nibble Knowledge CPU

LR4
---

LR4 is the label replacer written in C99 for the Nibble Knowledge CPU. It is under 500 lines of executable code, and simply replaces labels in an [AS4](http://github.com/Nibble-Knowledge/cpu-assembler "AS4") assembly file based on a simple definition file.

LR4 also strips comments and whitespace to ease the job of [AS4](http://github.com/Nibble-Knowledge/cpu-assembler "AS4").

### Useage ###
There is only one invocation of lr4:
* ./lr4 DEFINITION_FILE INPUT_ASSEMBLY OUTPUT_ASSEMBLY

Where DEFINITION_FILE is the label replacement definition file, INPUT_ASSEMBLY is the [AS4](http://github.com/Nibble-Knowledge/cpu-assembler "AS4") assembly file that requires replacement, and OUTPUT_ASSEMBLY is the output assembly file.

### Definition Files ###
Definition files have a very simple format. There are only 2 allowed constructs:
* Comments: prefixed with either ";" or "#" as in [AS4](http://github.com/Nibble-Knowledge/cpu-assembler "AS4").
* Replacements: these are in the form LABEL_NAME: ADDRESS. Every instance of LABEL_NAME in the input assembly file will be replaced with ADDRESS, optionally with an additional offset if LABEL_NAME is used in the form LABEL_NAME[OFFSET] in the input assembly file. 
* ADDRESS and OFFSET may be in hexadecimal, decimal, octal or binary. However, the rules slightly differ for each case.
	* In the case of ADDRESS: hexadecimal must be prefixed with "0x", and decimal is either without prefix or with "0d". 
	* In the case of OFFSET: hexadecimal may be without prefix or with "0x", but decimal requires a "0d" prefix. This is to accomodate the macro assembler. 
	* Octal always requires a prefix, either "0" or "0o"; and binary always requires the prefix "0b".

### INF header ###
LR4 also generates INF headers for [AS4](http://github.com/Nibble-Knowledge/cpu-assembler "AS4") if prompted, either by using the syntax "INF BASE_ADDRESS" or by utilising the same syntax as required in AS4 but excluding any data section information (as LR4 generates this). Examples are below:
* INF BASE_ADDRESS:

```nasm
INF 0x16
LOD data
HLT
data: .data 1 0x0
```
* Detailed INF:

```nasm
INF
PINF
BADR 0x16
EPINF
EINF
LOD data
HLT
data: .data 1 0x0
```

* Both will become:
```nasm
INF
PINF
BADR 0x0016
EPINF
DSEC D_SEC
DNUM 0x1
DSIZE 0x1
EINF
LOD data
HLT
D_SEC: data: .data 1 0x0
```
Custom pseudo instructions will be passed through untouched in the PINF...EPINF section.

### Example definition file ###

```nasm
what: 0xF3
huh: 0b10110
lol: 0453
# commentos
; plenty of them
```

### Example input assembly file ###

```nasm
		hey: LOD huh
LOD     lol
    LOD what
LOD huh[0b0011] ;hey
LOD lol[E] #comments
LOD what[0xD]
LOD what[011]
LOD lol[0o11]
LOD huh[0d14]
LOD who
LOD how
#they are useful
LOD no
LOD derp
LOD dope
no: .data 1 0b1010
how: .data 1 010
how2: .data 1 0o10
who: .data 1 0xF
derp: .data 1 12
derp .data 1 0d12
dope: .data 4 no
zurp: .data 4 huh
```

### Example output assembly file ###

```nasm
hey: LOD 0x16
LOD 0x12B
LOD 0xF3
LOD 0x19
LOD 0x139
LOD 0x100
LOD 0xFC
LOD 0x134
LOD 0x24
LOD who
LOD how
LOD no
LOD derp
LOD dope
no: .data 1 0b1010
how: .data 1 010
how2: .data 1 0o10
who: .data 1 0xF
derp: .data 1 12
derp .data 1 0d12
dope: .data 4 no
zurp: .data 4 0x16

```
