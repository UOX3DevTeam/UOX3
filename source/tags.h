// Version History
// 1.0		Shadowlord	16 October, 2001
//			Initial TAG structure declaration, as a simple linked list.

#ifndef __TAGS_H__
#define __TAGS_H__

struct TAG_
{
	struct TAG_ *nextTag;
	struct TAG_ *prevTag;
	char *name;
	jsval val;
};

typedef struct TAG_ TAG;

#endif
