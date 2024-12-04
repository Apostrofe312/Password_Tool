#include "settings.h"

GtkWidget* create_settings_tab(PasswordManager* manager) {
    GtkWidget *settingsTab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *label = gtk_label_new("Settings:");
    gtk_box_pack_start(GTK_BOX(settingsTab), label, FALSE, FALSE, 0);

    GtkWidget *toggleButton = gtk_check_button_new_with_label("Enable Feature X");
    gtk_box_pack_start(GTK_BOX(settingsTab), toggleButton, FALSE, FALSE, 0);

    GtkWidget *saveButton = gtk_button_new_with_label("Save Settings");

    g_signal_connect(saveButton, "clicked", G_CALLBACK(+[](GtkWidget *widget, gpointer data) {
        g_print("Settings saved!\n");
    }), NULL);

    gtk_box_pack_start(GTK_BOX(settingsTab), saveButton, FALSE, FALSE, 0);

    return settingsTab;
}
