#ifndef _TREEVIEW_EXAMPLE_H
#define _TREEVIEW_EXAMPLE_H

#include <gtk/gtk.h>
static char *mntpnt[1024];

void getmntpnt_to_treeview(char *mntpnt);
void treeview_init(GtkTreeView * p_treeview);
void treeview_populate(GtkTreeStore *p_treestore, char * directory);

#endif
