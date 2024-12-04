#ifndef SETTINGS_H
#define SETTINGS_H

#include <gtk/gtk.h>
#include "password_manager.h"

// Function declaration for creating the settings tab
GtkWidget* create_settings_tab(PasswordManager* manager);

#endif // SETTINGS_H
