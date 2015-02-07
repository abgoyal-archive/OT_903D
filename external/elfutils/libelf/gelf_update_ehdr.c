
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <gelf.h>
#include <string.h>

#include "libelfP.h"


int
gelf_update_ehdr (Elf *elf, GElf_Ehdr *src)
{
  int result = 0;

  if (elf == NULL)
    return 0;

  if (unlikely (elf->kind != ELF_K_ELF))
    {
      __libelf_seterrno (ELF_E_INVALID_HANDLE);
      return 0;
    }

  rwlock_wrlock (elf->lock);

  if (elf->class == ELFCLASS32)
    {
      Elf32_Ehdr *ehdr = elf->state.elf32.ehdr;

      if (ehdr == NULL)
	{
	  __libelf_seterrno (ELF_E_WRONG_ORDER_EHDR);
	  goto out;
	}

      /* We have to convert the data to the 32 bit format.  This might
	 overflow some fields so we have to test for this case before
	 copying.  */
      if (unlikely (src->e_entry > 0xffffffffull)
	  || unlikely (src->e_phoff > 0xffffffffull)
	  || unlikely (src->e_shoff > 0xffffffffull))
	{
	  __libelf_seterrno (ELF_E_INVALID_DATA);
	  goto out;
	}

      /* Copy the data.  */
      memcpy (ehdr->e_ident, src->e_ident, EI_NIDENT);
#define COPY(name) \
      ehdr->name = src->name
      COPY (e_type);
      COPY (e_machine);
      COPY (e_version);
      COPY (e_entry);
      COPY (e_phoff);
      COPY (e_shoff);
      COPY (e_flags);
      COPY (e_ehsize);
      COPY (e_phentsize);
      COPY (e_phnum);
      COPY (e_shentsize);
      COPY (e_shnum);
      COPY (e_shstrndx);
    }
  else
    {
      Elf64_Ehdr *ehdr = elf->state.elf64.ehdr;

      if (ehdr == NULL)
	{
	  __libelf_seterrno (ELF_E_WRONG_ORDER_EHDR);
	  goto out;
	}

      /* Just copy the data.  */
      memcpy (ehdr, src, sizeof (Elf64_Ehdr));
    }

  result = 1;

 out:
  rwlock_unlock (elf->lock);

  return result;
}
