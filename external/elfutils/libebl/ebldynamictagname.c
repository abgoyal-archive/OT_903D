
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <inttypes.h>
#include <stdio.h>
#include <libeblP.h>


const char *
ebl_dynamic_tag_name (ebl, tag, buf, len)
     Ebl *ebl;
     int64_t tag;
     char *buf;
     size_t len;
{
  const char *res = ebl != NULL ? ebl->dynamic_tag_name (tag, buf, len) : NULL;

  if (res == NULL)
    {
      if (tag >= 0 && tag < DT_NUM)
	{
	  static const char *stdtags[] =
	    {
	      "NULL", "NEEDED", "PLTRELSZ", "PLTGOT", "HASH", "STRTAB",
	      "SYMTAB", "RELA", "RELASZ", "RELAENT", "STRSZ", "SYMENT",
	      "INIT", "FINI", "SONAME", "RPATH", "SYMBOLIC", "REL", "RELSZ",
	      "RELENT", "PLTREL", "DEBUG", "TEXTREL", "JMPREL", "BIND_NOW",
	      "INIT_ARRAY", "FINI_ARRAY", "INIT_ARRAYSZ", "FINI_ARRAYSZ",
	      "RUNPATH", "FLAGS", "ENCODING", "PREINIT_ARRAY",
	      "PREINIT_ARRAYSZ"
	    };

	  res = stdtags[tag];
	}
      else if (tag == DT_VERSYM)
	res = "VERSYM";
      else if (tag >= DT_GNU_PRELINKED && tag <= DT_SYMINENT)
	{
	  static const char *valrntags[] =
	    {
	      "GNU_PRELINKED", "GNU_CONFLICTSZ", "GNU_LIBLISTSZ",
	      "CHECKSUM", "PLTPADSZ", "MOVEENT", "MOVESZ", "FEATURE_1",
	      "POSFLAG_1", "SYMINSZ", "SYMINENT"
	    };

	  res = valrntags[tag - DT_GNU_PRELINKED];
	}
      else if (tag >= DT_GNU_CONFLICT && tag <= DT_SYMINFO)
	{
	  static const char *addrrntags[] =
	    {
	      "GNU_CONFLICT", "GNU_LIBLIST", "CONFIG", "DEPAUDIT", "AUDIT",
	      "PLTPAD", "MOVETAB", "SYMINFO"
	    };

	  res = addrrntags[tag - DT_GNU_CONFLICT];
	}
      else if (tag >= DT_RELACOUNT && tag <= DT_VERNEEDNUM)
	{
	  static const char *suntags[] =
	    {
	      "RELACOUNT", "RELCOUNT", "FLAGS_1", "VERDEF", "VERDEFNUM",
	      "VERNEED", "VERNEEDNUM"
	    };

	  res = suntags[tag - DT_RELACOUNT];
	}
      else if (tag == DT_AUXILIARY)
	res = "AUXILIARY";
      else if (tag == DT_FILTER)
	res = "FILTER";
      else
	{
	  snprintf (buf, len, gettext ("<unknown>: %" PRId64), tag);

	  res = buf;

	}
    }

  return res;
}
