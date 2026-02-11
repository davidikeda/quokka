#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

typedef enum {
    /* Literals */
    TOK_TRUE,
    TOK_FALSE,
    TOK_NULL,
    TOK_NIL,
    TOK_NONE,

    /* Declarations / Structure */
    TOK_NEW,
    TOK_CLASS,
    TOK_OBJECT,
    TOK_FUNCT,
    TOK_ENUM,
    TOK_TEMPLATE,
    TOK_GENERIC,

    /* Control Flow */
    TOK_IF,
    TOK_THEN,
    TOK_ELSE,
    TOK_ELIF,
    TOK_FOR,
    TOK_WHILE,
    TOK_DO,
    TOK_UNTIL,
    TOK_FOREACH,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_EXIT,
    TOK_PASS,
    TOK_YIELD,
    TOK_RESUME,

    /* Exception / Error Handling */
    TOK_TRY,
    TOK_EXCEPT,
    TOK_CATCH,
    TOK_FINALLY,
    TOK_RAISE,
    TOK_THROW,

    /* Logic */
    TOK_AND,
    TOK_OR,
    TOK_NOT,
    TOK_NAND,
    TOK_NOR,
    TOK_XOR,
    TOK_XNOR,

    /* I/O */
    TOK_READ,
    TOK_WRITE,
    TOK_PRINT,
    TOK_LOG,
    TOK_INPUT,
    TOK_PROMPT,

    /* Scope & Storage */
    TOK_GLOBAL,
    TOK_LOCAL,
    TOK_STATIC,
    TOK_PERSISTENT,
    TOK_VOLATILE,

    /* Types */
    TOK_VOID,
    TOK_STR,
    TOK_INT,
    TOK_FLOAT,
    TOK_DOUBLE,
    TOK_BYTE,
    TOK_BOOL,
    TOK_CHAR,
    TOK_ARRAY,
    TOK_LIST,
    TOK_DICT,
    TOK_MAP,
    TOK_SET,

    /* Type Operations */
    TOK_CAST,
    TOK_TYPEOF,
    TOK_SIZEOF,
    TOK_LENGTH,
    TOK_COUNT,

    /* Import / Export / Interop */
    TOK_IMPORT,
    TOK_EXPORT,
    TOK_AS,
    TOK_PYTHON,
    TOK_JAVA,
    TOK_KOTLIN,
    TOK_C,

    /* File / Language Identifiers */
    TOK_JOEY,
    TOK_QK,
    TOK_QUOKKA,

    /* USB / Device */
    TOK_USBIN,
    TOK_USBOUT,
    TOK_SEND,
    TOK_RECEIVE,
    TOK_TRANSMIT,
    TOK_DEVICE,
    TOK_CONNECT,
    TOK_DISCONNECT,
    TOK_TIMEOUT,
    TOK_STATUS,
    TOK_ATTACH,
    TOK_DETACH,
    TOK_ROUTE,
    TOK_REROUTE,

    /* Transmission Integrity */
    TOK_CHECKSUM,
    TOK_HASH,
    TOK_SIGN,
    TOK_VERIFY,

    /* Sync / Concurrency */
    TOK_SYNC,
    TOK_ASYNC,
    TOK_AWAIT,
    TOK_DAEMON,
    TOK_THREAD,
    TOK_TASK,
    TOK_LOCK,
    TOK_UNLOCK,
    TOK_MUTEX,
    TOK_PRIORITY,

    /* Memory */
    TOK_ALLOCATE,
    TOK_FREE,
    TOK_MALLOC,
    TOK_DISPOSE,

    /* Buffers / Queues */
    TOK_QUEUE,
    TOK_BUFFER,
    TOK_PUSH,
    TOK_POP,
    TOK_SHIFT,
    TOK_UNSHIFT,

    /* Lifecycle */
    TOK_FINALIZE,
    TOK_CLOSE,
    TOK_RESET,
    TOK_RESTART,
    TOK_WATCHDOG,

    /* Compression / Encoding */
    TOK_COMPRESS,
    TOK_DECOMPRESS,
    TOK_ENCODE,
    TOK_DECODE,

    /* Security */
    TOK_AUTHORIZE,
    TOK_AUTHENTICATE,

    /* Observability */
    TOK_DEBUG,
    TOK_TRACE,
    TOK_OBSERVE,
    TOK_EMIT,
    TOK_PROFILE,
    TOK_STATS,

    /* Configuration */
    TOK_CONFIGURE,
    TOK_SETUP,
    TOK_MODE,
    TOK_PROFILE_MODE,

    /* Validation */
    TOK_SCHEMA,
    TOK_CONTRACT,
    TOK_VALIDATE,

    /* References */
    TOK_ALIAS,
    TOK_REF,
    TOK_POINTER,

    /* Execution */
    TOK_EXEC,
    TOK_EVAL,

    /* Events */
    TOK_ONCONNECT,
    TOK_ONRECEIVE,
    TOK_ONERROR,
    TOK_ONDISCONNECT,

    /* Metadata */
    TOK_HEADER,
    TOK_FOOTER,

    /* Iteration Helpers */
    TOK_RANGE,
    TOK_STEP,

    /* Special */
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_STRING,
    TOK_EOF,
    TOK_UNKNOWN

} TokenType;

typedef struct Lexer
{
    FILE *file;
    int current;
    int next;
    int line;
    int column;
} Lexer;


typedef struct
{
    TokenType type;
    char *value;
    int line;
    int column;
} Token;

Lexer *lexerInit(FILE *file);
Token  lexerNextToken(Lexer *lexer);
void  lexerFree(Lexer *lexer);

#endif