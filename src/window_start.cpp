#include "window_start.h"
#include "window_proceed.h"
#include "pictureshow.h"
#include "videoplayer.h"

#include <gtk/gtk.h>
#include <string>
#include <iostream>


std::string global_input_text;

/**
 * @brief 初始化并显示启动窗口
 *
 * 初始化GTK环境，并创建一个带有图片和视频选择按钮的启动窗口。此窗口作为程序主窗口，
 * 用户选择文件类型后，点击按钮进入下一步。
 *
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组
 *
 * @return 0 成功初始化并显示窗口
 */
int window_start(int argc, char *argv[])
{
    GtkWidget *start_window;
    GtkWidget *start_label;
    GtkWidget *start_button_picture;
    GtkWidget *start_button_video;
    GtkWidget *start_box_window;
    GtkWidget *start_box_text;
    GtkWidget *start_box_button;

    // 初始化GTK
    gtk_init(&argc, &argv);

    // 创建主窗口
    start_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);                         // 创建主窗口
    gtk_window_set_title(GTK_WINDOW(start_window), "zplayer");                  // 设置窗口的标题为 "zplayer"
    gtk_window_set_default_size(GTK_WINDOW(start_window), 640, 480);            // 设置窗口的默认大小为宽640像素，高480像素
    gtk_window_set_position(GTK_WINDOW(start_window), GTK_WIN_POS_CENTER);      // 设置窗口在屏幕的中心显示
    g_signal_connect(start_window, "destroy", G_CALLBACK(gtk_main_quit), NULL); // 连接信号 "destroy" 到回调函数 gtk_main_quit，当窗口关闭时调用 gtk_main_quit 退出GTK主循环
                                                                                // 这里的 NULL 参数表示传递给回调函数的自定义数据为空
    
    //box_text的实现
    start_label = gtk_label_new("Select the file type.");                       // 创建一个标签控件

    start_box_text = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 50);               // 创建一个容器
    gtk_box_set_homogeneous(GTK_BOX(start_box_text), TRUE);                     // 控件大小不均匀分布
    gtk_container_set_border_width(GTK_CONTAINER(start_box_text), 20);          // 设置容器的边框宽度
    gtk_box_pack_start(GTK_BOX(start_box_text), start_label, TRUE, TRUE, 0);    // 将控件放入容器

    //box_button的实现
    start_button_picture = gtk_button_new_with_label("picture");                // 创建按钮button_picture
    gtk_widget_set_size_request(start_button_picture, 160, 120);                // 设置按钮的大小
    g_signal_connect(start_button_picture, "clicked", G_CALLBACK(on_button_picture_clicked), NULL);  //连接信号

    start_button_video = gtk_button_new_with_label("video");                    // 创建按钮button_video
    gtk_widget_set_size_request(start_button_video, 160, 120);                  // 设置按钮的大小
    g_signal_connect(start_button_video, "clicked", G_CALLBACK(on_button_video_clicked), NULL);      //连接信号

    start_box_button = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 50);             // 创建容器
    gtk_box_set_homogeneous(GTK_BOX(start_box_button), TRUE);                   // 控件大小不均匀分布
    gtk_container_set_border_width(GTK_CONTAINER(start_box_button), 20);        // 设置容器的边框宽度
    gtk_box_pack_start(GTK_BOX(start_box_button), start_button_picture, FALSE, FALSE, 20); // 将控件放入容器
    gtk_box_pack_start(GTK_BOX(start_box_button), start_button_video, FALSE, FALSE, 20);

    //box_window的实现
    start_box_window = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);                // 创建容器
    gtk_box_set_homogeneous(GTK_BOX(start_box_window), TRUE);                    // 控件大小不均匀分布
    gtk_container_set_border_width(GTK_CONTAINER(start_box_window), 20);         // 设置容器的边框宽度
    gtk_box_pack_start(GTK_BOX(start_box_window), start_box_button, TRUE, TRUE, 0); // 将控件放入容器
    gtk_box_pack_start(GTK_BOX(start_box_window), start_box_text, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(start_window), start_box_window);            // 将容器添加到窗口中

    // 显示所有窗口小部件
    gtk_widget_show_all(start_window);

    // 进入GTK主循环
    gtk_main();

    return 0;
}

/**
 * @brief 响应按钮点击事件
 *
 * 当按钮被点击时，该函数会被调用。它首先设置全局输入文本为 '0'，
 * 然后调用 window_proceed() 函数进行后续操作。之后，它会尝试显示与 global_input_text 相关的图片。
 * 如果显示图片失败，则会输出错误信息。
 *
 * @param widget 触发事件的 GTK 部件指针
 * @param data 与事件相关的数据指针
 */
static void on_button_picture_clicked(GtkWidget *widget, gpointer data) {
    global_input_text = '0';
    window_proceed();   //在函数内创建一个窗口获取文件目录
    //显示图片
    std::cout<<global_input_text<<std::endl;
    if (!showImage(global_input_text)) {
        std::cerr << "Failed to display image." << std::endl;
        return;
    }
    global_input_text = '0';
}

/**
 * @brief 按钮点击回调函数：播放视频
 *
 * 当指定的按钮被点击时，该函数将被调用。该函数会设置全局输入文本为'0'，
 * 调用 window_proceed() 函数继续处理，并尝试播放视频。
 * 如果播放视频失败，则输出错误信息并返回。
 *
 * @param widget 触发事件的按钮组件指针
 * @param data 传递给回调函数的数据指针，在此处未使用
 */
static void on_button_video_clicked(GtkWidget *widget, gpointer data) {
    global_input_text = '0';
    window_proceed();           //在函数内创建一个窗口获取文件目录
    if (!playVideo(global_input_text)) {
        std::cerr << "Failed to display video." << std::endl;
        return;
    }
    global_input_text = '0';
    return;
}



