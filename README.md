The scrypt key derivation function
----------------------------------


The scrypt key derivation function was originally developed for use in the
[Tarsnap online backup system](http://www.tarsnap.com/index.html) and is
designed to be far more secure against hardware brute-force attacks than
alternative functions such as [PBKDF2](http://en.wikipedia.org/wiki/PBKDF2) or
[bcrypt](http://www.openbsd.org/papers/bcrypt-paper.ps).

We estimate that on modern (2009) hardware, if 5 seconds are spent computing a
derived key, the cost of a hardware brute-force attack against `scrypt` is
roughly 4000 times greater than the cost of a similar attack against bcrypt (to
find the same password), and 20000 times greater than a similar attack against
PBKDF2.

Details of the `scrypt` key derivation function are given in a paper which was
presented at the [BSDCan'09](http://www.bsdcan.org/2009/) conference:

* Colin Percival, [Stronger Key Derivation via Sequential Memory-Hard
  Functions](http://www.tarsnap.com/scrypt/scrypt.pdf), presented at BSDCan'09,
  May 2009.
* Conference presentation slides:
  [PDF](http://www.tarsnap.com/scrypt/scrypt-slides.pdf).


The scrypt encryption utility
-----------------------------

A simple password-based encryption utility is available as a demonstration of
the `scrypt` key derivation function. On modern hardware and with default
parameters, the cost of cracking the password on a file encrypted by `scrypt
enc` is approximately 100 billion times more than the cost of cracking the same
password on a file encrypted by `openssl enc`; this means that a five-character
password using `scrypt` is stronger than a ten-character password using
`openssl`.

The `scrypt` utility can be invoked as `scrypt enc infile [outfile]` to encrypt
data (if `outfile` is not specified, the encrypted data is written to the
standard output), or as `scrypt dec infile [outfile]` to decrypt data (if
outfile is not specified, the decrypted data is written to the standard
output). `scrypt` also supports three command-line options:

* `-t maxtime` will instruct `scrypt` to spend at most maxtime seconds
  computing the derived encryption key from the password; for encryption, this
  value will determine how secure the encrypted data is, while for decryption
  this value is used as an upper limit (if `scrypt` detects that it would take
  too long to decrypt the data, it will exit with an error message).
* `-m maxmemfrac` instructs `scrypt` to use at most the specified fraction of
  the available RAM for computing the derived encryption key. For encryption,
  increasing this value might increase the security of the encrypted data,
  depending on the `maxtime` value; for decryption, this value is used as an
  upper limit and may `cause` scrypt to exit with an error.
* `-M maxmem` instructs `scrypt` to use at most the specified number of bytes
  of RAM when computing the derived encryption key.

If the encrypted data is corrupt, `scrypt dec` will exit with a non-zero
status.  However, **`scrypt dec` may produce output before it determines that
the encrypted data was corrupt**, so for applications which require data to be
authenticated, you must store the output of `scrypt dec` in a temporary
location and check `scrypt`'s exit code before using the decrypted data.

The `scrypt` utility has been tested on FreeBSD, NetBSD, OpenBSD, Linux
(Slackware, CentOS, Gentoo, Ubuntu), Solaris, OS X, Cygwin, and GNU Hurd. To
build scrypt, extract the tarball and run `./configure` && `make`.

* [scrypt version 1.2.0 source
  tarball](https://www.tarsnap.com/scrypt/scrypt-1.2.0.tgz)
* [GPG-signed SHA256 for scrypt version
  1.2.0](https://www.tarsnap.com/scrypt/scrypt-sigs-1.2.0.asc) (signature
  generated using Tarsnap [code signing
  key](https://www.tarsnap.com/tarsnap-signing-key.asc))

In addition, `scrypt` is available in the OpenBSD and FreeBSD ports trees and
in NetBSD pkgsrc as `security/scrypt`.


Using scrypt as a KDF
---------------------

To use scrypt as a
[key derivation function](https://en.wikipedia.org/wiki/Key_derivation_function)
(KDF), take a
look at the `lib/crypto/crypto_enc.h` header, which provides:

```
/**
 * crypto_scrypt(passwd, passwdlen, salt, saltlen, N, r, p, buf, buflen):
 * Compute scrypt(passwd[0 .. passwdlen - 1], salt[0 .. saltlen - 1], N, r,
 * p, buflen) and write the result into buf.  The parameters r, p, and buflen
 * must satisfy r * p < 2^30 and buflen <= (2^32 - 1) * 32.  The parameter N
 * must be a power of 2 greater than 1.
 *
 * Return 0 on success; or -1 on error.
 */
int crypto_scrypt(const uint8_t *, size_t, const uint8_t *, size_t, uint64_t,
    uint32_t, uint32_t, uint8_t *, size_t);
```


Mailing list
------------

The scrypt key derivation function and the scrypt encryption utility are
discussed on the <scrypt@tarsnap.com> mailing list.

Update 12-2-2015
----------------
This command (tested on newer linux / OSX) should configure the repository for you to build:
  * autoreconf -i
  * ./configure
  * make

If that doesn't work, try the following steps (tested on linux / osx) individually to 
troubleshoot the build.
  * (g)libtoolize --force
  * aclocal
  * autoheader
  * automake --force-missing --add-missing
  * autoconf
  * ./configure

Test build is fixed, after you make the setup, you can run test vectors via
```
cd test
./test_scrypt.sh
```
Or view the tests:
```
cd test
./test_scrypt
```
Added configure.ac check for openssl/aes.h header file
