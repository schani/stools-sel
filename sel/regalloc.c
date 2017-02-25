#include <stdlib.h>
#include <string.h>

typedef struct
{
  unsigned int  uiLine;
  char          acName[13];
  unsigned int  uiSize;    
  void         *pBlock;
} HEAP_ITEM;
                     
unsigned int  uiItems;
HEAP_ITEM    *phiItems;

void reg_init (unsigned int uiNewItems)
{ 
  
  unsigned int uiCounter;

  uiItems = uiNewItems;
  phiItems = malloc(sizeof(HEAP_ITEM) * uiItems);
  for (uiCounter = 0; uiCounter < uiItems; uiCounter++)
  {
    phiItems[uiCounter].uiLine = phiItems[uiCounter].uiSize = 0;
    phiItems[uiCounter].pBlock = 0L;
    strcpy(phiItems[uiCounter].acName, "");
  }
}

void* reg_alloc (unsigned int uiSize, unsigned int uiLine, char *pcName)
{
  
  void         *pBlock;
  unsigned int  uiCounter;
  
  pBlock = malloc(uiSize);
  for (uiCounter = 0; uiCounter < uiItems; uiCounter++)
    if (phiItems[uiCounter].uiSize == 0)
    {
      phiItems[uiCounter].uiSize = uiSize;
      phiItems[uiCounter].uiLine = uiLine;
      phiItems[uiCounter].pBlock = pBlock;
      strcpy(phiItems[uiCounter].acName, pcName);
      break;
    }
  return(pBlock);
}

void* reg_calloc (unsigned int uiSize, unsigned int uiNumber, 
                  unsigned int uiLine, char *pcName)
{
  
  void         *pBlock;
  unsigned int  uiCounter;
  
  pBlock = calloc(uiSize, uiNumber);
  for (uiCounter = 0; uiCounter < uiItems; uiCounter++)
    if (phiItems[uiCounter].uiSize == 0)
    {
      phiItems[uiCounter].uiSize = uiSize * uiNumber;
      phiItems[uiCounter].uiLine = uiLine;
      phiItems[uiCounter].pBlock = pBlock;
      strcpy(phiItems[uiCounter].acName, pcName);
      break;
    }
  return(pBlock);
}
 
void reg_free (void *pBlock)
{

  unsigned int uiCounter;  

  for (uiCounter = 0; uiCounter < uiItems; uiCounter++)
    if (phiItems[uiCounter].pBlock == pBlock)
    {
      phiItems[uiCounter].uiSize = 0;
      phiItems[uiCounter].uiLine = 0;
      phiItems[uiCounter].pBlock = 0L;
      strcpy(phiItems[uiCounter].acName, "");
      break;
    }        
  free(pBlock);
}
   