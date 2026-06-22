# HuffMan Encoding

## What Can It Do?
It can compress and decompress text files. It can decrease text files' size by a great margin (about 30% ~ 40% in my tests)

## How to Use It?
License: MIT  
To use it you just have to say:  
```

./zipper "InputFile" "outputFile"

````
And I have made a power-shell script to automate compiling and running the tests if you want to:
```

./run-test.ps1

```

## The Code

### The Encoded File Structure
FILE STRUCTURE:
```
The header (14 bytes)
HUF (signature) (3 bytes)
File size before compression (4 bytes)
number of unique symbols (2 bytes)
encoding table:
    character value in ascii (1 byte)
    its weight (4 bytes)
compressed file
```

### The Project Structure
The project structe is like this:
```
src/
    zipper.cpp
include/
    inc.h
    objects/
        tree.h
        bitwriter.h
        bitreader.h
    types/
        encodedchars.h
        pqcomp.h
examples/
    has some text files for testing
````

### What Does Each File Do? And How Does The Code Work?

* `inc.h` - has all the declarations, headings, one inline fucntion

* `pqcmop.h` - the priority queue comparsion method is in this file, as a struct

* `encodedchars.h` - a struct to keep track of bits and their length

* `tree.h` - has the object implemention of a normal tree using shared pointers instead of normal pointers, each node has a value (a character ascii value if it is a character, and -1 in case of EOF or any node)
  and a weight to make the Huffman tree

* `bitwriter.h` - has the implemention of writing encoded bits, it does so by accumulating bits unitl they are of length 8 bits,
  it also has a method to write remaining bits and padding

* `bitreader.h` - is a file that reades the encoded file and decodes it to a normal text file, it works by also accumulting bits and checking if they are in the map to then write it to the decompressed file and deleting the bits from the buffers

* `zipper.cpp` - has the main entry point and some helper functions

## Examples

You can find the files in the folder examples/  
These numbers are from my machine, and if you could notice the last file the compression wasn't too significant, that's because the file has ranodm characters, so the function couldn't work well, so did it happen with the pictures, one of them didn't compress well because it has many color values, but the other one (Sobel operator was run on it) did compress well, becuase there are a lot of black pixels and some white.

---

***[Adventures of Sherlock Holmes](https://github.com/Eyad-Jawad/HuffmanCompression/blob/Main/examples/Adventures%20of%20Sherlock%20Holmes)***:

Compression time:   **0.109s**  
decompression time: **0.193s**  
Total Time:         **0.302s**

File size before compression: **607kb**  
File size after compression:  **356kb**  
Saved: **41%** of space  

---

***[The Tempest](https://github.com/Eyad-Jawad/HuffmanCompression/blob/Main/examples/The%20Tempest)***:

Compression time:   **0.037s**  
decompression time: **0.066s**  
Total Time:         **0.103s**  

File size before compression: **170kb**  
File size after compression:  **101kb**  
Saved: **40%** of space  

---

***[Ranodm](https://github.com/Eyad-Jawad/HuffmanCompression/blob/Main/examples/random)***:

Compression time:   **1.591s**  
decompression time: **3.53s**  
Total Time:         **5.121s**

File size before compression: **9765kb**  
File size after compression:  **8119kb**  
Saved: **16%** of space  


---

***[catPic](https://github.com/Eyad-Jawad/HuffmanCompression/blob/Main/examples/Images/no%20filter)***:  

Compression time:   **5.814s**  
decompression time: **14.602s**  
Total Time:         **20.416s**  

File size before compression: **22633kb**  
File size after compression:  **20883kb**  
Saved: **7%** of space

---

***[catPicWithSobel](https://github.com/Eyad-Jawad/HuffmanCompression/blob/Main/examples/Images/filtered)***:  

Compression time:   **5.601s**  
decompression time: **9.203s**  
Total Time:         **14.804s**  

File size before compression: **22633kb**  
File size after compression:  **13440kb**  
Saved: **40%** of space

---

## What Did I Learn?

Honestly I learned a lot about bits and how to work with them, this porject also got me to remember some data structures that I have'nt used in a while, like trees or pqs, I also learned how to structrue my files, almost, I don't claim my structure is beautiful, but it is decent.  


This project also taught me more about memory traversal, smart pointers, priority queues, how to use object in a project, and perhaps most important of all, how does compression actually work.  


It took me more than **60 hours** to make, the idea itself took me about 13h to make, but optimizations and debugging took the rest.  


While there's a room for improvement, like optimizing file reading/writing, and perhaps making the buffers bigger than they are for scaling up (or not using ints but vectors of them, perhaps), you could also scale it up to include not only text files, but every kind of file as well, but for me, I'm quite content with this.

Thank you for reading
-*Eyad*.
