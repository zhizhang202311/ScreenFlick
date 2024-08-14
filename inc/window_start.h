#ifndef WINDOW_START_H
#define WINDOW_START_H

#include <gtk/gtk.h>
#include <string>
#include <iostream>



int window_start(int argc, char *argv[]);
static void on_button_picture_clicked(GtkWidget *widget, gpointer data);
static void on_button_video_clicked(GtkWidget *widget, gpointer data);

#endif // WINDOW_START_H