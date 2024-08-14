#include "window_proceed.h"

#include <gtk/gtk.h>
#include <string>
#include <iostream>


extern std::string global_input_text;

/**
 * @brief 提交按钮点击事件处理函数
 *
 * 当提交按钮被点击时，该函数将被调用。它会从关联的输入框中获取文本，并存储在全局变量中。
 * 随后，它会找到并销毁包含该按钮的窗口。
 *
 * @param widget 被点击的按钮对象
 * @param data 指向关联输入框的指针
 */
static void on_button_submit_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *entry = GTK_WIDGET(data);
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(entry));
    global_input_text = text;                                           //获取文本
    GtkWindow *window = GTK_WINDOW(gtk_widget_get_toplevel(widget));    //获取窗口
    gtk_widget_destroy(GTK_WIDGET(window));                             //摧毁窗口


}  

/**
 * @brief 处理窗口相关操作
 *
 * 该函数负责创建和初始化一个GTK窗口，其中包括一个文本输入框和一个按钮。
 * 当用户点击按钮时，会调用指定的回调函数（on_button_submit_clicked）。
 *
 * @return 返回值始终为0，表示函数正常执行完毕
 */
int window_proceed()
{
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *entry;
    GtkWidget *box;

    // 初始化GTK
    //gtk_init();

    // 创建一个新窗口
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Entry Example");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 创建一个文本输入框
    entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter the file path");
    gtk_widget_set_size_request(entry, 300, 30); // 设置文本输入框的大小

    // 创建一个按钮
    button = gtk_button_new_with_label("确认");
    gtk_widget_set_size_request(button, 100, 30); // 设置按钮的大小
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_submit_clicked), entry);

    // 创建一个盒子容器，并将文本输入框和按钮添加到其中
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);// 设置控件之间的间距为10
    gtk_box_set_homogeneous(GTK_BOX(box), FALSE); // 控件大小不均匀分布
    gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(box), 20); // 设置容器的边框宽度
    
    gtk_container_add(GTK_CONTAINER(window), box);

    // 显示所有窗口小部件
    gtk_widget_show_all(window);

    // 进入GTK主循环
    gtk_main();

    return 0;
}

