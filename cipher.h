#ifndef CIPHER_H
#define CIPHER_H

/***
  * Polybius Cipher
  *
  * This is a polybius cipher implementation.
  */

// define polybius square size
#define N 6

/***
  * Define the polybius square of this program.
  *
  *     1  2  3  4  5  6
  *  1  A  B  C  D  E  F
  *  2  G  H  I  J  K  L
  *  3  M  N  O  P  Q  R
  *  4  S  T  U  V  W  X
  *  5  Y  Z  0  1  2  3
  *  6  4  5  6  7  8  9
  */
typedef struct {
  char square[N][N];
}PolybiusTable_t;

/***
  * Create a new polybius table.
  *
  * @return the newly created polybius table.
  */
PolybiusTable_t* createPolybiusTable();

/***
  * Encode the given message using the polybius square.
  *
  * @param message the message to encode.
  * @return the encoded message.
  */
char* pbEncode(const char *plaintext);

/***
  * Decode the given message using the polybius square.
  *
  * @param ciphertext the ciphertext to decode.
  * @return the decoded message.
  */
char* pbDecode(const char *ciphertext);

#endif // CIPHER_H