/***************************************************************************
 ***************************************************************************
 ***                                                                     ***
 ***                   Schani Electronics Language                       ***
 ***                                                                     ***
 ***                   Headerdatei fr Interpreter                       ***
 ***                                                                     ***
 ***                  (c) 1993 by Schani Electronics                     ***
 ***                                                                     ***
 ***************************************************************************
 ***************************************************************************/

#define CHAR_CONSTANT    1
#define INT_CONSTANT     2
#define FLOAT_CONSTANT   3
#define STRING_CONSTANT  4
#define IDENTIFIER       5

#define IF               6
#define WHILE            7
#define FOR              8
#define SWITCH           9
#define REPEAT          10
#define END             11
#define UNTIL           12
#define ELSE            13
#define CASE            14
#define DEFAULT         15

#define RETURN          16
#define BREAK           17
#define T_CHAR          18
#define T_INT           19
#define T_FLOAT         20
#define T_VOID          21
#define GLOBAL          22
#define LOCAL           23
#define SIZEOF          24
#define SUB             25
#define STRUCT          26
#define BEGIN           27
#define NEW             28
#ifdef _WINNT
#undef DELETE
#endif
#define DELETE          29
#define EXTERN          30
#define TO              31
#define DOWNTO          32
#define STEP            33

#define ASSIGN          34   
#define LAND            35
#define LOR             36
#define LXOR            37
#define L               38
#define G               39
#define LE              40
#define GE              41
#define NE              42
#define AND             43
#define OR              44
#define XOR             45
#define LS              46
#define RS              47
#define PLUS            48
#define MINUS           49
#define MULT            50
#define DIV             51
#define MOD             52
#define LNOT            53
#define NOT             54
#define SEMICOLON       55
#define OPENbracket     56
#define CLOSEbracket    57
#define OPENindex       58
#define CLOSEindex      59
#define TYPEspec        60
#define COMMA           61
#define ARROW           62
#define E               63

#define BACKSLASH       64
#define NEWLINE         65

#define INCLUDE         66
#define DEFINE          67

#define LEX_NONE       254
#define LEX_ERROR      255

#define LEX_INCLUDE_LEVELS   16
#define LEX_BUFFER_SIZE    1024

#define LEX_EOF               (plbufBuffer->uiBufferPos ==                  \
                               plbufBuffer->uiBufferLength &&               \
                               buf_eof(plbufBuffer->pbufFile))
#define LEX_CHAR              (plbufBuffer->pcBuffer[plbufBuffer->uiBufferPos])
#define LEX_INPUT             (aplbufStack[uiIncludeLevel])
#define LEX_GET_TOKEN         (lex_get_token(LEX_INPUT))

#define LOCAL_GLOBAL     ((ULONG)-1)
#define NO_ACTION        ((ULONG)-2)
  
typedef char      TYPE_CHAR;
typedef long      TYPE_INT;
typedef double    TYPE_FLOAT;
                  
typedef struct
{
  BUFFER *pbufFile;
  CHAR   *pcBuffer;
  UINT    uiBufferPos;
  UINT    uiBufferLength;
} SEL_LEX_BUFFER;

typedef struct
{
  UCHAR      ucType;
  TYPE_CHAR *pcString;
  TYPE_CHAR  cChar;
  TYPE_FLOAT fFloat;
  TYPE_INT   iInt;
  ULONG      ulNext;
} SEL_TOKEN;              

typedef struct _SEL_TOKEN_LIST
{
  SEL_TOKEN               tokToken;
  struct _SEL_TOKEN_LIST *ptoklNext;
} SEL_TOKEN_LIST;         

typedef struct _SEL_DEFINE_TREE
{
  CHAR                    *pcName;
  SEL_TOKEN_LIST          *ptoklToken;
  struct _SEL_DEFINE_TREE *pdeftLeft;
  struct _SEL_DEFINE_TREE *pdeftRight;
} SEL_DEFINE_TREE;
                               
typedef struct _SEL_LEX_SYMBOL
{
  CHAR                   *pcName;
  UINT                    uiNumber;
  struct _SEL_LEX_SYMBOL *psymNext;
} SEL_LEX_SYMBOL;

typedef struct _SEL_SYMBOL_TREE
{
  SEL_LEX_SYMBOL          *psymSymbol;
  struct _SEL_SYMBOL_TREE *psymtLeft;
  struct _SEL_SYMBOL_TREE *psymtRight;
} SEL_SYMBOL_TREE;                            

typedef struct _SEL_SYMBOL
{
  CHAR *pcName;
  UINT  uiHashValue;
} SEL_SYMBOL;

typedef struct
{
  CHAR       *pcName;
  UCHAR      *pucType;
  ULONG       ulValue;
  TYPE_FLOAT  fValue;
} SEL_VARIABLE;

typedef struct _SEL_VAR_LIST
{
  SEL_VARIABLE          varVariable; 
  struct _SEL_VAR_LIST *pvarlNext;
} SEL_VAR_LIST;

typedef struct _SEL_VAR_TREE
{
  SEL_VARIABLE          varVariable;
  struct _SEL_VAR_TREE *pvartLeft;
  struct _SEL_VAR_TREE *pvartRight;
} SEL_VAR_TREE;

typedef struct _SEL_STRUCT_ELEMENT
{
  CHAR                       *pcName;
  UCHAR                      *pucType;
  UINT                        uiOffset;       
  struct _SEL_STRUCT_ELEMENT *pelemNext;
} SEL_STRUCT_ELEMENT;

typedef struct
{
  CHAR               *pcName;
  SEL_STRUCT_ELEMENT *pelemFirst;
  UINT                uiSizeof;
  UINT                uiNumber;
} SEL_STRUCT;        

typedef struct _SEL_STRUCT_LIST
{
  SEL_STRUCT              *pstructStruct;
  struct _SEL_STRUCT_LIST *pstructlNext;
} SEL_STRUCT_LIST;                          
  
typedef struct _SEL_TYPE_LIST
{
  UCHAR                 *pucType;
  struct _SEL_TYPE_LIST *ptypelNext;
} SEL_TYPE_LIST;
         
#define SEL_EXPR_EXPR             1
#define SEL_EXPR_OPERATOR         2

typedef struct _SEL_EXPR
{
  UCHAR             ucType;
  UCHAR             ucDiscardable;
  UCHAR            *pucType;
  ULONG             ulValue;
  TYPE_FLOAT        fValue;
  struct _SEL_EXPR *pexprLeft;
  struct _SEL_EXPR *pexprRight;
  struct _SEL_EXPR *pexprNext;
} SEL_EXPR;
             
typedef struct _SEL_PARAM
{
  CHAR              *pcName;
  UCHAR             *pucType;
  struct _SEL_PARAM *pparamNext;
} SEL_PARAM;

#define SEL_FUNC_RTL           1
#define SEL_FUNC_SEL           2
                               
struct _SEL_PROGRAM;             

typedef SEL_EXPR*(*SEL_RTL_FUNC)(struct _SEL_PROGRAM*);

typedef struct
{
  CHAR        *pcName;
  UCHAR       *pucReturnType;
  SEL_PARAM   *pparamFirst;
  UINT         uiMP;
  ULONG        ulIP;
  UCHAR        ucType;
  SEL_RTL_FUNC rtl_func;
} SEL_FUNC; 

typedef struct _SEL_FUNC_LIST
{
  SEL_FUNC              *pfuncFunction;
  struct _SEL_FUNC_LIST *pfunclNext;
} SEL_FUNC_LIST;

typedef struct
{             
  SEL_FUNC     *pfuncFunction;
  SEL_VAR_TREE *pvartVariables;
  UINT          uiMP;
  ULONG         ulIP;
} SEL_FUNC_STACK;

typedef struct
{
  SEL_VAR_LIST       **ppvarlVariables;
  struct _SEL_SYMBOL  *psymSymbols;
  SEL_STRUCT          *pstructStructs;
  SEL_STRUCT_LIST    **ppstructlStructs;  
  SEL_FUNC_LIST      **ppfunclFuncs;
  UINT                 uiLines;
  UCHAR               *pucLines;
  UINT                 uiFirstStruct;
  UINT                 uiStructs;
  CHAR                *pcSymbolTable;
  UCHAR               *pucSource;         
  UINT                 uiLocals;
  ULONG               *pulLocals;
  UINT                 uiGlobals;
  ULONG               *pulGlobals;
  UINT                 uiFuncs;
  ULONG               *pulFuncs;
} SEL_MODULE;                                     

#include <setjmp.h>            

typedef struct
{
  UCHAR *pucTypes;
  UINT  *puiFreeTypes;
  UINT   uiNextFree;
} SEL_TYPES;

typedef struct
{
  SEL_EXPR *pexprExprs;
  UINT     *puiFreeExprs;
  UINT      uiNextFree;
} SEL_EXPRS;                    

typedef struct
{
  void      **ppBlocks;
  UINT        uiBlockTop;
  CNT_STACK **ppsStacks;
  UINT        uiStackTop;
} SEL_MEM_BLOCKS;

typedef struct
{
  SEL_VAR_TREE **ppvartVarTrees;
  UINT           uiTreesUsed;
} SEL_VAR_BLOCKS;

typedef struct _SEL_PROGRAM
{                                   
  UINT              uiModules;
  SEL_MODULE      **ppmodModules;
  SEL_TYPE_LIST   **pptypelTypes;
  SEL_STRUCT       *pstructStructs;
  SEL_STRUCT_LIST **ppstructlStructs;
  SEL_VAR_LIST    **ppvarlVariables;
  UINT              uiStructs;
  SEL_FUNC_LIST   **ppfunclFuncs;
  SEL_FUNC_STACK   *pfstkFuncStack;
  SEL_MEM_BLOCKS    mbMemBlocks;
  SEL_TOKEN         tokToken;
  CHAR            **ppcModules;
  UCHAR             ucBlock;
  UCHAR             ucReason;
  UINT              uiFP;         
  UINT              uiPP;
  UINT              uiMP;
  ULONG             ulIP;           
  jmp_buf           jbErrorJump;
} SEL_PROGRAM;

#define SEL_HASH_SIZE             37
#define SEL_FUNC_STACK_SIZE       30
#define SEL_TYPE_SIZE             20
#define SEL_TYPE_POOL_SIZE       400
#define SEL_EXPR_POOL_SIZE       200
#define SEL_VAR_POOL_SIZE        400
#define SEL_BLOCK_NUMBER          30
#define SEL_STACK_NUMBER          30

#define SEL_TYPE_INT               1
#define SEL_TYPE_FLOAT             2
#define SEL_TYPE_CHAR              3
#define SEL_TYPE_VOID              4
#define SEL_TYPE_POINTER           5
#define SEL_TYPE_ARRAY             6
#define SEL_TYPE_STRUCT            7
#define SEL_TYPE_REFERENCE         8
#define SEL_TYPE_STRING            9

#define SEL_INT                    man_trivial_type(SEL_TYPE_INT)
#define SEL_FLOAT                  man_trivial_type(SEL_TYPE_FLOAT)
#define SEL_CHAR                   man_trivial_type(SEL_TYPE_CHAR)
#define SEL_VOID                   man_trivial_type(SEL_TYPE_VOID)
#define SEL_STRING                 man_trivial_type(SEL_TYPE_STRING)
#define SEL_POINTER                man_trivial_type(SEL_TYPE_POINTER)
                                    
#define SEL_BLOCK_NONE             0
#define SEL_BLOCK_LOCAL            1
#define SEL_BLOCK_GLOBAL           2
#define SEL_BLOCK_FUNC             3                                         
                                    
#define SEL_STACK_FUNC             1

#define SEL_LOOK_AHEAD             (pmodModule->pucSource[pprogProgram->ulIP])
#define SEL_ACT_TOKEN              (pprogProgram->tokToken)

#if defined MSDOS || defined __MSDOS__
#define SEL_FILE_FORMAT            1
#else
#define SEL_FILE_FORMAT            2
#endif
                                                            
void  reg_init   (UINT);
void* reg_alloc  (UINT, UINT, CHAR*);
void* reg_calloc (UINT, UINT, UINT, CHAR*);
void  reg_free   (void*);
          
#ifdef SEL_DEBUG
#define utl_alloc(s)               reg_alloc(s, __LINE__, __FILE__)
#define utl_calloc(s,n)            reg_calloc(s, n, __LINE__, __FILE__)
#define utl_realloc(m,s)           realloc(m,s)
#define utl_free(s)                reg_free(s)
#else
#define utl_alloc(s)               malloc(s)
#define utl_calloc(s,n)            calloc(s,n)
#define utl_realloc(m,s)           realloc(m,s)
#endif

#ifndef __TURBOC__
#ifndef _WATCOM
#define cputs(s)       _cputs(s)
#define cgets(s)       _cgets(s)
#define cprintf        _cprintf
#define cscanf         _cscanf
#define getch          _getch
#define getche         _getche
#endif
#endif

CHAR                lex_process_char         (CHAR*);
void                lex_process_string       (CHAR*);
UINT                lex_add_symbol           (CHAR*);
UINT                lex_get_symbol_number    (CHAR*);
void                lex_put_token            (SEL_TOKEN*);
void                lex_include              (void);
void                lex_add_define           (void);
SEL_TOKEN_LIST*     lex_get_define           (CHAR*);
void                lex_exit                 (CHAR*);
UINT                lex_get_symbol_table     (ULONG*);
void                lex_write_symbol_table   (void);
void                lex_write_queue          (CNT_QUEUE*);
void                lex_write_output         (void);
void                lex_tokenize_source      (CHAR*, CHAR*);

void                mod_init_locals          (SEL_PROGRAM*);
void                mod_init_globals         (SEL_PROGRAM*);
void                mod_add_func             (SEL_PROGRAM*, SEL_FUNC*);
void                mod_init_funcs           (SEL_PROGRAM*);
SEL_MODULE*         mod_init                 (SEL_PROGRAM*, CHAR*, UINT*,
                                              UINT*);
                 
void                mem_init                 (SEL_PROGRAM*);
void                mem_collect_blocks       (SEL_MEM_BLOCKS*);
void                mem_collect_stacks       (SEL_MEM_BLOCKS*);
void*               mem_alloc                (SEL_PROGRAM*, UINT);
void*               mem_calloc               (SEL_PROGRAM*, UINT, UINT);
void*               mem_realloc              (SEL_PROGRAM*, void*, UINT);
void                mem_free                 (SEL_PROGRAM*, void*);
void                mem_register_stack       (SEL_PROGRAM*, CNT_STACK*);
void                mem_unregister_stack     (SEL_PROGRAM*, CNT_STACK*);
void                mem_delete_all           (SEL_PROGRAM*);
                             
UCHAR*              man_alloc_type           (void);
void                man_free_type            (UCHAR*);
SEL_EXPR*           man_alloc_expr           (void);
void                man_free_expr            (SEL_EXPR*);
SEL_VAR_TREE*       man_alloc_var_tree       (void);
void                man_free_var_tree        (SEL_VAR_TREE*);
void                man_free_function        (SEL_FUNC*);
UCHAR*              man_trivial_type         (UCHAR);
UCHAR               man_type_greater         (UCHAR, UCHAR);
UCHAR*              man_copy_type            (UCHAR*);
UINT                man_line                 (SEL_PROGRAM*);
void                man_error                (SEL_PROGRAM*, UINT, CHAR*);
void                man_fatal_error          (UINT, CHAR*);
SEL_STRUCT*         man_struct_in_table      (CHAR*, SEL_STRUCT_LIST**);
UINT                man_sizeof               (SEL_PROGRAM*, UCHAR*);
UINT                man_struct_offsets       (SEL_PROGRAM*, 
                                              SEL_STRUCT_ELEMENT*);
UINT                man_add_struct           (SEL_PROGRAM*, UINT,
                                              SEL_STRUCT_ELEMENT*);
UINT                man_add_empty_struct     (SEL_PROGRAM*, UINT);
UINT                man_struct_token2number  (SEL_PROGRAM*, UINT);
UCHAR*              man_type2reference       (UCHAR*);
void                man_delete_func          (SEL_FUNC*);
void                man_delete_var_tree      (SEL_VAR_TREE*);
ULONG               man_var_reference        (SEL_VARIABLE*);
ULONG               man_expr_reference       (SEL_EXPR*);
void                man_remove_reference     (SEL_EXPR*);
void                man_dereference          (SEL_EXPR*);
void                man_reference            (SEL_EXPR*);
UCHAR               man_token2reference      (SEL_PROGRAM*, UINT, SEL_EXPR*);
SEL_VARIABLE*       man_get_variable         (SEL_PROGRAM*, CHAR*);
SEL_EXPR*           man_int_expr             (TYPE_INT, SEL_EXPR*);
TYPE_INT            man_int_value            (SEL_EXPR*);
SEL_EXPR*           man_char_expr            (TYPE_CHAR, SEL_EXPR*);
TYPE_CHAR           man_char_value           (SEL_EXPR*);
SEL_EXPR*           man_float_expr           (TYPE_FLOAT, SEL_EXPR*);
TYPE_FLOAT          man_float_value          (SEL_EXPR*);
SEL_EXPR*           man_string_expr          (TYPE_CHAR*, SEL_EXPR*);
UCHAR*              man_make_pointer         (UCHAR*);
SEL_EXPR*           man_pointer_expr         (void*, UCHAR*, SEL_EXPR*);
void*               man_pointer_value        (SEL_EXPR*);
void                man_delete_expr          (SEL_EXPR*);
SEL_STRUCT*         man_get_struct_by_number (SEL_PROGRAM*, UINT);
void                man_struct_element       (SEL_PROGRAM*, SEL_EXPR*, CHAR*);
void                man_array_element        (SEL_PROGRAM*, SEL_EXPR*, UINT);
UCHAR               man_same_type            (UCHAR*, UCHAR*);
CHAR*               man_type2sel_form        (SEL_PROGRAM*, UCHAR*);
void                man_cast                 (SEL_PROGRAM*, SEL_EXPR*, UCHAR*);
UCHAR               man_type                 (UCHAR*);
void                man_init_variable        (SEL_PROGRAM*, SEL_VARIABLE*);
SEL_VARIABLE*       man_add_variable         (SEL_PROGRAM*, CHAR*, UCHAR*);
void                man_assign               (SEL_PROGRAM*, ULONG, SEL_EXPR*);
void                man_assign_variable      (SEL_PROGRAM*, SEL_VARIABLE*,
                                              SEL_EXPR*);
                                         
UINT                prg_hash                 (CHAR*);
SEL_EXPR*           prg_call_func            (SEL_PROGRAM*, CHAR*, SEL_EXPR*);
SEL_EXPR*           prg_run_func             (SEL_PROGRAM*, CHAR*, SEL_EXPR*);
SEL_PROGRAM*        prg_init                 (CHAR**);

void                par_get_token            (SEL_PROGRAM*, UCHAR, UINT);
void                par_matching_bracket     (SEL_PROGRAM*);
void                par_search_token         (SEL_PROGRAM*, UCHAR);
UCHAR*              par_type                 (SEL_PROGRAM*);
UCHAR*              par_formal_type          (SEL_PROGRAM*);
SEL_PARAM*          par_formal_specs         (SEL_PROGRAM*);
SEL_PARAM*          par_formal_params        (SEL_PROGRAM*);
SEL_FUNC*           par_sub                  (SEL_PROGRAM*);
SEL_STRUCT_ELEMENT* par_var_decls            (SEL_PROGRAM*);
SEL_PARAM*          par_param_types          (SEL_PROGRAM*);
SEL_FUNC*           par_prototype            (SEL_PROGRAM*);
void                par_global_decl          (SEL_PROGRAM*);
void                par_local                (SEL_PROGRAM*);
void                par_global               (SEL_PROGRAM*);
SEL_EXPR*           par_arguments            (SEL_PROGRAM*);
void                par_func_vars            (SEL_PROGRAM*);
void                par_constant             (SEL_PROGRAM*);
SEL_EXPR*           par_primary              (SEL_PROGRAM*);
SEL_EXPR*           par_unary                (SEL_PROGRAM*);
SEL_EXPR*           par_level1               (SEL_PROGRAM*);
SEL_EXPR*           par_level2               (SEL_PROGRAM*);
SEL_EXPR*           par_level3               (SEL_PROGRAM*);
SEL_EXPR*           par_level4               (SEL_PROGRAM*);
SEL_EXPR*           par_level5               (SEL_PROGRAM*);
SEL_EXPR*           par_level6               (SEL_PROGRAM*);
SEL_EXPR*           par_expr                 (SEL_PROGRAM*);
SEL_EXPR*           par_statement            (SEL_PROGRAM*);
                       
SEL_EXPR*           rtl_write_int            (SEL_PROGRAM*);
SEL_EXPR*           rtl_read_int             (SEL_PROGRAM*);
SEL_EXPR*           rtl_write_float          (SEL_PROGRAM*);
SEL_EXPR*           rtl_read_float           (SEL_PROGRAM*);
SEL_EXPR*           rtl_write_char           (SEL_PROGRAM*);
SEL_EXPR*           rtl_read_char            (SEL_PROGRAM*);
SEL_EXPR*           rtl_write_string         (SEL_PROGRAM*);
SEL_EXPR*           rtl_read_string          (SEL_PROGRAM*);
SEL_EXPR*           rtl_randomize            (SEL_PROGRAM*);
SEL_EXPR*           rtl_random               (SEL_PROGRAM*);
SEL_EXPR*           rtl_mem_alloc            (SEL_PROGRAM*);
SEL_EXPR*           rtl_mem_re_alloc         (SEL_PROGRAM*);
SEL_EXPR*           rtl_mem_free             (SEL_PROGRAM*);
SEL_EXPR*           rtl_mem_avail            (SEL_PROGRAM*);        
SEL_EXPR*           rtl_vio_w_p_c_a          (SEL_PROGRAM*);
SEL_EXPR*           rtl_vio_w_w_c_a          (SEL_PROGRAM*);
SEL_EXPR*           rtl_vio_w_s_a            (SEL_PROGRAM*);
SEL_EXPR*           rtl_vio_redraw           (SEL_PROGRAM*);
