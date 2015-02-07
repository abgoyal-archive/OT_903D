
#include "tomcrypt.h"


static const char *err_2_str[] =
{
   "CRYPT_OK",
   "CRYPT_ERROR",
   "Non-fatal 'no-operation' requested.",

   "Invalid keysize for block cipher.",
   "Invalid number of rounds for block cipher.",
   "Algorithm failed test vectors.",

   "Buffer overflow.",
   "Invalid input packet.",

   "Invalid number of bits for a PRNG.",
   "Error reading the PRNG.",

   "Invalid cipher specified.",
   "Invalid hash specified.",
   "Invalid PRNG specified.",

   "Out of memory.",

   "Invalid PK key or key type specified for function.",
   "A private PK key is required.",

   "Invalid argument provided.",
   "File Not Found",

   "Invalid PK type.",
   "Invalid PK system.",
   "Duplicate PK key found on keyring.",
   "Key not found in keyring.",
   "Invalid sized parameter.",

   "Invalid size for prime.",

};

const char *error_to_string(int err)
{
   if (err < 0 || err >= (int)(sizeof(err_2_str)/sizeof(err_2_str[0]))) {
      return "Invalid error code.";
   } else {
      return err_2_str[err];
   }   
}


/* $Source: /cvs/libtom/libtomcrypt/src/misc/error_to_string.c,v $ */
/* $Revision: 1.4 $ */
/* $Date: 2006/03/31 14:15:35 $ */
