
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <gelf.h>
#include <string.h>

#include "libelfP.h"


GElf_Versym *
gelf_getversym (data, ndx, dst)
     Elf_Data *data;
     int ndx;
     GElf_Versym *dst;
{
  Elf_Data_Scn *data_scn = (Elf_Data_Scn *) data;
  Elf_Scn *scn;
  GElf_Versym *result;

  if (data == NULL)
    return NULL;

  if (unlikely (data->d_type != ELF_T_HALF))
    {
      __libelf_seterrno (ELF_E_INVALID_HANDLE);
      return NULL;
    }

  /* This is the one place where we have to take advantage of the fact
     that an `Elf_Data' pointer is also a pointer to `Elf_Data_Scn'.
     The interface is broken so that it requires this hack.  */
  scn = data_scn->s;

  /* It's easy to handle this type.  It has the same size for 32 and
     64 bit objects.  */
  assert (sizeof (GElf_Versym) == sizeof (Elf32_Versym));
  assert (sizeof (GElf_Versym) == sizeof (Elf64_Versym));

  rwlock_rdlock (scn->elf->lock);

  /* The data is already in the correct form.  Just make sure the
     index is OK.  */
  if (unlikely ((ndx + 1) * sizeof (GElf_Versym) > data->d_size))
    {
      __libelf_seterrno (ELF_E_INVALID_INDEX);
      result = NULL;
    }
  else
    {
      *dst = ((GElf_Versym *) data->d_buf)[ndx];

      result = dst;
    }

  rwlock_unlock (scn->elf->lock);

  return result;
}
