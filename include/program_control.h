#ifndef BASIC_HEADER
    #define BASIC_HEADER
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <gtk/gtk.h>
#endif

bool 
check_device(const char *device_id);

void
remove_spaces(char* str);

void
remove_all_children(GtkWidget *widget);

void
get_parent_directory(char *path);

// 选定主题
void
select_theme(GtkWidget *button, GtkBuilder *builder);

// 刷新主题列表
void
refresh_theme_list(GtkWidget *button, GtkBuilder *builder);

// 应用选定的主题
void
apply_theme(GtkWidget *button, GtkBuilder *builder);

// 恢复为默认主题
void
reset_default_theme(GtkWidget *button, GtkBuilder *builder);

//打开主题设置窗口
void
open_setting(GtkButton *button);

// 打开关于窗口
void
open_about(GtkButton *button);