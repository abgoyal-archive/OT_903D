
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <inttypes.h>
#include <stdio.h>
#include <stddef.h>
#include <libeblP.h>


void
ebl_core_note (ebl, name, type, descsz, desc)
     Ebl *ebl;
     const char *name;
     uint32_t type;
     uint32_t descsz;
     const char *desc;
{
  int class = gelf_getclass (ebl->elf);

  if (! ebl->core_note (name, type, descsz, desc))
    /* The machine specific function did not know this type.  */
    switch (type)
      {
      case NT_PLATFORM:
	printf (gettext ("    Platform: %.*s\n"), (int) descsz, desc);
	break;

      case NT_AUXV:
	;
	size_t cnt;
	size_t elsize = (class == ELFCLASS32
			 ? sizeof (Elf32_auxv_t) : sizeof (Elf64_auxv_t));

	for (cnt = 0; (cnt + 1) * elsize <= descsz; ++cnt)
	  {
	    unsigned long int atype;
	    unsigned long int val;

	    if (class == ELFCLASS32)
	      {
		Elf32_auxv_t *auxv = &((Elf32_auxv_t *) desc)[cnt];

		atype = auxv->a_type;
		val = auxv->a_un.a_val;
	      }
	    else
	      {
		Elf64_auxv_t *auxv = &((Elf64_auxv_t *) desc)[cnt];

		atype = auxv->a_type;
		val = auxv->a_un.a_val;
	      }

	    /* XXX Do we need the auxiliary vector info anywhere
	       else?  If yes, move code into a separate function.  */
	    const char *at;

	    switch (atype)
	      {
#define NEW_AT(name) case AT_##name: at = #name; break
		NEW_AT (NULL);
		NEW_AT (IGNORE);
		NEW_AT (EXECFD);
		NEW_AT (PHDR);
		NEW_AT (PHENT);
		NEW_AT (PHNUM);
		NEW_AT (PAGESZ);
		NEW_AT (BASE);
		NEW_AT (FLAGS);
		NEW_AT (ENTRY);
		NEW_AT (NOTELF);
		NEW_AT (UID);
		NEW_AT (EUID);
		NEW_AT (GID);
		NEW_AT (EGID);
		NEW_AT (CLKTCK);
		NEW_AT (PLATFORM);
		NEW_AT (HWCAP);
		NEW_AT (FPUCW);
		NEW_AT (DCACHEBSIZE);
		NEW_AT (ICACHEBSIZE);
		NEW_AT (UCACHEBSIZE);
		NEW_AT (IGNOREPPC);

	      default:
		at = "???";
		break;
	      }

	    switch (atype)
	      {
	      case AT_NULL:
	      case AT_IGNORE:
	      case AT_IGNOREPPC:
	      case AT_NOTELF:
	      default:
		printf ("    %s\n", at);
		break;

	      case AT_EXECFD:
	      case AT_PHENT:
	      case AT_PHNUM:
	      case AT_PAGESZ:
	      case AT_UID:
	      case AT_EUID:
	      case AT_GID:
	      case AT_EGID:
	      case AT_CLKTCK:
	      case AT_FPUCW:
	      case AT_DCACHEBSIZE:
	      case AT_ICACHEBSIZE:
	      case AT_UCACHEBSIZE:
		printf ("    %s: %" PRId64 "\n", at, (int64_t) val);
		break;

	      case AT_PHDR:
	      case AT_BASE:
	      case AT_FLAGS:	/* XXX Print flags?  */
	      case AT_ENTRY:
	      case AT_PLATFORM:	/* XXX Get string?  */
	      case AT_HWCAP:	/* XXX Print flags?  */
		printf ("    %s: %" PRIx64 "\n", at, (uint64_t) val);
		break;
	      }

	    if (atype == AT_NULL)
	      /* Reached the end.  */
	      break;
	  }
	break;

      default:
	/* Unknown type.  */
	break;
      }
}
