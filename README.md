## Perform integer ensemble comparaisons


#### Usage

```sh
./setCompare -i "IFILENAME1&IFILENAME2" -e FILE_EXTENSION \
-l FILES_LOCATION -s SEED_FILE -f FILE_OUT \
-o "OFILENAME1&OFILENAME2"
```

Where,

* **IFILENAMEx**  is the name of a file carrying words to be **included**, list of such files uses the '&' separator
* **OFILENAMEx**  is the name of a file carrying words to be **excluded**, list of such files uses the '&' separator [OPTIONAL]
* **SEED_FILE** is the absolute path to a list of integers to be included [OPTIONAL]
* **FILE_OUT**  is the absolute path to the output file
* **FILE_EXTENSION** is the extension (with no dot) of the file specified by the *-i*, *-o* flags

#### INPUT, integer file format
* 1st line stores total number of items
* Other lines store two integer fields, with the value of the number and a weight used for final sorting
```

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

Are reported the number of sets and operations, followed by the compostion of the final set with its values sorted by decreasing weight, with each item specied as `value:weight`

```
Final set (Intersect of 1 sets) - (Union of 1 sets)
# 7 items set
69022343222:66,100002:44,15:14,2:10,10:10,8:2,89:2
```