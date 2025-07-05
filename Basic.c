#include "Basic.h"

// -------------------------------------------------
// Global Buffers and State
// -------------------------------------------------

u8 fast_vars[26] = {0};
u8 mix_buf[MIX_BUF_LEN * 2] = {0};
u8 mix_bank = 0;
u8 blit_key_color = BLIT_KEY_DEFAULT;

u32 program_start = SPI_RAM_PROG_START;
u32 program_end = SPI_RAM_PROG_START;
u32 current_line_ptr = SPI_RAM_PROG_START;
char input_buffer[MAX_LINE_LEN];

Token token_stream[32];
u8 token_index = 0;

// -------------------------------------------------
// SPI RAM Utilities (Mocked for now)
// -------------------------------------------------

void SpiRamWriteU8(u8 bank, u16 addr, u8 data);
u8 SpiRamReadU8(u8 bank, u16 addr);
void SpiRamSeqReadStart(u8 bank, u16 addr);
u8 SpiRamSeqReadU8(void);
void SpiRamSeqReadEnd(void);
void SpiRamSeqWriteStart(u8 bank, u16 addr);
void SpiRamSeqWriteU8(u8 value);
void SpiRamSeqWriteEnd(void);

// -------------------------------------------------
// Tokenizer and Line Utilities
// -------------------------------------------------

static u8 is_var_start(char c){
	return (c >= 'A' && c <= 'Z');
}

void TokenizeLine(char *line){
	char *p = line;
	token_index = 0;

	while(*p){
		while(*p == ' ') p++;

		if(*p >= '0' && *p <= '9'){
			token_stream[token_index].type = TOK_NUMBER;
			token_stream[token_index].num = atoi(p);
			while(*p >= '0' && *p <= '9') p++;
		}else if(is_var_start(*p)){
			token_stream[token_index].type = TOK_VAR;
			token_stream[token_index].name[0] = *p++;
			token_stream[token_index].name[1] = 0;
		}else if(*p == '"'){
			token_stream[token_index].type = TOK_STRING;
			p++;
			char *start = p;
			while(*p && *p != '"') p++;
			u8 len = p - start;
			memcpy(token_stream[token_index].str, start, len);
			token_stream[token_index].str[len] = 0;
			if(*p == '"') p++;
		}else if(*p == '('){
			token_stream[token_index].type = TOK_LPAREN;
			p++;
		}else if(*p == ')'){
			token_stream[token_index].type = TOK_RPAREN;
			p++;
		}else{
			token_stream[token_index].type = TOK_OP;
			token_stream[token_index].op = *p++;
		}

		token_index++;
	}
	token_stream[token_index].type = TOK_END;
}

// -------------------------------------------------
// Line Editor
// -------------------------------------------------

void LineInput(){
	u8 len = 0;
	while(1){
		u8 ch = GetChar();
		if(ch == '' || ch == '
'){
			input_buffer[len] = 0;
			break;
		}
		if(ch == 8 && len > 0){
			len--;
			printf("\b \b");
		}else if(len < MAX_LINE_LEN-1){
			input_buffer[len++] = ch;
			PutChar(ch);
		}
	}
}

// -------------------------------------------------
// Expression Evaluation (Simplified for integers)
// -------------------------------------------------

s32 EvalExpression(){
	u8 i = 0;
	if(token_stream[i].type == TOK_NUMBER)
		return token_stream[i++].num;
	if(token_stream[i].type == TOK_VAR)
		return fast_vars[token_stream[i++].name[0] - 'A'];
	return 0;
}

// -------------------------------------------------
// Interpreter Loop
// -------------------------------------------------

void InitBasic(){
	program_start = SPI_RAM_PROG_START;
	program_end = SPI_RAM_PROG_START;
	current_line_ptr = SPI_RAM_PROG_START;
}

void RunBasic(){
	while(1){
		printf("> ");
		LineInput();

		if(strlen(input_buffer)==0)
			continue;

		TokenizeLine(input_buffer);

		// Only very basic command parsing here
		if(strncmp(input_buffer, "PRINT", 5)==0){
			if(token_stream[0].type == TOK_STRING){
				printf("%s\n", token_stream[0].str);
			}else{
				printf("%ld\n", EvalExpression());
			}
		}else if(strncmp(input_buffer,"LET",3)==0){
			u8 var = token_stream[0].name[0] - 'A';
			s32 val = EvalExpression();
			fast_vars[var] = (u8)val;
		}
	}
}
