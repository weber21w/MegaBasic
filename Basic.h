#ifndef UZEBOX_BASIC_H
#define UZEBOX_BASIC_H

#include <stdint.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef int16_t s16;
typedef uint16_t u16;
typedef int32_t s32;
typedef uint32_t u32;

// -------------------------------------------------
// Constants and Limits
// -------------------------------------------------
#define MAX_LINE_LEN     128
#define MAX_LINES        512
#define MAX_STRINGS      64
#define MAX_STRING_LEN   64
#define MAX_ARRAYS       32
#define MIX_BUF_LEN      262
#define BLIT_KEY_DEFAULT 0xFE
#define SPI_RAM_PROG_START 0x000000UL

// -------------------------------------------------
// Types
// -------------------------------------------------
typedef enum {
	VARTYPE_INT,
	VARTYPE_FLOAT,
	VARTYPE_STRING
} VarType;

typedef enum {
	TOK_NUMBER, TOK_STRING, TOK_VAR, TOK_OP, TOK_LPAREN, TOK_RPAREN, TOK_END
} TokenType;

typedef struct {
	TokenType type;
	union {
		s32 num;
		char str[MAX_STRING_LEN];
		char name[14];
		char op;
	};
} Token;

typedef struct {
	char name[14];
	VarType type;
	u16 addr;
	u8 flags;
} Variable;

typedef struct {
	char name[14];
	u8 dims;
	u16 size;
	u16 addr;
	VarType type;
} Array;

typedef struct {
	char name[14];
	u16 addr;
} FunctionDef;

typedef void (*CommandHandler)(char*);
typedef struct {
	const char *name;
	CommandHandler handler;
} CommandEntry;

// -------------------------------------------------
// Globals
// -------------------------------------------------
extern u8 fast_vars[26];
extern u8 mix_buf[MIX_BUF_LEN * 2];
extern u8 mix_bank;
extern u8 blit_key_color;
extern u32 program_start;
extern u32 program_end;
extern u32 current_line_ptr;
extern char input_buffer[MAX_LINE_LEN];
extern Token token_stream[32];
extern u8 token_index;

// -------------------------------------------------
// Public API
// -------------------------------------------------
void InitBasic(void);
void RunBasic(void);
extern const CommandEntry commands[] PROGMEM;

#endif
