/*
   Memory Leak Detection - Header
   Copyright (c) 2012-2015 Cheryl Natsu 
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FUNC_LEN 255
#define FILENAME_LEN 255

/* Memory Block List Node Data Structure 
 * record memory block information including
 * function name, filename, line number and size of memory block */
struct mem_block_list_node
{
	void *ptr;
	char func[FILENAME_LEN];
	char filename[FILENAME_LEN];
	int line_no;
	size_t size;
	struct mem_block_list_node *next;
};

/* Memory block List */
struct mem_block_list
{
	struct mem_block_list_node *begin;
	struct mem_block_list_node *end;
	size_t size;
};

/* global variable of memory block */
struct mem_block_list *mem_block_list_g = NULL;

/* working mode */
#define MEMLEAK_MODE_AUTO 0
#define MEMLEAK_MODE_MANUAL 1
#define MEMLEAK_MODE_DEFAULT (MEMLEAK_MODE_AUTO)
int memleak_working_mode = MEMLEAK_MODE_DEFAULT;

/* change memleak working mode */
int memleak_mode(int value)
{
	memleak_working_mode = value;
	return 0;
}

void memleak_report(void);
void memleak_uninit(void);
/* initialize memory block list global variable */
int memleak_init(void)
{
	/* Prevent multiple initialize */
	if (mem_block_list_g != NULL) return 0;
	/* Uninitialize memory block information when exit*/
	if (memleak_working_mode == MEMLEAK_MODE_AUTO)
		atexit(memleak_uninit);
	mem_block_list_g = (struct mem_block_list *)malloc(sizeof(struct mem_block_list));
	if (mem_block_list_g == NULL)
	{
		return -1;
	}
	mem_block_list_g->begin = mem_block_list_g->end = NULL;
	mem_block_list_g->size = 0;
	return 0;
}

/* append a new block to memory block list with information */
int memleak_append_node(void *ptr, size_t size, char *func, char *filename, int line_no)
{
	struct mem_block_list_node *new_node = 
		(struct mem_block_list_node *)malloc(sizeof(struct mem_block_list_node));
	if (new_node == NULL) return -1;
	new_node->next = NULL;
	new_node->ptr = ptr;
	new_node->line_no = line_no;
	strncpy(new_node->func, func, FUNC_LEN);
	strncpy(new_node->filename, filename, FILENAME_LEN);
	new_node->size = size;
	if (mem_block_list_g->begin == NULL)
	{
		mem_block_list_g->begin = mem_block_list_g->end = new_node;
	}
	else
	{
		mem_block_list_g->end->next = new_node;
		mem_block_list_g->end = new_node;
	}
	mem_block_list_g->size++;
	return 0;
}

/* remove an existend block from memory block list */
int memleak_remove_node(void *ptr)
{
	struct mem_block_list_node *cur_node = mem_block_list_g->begin, *prev_node = NULL;
	while (cur_node != NULL)
	{
		if (ptr == cur_node->ptr)
		{
			if (cur_node == mem_block_list_g->begin)
			{
				mem_block_list_g->begin = cur_node->next;
			}
			if (cur_node == mem_block_list_g->end)
			{
				if (prev_node != NULL)
				{
					prev_node->next = NULL;
					mem_block_list_g->end = prev_node;
				}
			}
			if (prev_node != NULL) prev_node->next = cur_node->next;
			free(cur_node);
			mem_block_list_g->size--;
			return 0;
		}
		prev_node = cur_node;
		cur_node = cur_node->next;
	}
	return -1;
}

/* uninitialize list and nodes used to record memory blocks */
void memleak_uninit(void)
{
	if (mem_block_list_g == NULL) return;
	if (memleak_working_mode == MEMLEAK_MODE_AUTO)
		memleak_report();
	struct mem_block_list_node *cur_node = mem_block_list_g->begin, *next_node;
	while (cur_node != NULL)
	{
		next_node = cur_node->next;
		free(cur_node->ptr);
		free(cur_node);
		cur_node = next_node;
	}
	free(mem_block_list_g);
}

/* Print memory blocks not being freed */
void memleak_report(void)
{
	int count = 0;
	struct mem_block_list_node *cur_node = mem_block_list_g->begin;
	while (cur_node != NULL)
	{
		printf("%u byte(s) lost at 0x%08x: %s (%s:%d)\n", cur_node->size, (unsigned int)cur_node->ptr, cur_node->func, cur_node->filename, cur_node->line_no);
		count++;
		cur_node = cur_node->next;
	}
	if (count == 0) printf("All blocks were freed\n");
	else printf("%d block(s) hasn't been freed\n", count);
}

void *memleak_calloc(size_t nmemb, size_t size, char *func, char *filename, int line_no)
{
	if (mem_block_list_g == NULL) memleak_init();
	void *ptr = (void *)calloc(nmemb, size);
	return ptr;
}

void *memleak_malloc(size_t size, char *func, char *filename, int line_no)
{
	if (mem_block_list_g == NULL) memleak_init();
	void *ptr = (void *)malloc(size);
	if (ptr != NULL) memleak_append_node(ptr, size, func, filename, line_no);
	return ptr;
}

void memleak_free(void *ptr, char *func, char *filename, int line_no)
{
	if (mem_block_list_g == NULL) memleak_init();
	if (ptr == NULL) 
	{
		printf("Free NULL pointer: %s (%s:%d)\n", func, filename, line_no);
		return;
	}
	if (memleak_remove_node(ptr) != 0)
	{
		printf("Invalid free at 0x%08x.\n", (unsigned int)ptr);
		return;
	}
	free(ptr);
}

void *memleak_realloc(void *ptr, size_t size, char *func, char *filename, int line_no)
{
	void *new_ptr;
	new_ptr = realloc(ptr, size);
	if (new_ptr != NULL)
	{
		memleak_remove_node(ptr);
		memleak_append_node(ptr, size, func, filename, line_no);
		return new_ptr;
	}
	return ptr;
}

