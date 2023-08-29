#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <gtk/gtk.h>
#include <regex.h>

#include <fcntl.h>
#include <errno.h>
#include <linux/fs.h>
#include <ext2fs/ext2_fs.h>
#include <ext2fs/ext2_types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

struct ext2_super_block sb;
struct ext2_group_desc *group_desc;
struct ext2_inode inode;
struct ext2_inode_large inode_entry;


#include "treeview.h"
//#include "ext2_reader.h"

static GtkBuilder *builder;
static GObject *window;
static GObject *entry_devicepath;
static GObject *entry_groupnumber;
static GObject *entry_inode;

static GObject *bbmdatabox;
static GObject *inodedatabox;

static GObject *b1 ;
static GObject *b2 ;
static GObject *b3 ;
static GObject *b4 ;
static GObject *b5 ;
static GObject *b6 ;
static GObject *b7 ;
static GObject *b8 ;
static GObject *b9 ;
static GObject *b10;
static GObject *b11;
static GObject *b12;
static GObject *b13;
static GObject *b14;
static GObject *b15;
static GObject *b16;
static GObject *b17;
static GObject *b18;

static GObject *c1;
static GObject *c2;
static GObject *c3;
static GObject *c4;
static GObject *c5;
static GObject *c6;

static GObject *i1 ;
static GObject *i2 ;
static GObject *i3 ;
static GObject *i4 ;
static GObject *i5 ;
static GObject *i6 ;
static GObject *i7 ;
static GObject *i8 ;
static GObject *i9 ;
static GObject *i10;
static GObject *i11;
static GObject *i12;

static GObject *toolbutton1;
static GObject *toolbutton2;
static GObject *toolbutton3;
static GObject *toolbutton4;
static GObject *toolbutton5;
static GObject *toolbutton6;
static GObject *toolbutton7;
static GObject *toolbutton8;
static GObject *toolbutton9;
static GObject *toolbutton10;

static GObject *labelmain;
static GObject *labelbgdisplayrange;
static GObject *labelbgstatus;
static GObject *labelbootblock;
static GObject *labelinodeselect;


static GtkTreeView *p_treeview;

extern char *mntpnt[1024];
extern int current_block_group = -1;
extern int current_ino = -1;
char global_devicepath[1024];

void hex_to_ascii(const unsigned char* hex, size_t hex_len, char* ascii, size_t ascii_len) {
    static const char* hex_chars = "0123456789abcdef";
    size_t i, j;

    for (i = 0, j = 0; i < hex_len && j + 1 < ascii_len; ++i, j += 2) {
        ascii[j] = hex_chars[hex[i] >> 4];
        ascii[j + 1] = hex_chars[hex[i] & 0xf];
    }

    ascii[j] = '\0';
}

void boot_read(){

    char *finalbuff[1024];
    char ascii[2048];
    FILE* fp;
    char *command[100];
    sprintf(command , "sudo od -Ax -t x1 -N 1024 %s" , global_devicepath);
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen");
        exit(1);
    }
    
    fread(finalbuff, 1, 1024, fp);
    
    if (pclose(fp) == -1) {
        perror("pclose");
        exit(1);
    }

    hex_to_ascii(finalbuff, 1024, ascii, sizeof(ascii));

    gtk_label_set_text(GTK_LABEL(labelbootblock), finalbuff);
}

void inodepopulateboxes(char *global_devicepath, int in_no) {

    int fd = open(global_devicepath, O_RDONLY);

    if (fd == -1) {
        perror("Failed to open file");
        exit(1);
    }

    int ino = in_no;
    int count, blockno;
    int total = 0;

    lseek64(fd, 1024, SEEK_CUR); // skipping the first bootblock

    count = read(fd, &sb, sizeof(struct ext2_super_block)); // reading the sb

    // printf("Size of super block : %lu\n", sizeof(struct ext2_super_block));
    // printf("Magic number : %x\n", sb.s_magic);
    // printf("inodes count : %d\n", sb.s_inodes_count);
    // printf("size of group desc : %lu\n", sizeof(struct ext2_group_desc));

    int ino_size = sb.s_inode_size;
    int bl_size = 1024 << sb.s_log_block_size;

    // printf("Block size : %d\n", bl_size);

    int buf[bl_size / 4];
    
    int bg_no = (ino - 1) / sb.s_inodes_per_group;
    int ino_index = (ino - 1) % sb.s_inodes_per_group;

    lseek64(fd, 1024 + bl_size + bg_no * sizeof(group_desc), SEEK_SET);
    count = read(fd, group_desc, sizeof(struct ext2_group_desc));

    // printf("Inode Table: %d\n", group_desc->bg_inode_table);

    unsigned long inode_offset =
        group_desc->bg_inode_table * bl_size + ino_index * ino_size;

    lseek64(fd, inode_offset, SEEK_SET);
    read(fd, &inode, sizeof(inode));

    // printf("Inode: %d\n", ino);
    // printf("2Mode: %#o\n", inode.i_mode & 0xFFF);
    // printf("3Flags: 0x%x\n", inode.i_flags);
    // printf("4Generation: %u\n", inode.i_generation);
    // printf("5User: %u\n", inode.i_uid);
    // printf("6Group: %u\n", inode.i_gid);
    // printf("7Size: %d\n", inode.i_size);
    // printf("8Project: %u\n", inode_entry.i_projid);
    // printf("9File ACL: %u\n", inode.i_file_acl);
    // printf("10Links: %u\n", inode.i_links_count);
    // printf("11Block count: %d\n", inode.i_blocks);
    // printf("12Size of extra inode fields: %u\n", inode_entry.i_extra_isize);

    // if (S_ISREG(inode.i_mode))
    // {
    //     printf("Type: regular\n");
    // }
    // else if (S_ISDIR(inode.i_mode))
    // {
    //     printf("Type: directory\n");
    // }
    // else if (S_ISLNK(inode.i_mode))
    // {
    //     printf("Type: symbolic link\n");
    // }
    // else
    // {
    //     printf("Type: unknown\n");
    // }

    char *ibuff1[50];
    char *ibuff2[50];
    char *ibuff3[50];
    char *ibuff4[50];
    char *ibuff5[50];
    char *ibuff6[50];
    char *ibuff7[50];
    char *ibuff8[50];
    char *ibuff9[50];
    char *ibuff10[50];
    char *ibuff11[50];
    char *ibuff12[50];

    sprintf(ibuff1,"%d",  ino);

    if (S_ISREG(inode.i_mode)){
        sprintf(ibuff2,"Regular");
    }
    else if (S_ISDIR(inode.i_mode)){
        sprintf(ibuff2,"Directory");
    }
    else if (S_ISLNK(inode.i_mode)){
        sprintf(ibuff2,"Symbolic Link");
    }
    else{
        printf("Unknown");
    }

    sprintf(ibuff3,"0x%x", inode.i_flags);
    sprintf(ibuff4,"%u"  , inode.i_generation);
    sprintf(ibuff5,"%u"  , inode.i_uid);
    sprintf(ibuff6,"%u"  , inode.i_gid);
    sprintf(ibuff7,"%u"  , inode.i_size);
    sprintf(ibuff8,"%u"  , inode_entry.i_projid);
    sprintf(ibuff9,"%u"  , inode.i_file_acl);
    sprintf(ibuff10,"%u" , inode.i_links_count);
    sprintf(ibuff11,"%d" , inode.i_blocks);
    sprintf(ibuff12,"%u" , inode_entry.i_extra_isize);

    close(fd);

    gtk_label_set_text(GTK_LABEL(i1),  ibuff1);
    gtk_label_set_text(GTK_LABEL(i2),  ibuff2);
    gtk_label_set_text(GTK_LABEL(i3),  ibuff3);
    gtk_label_set_text(GTK_LABEL(i4),  ibuff4);
    gtk_label_set_text(GTK_LABEL(i5),  ibuff5);
    gtk_label_set_text(GTK_LABEL(i6),  ibuff6);
    gtk_label_set_text(GTK_LABEL(i7),  ibuff7);
    gtk_label_set_text(GTK_LABEL(i8),  ibuff8);
    gtk_label_set_text(GTK_LABEL(i9),  ibuff9);
    gtk_label_set_text(GTK_LABEL(i10), ibuff10);
    gtk_label_set_text(GTK_LABEL(i11), ibuff11);
    gtk_label_set_text(GTK_LABEL(i12), ibuff12);
}

void printinodebitmap(char *global_devicepath, int current_block_group) {

    int fd = open(global_devicepath, O_RDONLY);

    if (fd == -1) {
        perror("Failed to open file");
        exit(1);
    }

    // Read the sb
    lseek(fd, 1024, SEEK_SET);   //bootblock
    read(fd, &sb, sizeof(struct ext2_super_block));
    
    int group_descTableOffset = 1024 + (sb.s_block_group_nr * sizeof(struct ext2_group_desc));
    
    lseek(fd, group_descTableOffset + (current_block_group * sizeof(struct ext2_group_desc)), SEEK_SET);
    read(fd, group_desc, sizeof(struct ext2_group_desc));
    
    int inodeTableBlock = group_desc->bg_inode_table;
    
    int inodeSize = sb.s_inode_size > sizeof(struct ext2_inode) ? sb.s_inode_size : sizeof(struct ext2_inode);
    
    inodedatabox = GTK_BOX(gtk_builder_get_object(builder, "inodedatabox"));

    int i;
    for (i = 0; i < sb.s_inodes_per_group; i++) {
        int inodeOffset = inodeTableBlock * sb.s_log_block_size + i * inodeSize;
        
        lseek(fd, inodeOffset, SEEK_SET);
        read(fd, &inode, sizeof(struct ext2_inode));
        int start_inode = current_block_group * sb.s_inodes_per_group + 1;

        // printf("%d ", i + start_inode);

        char ino_button_id[20];
        sprintf(ino_button_id, "inode_%d", i + start_inode);
        GtkWidget *ino_button = gtk_button_new_with_label(ino_button_id);
        gtk_box_pack_start(inodedatabox, ino_button, FALSE, FALSE, 0);
        gtk_widget_show_all(GTK_WIDGET(ino_button));
    
    }
}

void printblockbitmap(char *global_devicepath, int current_block_group){

    off_t block_group_offset;
    int block_size;
    block_size = 1024 << sb.s_log_block_size;

    int blockbitmapsize = sb.s_blocks_per_group / 8;

    printf(" blockbitmapsize %d ", blockbitmapsize);

    char block_bitmap[sb.s_blocks_per_group];

    int fd = open(global_devicepath, O_RDONLY);

    if (fd == -1) {
        perror("Failed to open file");
        exit(1);
    }

    block_group_offset = block_size * current_block_group;

    off_t offset = lseek(fd, block_group_offset, SEEK_SET);
    if (offset == -1) {
        perror("Failed to seek to block group offset");
        close(fd);
        exit(1);
    }

    ssize_t bytes_read = read(fd, block_bitmap, blockbitmapsize);
    if (bytes_read == -1) {
        perror("Failed to read block bitmap");
        close(fd);
        exit(1);
    }

    //for loop 

    int occupied_blocks = 0;

    for (int i = 0; i < sb.s_blocks_per_group; i++) {
        if ((block_bitmap[i / 8] & (1 << (i % 8))) != 0) {
        	// printf("%d " , i + current_block_group * sb.s_blocks_per_group + 1);
        	occupied_blocks++; 
        }
    }

    // get the GtkBox widget from the glade file by its id
    bbmdatabox = GTK_BOX(gtk_builder_get_object(builder, "bbmdatabox"));

    for (int i = 0; i < sb.s_blocks_per_group; i++) {
        if ((block_bitmap[i / 8] & (1 << (i % 8))) != 0) {
            char button_id[20];
            sprintf(button_id, "block_%d", i + current_block_group * sb.s_blocks_per_group + 1);
            // create a new button with the given id
            GtkWidget *button = gtk_button_new_with_label(button_id);
            // add the button to the box
            gtk_box_pack_start(bbmdatabox, button, FALSE, FALSE, 0);
            gtk_widget_show_all(GTK_WIDGET(button));
        }
    }

}

void read_group_desc(char *global_devicepath, int current_block_group) {

    int fd = open(global_devicepath, O_RDONLY);
    int ret;
    off_t offset;
    int block_size;
    block_size = 1024 << sb.s_log_block_size;
    offset = block_size * 2 + current_block_group * sizeof(struct ext2_group_desc);
    group_desc = malloc(sizeof(struct ext2_group_desc));
    if (group_desc == NULL)    {
        printf("Error: failed to allocate memory for group descriptor\n");
        exit(1);
    }

    ret = lseek(fd, offset, SEEK_SET);
    if (ret < 0)
    {
        printf("Error: failed to seek to group descriptor offset\n");
        exit(1);
    }

    ret = read(fd, group_desc, sizeof(struct ext2_group_desc));
    if (ret < 0)
    {
        printf("Error: failed to read group descriptor\n");
        exit(1);
    }

    char *cbuff1[100];
    char *cbuff2[100];
    char *cbuff3[100];
    char *cbuff4[100];
    char *cbuff5[100];
    char *cbuff6[100];

    sprintf(cbuff1,"%d",  group_desc->bg_block_bitmap);
    sprintf(cbuff2,"%d",  group_desc->bg_inode_bitmap);
    sprintf(cbuff3,"%d",  group_desc->bg_inode_table);
    sprintf(cbuff4,"%d",  group_desc->bg_free_blocks_count);
    sprintf(cbuff5,"%d",  group_desc->bg_free_inodes_count);
    sprintf(cbuff6,"%d",  group_desc->bg_used_dirs_count);

    close(fd);

    gtk_label_set_text(GTK_LABEL(c1),  cbuff1);
    gtk_label_set_text(GTK_LABEL(c2),  cbuff2);
    gtk_label_set_text(GTK_LABEL(c3),  cbuff3);
    gtk_label_set_text(GTK_LABEL(c4),  cbuff4);
    gtk_label_set_text(GTK_LABEL(c5),  cbuff5);
    gtk_label_set_text(GTK_LABEL(c6),  cbuff6);

    free(group_desc);
}

void read_superblock(char *global_devicepath) {

    int fd = open(global_devicepath, O_RDONLY);
    off_t offset = 1024;
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
    }
    if (read(fd, &sb, sizeof(sb)) == -1) {
        perror("read");
        close(fd);
    }
    
    int block_size = 1024 << sb.s_log_block_size; // Calculate block size based on log_block_size field

    char *a1[100];
    char *a2[100];
    char *a3[100];
    char *a4[100];
    char *a5[100];
    char *a6[100];
    char *a7[100];
    char *a8[100];
    char *a9[100];
    char *a10[100];
    char *a11[100];
    char *a12[100];
    char *a13[100];
    char *a14[100];
    char *a15[100];
    char *a16[100];
    char *a17[100];
    char *a18[100];

    sprintf(a1,"%d",  sb.s_inodes_count);
    sprintf(a2,"%d",  sb.s_blocks_count);
    sprintf(a3,"%d",  sb.s_r_blocks_count);
    sprintf(a4,"%d",  sb.s_free_blocks_count);
    sprintf(a5,"%d",  sb.s_free_inodes_count);
    sprintf(a6,"%d",  sb.s_first_data_block);
    sprintf(a7,"%d",  block_size);
    sprintf(a8,"%d",  sb.s_blocks_per_group);
    sprintf(a9,"%d",  sb.s_inodes_per_group);
    sprintf(a10,"%d", sb.s_mnt_count);
    sprintf(a11,"%d", sb.s_max_mnt_count);
    sprintf(a12,"%x", sb.s_magic);
    sprintf(a13,"%d", sb.s_state);
    sprintf(a14,"%d", sb.s_errors);
    sprintf(a15,"%d", sb.s_minor_rev_level);
    sprintf(a16,"%d", sb.s_lastcheck);
    sprintf(a17,"%d", sb.s_checkinterval);
    sprintf(a18,"%d", sb.s_creator_os);

    close(fd);

    gtk_label_set_text(GTK_LABEL(b1),  a1);
    gtk_label_set_text(GTK_LABEL(b2),  a2);
    gtk_label_set_text(GTK_LABEL(b3),  a3);
    gtk_label_set_text(GTK_LABEL(b4),  a4);
    gtk_label_set_text(GTK_LABEL(b5),  a5);
    gtk_label_set_text(GTK_LABEL(b6),  a6);
    gtk_label_set_text(GTK_LABEL(b7),  a7);
    gtk_label_set_text(GTK_LABEL(b8),  a8);
    gtk_label_set_text(GTK_LABEL(b9),  a9);
    gtk_label_set_text(GTK_LABEL(b10), a10);
    gtk_label_set_text(GTK_LABEL(b11), a11);
    gtk_label_set_text(GTK_LABEL(b12), a12);
    gtk_label_set_text(GTK_LABEL(b13), a13);
    gtk_label_set_text(GTK_LABEL(b14), a14);
    gtk_label_set_text(GTK_LABEL(b15), a15);
    gtk_label_set_text(GTK_LABEL(b16), a16);
    gtk_label_set_text(GTK_LABEL(b17), a17);
    gtk_label_set_text(GTK_LABEL(b18), a18);
} 

void remove_widget(GtkWidget *widget, gpointer data) {
    gtk_container_remove(GTK_CONTAINER(data), widget);
}

void update_mntpnt(const char *devicepath) {
    char command[100];
    sprintf(command, "findmnt -n -o TARGET %s", devicepath);
    FILE *fp = popen(command, "r");
    if (fp != NULL) {
        if (fgets(mntpnt, sizeof(mntpnt), fp) != NULL) {
            size_t len = strlen(mntpnt);
            if (len > 0 && mntpnt[len-1] == '\n') {
                mntpnt[len-1] = '\0';
            }
        }
        pclose(fp);
    }
    strcpy(global_devicepath, devicepath);
}

void print_text() {
    const char *input = gtk_entry_get_text(GTK_ENTRY(entry_devicepath));
    // strcpy(dev, input);
    // printf("dada %d",dev);
    //printf("%s\n",mntpnt);
    update_mntpnt(input);
    printf("gloabaaal %s\n",global_devicepath);
    char command[100];
    sprintf(command, "sudo -S dumpe2fs %s > output.txt", input);
    int status = system(command);

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        char *mainlabelbuffer[1024];
        sprintf(mainlabelbuffer,"Command successful. Mounted at %s ", mntpnt);
        gtk_label_set_text(GTK_LABEL(labelmain), mainlabelbuffer);
        
    } else {
        FILE *fp = fopen("output.txt", "r");
        if (fp != NULL) {
            char buffer[1024];
            fgets(buffer, sizeof(buffer), fp);
            gtk_label_set_text(GTK_LABEL(labelmain), buffer);
            fclose(fp);
        } else {
            gtk_label_set_text(GTK_LABEL(labelmain), "Error: Invalid input.");
        }
    }

}

void read_max_bg_groups_from_output_txt() {
    FILE *fp;
    char line[256];
    int max_bg = -1;
    regex_t regex;
    regmatch_t matches[2];

    fp = fopen("output.txt", "r");
    if (fp == NULL) {
        printf("Could not open output.txt\n");
        return;
    }

    if (regcomp(&regex, "^Group ([0-9]+):", REG_EXTENDED) != 0) {
        printf("Could not compile regex\n");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (regexec(&regex, line, 2, matches, 0) == 0) {
            int bg_num = atoi(line + matches[1].rm_so);
            if (bg_num > max_bg) {
                max_bg = bg_num;
            }
        }
    }

    fclose(fp);

    char bg_label[100];
    sprintf(bg_label, "Block groups exist from 0 to %d", max_bg);
    //printf("%s\n", bg_label);
    gtk_label_set_text(GTK_LABEL(labelbgdisplayrange), bg_label);

    regfree(&regex);
}

void update_current_block_group() {
    char *final_buff[1024];
    const char *gninput = gtk_entry_get_text(GTK_ENTRY(entry_groupnumber));
    current_block_group = atoi(gninput);
    sprintf(final_buff," %d ", current_block_group);
    gtk_label_set_text(GTK_LABEL(labelbgstatus), final_buff);
}

void update_global_ino_from_entry() {
    char *finalbuff[1024];
    const char *ginput_ino = gtk_entry_get_text(GTK_ENTRY(entry_inode));
    current_ino = atoi(ginput_ino);
    sprintf(finalbuff,"Viewing Inode No :  %d  ", current_ino);
    //printf("Viewing Inode No :  %d  ", current_ino);
    gtk_label_set_text(GTK_LABEL(labelinodeselect), finalbuff);
}

void toolbutton1_clicked_cb(){
	print_text();
    // printf("%s\n",mntpnt);
}

void toolbutton2_clicked_cb(){
    getmntpnt_to_treeview(mntpnt);
    treeview_init(p_treeview);
}

void toolbutton3_clicked_cb(){
    printf("Bootblock Stuff\n");
    boot_read();
}

void toolbutton4_clicked_cb() {
    read_max_bg_groups_from_output_txt();
    read_superblock(global_devicepath);
}

void toolbutton5_clicked_cb() {
    update_current_block_group();
    gtk_container_foreach(GTK_CONTAINER(bbmdatabox), remove_widget, bbmdatabox);
    gtk_container_foreach(GTK_CONTAINER(inodedatabox), remove_widget, inodedatabox);
}

void toolbutton6_clicked_cb() {
    read_group_desc(global_devicepath, current_block_group);
    // printf("working\n");
}

void toolbutton7_clicked_cb() {
    printf(" Giving bbm \n");
    printblockbitmap(global_devicepath, current_block_group);
}

void toolbutton8_clicked_cb() {
    printf("Giving Inodes \n");
    printinodebitmap(global_devicepath, current_block_group);
}

void toolbutton9_clicked_cb() {
    inodepopulateboxes(global_devicepath, current_ino);
}

void toolbutton10_clicked_cb() {
    update_global_ino_from_entry();
}

int main (int argc, char *argv[]) {

    gtk_init (&argc, &argv);
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "debugfs.glade", NULL);
    
    gtk_builder_connect_signals(builder, NULL);
    
    window = gtk_builder_get_object (builder, "window");
    
    entry_devicepath = GTK_WIDGET(gtk_builder_get_object(builder, "entry_devicepath"));
    entry_groupnumber = GTK_WIDGET(gtk_builder_get_object(builder, "entry_groupnumber"));
    entry_inode = GTK_WIDGET(gtk_builder_get_object(builder, "entry_inode"));

    //data = GTK_WIDGET(gtk_builder_get_object(builder, "data")); //variable datablock

    toolbutton1 = GTK_WIDGET(gtk_builder_get_object(builder, "toolbutton1"));
    toolbutton2 = GTK_WIDGET(gtk_builder_get_object(builder, "toolbutton2"));
    toolbutton3 = GTK_WIDGET(gtk_builder_get_object(builder, "toolbutton3"));
    toolbutton4 = GTK_WIDGET(gtk_builder_get_object(builder, "toolbutton4"));
    toolbutton5 = GTK_WIDGET(gtk_builder_get_object(builder, "toolbutton5"));
    toolbutton6 = GTK_WIDGET(gtk_builder_get_object(builder, "toolbutton6"));
    toolbutton7 = GTK_WIDGET(gtk_builder_get_object(builder, "toolbutton7"));
    toolbutton8 = GTK_WIDGET(gtk_builder_get_object(builder, "toolbutton8"));
    toolbutton9 = GTK_WIDGET(gtk_builder_get_object(builder, "toolbutton9"));
    toolbutton10 = GTK_WIDGET(gtk_builder_get_object(builder, "toolbutton10"));

    labelmain = GTK_WIDGET(gtk_builder_get_object(builder, "labelmain"));
    labelbgdisplayrange = GTK_WIDGET(gtk_builder_get_object(builder, "labelbgdisplayrange"));
    labelbgstatus = GTK_WIDGET(gtk_builder_get_object(builder, "labelbgstatus"));
    labelbootblock = GTK_WIDGET(gtk_builder_get_object(builder, "labelbootblock"));
    labelinodeselect = GTK_WIDGET(gtk_builder_get_object(builder, "labelinodeselect"));

    b1  = GTK_WIDGET(gtk_builder_get_object(builder, "b1"));
    b2  = GTK_WIDGET(gtk_builder_get_object(builder, "b2"));
    b3  = GTK_WIDGET(gtk_builder_get_object(builder, "b3"));
    b4  = GTK_WIDGET(gtk_builder_get_object(builder, "b4"));
    b5  = GTK_WIDGET(gtk_builder_get_object(builder, "b5"));
    b6  = GTK_WIDGET(gtk_builder_get_object(builder, "b6"));
    b7  = GTK_WIDGET(gtk_builder_get_object(builder, "b7"));
    b8  = GTK_WIDGET(gtk_builder_get_object(builder, "b8"));
    b9  = GTK_WIDGET(gtk_builder_get_object(builder, "b9"));
    b10 = GTK_WIDGET(gtk_builder_get_object(builder, "b10"));
    b11 = GTK_WIDGET(gtk_builder_get_object(builder, "b11"));
    b12 = GTK_WIDGET(gtk_builder_get_object(builder, "b12"));
    b13 = GTK_WIDGET(gtk_builder_get_object(builder, "b13"));
    b14 = GTK_WIDGET(gtk_builder_get_object(builder, "b14"));
    b15 = GTK_WIDGET(gtk_builder_get_object(builder, "b15"));
    b16 = GTK_WIDGET(gtk_builder_get_object(builder, "b16"));
    b17 = GTK_WIDGET(gtk_builder_get_object(builder, "b17"));
    b18 = GTK_WIDGET(gtk_builder_get_object(builder, "b18"));

    c1 = GTK_WIDGET(gtk_builder_get_object(builder, "c1"));
    c2 = GTK_WIDGET(gtk_builder_get_object(builder, "c2"));
    c3 = GTK_WIDGET(gtk_builder_get_object(builder, "c3"));
    c4 = GTK_WIDGET(gtk_builder_get_object(builder, "c4"));
    c5 = GTK_WIDGET(gtk_builder_get_object(builder, "c5"));
    c6 = GTK_WIDGET(gtk_builder_get_object(builder, "c6"));

    i1  = GTK_WIDGET(gtk_builder_get_object(builder, "i1"));
    i2  = GTK_WIDGET(gtk_builder_get_object(builder, "i2"));
    i3  = GTK_WIDGET(gtk_builder_get_object(builder, "i3"));
    i4  = GTK_WIDGET(gtk_builder_get_object(builder, "i4"));
    i5  = GTK_WIDGET(gtk_builder_get_object(builder, "i5"));
    i6  = GTK_WIDGET(gtk_builder_get_object(builder, "i6"));
    i7  = GTK_WIDGET(gtk_builder_get_object(builder, "i7"));
    i8  = GTK_WIDGET(gtk_builder_get_object(builder, "i8"));
    i9  = GTK_WIDGET(gtk_builder_get_object(builder, "i9"));
    i10 = GTK_WIDGET(gtk_builder_get_object(builder, "i10"));
    i11 = GTK_WIDGET(gtk_builder_get_object(builder, "i11"));
    i12 = GTK_WIDGET(gtk_builder_get_object(builder, "i12"));

    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	p_treeview = (GtkTreeView*) gtk_builder_get_object(builder, "treeview1");

	//treeview_init(p_treeview,mntpnt); // testing parameter passing to treeview
    
    // gtk_widget_show_all(bg_window);

    gtk_main();
    return 0;
}

// void on_button_clicked(GtkButton *button, gpointer data) {

//     int *occupied_blocks = (int *)data;

//     // Retrieve the number of occupied blocks
//     int num_blocks = g_slist_length((GSList *)occupied_blocks);

//     for (int i = 0; i < num_blocks; i++) {
//         GtkWidget *new_button = gtk_button_new_with_label(g_strdup_printf("bno%d", ((int *)occupied_blocks[i])));
//         gtk_container_add(GTK_CONTAINER(data), new_button);
//         gtk_widget_show(new_button);
//     }
// }

// void print_inode_entry(int fd, int in_no) {
//     int count, blockno;
//     int total = 0;

//     struct ext2_super_block superblock;
//     struct ext2_group_desc group_desc;

//     struct ext2_dir_entry dirent;
//     int ino = in_no;

//     lseek64(fd, 1024, SEEK_CUR); // skipping the first bootblock

//     count = read(fd, &superblock, sizeof(struct ext2_super_block)); // reading the superblock

//     printf("Size of super block : %lu\n", sizeof(struct ext2_super_block));

//     printf("Magic number : %x\n", superblock.s_magic);
//     printf("inodes count : %d\n", superblock.s_inodes_count);
//     printf("size of group desc : %lu\n", sizeof(struct ext2_group_desc));
//     int ino_size = superblock.s_inode_size;
//     int bl_size = 1024 << superblock.s_log_block_size;
//     printf("Block size : %d\n", bl_size);
//     int buf[bl_size / 4];

//     int bg_no = (ino - 1) / superblock.s_inodes_per_group;
//     int ino_index = (ino - 1) % superblock.s_inodes_per_group;

//     lseek64(fd, 1024 + bl_size + bg_no * sizeof(group_desc), SEEK_SET);
//     count = read(fd, &group_desc, sizeof(struct ext2_group_desc));

//     printf("Inode Table: %d\n", group_desc.bg_inode_table);

//     unsigned long inode_offset =
//         group_desc.bg_inode_table * bl_size + ino_index * ino_size;

//     lseek64(fd, inode_offset, SEEK_SET);

//     read(fd, &inode, sizeof(inode));
//     printf("\n");

//     printf("Inode: %d\n", ino);

//     if (S_ISREG(inode.i_mode)){
//         printf("Type: regular\n");
//     }
//     else if (S_ISDIR(inode.i_mode)){
//         printf("Type: directory\n");
//     }
//     else if (S_ISLNK(inode.i_mode)){
//         printf("Type: symbolic link\n");
//     }
//     else{
//         printf("Type: unknown\n");
//     }

//     printf("Mode: %#o\n", inode.i_mode & 0xFFF);
//     printf("Flags: 0x%x\n", inode.i_flags);
//     printf("Generation: %u\n", inode.i_generation);
//     // printf("Version: 0x%08x:%08x\n", inode.i_version_hi, inode.i_version_hi);
//     printf("User: %u\n", inode.i_uid);
//     printf("Group: %u\n", inode.i_gid);
//     printf("Size: %d\n", inode.i_size);
//     printf("Project: %u\n", inode_entry.i_projid);
//     printf("File ACL: %u\n", inode.i_file_acl);
//     printf("Links: %u\n", inode.i_links_count);

//     printf("Block count: %d\n", inode.i_blocks);
//     printf("Size of extra inode fields: %u\n", inode_entry.i_extra_isize);
//     printf("BLOCKS: ");

//     // printing the block addresses (direct and indirect)

//     for (int i = 0; i < EXT2_N_BLOCKS; i++)
//     {
//         if (inode.i_block[i] != 0)
//         {
//             printf("%d ", inode.i_block[i]);
//             total++;
//         }

//         // single indirect block

//         if (i == 12)
//         {
//             printf("%d ", inode.i_block[i]);
//             lseek64(fd, bl_size * inode.i_block[i], SEEK_SET);
//             __u32 IND_block[bl_size / sizeof(__u32)];
//             read(fd, IND_block, bl_size);

//             for (int j = 0; j < bl_size / sizeof(__u32); j++)
//             {
//                 if (IND_block[j] != 0)
//                 {
//                     printf("%d ", IND_block[j]);
//                     total++;
//                 }
//             }
//             continue;
//         }

//         // double indirect block
//         if (i == 13)
//         {
//             printf("double indirect blocks : \n");
//             printf("%d->", inode.i_block[i]);
//             __u32 DIND_block[bl_size / sizeof(__u32)];

//             lseek64(fd, bl_size * inode.i_block[i], SEEK_SET);
//             read(fd, DIND_block, bl_size);

//             for (int j = 0; j < bl_size / sizeof(__u32); j++)
//             {
//                 if (DIND_block[j] != 0)
//                 {
//                     printf("%d->", DIND_block[j]);
//                     total++;

//                     lseek64(fd, bl_size * DIND_block[j], SEEK_SET);
//                     __u32 IND_block[bl_size / sizeof(__u32)];
//                     read(fd, IND_block, bl_size);

//                     for (int k = 0; k < bl_size / sizeof(__u32); k++)
//                     {
//                         if (IND_block[k] != 0)
//                         {
//                             printf("%d ", IND_block[k]);
//                             total++;
//                         }
//                     }
//                 }
//             }
//             continue;
//         }

//         // triple indirect block

//         if (i == 14)
//         {
//             printf("triple indirect blocks : \n");
//             printf("%d->", inode.i_block[i]);
//             lseek64(fd, bl_size * inode.i_block[i], SEEK_SET);
//             __u32 TIND_block[bl_size / sizeof(__u32)];
//             read(fd, TIND_block, bl_size);

//             for (int j = 0; j < bl_size / sizeof(__u32); j++)
//             {
//                 if (TIND_block[j] != 0)
//                 {
//                     printf("%d ", TIND_block[j]);
//                     total++;

//                     lseek64(fd, bl_size * TIND_block[j], SEEK_SET);
//                     __u32 DIND_block[bl_size / sizeof(__u32)];
//                     read(fd, DIND_block, bl_size);

//                     for (int k = 0; k < bl_size / sizeof(__u32); k++)
//                     {
//                         if (DIND_block[k] != 0)
//                         {
//                             printf("%d->", DIND_block[k]);
//                             total++;

//                             lseek64(fd, bl_size * DIND_block[k], SEEK_SET);
//                             __u32 IND_block[bl_size / sizeof(__u32)];
//                             read(fd, IND_block, bl_size);

//                             for (int l = 0; l < bl_size / sizeof(__u32); l++)
//                             {
//                                 if (IND_block[l] != 0)
//                                 {
//                                     printf("%d ", IND_block[l]);
//                                     total++;
//                                 }
//                             }
//                         }
//                     }
//                 }
//             }
//             continue;
//         }
//     }
// }