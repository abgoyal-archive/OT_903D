#include "tomcrypt.h"


#ifdef LTC_DER

int der_decode_choice(const unsigned char *in,   unsigned long *inlen,
                            ltc_asn1_list *list, unsigned long  outlen)
{
   unsigned long size, x, z;
   void          *data;

   LTC_ARGCHK(in    != NULL);
   LTC_ARGCHK(inlen != NULL);
   LTC_ARGCHK(list  != NULL);

   /* get blk size */
   if (*inlen < 2) {
      return CRYPT_INVALID_PACKET;
   }

   /* set all of the "used" flags to zero */
   for (x = 0; x < outlen; x++) {
       list[x].used = 0;
   }

   /* now scan until we have a winner */
   for (x = 0; x < outlen; x++) {
       size = list[x].size;
       data = list[x].data;

       switch (list[x].type) {
           case LTC_ASN1_INTEGER:
               if (der_decode_integer(in, *inlen, data) == CRYPT_OK) {
                  if (der_length_integer(data, &z) == CRYPT_OK) {
                      list[x].used = 1;
                      *inlen       = z;
                      return CRYPT_OK;
                  }
               }
               break;

           case LTC_ASN1_SHORT_INTEGER:
               if (der_decode_short_integer(in, *inlen, data) == CRYPT_OK) {
                  if (der_length_short_integer(size, &z) == CRYPT_OK) {
                      list[x].used = 1;
                      *inlen       = z;
                      return CRYPT_OK;
                  }
               }
               break;

           case LTC_ASN1_BIT_STRING:
               if (der_decode_bit_string(in, *inlen, data, &size) == CRYPT_OK) {
                  if (der_length_bit_string(size, &z) == CRYPT_OK) {
                     list[x].used = 1;
                     list[x].size = size;
                     *inlen       = z;
                     return CRYPT_OK;
                  }
               }
               break;

           case LTC_ASN1_OCTET_STRING:
               if (der_decode_octet_string(in, *inlen, data, &size) == CRYPT_OK) {
                  if (der_length_octet_string(size, &z) == CRYPT_OK) {
                     list[x].used = 1;
                     list[x].size = size;
                     *inlen       = z;
                     return CRYPT_OK;
                  }
               }
               break;

           case LTC_ASN1_NULL:
               if (*inlen == 2 && in[x] == 0x05 && in[x+1] == 0x00) {
                  *inlen = 2;
                  list[x].used   = 1;
                  return CRYPT_OK;
               }
               break;
                  
           case LTC_ASN1_OBJECT_IDENTIFIER:
               if (der_decode_object_identifier(in, *inlen, data, &size) == CRYPT_OK) {
                  if (der_length_object_identifier(data, size, &z) == CRYPT_OK) {
                     list[x].used = 1;
                     list[x].size = size;
                     *inlen       = z;
                     return CRYPT_OK;
                  }
               }
               break;

           case LTC_ASN1_IA5_STRING:
               if (der_decode_ia5_string(in, *inlen, data, &size) == CRYPT_OK) {
                  if (der_length_ia5_string(data, size, &z) == CRYPT_OK) {
                     list[x].used = 1;
                     list[x].size = size;
                     *inlen       = z;
                     return CRYPT_OK;
                  }
               }
               break;


           case LTC_ASN1_PRINTABLE_STRING:
               if (der_decode_printable_string(in, *inlen, data, &size) == CRYPT_OK) {
                  if (der_length_printable_string(data, size, &z) == CRYPT_OK) {
                     list[x].used = 1;
                     list[x].size = size;
                     *inlen       = z;
                     return CRYPT_OK;
                  }
               }
               break;

           case LTC_ASN1_UTF8_STRING:
               if (der_decode_utf8_string(in, *inlen, data, &size) == CRYPT_OK) {
                  if (der_length_utf8_string(data, size, &z) == CRYPT_OK) {
                     list[x].used = 1;
                     list[x].size = size;
                     *inlen       = z;
                     return CRYPT_OK;
                  }
               }
               break;

           case LTC_ASN1_UTCTIME:
               z = *inlen;
               if (der_decode_utctime(in, &z, data) == CRYPT_OK) {
                  list[x].used = 1;
                  *inlen       = z;
                  return CRYPT_OK;
               }
               break;

           case LTC_ASN1_SET:
           case LTC_ASN1_SETOF:
           case LTC_ASN1_SEQUENCE:
               if (der_decode_sequence(in, *inlen, data, size) == CRYPT_OK) {
                  if (der_length_sequence(data, size, &z) == CRYPT_OK) {
                     list[x].used = 1;
                     *inlen       = z;
                     return CRYPT_OK;
                  }
               }
               break;

           default:
               return CRYPT_INVALID_ARG;
       }
   }

   return CRYPT_INVALID_PACKET;
}

#endif

/* $Source: /cvs/libtom/libtomcrypt/src/pk/asn1/der/choice/der_decode_choice.c,v $ */
/* $Revision: 1.8 $ */
/* $Date: 2006/12/06 02:23:49 $ */
