# HuffMan Encoding

## What Can It Do?
It can compress and decompress text files, it is pretty useless since I do not write the encoding table or tree in the compressed file, but it can decrease text files' size by a great margin (about 30% ~ 40% in my tests)

## How to Use It?
To use it you just have to say:  
```

./zipper "InputFile" "outputFile"

````

## The Code

### The Encoded File Structure
FILE STRUCTURE:
```
The header (14 bytes)
HUF (signature) (3 bytes)
File size before compression (4 bytes)
number of unique symbols (1 byte)
encoding table:
    character value in ascii (1 byte)
    its encoded value or bits (4 bytes)
    the length of bytes because its hard to use bits in c++ (1 byte)
compressed file
```

### The Folder Structure
The folder structe is like this:
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
These numbers are from my machine, and if you could notice the last file the compression wasn't too significant, that's because the file has ranodm characters, so the function couldn't work well.

---

***[Adventures of Sherlock Holmes](https://github.com/Eyad-Jawad/HuffmanCompression/blob/Main/examples/Adventures%20of%20Sherlock%20Holmes)***:

Size before comperssion: **608kb**  
Size after comperssion: **357kb**  
Time: **1.048s**


---

***[The Tempest](https://github.com/Eyad-Jawad/HuffmanCompression/blob/Main/examples/The%20Tempest)***:

Size before comperssion: **171kb**  
Size after comperssion: **102kb**  
Time: **0.324s**


---

***[Ranodm](https://github.com/Eyad-Jawad/HuffmanCompression/blob/Main/examples/random)***:

Size before comperssion: **9766kb**  
Size after comperssion: **8133kb**  
Time: **16.288s**


---

## What Did I Learn?

Honestly I learned a lot about bits and how to work with them, this porject also got me to remember some data structures that I have'nt used in a while, like trees or pqs, I also learned how to structrue my files, almost, I don't claim my structure is beautiful, but it is decent.  


This project also taught me more about memory traversal, smart pointers, priority queues, how to use object in a project, and perhaps most important of all, how does compression actually work.  


It took me more than **40 hours** to make, the idea itself took me about 13h to make, but optimizations and debugging took the rest.  


While there's a room for improvement, like optimizing file reading/writing, and perhaps making the buffers bigger than they are for scaling up (or not using ints but vectors of them, perhaps), you could also scale it up to include not only text files, but every kind of file as well, but for me, I'm quite content with this.

Thank you for reading
-*Eyad*.
