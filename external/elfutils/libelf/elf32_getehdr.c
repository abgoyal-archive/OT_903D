
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <libelf.h>
#include <stddef.h>

#include "libelfP.h"

#ifndef LIBELFBITS
# define LIBELFBITS 32
#endif


ElfW2(LIBELFBITS,Ehdr) *
elfw2(LIBELFBITS,getehdr) (elf)
     Elf *elf;
{
  ElfW2(LIBELFBITS,Ehdr) *result;

  if (elf == NULL)
    return NULL;

  if (unlikely (elf->kind != ELF_K_ELF))
    {
      __libelf_seterrno (ELF_E_INVALID_HANDLE);
      return NULL;
    }

  rwlock_rdlock (elf->lock);

  if (elf->class == 0)
    elf->class = ELFW(ELFCLASS,LIBELFBITS);
  else if (unlikely (elf->class != ELFW(ELFCLASS,LIBELFBITS)))
    {
      __libelf_seterrno (ELF_E_INVALID_CLASS);
      result = NULL;
      goto out;
    }

  result = elf->state.ELFW(elf,LIBELFBITS).ehdr;

 out:
  rwlock_unlock (elf->lock);

  return result;
}
INTDEF(elfw2(LIBELFBITS,getehdr))
