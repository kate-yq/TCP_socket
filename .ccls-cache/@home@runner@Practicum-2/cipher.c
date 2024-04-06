/***
  * Implementation of cipher.h
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cipher.h"
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
*
*/

#define N 6

// Implementation of createPolybiusTable
PolybiusTable_t* createPolybiusTable(){
  PolybiusTable_t* table = (PolybiusTable_t*)malloc(sizeof(PolybiusTable_t));
  if (table == NULL) {
    return NULL;
  }
  char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; 
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      table->square[i][j] = alphabet[i * 5 + j];
    }
  }
  return table;
}

// Impletation of pbEncode
// Encode the given message using the polybius square.
char* pbEncode(const char *plaintext) {

  PolybiusTable_t *table = createPolybiusTable();
  if (table == NULL) {
    perror("failed to create cipher table");
    return NULL;
  }

  // allocate memory for encoded result, each char is replaced by two chars
  char *result = (char *)malloc(sizeof(char) * (strlen(plaintext) * 2 + 1));
  if (result == NULL){
    return NULL;
  }

  int index = 0;
  // iterate through each char in plaintext
  for (int i = 0; plaintext[i] != '\0'; i++) {
    //convert char to uppercase first
    char c = toupper(plaintext[i]);
    //if c is a letter, encode it
    int encoded = 0;
    // find row and column of the letter in the table
    for (int row = 0; row < N; row++) {
      for (int col = 0; col < N; col++) {
        if (table->square[row][col] == c) {
          // encode the letter by adding the row and column to the result
          // row and column starting from 1
          result[index++] = '1' + row;
          result[index++] = '1' + col;
          encoded = 1;
          break;
        }
      }
      if (encoded) break;
    }
    if (!encoded){
      // if c is not encoded, leave it unchanged in the result
      result[index++] = c;
      result[index++] = ' ';
    }
  }
  result[index] = '\0';

  free(table);
  
  return result;
}

// Impletation of pbDecode
// Decode the given message using the polybius square.
char* pbDecode(const char *ciphertext){

  PolybiusTable_t *table = createPolybiusTable();
  if (table == NULL) {
    perror("failed to create cipher table");
    return NULL;
  }
  
  int len = strlen(ciphertext);

  // check if the input size is even, if not, return NULL
  if (len % 2 != 0) {
    printf("Invalid ciphertext length. Must be even.\n");
    return NULL;
  }

  // initiate result string
  char *result = (char *)malloc(sizeof(char) * (len / 2 + 1));
  if (result == NULL) {
    return NULL;
  }

  // iterate through each pair of digits in the ciphertext
  int index = 0;
  for (int i = 0; ciphertext[i] != '\0'; ) {
    // if two consecutive char are both digits, decode them
    if (isdigit(ciphertext[i]) && isdigit(ciphertext[i + 1])) {
      int row = ciphertext[i++] - '1';
      int col = ciphertext[i++] - '1';
      if (row >= 0 && row < N && col >= 0 && col < N) {
        result[index++] = table->square[row][col];
      }
    }else {
      result[index++] = ciphertext[i++];
      i++;
    }
  }
  result[index] = '\0';
  
  free(table);
  
  return result;
}