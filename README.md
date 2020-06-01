# ISAAC32
 
A new implementation of the 32-bit Isaac PRNG in the C Programming language.

ISAAC (Indirection, Shift, Accumulate, Add, and Count) is a cryptographically secure pseudorandom number generator and a stream cipher designed by Robert J. Jenkins Jr. in 1993. The original code can be found here: [ISAAC: a fast cryptographic random number generator](http://burtleburtle.net/bob/rand/isaacafa.html)

This project was created to make using ISAAC in C programs simple and easy, while also adding the ability to have multiple instances of ISAAC existing simultaneously. Achieving this came at a cost of lower speed compared to the original code, but
it still remains a reasonably fast method of producing high-quality cryptographically secure pseudorandom numbers.

## Using The Code

Step 1, include isaac32.h in your file:

```C
#include "isaac32.h"
```

Step 2, declare one or more variables to store the state of the PRNG:

```C
ISAAC32 state1, state2;
```

Step 3, either seed the state or initialize it unseeded:

```C
isaac32_seed(seed_data, seed_data_size, state1);
isaac32_init(state2);
```

Step 4, read data from the PRNG:

```C
value = isaac32(state1);
isaac32_read(output, output_size, state2);
```

### Notes:

- ISAAC32 uses a 1024 byte seed
  - Seeds less than 1024 bytes will be padded with zeros to produce 1024 bytes
  - Seeds more than 1024 bytes will be truncated to 1024 bytes
- Using isaac32_init(state) is identical to using isaac_seed("", 0, state)
- Size values used in isaac32_seed and isaac32_read are size in bytes
- isaac32() returns a 32-bit (4-byte) number
- If the number of bytes being read using isaac32_read() is not a multiple of four, then isaac32_read() will generate a multiple of four bytes and discard the ones it didn't need

## Files

Files included in this project.

| File | Purpose |
| ---- | ------- |
| isaac32.h | The ISAAC32 PRNG |
| isaac32_test.c | Automated tests to check that ISAAC32 is working correctly |
| isaac32_cipher.c | Example program, uses ISAAC32 as a stream-cipher |
| isaac32_stdout.c | Example program, outputs data from ISAAC32 to stdout |
| randvect.c | Example program, produces output identical to randvect.txt |
| randvect.txt | Expected output from original Isaac programs by Bob Jenkins |
| README.md | This readme file |


## Contributors

This project was created by Polywoky in May 2020

## License

This project is released under the CC0 1.0 Universal public domain dedication.

### CC0 1.0 Universal Summary

This is a human-readable summary of the [Legal Code (read the full text)](https://creativecommons.org/publicdomain/zero/1.0/legalcode).

### No Copyright

The person who associated a work with this deed has dedicated the work to the public domain by waiving all of their rights to the work worldwide under copyright law, including all related and neighboring rights, to the extent allowed by law.

You can copy, modify, distribute, and perform the work, even for commercial purposes, all without asking permission.

### Other Information

In no way are the patent or trademark rights of any person affected by CC0, nor are the rights that other persons may have in the work or in how the work is used, such as publicity or privacy rights.

Unless expressly stated otherwise, the person who associated a work with this deed makes no warranties about the work, and disclaims liability for all uses of the work, to the fullest extent permitted by applicable law. When using or citing the work, you should not imply endorsement by the author or the affirmer.
