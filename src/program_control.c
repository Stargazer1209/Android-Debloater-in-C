#include "program_control.h"

#ifndef BASIC_HEADER
    #define BASIC_HEADER
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <gtk/gtk.h>
#endif

bool check_device(const char *device_id)
{
    // 获取当前目录
    char final_command[1024];
    memset(final_command, 0, sizeof(final_command));
    getcwd(final_command, sizeof(final_command));
    // 拼接ADB选择设备
    strcat(final_command, "\\adb.exe -s ");
    // 拼接设备ID
    strcat(final_command, device_id);
    // 拼接get-state
    strcat(final_command, " get-state");
    // 使用popen执行命令
    FILE *pin = popen(final_command, "r");
    if (pin == NULL)
    {
        perror("ADB命令执行错误!");
        return false;
    }
    char output_buffer[1024];
    memset(output_buffer, 0, sizeof(output_buffer));
    fgets(output_buffer, sizeof(output_buffer), pin);
    if (strcmp(output_buffer, "device\n") == 0)
    {
        pclose(pin);
        return true;
    }
    else
    {
        pclose(pin);
        return false;
    }
}

void 
remove_spaces(char* str) 
{
    int count = 0;
    for (int i = 0; str[i] != '\0'; i++) 
    {
        if (str[i] != ' ' && str[i] != '\n') {
            str[count++] = str[i]; // 在新位置放置非空格字符
        }
    }
    str[count] = '\0'; // 添加字符串结束符
}

void
remove_all_children(GtkWidget *widget)
{
    GtkWidget *children;

    children = gtk_widget_get_first_child(widget);

    while (children != NULL)
    {
        gtk_box_remove(GTK_BOX(widget), children);
        children = gtk_widget_get_first_child(widget);
    }
}

void
get_parent_directory(char *path)
{
    char *end = path + strlen(path);
    while (end > path && *end != '\\')
    {
        end--;
    }
    *end = '\0';
}

void
select_theme(GtkWidget *button, GtkBuilder *builder)
{
    // 获取主题名称
    const char *theme_name = gtk_button_get_label(GTK_BUTTON(button));

    // 修改显示的当前主题
    GtkWidget *theme_menu_button = GTK_WIDGET(gtk_builder_get_object(builder, "theme_menu_button"));
    gtk_menu_button_set_label(GTK_MENU_BUTTON(theme_menu_button), theme_name);
}

void
refresh_theme_list(GtkWidget *button, GtkBuilder *builder)
{
    gtk_button_set_label(GTK_BUTTON(button), "刷新中...");

    // 遍历主题文件夹，获取主题文件夹名称列表
    char get_folder_command[1024] = "dir /b /AD ";
    
    char folder_path[1024];
    memset(folder_path, 0, sizeof(folder_path));
    getcwd(folder_path, sizeof(folder_path));
    get_parent_directory(folder_path);
    
    strcat(get_folder_command, folder_path);
    strcat(get_folder_command, "\\share\\themes");
    FILE *pin = popen(get_folder_command, "r");
    if (pin == NULL)
    {
        perror("获取主题文件夹列表失败!");
        return;
    }
    // 清除之前的列表
    GtkWidget *theme_menu_box = GTK_WIDGET(gtk_builder_get_object(builder, "theme_menu_box"));
    remove_all_children(theme_menu_box);

    char output_buffer[1024];
    memset(output_buffer, 0, sizeof(output_buffer));
    while(fgets(output_buffer, sizeof(output_buffer), pin) != NULL)
    {
        remove_spaces(output_buffer);
        printf("theme_button:..%s..\n", output_buffer);
        GtkWidget *theme_button = gtk_button_new_with_label(output_buffer);
        gtk_widget_set_size_request(theme_button, 37, -1);
        g_signal_connect(theme_button, "clicked", G_CALLBACK(select_theme), builder);
        gtk_box_append(GTK_BOX(theme_menu_box), theme_button);
    }

    pclose(pin);

    gtk_button_set_label(GTK_BUTTON(button), "刷新主题列表");
}

void
apply_theme(GtkWidget *button, GtkBuilder *builder)
{
    gtk_button_set_label(GTK_BUTTON(button), "应用中...");

    // 获取当前选定的主题
    GtkWidget *theme_menu_button = GTK_WIDGET(gtk_builder_get_object(builder, "theme_menu_button"));
    const char *theme_name = gtk_menu_button_get_label(GTK_MENU_BUTTON(theme_menu_button));

    // 修改配置文件
    char folder_path[1024];
    memset(folder_path, 0, sizeof(folder_path));
    getcwd(folder_path, sizeof(folder_path));
    get_parent_directory(folder_path);
    strcat(folder_path, "\\etc\\gtk-4.0\\settings.ini");

    FILE *fin = fopen(folder_path, "r");
    char full_content[2048], output_buffer[512];
    memset(full_content, 0, sizeof(full_content));
    memset(output_buffer, 0, sizeof(output_buffer));

    while (fgets(output_buffer, sizeof(output_buffer), fin) != NULL)
    {
        if (strstr(output_buffer, "gtk-theme-name=") != NULL)
        {
            strcat(full_content, "gtk-theme-name=");
            strcat(full_content, theme_name);
            strcat(full_content, "\n");
        }
        else
        {
            strcat(full_content, output_buffer);
        }
    }
    fclose(fin);

    FILE *fout = fopen(folder_path, "w");
    fputs(full_content, fout);
    fclose(fout);

    gtk_button_set_label(GTK_BUTTON(button), "应用选中主题");
}

void
reset_default_theme(GtkWidget *button, GtkBuilder *builder)
{
    gtk_button_set_label(GTK_BUTTON(button), "恢复中...");

    // 修改显示的当前主题
    GtkWidget *theme_menu_button = GTK_WIDGET(gtk_builder_get_object(builder, "theme_menu_button"));
    gtk_menu_button_set_label(GTK_MENU_BUTTON(theme_menu_button), "Fluent2");

    // 修改配置文件
    char folder_path[1024];
    memset(folder_path, 0, sizeof(folder_path));
    getcwd(folder_path, sizeof(folder_path));
    get_parent_directory(folder_path);
    strcat(folder_path, "\\etc\\gtk-4.0\\settings.ini");

    FILE *fin = fopen(folder_path, "r");
    char full_content[2048], output_buffer[512];
    memset(full_content, 0, sizeof(full_content));
    memset(output_buffer, 0, sizeof(output_buffer));
    while (fgets(output_buffer, sizeof(output_buffer), fin) != NULL)
    {
        if (strstr(output_buffer, "gtk-theme-name=") != NULL)
        {
            strcat(full_content, "gtk-theme-name=Fluent2\n");
        }
        else
        {
            strcat(full_content, output_buffer);
        }
    }
    fclose(fin);

    FILE *fout = fopen(folder_path, "w");
    fputs(full_content, fout);
    fclose(fout);

    gtk_button_set_label(GTK_BUTTON(button), "恢复默认主题");
}

void
open_setting(GtkButton *button)
{
    // 打开设置窗口
    GtkWidget *win;
    GtkWidget *btn;

    // 从资源文件中加载设置窗口
    GtkBuilder *builder = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
    win = GTK_WIDGET(gtk_builder_get_object(builder, "settings_window"));

    // 绑定各个按钮的回调函数
    // 重置主题
    btn = GTK_WIDGET(gtk_builder_get_object(builder, "reset"));
    g_signal_connect(btn, "clicked", G_CALLBACK(reset_default_theme), builder);
    // 应用主题按钮
    btn = GTK_WIDGET(gtk_builder_get_object(builder, "apply"));
    g_signal_connect(btn, "clicked", G_CALLBACK(apply_theme), builder);
    // 刷新主题列表
    btn = GTK_WIDGET(gtk_builder_get_object(builder, "refresh"));
    g_signal_connect(btn, "clicked", G_CALLBACK(refresh_theme_list), builder);
    refresh_theme_list(btn, builder);// 窗口加载前刷新一次
    // 主题选择器构建
    btn = GTK_WIDGET(gtk_builder_get_object(builder, "theme_menu_button"));
    char current_theme[256];
    memset(current_theme, 0, sizeof(current_theme));

    char folder_path[1024];// 获取程序根目录
    memset(folder_path, 0, sizeof(folder_path));
    getcwd(folder_path, sizeof(folder_path));
    get_parent_directory(folder_path);
    strcat(folder_path, "\\etc\\gtk-4.0\\settings.ini");

    FILE *fin = fopen(folder_path, "r");// 读取当前主题名称
    while (fgets(current_theme, sizeof(current_theme), fin) != NULL)
    {
        if (strstr(current_theme, "gtk-theme-name") != NULL)
        {
            break;
        }
    }
    fclose(fin);
    char *theme_name = strstr(current_theme, "=");
    theme_name++;
    remove_spaces(theme_name);
    gtk_menu_button_set_label(GTK_MENU_BUTTON(btn), theme_name);    
    // 显示设置窗口
    gtk_window_present(GTK_WINDOW(win));
}

void
open_about(GtkButton *button)
{
    // 打开关于窗口
    GtkWidget *win;
    GtkWidget *btn;

    // 从资源文件中加载关于窗口
    GtkBuilder *builder = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
    win = GTK_WIDGET(gtk_builder_get_object(builder, "about"));
    
    // 显示关于窗口
    gtk_window_present(GTK_WINDOW(win));
}