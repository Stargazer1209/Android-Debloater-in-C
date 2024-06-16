#include "program_control.h"
#include "device_func.h"
#include "app_func.h"

#ifndef BASIC_HEADER
    #define BASIC_HEADER
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <gtk/gtk.h>
#endif

static void
app_activate(GApplication *app)
{
    // 定义窗口和按钮
    GtkWidget *win = NULL;
    GtkWidget *scroll = NULL;
    GtkWidget *btn1 = NULL;
    GtkWidget *btn2 = NULL;
    GtkWidget *box = NULL;

    // 定义文本视图和文本缓冲
    GtkWidget *text_view = NULL;
    GtkTextBuffer *text_buffer = NULL;
    
    // 从资源文件中加载主窗口
    GtkBuilder *build = gtk_builder_new_from_resource("/com/github/Stargazer1209/AndroidTools/resources/AndroidSystemSimplify.ui");
    
    // 将窗口绑定应用
    win = GTK_WIDGET(gtk_builder_get_object(build, "main_window"));
    gtk_window_set_application(GTK_WINDOW(win), GTK_APPLICATION(app));

    // 将设备按钮信号绑定相关函数
    btn1 = GTK_WIDGET(gtk_builder_get_object(build, "ref_dev"));
    box = GTK_WIDGET(gtk_builder_get_object(build, "dev_list"));
    g_signal_connect(btn1, "clicked", G_CALLBACK(refresh_dev_list), build);
    GtkWidget *ref_dev_list_btn = btn1;
    
    btn2 = GTK_WIDGET(gtk_builder_get_object(build, "reboot_dev"));
    g_signal_connect(btn2, "clicked", G_CALLBACK(reboot_device), NULL);

    // 将应用按钮信号绑定相关函数
    btn1 = GTK_WIDGET(gtk_builder_get_object(build, "disable"));
    g_signal_connect(btn1, "clicked", G_CALLBACK(clean_app), build);

    btn1 = GTK_WIDGET(gtk_builder_get_object(build, "enable"));
    g_signal_connect(btn1, "clicked", G_CALLBACK(enable_app), build);
    
    btn2 = GTK_WIDGET(gtk_builder_get_object(build, "ref_app"));
    text_view = GTK_WIDGET(gtk_builder_get_object(build, "app_list_text"));
    g_signal_connect(btn2, "clicked", G_CALLBACK(refresh_app_list), build);
    GtkWidget *ref_app_list_btn = btn2;

    // 软件自身按钮
    btn1 = GTK_WIDGET(gtk_builder_get_object(build, "theme_button"));
    g_signal_connect(btn1, "clicked", G_CALLBACK(open_setting), NULL);

    btn2 = GTK_WIDGET(gtk_builder_get_object(build, "about_button"));
    g_signal_connect(btn2, "clicked", G_CALLBACK(open_about), NULL);
    
    // 显示窗口
    gtk_window_present(GTK_WINDOW(win));
    refresh_dev_list(GTK_BUTTON(ref_dev_list_btn), build);// 应用启动时自动获取最新设备列表
}

int main(int argc, char **argv)
{
    GtkApplication *app;// 应用
    int stat;// 状态码

    app = gtk_application_new("com.github.Stargazer1209.AndroidTools", G_APPLICATION_DEFAULT_FLAGS);// 创建应用
    g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);// 绑定应用信号
    stat = g_application_run(G_APPLICATION(app), argc, argv);// 运行应用

    g_object_unref(app);// 释放应用

    system("adb kill-server");// 关闭adb服务
    return stat;
}