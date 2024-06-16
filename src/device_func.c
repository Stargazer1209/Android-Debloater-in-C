#include "device_func.h"
#include "program_control.h"

#ifndef BASIC_HEADER
    #define BASIC_HEADER
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <gtk/gtk.h>
#endif

void
refresh_dev_list(GtkButton *button, GtkBuilder *main_builder)
{
    gtk_button_set_label(button, "刷新中...");

    GtkWidget *box = GTK_WIDGET(gtk_builder_get_object(main_builder, "dev_list"));
    remove_all_children(box);
    bool has_dev = false;
    GtkWidget *device_group = NULL;

    /**< adb 输出的 File 指针 */
    FILE *adb_output = NULL;
    // 获取当前目录
    char final_command[1024];
    memset(final_command, 0, sizeof(final_command));
    getcwd(final_command, sizeof(final_command));
    // 列出设备列表的ADB命令
    strcat(final_command, "\\adb.exe devices -l");
    // 存储输出的缓冲数组
    char output_buffer[1024];
    memset(output_buffer, 0, sizeof(output_buffer));
    // 存储全部数组的缓冲数组
    char all_output[4096];
    memset(all_output, 0, sizeof(all_output));

    // 执行adb命令并获取输出
    adb_output = popen(final_command, "r");
    if (adb_output == NULL) {
        perror("ADB命令执行错误!");
        return;
    }

    // 读取adb输出并写入到text_view
    while (fgets(output_buffer, sizeof(output_buffer), adb_output) != NULL) {
        strcat(all_output, output_buffer);
        printf("devrefout:..%s..", output_buffer);
        // 测试用
        //strcpy(output_buffer, "id1234 device model:设备1");
        // 测试end
        if ((strcmp(output_buffer, "List of devices attached\n") == 0) || (strcmp(output_buffer, "\n") == 0))// 略过标题和空行
        {
            continue;
        }

        if (strstr(output_buffer, "device") != NULL) {
            has_dev = true;
            GtkBuilder *builder = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
            GtkWidget *device = GTK_WIDGET(gtk_builder_get_object(builder, "device"));
            
            // 提取设备标识符
            char *sn_end = strstr(output_buffer, "device");
            char sn[256];
            memset(sn, 0, sizeof(sn));
            strncpy(sn, output_buffer, sn_end - output_buffer);
            sn[sn_end - output_buffer] = '\0';
            remove_spaces(sn);

            // 提取设备型号
            char *model_start = strstr(output_buffer, "model:");
            char model[256];
            memset(model, 0, sizeof(model));
            if (model_start != NULL)
            {
                sscanf(model_start, "model:%s", model);
            }
            
            // 设置按钮标签和名称
            gtk_check_button_set_label(GTK_CHECK_BUTTON(device), model);
            gtk_widget_set_name(device, sn);
            g_signal_connect(device, "toggled", G_CALLBACK(select_dev), main_builder);
            if (device_group == NULL)
            {
                device_group = device;
            }
            else
            {
                gtk_check_button_set_group(GTK_CHECK_BUTTON(device), GTK_CHECK_BUTTON(device_group));
            }
            gtk_box_append(GTK_BOX(box), device);
            // output_buffer中包含"device"
        }
        else {
            // output_buffer中不包含"device"
            continue;
        }
    }

    // 关闭adb输出流
    pclose(adb_output);
    // 检查是否没有设备连接
    if (has_dev == false)
    {
        GtkBuilder *build = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
        GtkWidget *warning = GTK_WIDGET(gtk_builder_get_object(build, "warning"));
        GtkWidget *label = GTK_WIDGET(gtk_builder_get_object(build, "warning_message"));
        gtk_label_set_text(GTK_LABEL(label), "未找到已连接设备,请检查:\n1.手机状态是否正常\n2.是否用USB线连接了手机和本机\n3.手机是否开启了USB调试模式\n4.手机是否已经授权本机连接");
        gtk_window_present(GTK_WINDOW(warning));
    }
    
    gtk_button_set_label(button, "刷新设备列表");
}

void
select_dev(GtkWidget *widget, GtkBuilder *builder)
{
    gboolean active = gtk_check_button_get_active(GTK_CHECK_BUTTON(widget));

    if (active)
    {
        const char *device_id = gtk_widget_get_name(widget);
        FILE *fout = fopen("device_id", "w");
        fprintf(fout, "%s", device_id);
        fclose(fout);

        GtkWidget *app_list_box = GTK_WIDGET(gtk_builder_get_object(builder, "known_app_list_box"));
        remove_all_children(app_list_box);
        app_list_box = GTK_WIDGET(gtk_builder_get_object(builder, "unknown_app_list_box"));
        remove_all_children(app_list_box);

        printf("select_dev:..%s..\n", device_id);
    }

    return;
}

void
reboot_device(GtkButton *button)
{
    gtk_button_set_label(GTK_BUTTON(button), "重启中");

    // 拼接命令字符串
    // 获取设备ID
    FILE *fin = fopen("device_id", "r");
    char device_id[256];
    memset(device_id, 0, sizeof(device_id));
    fscanf(fin, "%s", device_id);
    fclose(fin);
    // 检查设备状态
    if (!check_device(device_id))
    {
        GtkBuilder *build = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
        GtkWidget *warning = GTK_WIDGET(gtk_builder_get_object(build, "warning"));
        GtkWidget *label = GTK_WIDGET(gtk_builder_get_object(build, "warning_message"));
        
        gtk_label_set_text(GTK_LABEL(label), "选中的设备不可用 或 未选中设备! \n请刷新设备列表后重新选择设备. ");
        gtk_window_present(GTK_WINDOW(warning));

        gtk_button_set_label(GTK_BUTTON(button), "重启当前设备");
        return;
    }
    // 获取当前目录
    char final_command[1024];
    memset(final_command, 0, sizeof(final_command));
    getcwd(final_command, sizeof(final_command));
    // 预置ADB命令
    strcat(final_command, "\\adb.exe -s ");
    strcat(final_command, device_id);
    strcat(final_command, " reboot");

    // 执行adb命令
    system(final_command);

    GtkBuilder *build;
    GtkWidget *win;
    GtkWidget *label;
    build = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
    win = GTK_WIDGET(gtk_builder_get_object(build, "warning"));
    label = GTK_WIDGET(gtk_builder_get_object(build, "warning_message"));
    
    gtk_label_set_text(GTK_LABEL(label), "正在重启手机...\n请等待手机完全开机后再继续操作!!!");
    gtk_window_present(GTK_WINDOW(win));

    gtk_button_set_label(GTK_BUTTON(button), "重启当前设备");
}
