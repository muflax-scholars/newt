/* $Id: test_com.h,v 1.1 2003/10/25 18:06:01 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2003 */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Header file for test_com.c
 */

extern int is_child;

extern long *alloc(unsigned int nb);
extern void impossible(const char *fmt,...);
extern void nhext_error_handler(int class, const char *error);
extern int child_start(const char *argv0);
extern int child_read(void *handle, void *buf, unsigned int len);
extern int child_write(void *handle, void *buf, unsigned int len);
extern int child_wait0();
extern int child_wait();
extern int debug_read(void *handle, void *buf, unsigned int len);
extern int debug_write(void *handle, void *buf, unsigned int len);
extern int parent_read(void *handle, void *buf, unsigned int len);
extern int parent_write(void *handle, void *buf, unsigned int len);
extern void *get_child_readh();
extern void *get_child_writeh();
extern void *get_parent_readh();
extern void *get_parent_writeh();
