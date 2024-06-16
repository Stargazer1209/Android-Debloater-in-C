#ifndef BASIC_HEADER
    #define BASIC_HEADER
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <gtk/gtk.h>
#endif

void
find_app_in_list(char *package_name);

void
show_package_info(GtkWidget *app_button, GtkBuilder *builder);

//刷新应用列表
void
refresh_app_list(GtkButton *button, GtkBuilder *builder);

// 禁用已选中应用
void
clean_app(GtkButton *button, GtkBuilder *main_builder);

void
enable_app(GtkButton *button, GtkBuilder *main_builder);