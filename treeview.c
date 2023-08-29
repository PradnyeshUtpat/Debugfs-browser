#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>

#include "treeview.h"

static GHashTable	*hashtable;
static GError 		*error;
static GtkIconTheme *icon_theme;
static GdkPixbuf 	*pixbuf_folder;
static GdkPixbuf 	*pixbuf_file;

static void parse_line(GtkTreeStore *p_treestore, char * path);
static char* split_path(char * path);
static void create_columns(GtkTreeView  *p_treeview);

static char tree_mntpnt[1024];

void getmntpnt_to_treeview(char *mntpnt) {
    strcpy(tree_mntpnt, mntpnt);
}

void treeview_init(GtkTreeView  *p_treeview)
{
	static GtkTreeStore * p_treestore = NULL;

	char *internal_mnt[1024];

	strcpy(internal_mnt, tree_mntpnt);
	
	p_treestore = gtk_tree_store_new(
		4, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);

	create_columns(p_treeview);

	gtk_tree_view_set_model(p_treeview, (GtkTreeModel*) p_treestore);

	gtk_tree_selection_set_mode(
		gtk_tree_view_get_selection(p_treeview),
		GTK_SELECTION_SINGLE
	);
	
	printf("inside : %s\n",tree_mntpnt);
	treeview_populate(p_treestore, internal_mnt);
}

void treeview_populate(GtkTreeStore *p_treestore, char * directory)
{
	FILE *fp;
	char path[1035];
	
	sprintf(path, "/usr/bin/find %s -mindepth 1 -printf \"%%M %%P\n\" | sort -k 2,2", directory);

	/* Open the command for reading. */
	fp = popen(path, "r");
	if (fp == NULL) {
		printf("Failed to run command: %s\n", path);
		return;
	}

	/* Initialize objects */
	hashtable = g_hash_table_new(g_str_hash, g_str_equal);
	
	error = NULL;
	icon_theme = gtk_icon_theme_get_default();
	pixbuf_folder = gtk_icon_theme_load_icon(
		icon_theme, "folder", 16, GTK_ICON_LOOKUP_USE_BUILTIN, &error);
	pixbuf_file = gtk_icon_theme_load_icon(
		icon_theme, "text-x-generic", 16, GTK_ICON_LOOKUP_USE_BUILTIN, &error);

	while (fgets(path, sizeof(path)-1, fp) != NULL) {
		/* Remove newline from fgets */
		path[strcspn(path, "\n")] = 0;
		parse_line(p_treestore, path);
	}

	pclose(fp);
}

static char * split_path(char * path)
{
	int i=strlen(path);
	while (i>0) {
		if (path[i] == '/') {
			path[i] = 0;
			return &path[i+1];
		}
		i--;
	}
	return path;
}

static void parse_line(GtkTreeStore *p_treestore, char * path)
{
	GtkTreeModel * model = (GtkTreeModel*) p_treestore;
	GtkTreeIter parent_it, child_it;
	char * folder_path;
	char * filename;
	char * tree_path;
	char * full_path;
	
	// Copy full path before splitting (malloc)

	full_path = strdup(&path[11]);
	
	filename = split_path(&path[11]);
	
	folder_path = &path[11];
	
	// Lookup folder_path in hashtable, parent_it is parent tree element
	if (TRUE == g_hash_table_contains(hashtable, folder_path)) {
		tree_path = g_hash_table_lookup(hashtable, folder_path);
		gtk_tree_model_get_iter_from_string (model, &parent_it, tree_path);
		gtk_tree_store_append(p_treestore, &child_it, &parent_it);
	} else {
		//printf("Parent entry NOT FOUND: %s\n", folder_path);
		gtk_tree_store_append(p_treestore, &child_it, NULL);
	}
	
	char is_folder = (path[0] == 'd');
	
	// Insert element to tree
	gtk_tree_store_set(
		p_treestore, &child_it,
		0, is_folder ? pixbuf_folder : pixbuf_file,
		1, filename,
		2, is_folder ? "Directory" : "File",
		3, PANGO_WEIGHT_NORMAL,
		-1);
	
	// Get the path from inserted child
	tree_path = gtk_tree_model_get_string_from_iter(model, &child_it);
	
	g_hash_table_insert(hashtable, strdup(full_path), strdup(tree_path));
	
	//printf("parse_line:  %s\n", tree_path);
}

static void create_columns(GtkTreeView  *p_treeview)
{
	GtkTreeViewColumn   *col;
	GtkCellRenderer     *renderer;


	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "Name");


	gtk_tree_view_append_column(p_treeview, col);


	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_column_pack_start(col, renderer, FALSE);
	gtk_tree_view_column_add_attribute(col, renderer, "pixbuf", 0);


	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", 1);
	gtk_tree_view_column_add_attribute(col, renderer, "weight", 3);
	g_object_set(renderer, "weight-set", TRUE, NULL);


	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "Attributes");


	gtk_tree_view_append_column(p_treeview, col);


	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);


	gtk_tree_view_column_add_attribute(col, renderer, "text", 2);
	gtk_tree_view_column_add_attribute(col, renderer, "weight", 3);
	g_object_set(renderer, "weight-set", TRUE, NULL);
}


void expand_child (
	GtkTreeView *tree_view,
	GtkTreeIter *iter,
	GtkTreePath *path,
	gpointer     user_data)
{
	GtkTreeModel * tree_model =
		gtk_tree_view_get_model (tree_view);

	int n_children =
		gtk_tree_model_iter_n_children (tree_model, iter);
	
	if (n_children == 1) {
		gtk_tree_view_expand_row (tree_view, path, 1);
	}
}
