
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>

#include "libelfP.h"


Elf_Data *
elf_rawdata (scn, data)
     Elf_Scn *scn;
     Elf_Data *data;
{
  if (scn == NULL || scn->elf->kind != ELF_K_ELF)
    {
      __libelf_seterrno (ELF_E_INVALID_HANDLE);
      return NULL;
    }

  /* If `data' is not NULL this means we are not addressing the initial
     data in the file.  But this also means this data is already read
     (since otherwise it is not possible to have a valid `data' pointer)
     and all the data structures are initialized as well.  In this case
     we can simply walk the list of data records.  */
  if (data != NULL
      || (scn->data_read != 0 && (scn->flags & ELF_F_FILEDATA) == 0))
    {
      /* We don't allow accessing any but the data read from the file
	 as raw.  */
      __libelf_seterrno (ELF_E_DATA_MISMATCH);
      return NULL;
    }

  /* If the data for this section was not yet initialized do it now.  */
  if (scn->data_read == 0)
    {
      /* First thing we do is to read the data from the file.  There is
	 always a file (or memory region) associated with this descriptor
	 since otherwise the `data_read' flag would be set.  */
      if (__libelf_set_rawdata (scn) != 0)
	/* Something went wrong.  The error value is already set.  */
	return NULL;
    }

  /* Return the first data element in the list.  */
  return &scn->rawdata.d;
}
INTDEF(elf_rawdata)
