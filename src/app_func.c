#include "app_func.h"
#include "program_control.h"

#ifndef BASIC_HEADER
    #define BASIC_HEADER
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <gtk/gtk.h>
#endif

void
find_app_in_list(char *package_name)
{
    // 获取应用列表文件路径
    char list_path[1024];
    memset(list_path, 0, sizeof(list_path));
    getcwd(list_path, sizeof(list_path));
    get_parent_directory(list_path);
    strcat(list_path, "\\etc\\bloatware_data.txt");

    // 打开文件
    FILE *fin = fopen(list_path, "r");
    if (fin == NULL)
    {
        printf("Error: Cannot open file %s\n", list_path);
        return;
    }

    char current_package[1024], package_name_copy[1024];
    memset(current_package, 0, sizeof(current_package));
    memset(package_name_copy, 0, sizeof(package_name_copy));
    strcpy(package_name_copy, package_name);
    strcat(package_name_copy, "=");
    while (fgets(current_package, sizeof(current_package), fin) != NULL)
    {
        // 若找到已知应用则修改包名并返回
        if (strstr(current_package, package_name_copy) != NULL)
        {
            fclose(fin);
            strcpy(package_name, current_package);

            return;
        }
    }
    
    fclose(fin);
    return;
}

void
show_package_info(GtkWidget *app_button, GtkBuilder *builder)
{
    GtkWidget *package_info_textview = GTK_WIDGET(gtk_builder_get_object(builder, "package_info_textview"));
    GtkTextBuffer *package_info_textview_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(package_info_textview));

    const char *package_intro = gtk_widget_get_name(app_button);

    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(package_info_textview_buffer), package_intro, -1);

    return;
}

void
refresh_app_list(GtkButton *button, GtkBuilder *main_builder)
{
    gtk_button_set_label(button, "刷新中...");

    // 获取可执行文件路径
    char bin_path[1024];
    memset(bin_path, 0, sizeof(bin_path));
    getcwd(bin_path, sizeof(bin_path));

    // 获取当前选中的设备
    char current_device[1024];
    memset(current_device, 0, sizeof(current_device));
    FILE *fin = fopen("device_id", "r");
    if (fin == NULL)
    {
        GtkBuilder *builder = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
        GtkWidget *warning_message = GTK_WIDGET(gtk_builder_get_object(builder, "warning_message"));
        GtkWidget *warning_window = GTK_WIDGET(gtk_builder_get_object(builder, "warning"));
        gtk_label_set_text(GTK_LABEL(warning_message), "未选中设备! \n请刷新设备列表后重新选择. ");
        gtk_window_present(GTK_WINDOW(warning_window));
        
        gtk_button_set_label(button, "刷新应用列表");
        return;
    }
    fgets(current_device, sizeof(current_device), fin);
    fclose(fin);

    // 检查设备是否可用
    if (!check_device(current_device))
    {
        GtkBuilder *builder = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
        GtkWidget *warning_message = GTK_WIDGET(gtk_builder_get_object(builder, "warning_message"));
        GtkWidget *warning_window = GTK_WIDGET(gtk_builder_get_object(builder, "warning"));
        gtk_label_set_text(GTK_LABEL(warning_message), "选中的设备不可用! \n请刷新设备列表后重新选择. ");
        gtk_window_present(GTK_WINDOW(warning_window));

        gtk_button_set_label(button, "刷新应用列表");
        return;
    }
    
    // 构建要执行的命令
    char adb_command[1024];
    memset(adb_command, 0, sizeof(adb_command));
    strcpy(adb_command, bin_path);
    strcat(adb_command, "\\adb.exe -s ");
    strcat(adb_command, current_device);
    strcat(adb_command, " shell pm list packages -s");
    printf("Command: ..%s..\n", adb_command);

    // 执行命令
    fin = popen(adb_command, "r");
    if (fin == NULL)
    {
        printf("Error: Cannot execute command %s\n", adb_command);

        gtk_button_set_label(button, "刷新应用列表");
        return;
    }
    
    // 清空应用列表
    GtkWidget *app_list_box = NULL;
    app_list_box = GTK_WIDGET(gtk_builder_get_object(main_builder, "known_app_list_box"));
    remove_all_children(app_list_box);
    app_list_box = GTK_WIDGET(gtk_builder_get_object(main_builder, "unknown_app_list_box"));
    remove_all_children(app_list_box);
    
    // 读取输出
    char output_buffer[2048];
    memset(output_buffer, 0, sizeof(output_buffer));
    while (fgets(output_buffer, sizeof(output_buffer), fin) != NULL)
    {
        // 获取包名
        char package_name[1024];
        memset(package_name, 0, sizeof(package_name));
        remove_spaces(output_buffer);
        strcpy(package_name, strstr(output_buffer, ":") + 1);
        if (strlen(package_name) == 0)
        {
            continue;
        }

        // 查找是否是已知应用
        find_app_in_list(package_name);
        remove_spaces(package_name);
        printf("Package: ..%s..\n", package_name);

        // 构建应用按钮添加到列表
        char package_intro[1024];
        memset(package_intro, 0, sizeof(package_intro));
        if (strstr(package_name, "=") != NULL)
        {
            strcpy(package_intro, strstr(package_name, "=") + 1);
            strcpy(strstr(package_name, "="), "\0");
            app_list_box = GTK_WIDGET(gtk_builder_get_object(main_builder, "known_app_list_box"));
        }
        else
        {
            // continue; // 调试用
            strcpy(package_intro, "该组件功能尚不明了, 请谨慎禁用! ");
            app_list_box = GTK_WIDGET(gtk_builder_get_object(main_builder, "unknown_app_list_box"));
        }

        GtkWidget *app_button = gtk_check_button_new_with_label(package_name);
        gtk_widget_set_name(app_button, package_intro);
        g_signal_connect(app_button, "toggled", G_CALLBACK(show_package_info), main_builder);
        gtk_box_append(GTK_BOX(app_list_box), app_button);
    }
    fclose(fin);

    // 添加提示信息
    GtkWidget *package_info_textview = GTK_WIDGET(gtk_builder_get_object(main_builder, "package_info_textview"));
    GtkTextBuffer *package_info_textview_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(package_info_textview));
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(package_info_textview_buffer), "请点击上方应用名称以查看详细信息. ", -1);

    gtk_button_set_label(button, "刷新应用列表");
}

void
clean_app(GtkButton *button, GtkBuilder *main_builder)
{
    gtk_button_set_label(button, "禁用中...");

    // 获取可执行文件路径
    char bin_path[1024];
    memset(bin_path, 0, sizeof(bin_path));
    getcwd(bin_path, sizeof(bin_path));

    // 获取当前选中的设备
    char current_device[1024];
    memset(current_device, 0, sizeof(current_device));
    FILE *fin = fopen("device_id", "r");
    if (fin == NULL)
    {
        GtkBuilder *builder = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
        GtkWidget *warning_message = GTK_WIDGET(gtk_builder_get_object(builder, "warning_message"));
        GtkWidget *warning_window = GTK_WIDGET(gtk_builder_get_object(builder, "warning"));
        gtk_label_set_text(GTK_LABEL(warning_message), "未选中设备! \n请刷新设备列表后重新选择. ");
        gtk_window_present(GTK_WINDOW(warning_window));
        
        gtk_button_set_label(button, "禁用已选中应用");
        return;
    }
    fgets(current_device, sizeof(current_device), fin);
    fclose(fin);

    // 检查设备是否可用
    if (!check_device(current_device))
    {
        GtkBuilder *builder = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
        GtkWidget *warning_message = GTK_WIDGET(gtk_builder_get_object(builder, "warning_message"));
        GtkWidget *warning_window = GTK_WIDGET(gtk_builder_get_object(builder, "warning"));
        gtk_label_set_text(GTK_LABEL(warning_message), "选中的设备不可用! \n请刷新设备列表后重新选择. ");
        gtk_window_present(GTK_WINDOW(warning_window));

        gtk_button_set_label(button, "禁用已选中应用");
        return;
    }

    // 构建ADB命令
    char adb_command_prefix[1024];
    memset(adb_command_prefix, 0, sizeof(adb_command_prefix));
    strcpy(adb_command_prefix, bin_path);
    strcat(adb_command_prefix, "\\adb.exe -s ");
    strcat(adb_command_prefix, current_device);
    strcat(adb_command_prefix, " shell pm disable-user --user 0 ");

    // 查找选中的应用并执行命令
    GtkWidget *app_list_box = GTK_WIDGET(gtk_builder_get_object(main_builder, "known_app_list_box"));
    GtkWidget *app_button = gtk_widget_get_first_child(app_list_box);
    while (app_button != NULL)
    {
        if (!gtk_check_button_get_active(GTK_CHECK_BUTTON(app_button)))
        {
            app_button = gtk_widget_get_next_sibling(app_button);
            continue;
        }

        char adb_command[1024];
        memset(adb_command, 0, sizeof(adb_command));
        strcpy(adb_command, adb_command_prefix);
        strcat(adb_command, gtk_check_button_get_label(GTK_CHECK_BUTTON(app_button)));

        printf("Command: ..%s..\n", adb_command);
        system(adb_command);

        app_button = gtk_widget_get_next_sibling(app_button);
    }

    app_list_box = GTK_WIDGET(gtk_builder_get_object(main_builder, "unknown_app_list_box"));
    app_button = gtk_widget_get_first_child(app_list_box);
    while (app_button != NULL)
    {
        if (!gtk_check_button_get_active(GTK_CHECK_BUTTON(GTK_CHECK_BUTTON(app_button))))
        {
            app_button = gtk_widget_get_next_sibling(app_button);
            continue;
        }

        char adb_command[1024];
        memset(adb_command, 0, sizeof(adb_command));
        strcpy(adb_command, adb_command_prefix);
        strcat(adb_command, gtk_check_button_get_label(GTK_CHECK_BUTTON(app_button)));

        printf("Command: ..%s..\n", adb_command);
        system(adb_command);

        app_button = gtk_widget_get_next_sibling(app_button);
    }

    GtkBuilder *builder = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
    GtkWidget *warning_message = GTK_WIDGET(gtk_builder_get_object(builder, "warning_message"));
    GtkWidget *warning_window = GTK_WIDGET(gtk_builder_get_object(builder, "warning"));
    gtk_label_set_text(GTK_LABEL(warning_message), "选择的应用禁用已完成. ");
    gtk_window_present(GTK_WINDOW(warning_window));

    gtk_button_set_label(button, "禁用已选中应用");
}

void
enable_app(GtkButton *button, GtkBuilder *main_builder)
{
    gtk_button_set_label(button, "启用中...");

    // 获取可执行文件路径
    char bin_path[1024];
    memset(bin_path, 0, sizeof(bin_path));
    getcwd(bin_path, sizeof(bin_path));

    // 获取当前选中的设备
    char current_device[1024];
    memset(current_device, 0, sizeof(current_device));
    FILE *fin = fopen("device_id", "r");
    if (fin == NULL)
    {
        GtkBuilder *builder = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
        GtkWidget *warning_message = GTK_WIDGET(gtk_builder_get_object(builder, "warning_message"));
        GtkWidget *warning_window = GTK_WIDGET(gtk_builder_get_object(builder, "warning"));
        gtk_label_set_text(GTK_LABEL(warning_message), "未选中设备! \n请刷新设备列表后重新选择. ");
        gtk_window_present(GTK_WINDOW(warning_window));
        
        gtk_button_set_label(button, "恢复已选中应用");
        return;
    }
    fgets(current_device, sizeof(current_device), fin);
    fclose(fin);

    // 检查设备是否可用
    if (!check_device(current_device))
    {
        GtkBuilder *builder = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
        GtkWidget *warning_message = GTK_WIDGET(gtk_builder_get_object(builder, "warning_message"));
        GtkWidget *warning_window = GTK_WIDGET(gtk_builder_get_object(builder, "warning"));
        gtk_label_set_text(GTK_LABEL(warning_message), "选中的设备不可用! \n请刷新设备列表后重新选择. ");
        gtk_window_present(GTK_WINDOW(warning_window));

        gtk_button_set_label(button, "恢复已选中应用");
        return;
    }

    // 构建ADB命令
    char adb_command_prefix[1024];
    memset(adb_command_prefix, 0, sizeof(adb_command_prefix));
    strcpy(adb_command_prefix, bin_path);
    strcat(adb_command_prefix, "\\adb.exe -s ");
    strcat(adb_command_prefix, current_device);
    strcat(adb_command_prefix, " shell pm enable ");

    // 查找选中的应用并执行命令
    GtkWidget *app_list_box = GTK_WIDGET(gtk_builder_get_object(main_builder, "known_app_list_box"));
    GtkWidget *app_button = gtk_widget_get_first_child(app_list_box);
    while (app_button != NULL)
    {
        if (!gtk_check_button_get_active(GTK_CHECK_BUTTON(app_button)))
        {
            app_button = gtk_widget_get_next_sibling(app_button);
            continue;
        }

        char adb_command[1024];
        memset(adb_command, 0, sizeof(adb_command));
        strcpy(adb_command, adb_command_prefix);
        strcat(adb_command, gtk_check_button_get_label(GTK_CHECK_BUTTON(app_button)));

        printf("Command: ..%s..\n", adb_command);
        system(adb_command);

        app_button = gtk_widget_get_next_sibling(app_button);
    }

    app_list_box = GTK_WIDGET(gtk_builder_get_object(main_builder, "unknown_app_list_box"));
    app_button = gtk_widget_get_first_child(app_list_box);
    while (app_button != NULL)
    {
        if (!gtk_check_button_get_active(GTK_CHECK_BUTTON(GTK_CHECK_BUTTON(app_button))))
        {
            app_button = gtk_widget_get_next_sibling(app_button);
            continue;
        }

        char adb_command[1024];
        memset(adb_command, 0, sizeof(adb_command));
        strcpy(adb_command, adb_command_prefix);
        strcat(adb_command, gtk_check_button_get_label(GTK_CHECK_BUTTON(app_button)));

        printf("Command: ..%s..\n", adb_command);
        system(adb_command);

        app_button = gtk_widget_get_next_sibling(app_button);
    }

    GtkBuilder *builder = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
    GtkWidget *warning_message = GTK_WIDGET(gtk_builder_get_object(builder, "warning_message"));
    GtkWidget *warning_window = GTK_WIDGET(gtk_builder_get_object(builder, "warning"));
    gtk_label_set_text(GTK_LABEL(warning_message), "选择的应用启用已完成. ");
    gtk_window_present(GTK_WINDOW(warning_window));

    gtk_button_set_label(button, "恢复已选中应用");
}