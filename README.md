# TOOLBOX to work with Kmer -- integer representation

## Working with PYTHON

### (up to 32)k-mer 2bits encoding

#### Building and distributing the python module "twobits"

##### Edit setup.py and MANIFEST.in
[MANIFEST.in cheatsheet](https://packaging.python.org/guides/using-manifest-in/)

##### Build & Test

```sh
\rm -rf ./build twobits.so
python setup.py build
ln -s build/lib.macosx-10.9-x86_64-3.8/*so twobits.so
python ./test.py
```

##### Create dist & upload

```sh
python setup.py sdist
python -m twine upload --repository-url https://upload.pypi.org/legacy/ dist/*
```

##### Usage examples

###### Coding / Decoding

```python
import twobits
twobits.encode("GCAGTTTGTAGTCGAAGACGGTCTTCGCATCG")
# returns 15252441178061577511
twobits.decode(15252441178061577511, 32)
# returns "GCAGTTTGTAGTCGAAGACGGTCTTCGCATCG"

twobits.encode("AA")
# Returns 0
twobits.decode(0, 2)
# Returns "AA"
```

###### Truncating
Here, the sizes of the total and truncated words must be passed explicitely

```python
longWord       = 'AAAAAAAAAGCGTTCACCCGTGG'
lenLongWord    = len(lonWord)                                # 23
longWordValue  = twobits.encode('AAAAAAAAAGCGTTCACCCGTGG')   # 233379311
shortWordValue = twobits.truncate(longWord, 23, 10)          # 595439

twobits.decode(shortWordValue, 10)
>>'TCACCCGTGG'
```

Truncation is always performed on the left-end side of the string.
## Working with executable

## Perform integer ensemble comparaisons

### Compiling the program

```shell
gcc -Wall -std=c99 main.c custom_*.c two_bits_encoding.c -I./include -o setCompare -pedantic -lm
```

#### Expected formats

One file per genome, with sgRNA encoded as 64bits unsigned integers under the format described [here](https://github.com/MMSB-MOBI/CSTB_core).

#### Usage

```sh
./setCompare -i "IFILENAME1&IFILENAME2" -e FILE_EXTENSION \
-l FILES_LOCATION -f FILE_OUT \
-o "OFILENAME1&OFILENAME2"
-c 19
```

Where,

* **IFILENAMEx**  is the name of a file carrying words to be **included**, list of such files uses the '&' separator
* **OFILENAMEx**  is the name of a file carrying words to be **excluded**, list of such files uses the '&' separator [OPTIONAL]
* **SEED_FILE** is the absolute path to a list of integers to be included [OPTIONAL]
* **FILE_OUT**  is the absolute path to the output file
* **FILE_EXTENSION** is the extension (with no dot) of the file specified by the *-i*, *-o* flags
* `-c` is the size of truncated word representations used for ensemble comparisons.

#### INPUT, integer file format

* 1st line stores total number of items
* Other lines store two integer fields, with the value of the number and a weight used for final sorting

```text
11
2 5
5 3
7 1
8 1
10 5
15 7
22 12
89 1
333 1
100002 22
69022343222 33
```

#### OUTPUT, results file format

##### Default

Are reported the number of sets and operations, followed by the compostion of the final set with its values sorted on a single line by decreasing weight, with each item specified as `value:weight`.

```text
{"comments" : "Final set (Intersect of 2 sets) - (Union of 0 sets)", "size" : 51, "codec" : "twobits", "length" : 23 }
# 7 items set
69022343222:66,100002:44,15:14,2:10,10:10,8:2,89:2
```

The header line is a json document of the following structure
```json
{ 
    "comments" : "Final set (Intersect of 2 sets) - (Union of 0 sets)", // Summary of the seet operations
    "size" : 51,                                                        // Total number word satisfying set constraints
    "codec" : "twobits",                                                // encoding scheme
    "length" : 23                                                       // size of the corresponding sgRNA words
}
```

##### Comparison involving truncated words

Here is a sample of the format

```text
{"comments" : "Final set (Intersect of 2 sets) - (Union of 0 sets)", "size" : 4, "codec" : "twobits", "from" : 23, "to" : 3 }
# 4 items set
31:425900[237513503, ... ,10600922015]
47:225225[149430063, ... ,212042863]
```

It has two differences:

* The json meta-inf

* The word composition

###### The json meta-inf

It features the length of the non-truncated and truncated words are the respective values of the `from` and `to` keys.

```json
{
    "comments" : "Final set (Intersect of 2 sets) - (Union of 0 sets)",
    "size" : 4,
    "codec" : "twobits",
    "from" : 23,
    "to" : 3
}
```

###### The word composition

One line by truncated word that respect the constraints. Each line features the integer representation of one truncated word, its number of occurences and the list of its corresponding full-length words as an array of their integer representation values.

**eg:**  A truncated word of value `31` has `425900` occurences and `237513503` is the value of one of its full-length word.

```text
31:425900[237513503, ... ,10600922015]
```
