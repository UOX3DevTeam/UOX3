/*
 * Originally by Linus Torvalds.
 * Smart CONFIG_* processing by Werner Almesberger, Michael Chastain.
 *
 * Usage: mkdep file ...
 * 
 * Read source files and output makefile dependency lines for them.
 * I make simple dependency lines for #include <*.h> and #include "*.h".
 * I also find instances of CONFIG_FOO and generate dependencies
 *    like include/config/foo.h.
 *
 * knoxos : changed for the needs of uox3
 *          * removed HPATH stuff
 *          -----------------------------------------------------------
 *          NOTICE: This file is only needed for dependencies of linux
 *                  builds, you do not need it for windows, or to compile
 *                  the server itself
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>



char __depname[512] = "\n\t@touch ";
#define depname (__depname+9)
int hasdep;

struct path_struct {
	int len;
	char buffer[256-sizeof(int)];
} path_array[2] = {
	{  0, "" },
	{  0, "" }
};



/*
 * This records all the configuration options seen.
 * In perl this would be a hash, but here it's a long string
 * of values separated by newlines.  This is simple and
 * extremely fast.
 */
char * str_config  = NULL;
int    size_config = 0;
int    len_config  = 0;



/*
 * Grow the configuration string to a desired length.
 * Usually the first growth is plenty.
 */
void grow_config(int len)
{
	if (str_config == NULL) {
		len_config  = 0;
		size_config = 4096;
		str_config  = malloc(4096);
		if (str_config == NULL)
			{ perror("malloc"); exit(1); }
	}

	while (len_config + len > size_config) {
		str_config = realloc(str_config, size_config *= 2);
		if (str_config == NULL)
			{ perror("malloc"); exit(1); }
	}
}



/*
 * Lookup a value in the configuration string.
 */
int is_defined_config(const char * name, int len)
{
	const char * pconfig;
	const char * plast = str_config + len_config - len;
	for ( pconfig = str_config + 1; pconfig < plast; pconfig++ ) {
		if (pconfig[ -1] == '\n'
		&&  pconfig[len] == '\n'
		&&  !memcmp(pconfig, name, len))
			return 1;
	}
	return 0;
}



/*
 * Add a new value to the configuration string.
 */
void define_config(int convert, const char * name, int len)
{
	grow_config(len + 1);

	memcpy(str_config+len_config, name, len);

	if (convert) {
		int i;
		for (i = 0; i < len; i++) {
			char c = str_config[len_config+i];
			if (isupper(c)) c = tolower(c);
			if (c == '_')   c = '/';
			str_config[len_config+i] = c;
		}
	}

	len_config += len;
	str_config[len_config++] = '\n';
}



/*
 * Clear the set of configuration strings.
 */
void clear_config(void)
{
	len_config = 0;
	define_config(0, "", 0);
}



/*
 * Handle an #include line.
 */
void handle_include(int type, const char * name, int len)
{
	struct path_struct *path = path_array+type;

	if (len == 14 && !memcmp(name, "linux/config.h", len))
		return;

	if (len >= 7 && !memcmp(name, "config/", 7))
		define_config(0, name+7, len-7-2);

	memcpy(path->buffer+path->len, name, len);
	path->buffer[path->len+len] = '\0';
	if (access(path->buffer, F_OK) != 0)
		return;

	if (!hasdep) {
		hasdep = 1;
		printf("%s:", depname);
	}
	printf(" \\\n   %s", path->buffer);
}



/*
 * Record the use of a CONFIG_* word.
 */
void use_config(const char * name, int len)
{
	char *pc;
	int i;

	pc = path_array[0].buffer + path_array[0].len;
	memcpy(pc, "config/", 7);
	pc += 7;

	for (i = 0; i < len; i++) {
	    char c = name[i];
	    if (isupper(c)) c = tolower(c);
	    if (c == '_')   c = '/';
	    pc[i] = c;
	}
	pc[len] = '\0';

	if (is_defined_config(pc, len))
	    return;

	define_config(0, pc, len);

	if (!hasdep) {
		hasdep = 1;
		printf("%s: ", depname);
	}
	printf(" \\\n   $(wildcard %s.h)", path_array[0].buffer);
}



/*
 * Macros for stunningly fast map-based character access.
 * __buf is a register which holds the current word of the input.
 * Thus, there is one memory access per sizeof(unsigned long) characters.
 */

#if defined(__alpha__) || defined(__i386__) || defined(__MIPSEL__) || defined(__arm__)
#define LE_MACHINE
#endif

#ifdef LE_MACHINE
#define next_byte(x) (x >>= 8)
#define current ((unsigned char) __buf)
#else
#define next_byte(x) (x <<= 8)
#define current (__buf >> 8*(sizeof(unsigned long)-1))
#endif

#define GETNEXT { \
	next_byte(__buf); \
	if ((unsigned long) next % sizeof(unsigned long) == 0) { \
		if (next >= end) \
			break; \
		__buf = * (unsigned long *) next; \
	} \
	next++; \
}

/*
 * State machine macros.
 */
#define CASE(c,label) if (current == c) goto label
#define NOTCASE(c,label) if (current != c) goto label

/*
 * Yet another state machine speedup.
 */
#define MAX2(a,b) ((a)>(b)?(a):(b))
#define MIN2(a,b) ((a)<(b)?(a):(b))
#define MAX6(a,b,c,d,e,f) (MAX2(a,MAX2(b,MAX2(c,MAX2(d,MAX2(e,f))))))
#define MIN6(a,b,c,d,e,f) (MIN2(a,MIN2(b,MIN2(c,MIN2(d,MIN2(e,f))))))



/*
 * The state machine looks for (approximately) these Perl regular expressions:
 *
 *    m|\/\*.*?\*\/|
 *    m|'.*?'|
 *    m|".*?"|
 *    m|#\s*include\s*"(.*?)"|
 *    m|#\s*include\s*<(.*?>"|
 *    m|#\s*(?define|undef)\s*CONFIG_(\w*)|
 *    m|(?!\w)CONFIG_|
 *    m|__SMP__|
 *
 * About 98% of the CPU time is spent here, and most of that is in
 * the 'start' paragraph.  Because the current characters are
 * in a register, the start loop usually eats 4 or 8 characters
 * per memory read.  The MAX6 and MIN6 tests dispose of most
 * input characters with 1 or 2 comparisons.
 */
void state_machine(const char * map, const char * end)
{
	const char * next = map;
	const char * map_dot;
	unsigned long __buf = 0;

	for (;;) {
start:
	GETNEXT
__start:
	if (current > MAX6('/','\'','"','#','C','_')) goto start;
	if (current < MIN6('/','\'','"','#','C','_')) goto start;
	CASE('/',  slash);
	CASE('\'', squote);
	CASE('"',  dquote);
	CASE('#',  pound);
	CASE('C',  cee);
	CASE('_',  underscore);
	goto start;

/* / */
slash:
	GETNEXT
	NOTCASE('*', __start);
slash_star_dot_star:
	GETNEXT
__slash_star_dot_star:
	NOTCASE('*', slash_star_dot_star);
	GETNEXT
	NOTCASE('/', __slash_star_dot_star);
	goto start;

/* '.*?' */
squote:
	GETNEXT
	CASE('\'', start);
	NOTCASE('\\', squote);
	GETNEXT
	goto squote;

/* ".*?" */
dquote:
	GETNEXT
	CASE('"', start);
	NOTCASE('\\', dquote);
	GETNEXT
	goto dquote;

/* #\s* */
pound:
	GETNEXT
	CASE(' ',  pound);
	CASE('\t', pound);
	CASE('i',  pound_i);
	CASE('d',  pound_d);
	CASE('u',  pound_u);
	goto __start;

/* #\s*i */
pound_i:
	GETNEXT NOTCASE('n', __start);
	GETNEXT NOTCASE('c', __start);
	GETNEXT NOTCASE('l', __start);
	GETNEXT NOTCASE('u', __start);
	GETNEXT NOTCASE('d', __start);
	GETNEXT NOTCASE('e', __start);
	goto pound_include;

/* #\s*include\s* */
pound_include:
	GETNEXT
	CASE(' ',  pound_include);
	CASE('\t', pound_include);
	map_dot = next;
	CASE('"',  pound_include_dquote);
	CASE('<',  pound_include_langle);
	goto __start;

/* #\s*include\s*"(.*)" */
pound_include_dquote:
	GETNEXT
	CASE('\n', start);
	NOTCASE('"', pound_include_dquote);
	handle_include(1, map_dot, next - map_dot - 1);
	goto start;

/* #\s*include\s*<(.*)> */
pound_include_langle:
	GETNEXT
	CASE('\n', start);
	NOTCASE('>', pound_include_langle);
	handle_include(0, map_dot, next - map_dot - 1);
	goto start;

/* #\s*d */
pound_d:
	GETNEXT NOTCASE('e', __start);
	GETNEXT NOTCASE('f', __start);
	GETNEXT NOTCASE('i', __start);
	GETNEXT NOTCASE('n', __start);
	GETNEXT NOTCASE('e', __start);
	goto pound_define_undef;

/* #\s*u */
pound_u:
	GETNEXT NOTCASE('n', __start);
	GETNEXT NOTCASE('d', __start);
	GETNEXT NOTCASE('e', __start);
	GETNEXT NOTCASE('f', __start);
	goto pound_define_undef;

/* #\s*(define|undef)\s*CONFIG_(\w*) */
pound_define_undef:
	GETNEXT
	CASE(' ',  pound_define_undef);
	CASE('\t', pound_define_undef);

	        NOTCASE('C', __start);
	GETNEXT NOTCASE('O', __start);
	GETNEXT NOTCASE('N', __start);
	GETNEXT NOTCASE('F', __start);
	GETNEXT NOTCASE('I', __start);
	GETNEXT NOTCASE('G', __start);
	GETNEXT NOTCASE('_', __start);

	map_dot = next;
pound_define_undef_CONFIG_word:
	GETNEXT
	if (isalnum(current) || current == '_')
		goto pound_define_undef_CONFIG_word;
	define_config(1, map_dot, next - map_dot - 1);
	goto __start;

/* \<CONFIG_(\w*) */
cee:
	if (next >= map+2 && (isalnum(next[-2]) || next[-2] == '_'))
		goto start;
	GETNEXT NOTCASE('O', __start);
	GETNEXT NOTCASE('N', __start);
	GETNEXT NOTCASE('F', __start);
	GETNEXT NOTCASE('I', __start);
	GETNEXT NOTCASE('G', __start);
	GETNEXT NOTCASE('_', __start);

	map_dot = next;
cee_CONFIG_word:
	GETNEXT
	if (isalnum(current) || current == '_')
		goto cee_CONFIG_word;
	use_config(map_dot, next - map_dot - 1);
	goto __start;

/* __SMP__ */
underscore:
	GETNEXT NOTCASE('_', __start);
	GETNEXT NOTCASE('S', __start);
	GETNEXT NOTCASE('M', __start);
	GETNEXT NOTCASE('P', __start);
	GETNEXT NOTCASE('_', __start);
	GETNEXT NOTCASE('_', __start);
	use_config("SMP", 3);
	goto __start;

    }
}



/*
 * Generate dependencies for one file.
 */
void do_depend(const char * filename, const char * command)
{
	int mapsize;
	int pagesizem1 = getpagesize()-1;
	int fd;
	struct stat st;
	char * map;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror(filename);
		return;
	}

	fstat(fd, &st);
	if (st.st_size == 0) {
		fprintf(stderr,"%s is empty\n",filename);
		close(fd);
		return;
	}

	mapsize = st.st_size;
	mapsize = (mapsize+pagesizem1) & ~pagesizem1;
	map = mmap(NULL, mapsize, PROT_READ, MAP_PRIVATE, fd, 0);
	if ((long) map == -1) {
		perror("mkdep: mmap");
		close(fd);
		return;
	}
	if ((unsigned long) map % sizeof(unsigned long) != 0)
	{
		fprintf(stderr, "do_depend: map not aligned\n");
		exit(1);
	}

	hasdep = 0;
	clear_config();
	state_machine(map, map+st.st_size);
	if (hasdep)
		puts(command);

	munmap(map, mapsize);
	close(fd);
}



/*
 * Generate dependencies for all files.
 */
int main(int argc, char **argv)
{
	int len = 0;

	path_array[0].buffer[len] = '\0';
	path_array[0].len = len;

	while (--argc > 0) {
		const char * filename = *++argv;
		const char * command  = __depname;
		len = strlen(filename);
		memcpy(depname, filename, len+1);
		if (len > 2 && filename[len-2] == '.') {
			if (filename[len-1] == 'c' || filename[len-1] == 'S') {
			    depname[len-1] = 'o';
			    command = "";
			}
		}
		do_depend(filename, command);
	}
	return 0;
}
