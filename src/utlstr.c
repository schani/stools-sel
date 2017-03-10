#include <gemein.h>
#include <utility.h>

#include <string.h>

void utl_strdel (CHAR *pcText, INT iStart, INT iLength)
{
  CHAR *pcSource,
       *pcDest;

  pcDest = (pcText + iStart);
  pcSource = (pcText + iStart + iLength);
  for ( ; *pcSource; pcSource++, pcDest++)
    *pcDest = *pcSource;
  *pcDest = *pcSource;
}                          

void utl_strins (CHAR *pcText, INT iPos, CHAR cChar)
{
  CHAR *pcChar;

  pcChar = (pcText + strlen(pcText));
  for ( ; pcChar - pcText > iPos; pcChar--)
    *(pcChar + 1) = *pcChar;
  *(pcChar + 1) = *pcChar;
  *pcChar = cChar;
}

void utl_ltrim (CHAR *pcString)
{
  while (*pcString == 32)
    utl_strdel(pcString, 0, 1);
}

void utl_rtrim (CHAR *pcString)
{
  INT iCounter;

  iCounter = strlen(pcString) - 1;
  while (pcString[iCounter] == 32)
  {
    pcString[iCounter] = 0;
    iCounter--;
  }
}

CHAR utl_upper (CHAR cBuchstabe)
{
  if ((cBuchstabe > 96) && (cBuchstabe < 123))
    return(cBuchstabe & 0xdf);
  switch (cBuchstabe)
  {
    case '„' :
      return('Ž');
    case '”' :
      return('™');
    case '' :
      return('š');
    default  :
      return(cBuchstabe);
  }
}

CHAR utl_lower (CHAR cBuchstabe)
{
  if ((cBuchstabe > 64) && (cBuchstabe < 91))
    return(cBuchstabe | 0x20);
  switch (cBuchstabe)
  {
    case 'Ž' :
      return('„');
    case '™' :
      return('”');
    case 'š' :
      return('');
    default  :
      return(cBuchstabe);
  }
}

CHAR* utl_str_upper (CHAR *pcString)
{
  CHAR *pcZaehler;

  for (pcZaehler = pcString; *pcZaehler; pcZaehler++)
    *pcZaehler = utl_upper(*pcZaehler);
  return pcString;
}

CHAR* utl_str_lower (CHAR *pcString)
{
  CHAR *pcZaehler;

  for (pcZaehler = pcString; *pcZaehler; pcZaehler++)
    *pcZaehler = utl_lower(*pcZaehler);
  return pcString;
}

INT utl_hot_strlen (CHAR *pcText)
{
  INT iReturnVar = 0;
  
  for (; *pcText; pcText++)
    if (*pcText != '~' && *pcText != '#')
      iReturnVar++;
  return iReturnVar;
}

void
utl_split_path (CHAR *path, CHAR *dir, CHAR *name, CHAR *ext)
{
    CHAR *slash = strrchr(path, '/');
    CHAR *filename;
    if (slash) {
        filename = slash + 1;
        strncpy(dir, path, filename - path);
        dir[filename - path] = 0;
    } else {
        filename = path;
        dir[0] = 0;
    }
    CHAR *dot = strrchr(filename, '.');
    if (dot) {
        strncpy(name, filename, dot - filename);
        name[dot - filename] = 0;
        strcpy(ext, dot);
    } else {
        strcpy(name, filename);
        ext[0] = 0;
    }
}

