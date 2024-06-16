#ifndef BASIC_HEADER
    #define BASIC_HEADER
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <gtk/gtk.h>
#endif

void
select_dev(GtkWidget *widget, GtkBuilder *builder);

void
refresh_dev_list(GtkButton *button, GtkBuilder *builder);

void
reboot_device(GtkButton *button);
