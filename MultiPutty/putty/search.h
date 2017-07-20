#ifndef PUTTY_SEARCH_H
#define PUTTY_SEARCH_H

void add_search_pos(int line, int pos);
void clean_serarch_buf();
wchar_t* get_search_text();
void set_search_text(wchar_t* w);
unsigned short get_search_settings();
void set_search_settings(unsigned short s);
void resize_search_buf();
void init_search_for_paint(int line, int pos);
int is_search_line(int line, int pos);
void set_search_text_size(int c);
void pop_search_buf(int c);
wchar_t* find_str(const wchar_t *phaystack, const wchar_t *pneedle, int bMatchCase);
int set_cur_pos(unsigned short cmd);
void dump_serarch_buf(char* text);
#endif //PUTTY_SEARCH_H
