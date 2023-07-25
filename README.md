# one\_way\_hash
One way hash function

A simple function based on Java's string hash
function, adapted to produce a larger signature.

# Testing:
The code has been tested using:

## gentest.pl

This Perl script generates random ascii strings
of varrying length up to 4099 characters. The
test then hashes each string, and records
the results and the associated source string in 
a hash table. If there are any duplicates, it
is reported on the command line.

The code was tested to have no collisions given
50000 unique strings. And this was compared to the
results of md5sum on Linux.

# Security
No claim is being made as to the security of this
approach. The software is being provided AS-IS.
Future research is required.

# Author
Roger Doss
