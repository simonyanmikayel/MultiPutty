#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "putty.h"
#include "misc.h"
#include "search.h"
#include "resource.h"

#pragma warning(disable : 4996) //4996: This function or variable may be unsafe.

static int find_line(int i);

#pragma pack( push, 1 )
typedef struct {
  int line;
  int *pos;
  int len; //amount of actual data
}serarch_line;

typedef struct {
  wchar_t* search_text;
  int search_text_size;
  unsigned short search_settings;
  unsigned short count; //count of found 
  unsigned short cur;   //current search pos
  int len; //amount of actual data
  int cur_line_index;
  int cur_pos_index;
  int  size; //amount of allocated space
}serarch_buf;
#pragma pack( pop )

#define MEM_ISSUE_FIXED
#ifdef MEM_ISSUE_FIXED
serarch_line *g_srch_line = 0;
#else
#define MAX_SRCH_LINE 1024
serarch_line g_srch_line[MAX_SRCH_LINE];
#endif


//extern serarch_buf g_serarch_buf;
static serarch_buf g_serarch_buf = { 0 };

wchar_t* get_search_text() { return g_serarch_buf.search_text; }
void set_search_text(wchar_t* w) { g_serarch_buf.search_text = wcsdup(w); }
unsigned short get_search_settings() { return g_serarch_buf.search_settings; }
void set_search_settings(unsigned short s) { g_serarch_buf.search_settings = s; }
void set_search_text_size(int c) { g_serarch_buf.search_text_size = c; }

void dump_serarch_buf(char* text)
{
#ifdef MEM_ISSUE_FIXED
#endif
}

void resize_search_buf()
{
  return; //!! we have an problrm on sresize(g_srch_line, g_serarch_buf.len, serarch_line);
#ifdef MEM_ISSUE_FIXED
  dump_serarch_buf("resize_search_buf");
  if (g_serarch_buf.len == 0)
  {
    sfree(g_srch_line);
    g_srch_line = 0;
  }
  else
  {
    sresize(g_srch_line, g_serarch_buf.len, serarch_line);
  }
  g_serarch_buf.size = g_serarch_buf.len;
  dump_serarch_buf("resize_search_buf 1 ");
#endif
}

void add_line_pos(serarch_line *s_line, int pos)
{
  s_line->len++;
  s_line->pos = sresize(s_line->pos, s_line->len, int);
  s_line->pos[s_line->len - 1] = pos;
}

void add_search_pos(int line, int pos)
{
  if (g_serarch_buf.len > 0xFFF0 || g_serarch_buf.count > 0xFFF0)
    return;
  if (g_serarch_buf.len > 0 && g_srch_line[g_serarch_buf.len - 1].line > line)
    return; //incorrect line
  if (g_serarch_buf.len > 0 && g_srch_line[g_serarch_buf.len - 1].line == line)
  {
    serarch_line *s_line = g_srch_line + (g_serarch_buf.len - 1);
    if (s_line->pos[s_line->len - 1] >= pos)
      return; //incorrect pos
    add_line_pos(s_line, pos);
  }
  else 
  {
#ifdef MEM_ISSUE_FIXED
    if (g_serarch_buf.len >= g_serarch_buf.size) {
      g_serarch_buf.size = g_serarch_buf.len + 512;
      dump_serarch_buf("add_search_pos 0 ");
      g_srch_line = sresize(g_srch_line, g_serarch_buf.size+1, serarch_line); //!! we have an problrm on sresize(g_srch_line, g_serarch_buf.len, serarch_line);
      dump_serarch_buf("add_search_pos 1 ");
    }
#else
    if (g_serarch_buf.len >= MAX_SRCH_LINE - 1)
      return;
#endif
    g_serarch_buf.len++;
    serarch_line *s_line = g_srch_line + (g_serarch_buf.len - 1);
    dump_serarch_buf("add_search_pos 2 ");
    memset(s_line, 0, sizeof(serarch_line));
    s_line->line = line;
    add_line_pos(s_line, pos);
    
  }
  g_serarch_buf.count++;
}

static int paint_line_index = -1;
static int paint_pos_index = -1;
void clean_serarch_buf()
{
  while (g_serarch_buf.len)
  {
    if (g_srch_line[g_serarch_buf.len - 1].pos)
      sfree(g_srch_line[g_serarch_buf.len - 1].pos);
    g_serarch_buf.len--;
  }
#ifdef MEM_ISSUE_FIXED
  dump_serarch_buf("clean_serarch_buf 0 ");
  sfree(g_srch_line);
  g_srch_line = 0;
  dump_serarch_buf("clean_serarch_buf 1 ");
#endif
  sfree(g_serarch_buf.search_text);
  memset(&g_serarch_buf, 0, sizeof(g_serarch_buf));

  paint_line_index = -1;
  paint_pos_index = -1;
}

void pop_search_buf(int c)
{
  if (!g_serarch_buf.len || c <= 0)
    return;

  int i = find_line(c - 1);
  int del = 0;
  if (i >= g_serarch_buf.len) {
    assert(i == g_serarch_buf.len);
    i = g_serarch_buf.len - 1;
  }

  if (g_srch_line[i].line < c)
  {
    for (; i >= 0; i--)
    {
      g_serarch_buf.count -= g_srch_line[i].len;
      g_serarch_buf.cur -= g_srch_line[i].len;
      sfree(g_srch_line[i].pos);
      g_srch_line[i].pos = 0;
      del++;
    }
  }

  for (i = del; i < g_serarch_buf.len; i++)
  {
    if (del) {
      g_srch_line[i - del] = g_srch_line[i];
    }
    assert(g_srch_line[i - del].line > c);
    g_srch_line[i - del].line -= c;
  }

  if (del)
  {
    g_serarch_buf.cur_line_index -= del;
    if (g_serarch_buf.cur_line_index < 0)
      g_serarch_buf.cur_line_index = 0;

    if (g_serarch_buf.len > del) {
      g_serarch_buf.len -= del;
      resize_search_buf();
    }
    else {
      g_serarch_buf.len = 0;
      clean_serarch_buf();
    }
    set_cur_pos(ID_SEARCH_REFRESH);
  }
  paint_line_index = 0;
  paint_pos_index = 0;

}

void validate_cur()
{
  if (g_serarch_buf.len)
  {
    if (g_serarch_buf.count < 0)
      g_serarch_buf.count = 0;
    if (g_serarch_buf.cur <= 0)
      g_serarch_buf.cur = 1;
    if (g_serarch_buf.cur > g_serarch_buf.count)
      g_serarch_buf.cur = g_serarch_buf.count;

    if (g_serarch_buf.cur_line_index < 0)
      g_serarch_buf.cur_line_index = 0;
    if (g_serarch_buf.cur_line_index >= g_serarch_buf.len)
      g_serarch_buf.cur_line_index = g_serarch_buf.len - 1;

    if (g_serarch_buf.cur_pos_index < 0) {
      if (g_serarch_buf.cur_line_index > 0) {
        g_serarch_buf.cur_line_index--;
        g_serarch_buf.cur_pos_index = g_srch_line[g_serarch_buf.cur_line_index].len - 1;
      }
      else
        g_serarch_buf.cur_pos_index = 0;
    }

    if (g_serarch_buf.cur_pos_index >= g_srch_line[g_serarch_buf.cur_line_index].len) {
      if (g_serarch_buf.cur_line_index < g_serarch_buf.len - 1) {
        g_serarch_buf.cur_line_index++;
        g_serarch_buf.cur_pos_index = 0;
      }
      else {
        g_serarch_buf.cur_pos_index = g_srch_line[g_serarch_buf.cur_line_index].len - 1;
      }
    }

    if (g_serarch_buf.cur < 1) {
      g_serarch_buf.cur = 1;
    }

    if (g_serarch_buf.cur > g_serarch_buf.count) {
      g_serarch_buf.cur = g_serarch_buf.count;
    }
    if (g_serarch_buf.cur == g_serarch_buf.count) {
      g_serarch_buf.cur_line_index = g_serarch_buf.len - 1;
      g_serarch_buf.cur_pos_index = g_srch_line[g_serarch_buf.cur_line_index].len - 1;
    }
    if (g_serarch_buf.cur == 1) {
      g_serarch_buf.cur_line_index = 0;
      g_serarch_buf.cur_pos_index = 0;
    }
  }
  else
  {
    g_serarch_buf.cur = 0;
    g_serarch_buf.cur_line_index = 0;
    g_serarch_buf.cur_pos_index = 0;
  }
  
}

int set_cur_pos(unsigned short cmd)
{
  validate_cur();
  switch (cmd) {
  case ID_SEARCH_NEXT:
    g_serarch_buf.cur++;
    g_serarch_buf.cur_pos_index++;
    break;
  case ID_SEARCH_PREV:
    g_serarch_buf.cur--;
    g_serarch_buf.cur_pos_index--;
    break;
  case ID_SEARCH_FIRST:
    g_serarch_buf.cur = 1;
    break;
  case ID_SEARCH_REFRESH:
  case ID_SEARCH_LAST:
    g_serarch_buf.cur = g_serarch_buf.count;
    break;
  }
  validate_cur();

  PostMessage(hwndHost, WM_MENUCOMMAND, MSG_PUTTY_SEARCH_RESULT, MAKELONG(g_serarch_buf.cur, g_serarch_buf.count));
  return g_serarch_buf.len ? g_srch_line[g_serarch_buf.cur_line_index].line : 0xFFFFFFF;
}

static int find_line(int i)
{
  int l = 0;
  int r = g_serarch_buf.len - 1;
  while (l <= r)
  {
    int m = l + (r - l) / 2;
    int x = g_srch_line[m].line;

    // Check if x present at mid
    if (i == x) {
      return m; //exact value
    }

    // If x smaller, ignore left half
    if (i > x) {
      l = m + 1;
    }

    // If x is greater, ignore right half
    else {
      r = m - 1;
    }
  }
  //return the line which contains next nearist line
  return l; // if we reach here, then element was not present
}

void init_search_for_paint(int line, int pos)
{
  paint_pos_index = 0;
  paint_line_index = find_line(line);
}

int is_search_line(int line, int pos)
{
  serarch_line *s_line;
  if (paint_line_index >= g_serarch_buf.len)
    return 0;
  while (paint_line_index < g_serarch_buf.len && g_srch_line[paint_line_index].line < line) {
    paint_line_index++;
    paint_pos_index = 0;
    if (paint_line_index == g_serarch_buf.len)
      return 0;
  }
  if (g_srch_line[paint_line_index].line > line)
    return 0;
  s_line = g_srch_line + paint_line_index;
  while (s_line->pos[paint_pos_index] + g_serarch_buf.search_text_size < pos && paint_pos_index < s_line->len) {
    paint_pos_index++;
  }
  if (paint_pos_index >= s_line->len) {
    paint_pos_index = 0;
    paint_line_index++;
    return 0;
  }
  if (s_line->pos[paint_pos_index] <= pos && s_line->pos[paint_pos_index] + g_serarch_buf.search_text_size > pos)
    return (g_serarch_buf.cur_line_index == paint_line_index && g_serarch_buf.cur_pos_index == paint_pos_index) ? 2 : 1;
  return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//http://stackoverflow.com/questions/211535/fastest-way-to-do-a-case-insensitive-substring-search-in-c-c
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef unsigned chartype;

wchar_t char_table[256];
static int initialised = 0;

void init_stristr(void)
{
  for (int i = 0; i < 256; i++)
  {
    char_table[i] = towupper(i);
  }
}

#define uppercase(_x) ( (chartype) (bMatchCase ? _x : (_x < 256 ? char_table[_x] : towupper(_x) )) )

wchar_t* find_str(const wchar_t *phaystack, const wchar_t *pneedle, int bMatchCase)
{
  register const wchar_t *haystack, *needle;
  register chartype b, c;

  if (!initialised)
  {
    initialised = 1;
    init_stristr();
  }

  if (!phaystack || !pneedle || !pneedle[0])
    goto ret0;

  haystack = (const wchar_t *)phaystack;
  needle = (const wchar_t *)pneedle;
  b = uppercase(*needle);

  haystack--;             /* possible ANSI violation */
  do
  {
    c = *++haystack;
    if (c == 0)
      goto ret0;
  } while (uppercase(c) != (int)b);

  c = *++needle; //dont pass ++needle to macros
  c = uppercase(c);
  if (c == '\0')
    goto foundneedle;

  ++needle;
  goto jin;

  for (;;)
  {
    register chartype a;
    register const wchar_t *rhaystack, *rneedle;

    do
    {
      a = *++haystack;
      if (a == 0)
        goto ret0;
      if (uppercase(a) == (int)b)
        break;
      a = *++haystack;
      if (a == 0)
        goto ret0;
    shloop:
      ;
    } while (uppercase(a) != (int)b);

  jin:
    a = *++haystack;
    if (a == 0)
      goto ret0;

    if (uppercase(a) != (int)c)
      goto shloop;

    rhaystack = haystack-- + 1;
    rneedle = needle;

    a = uppercase(*rneedle);
    if (uppercase(*rhaystack) == (int)a)
      do
      {
        if (a == 0)
          goto foundneedle;

        ++rhaystack;
        a = *++needle;
        a = uppercase(a);
        if (uppercase(*rhaystack) != (int)a)
          break;

        if (a == '\0')
          goto foundneedle;

        ++rhaystack;
        a = *++needle;
        a = uppercase(a);
      } while (uppercase(*rhaystack) == (int)a);

      needle = rneedle;       /* took the register-poor approach */

      if (a == 0)
        break;
  }

foundneedle:
  return (wchar_t*)haystack;
ret0:
  return 0;
}