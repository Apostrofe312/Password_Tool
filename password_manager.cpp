#include "password_manager.h"
#include <fstream>
#include <algorithm>

std::string getPasswordsFilePath() {
    std::string homeDir = getenv("HOME");
    return homeDir + "/.passwords.txt"; // Hidden file for passwords
}

PasswordManager::PasswordManager(GtkWidget* parent) : parentWindow(parent), maxPasswordLength(32) {
    passwordListBox = NULL;
    loadPasswords();
}

void PasswordManager::loadPasswords() {
    std::ifstream file(getPasswordsFilePath());
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            size_t posLink = line.find("Link: ");
            size_t posUsername = line.find("Username: ", posLink + 6);
            size_t posPassword = line.find("Password: ", posUsername + 10);
            if (posLink != std::string::npos && posUsername != std::string::npos && posPassword != std::string::npos) {
                std::string link = line.substr(posLink + 6, posUsername - (posLink + 6));
                std::string username = line.substr(posUsername + 10, posPassword - (posUsername + 10));
                std::string password = line.substr(posPassword + 10);
                passwords.push_back("Link: " + link + " Username: " + username + " Password: " + password);
            }
        }
        file.close();
    }
}

void PasswordManager::savePasswords() {
    std::ofstream file(getPasswordsFilePath());
    if (file.is_open()) {
        for (const auto& password : passwords) {
            file << password << std::endl;
        }
        file.close();
    }
}

void PasswordManager::addPassword(const std::string& link, const std::string& username, const std::string& password) {
    std::string formattedPassword = "Link: " + link + " Username: " + username + " Password: " + password;
    if (std::find(passwords.begin(), passwords.end(), formattedPassword) == passwords.end()) {
        passwords.push_back(formattedPassword);
        savePasswords();
    }
}

void PasswordManager::showPasswords() {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Saved Passwords",
                                                   GTK_WINDOW(parentWindow),
                                                   GTK_DIALOG_MODAL,
                                                   "_OK", GTK_RESPONSE_OK,
                                                   "_Copy", GTK_RESPONSE_APPLY,
                                                   "_Delete", GTK_RESPONSE_REJECT,
                                                   "_Export", GTK_RESPONSE_ACCEPT,
                                                   NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    if (passwords.empty()) {
        GtkWidget *label = gtk_label_new("No saved passwords.");
        gtk_container_add(GTK_CONTAINER(content_area), label);
    } else {
        if (passwordListBox != NULL) {
            gtk_widget_destroy(GTK_WIDGET(passwordListBox));
        }

        passwordListBox = GTK_LIST_BOX(gtk_list_box_new());

        for (const auto& entry : passwords) {
            GtkWidget *row = gtk_list_box_row_new();
            GtkWidget *label = gtk_label_new(entry.c_str());
            gtk_container_add(GTK_CONTAINER(row), label);
            gtk_list_box_insert(passwordListBox, row, -1);
        }

        gtk_container_add(GTK_CONTAINER(content_area), GTK_WIDGET(passwordListBox));
    }

    g_signal_connect(dialog, "response", G_CALLBACK(+[](GtkWidget *dialog, gint response_id, gpointer user_data) {
        PasswordManager* manager = static_cast<PasswordManager*>(user_data);
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

        if (response_id == GTK_RESPONSE_APPLY) {
            GtkListBox *list_box = GTK_LIST_BOX(manager->passwordListBox);
            GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(list_box);
            if (selected_row != NULL) {
                GtkWidget *label = gtk_bin_get_child(GTK_BIN(selected_row));
                if (GTK_IS_LABEL(label)) {
                    const char* passwordToCopy = gtk_label_get_text(GTK_LABEL(label));
                    if (passwordToCopy != NULL) {
                        GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
                        gtk_clipboard_set_text(clipboard, passwordToCopy, -1);
                    }
                }
            }
        } else if (response_id == GTK_RESPONSE_REJECT) {
            GtkListBox *list_box = GTK_LIST_BOX(manager->passwordListBox);
            GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(list_box);
            if (selected_row != NULL) {
                GtkWidget *label = gtk_bin_get_child(GTK_BIN(selected_row));
                if (GTK_IS_LABEL(label)) {
                    const char* passwordToRemove = gtk_label_get_text(GTK_LABEL(label));
                    if (passwordToRemove != NULL) {
                        manager->removePassword(passwordToRemove); // Remove password from vector
                        gtk_widget_destroy(GTK_WIDGET(selected_row)); // Remove row from list
                        manager->updatePasswordList(); // Update the display of the password list

                        // Check if there are no passwords left
                        if (manager->getPasswords().empty()) {
                            // If there are no passwords left, update the dialog content
                            GtkWidget *label = gtk_label_new("No saved passwords.");
                            gtk_container_add(GTK_CONTAINER(content_area), label);
                            gtk_widget_show_all(content_area); // Update visibility
                        }
                    }
                }
            }
        } else if (response_id == GTK_RESPONSE_ACCEPT) { // Handle export
            manager->exportPasswords();
        } else {
            gtk_widget_destroy(dialog);
        }
    }), this);

    gtk_widget_show_all(dialog);
}

void PasswordManager::updatePasswordList() {
    GList *children = gtk_container_get_children(GTK_CONTAINER(passwordListBox));
    for (GList *iter = children; iter != NULL; iter = iter->next) {
        GtkWidget *child = GTK_WIDGET(iter->data);
        gtk_widget_destroy(child); // Remove widget from container
    }
    g_list_free(children); // Free the list

    for (const auto& entry : passwords) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *label = gtk_label_new(entry.c_str());
        gtk_container_add(GTK_CONTAINER(row), label);
        gtk_list_box_insert(passwordListBox, row, -1);
    }

    gtk_widget_show_all(GTK_WIDGET(passwordListBox)); // Cast to GtkWidget*
}

void PasswordManager::removePassword(const std::string& passwordToRemove) {
    auto it = std::find(passwords.begin(), passwords.end(), passwordToRemove);
    if (it != passwords.end()) {
        passwords.erase(it); // Remove password from vector
        savePasswords(); // Save changes to the file
    }
}

void PasswordManager::exportPasswords() {
    std::string homeDir = getenv("HOME"); // Get home directory path
    std::ofstream file(homeDir + "/passwords_export.txt"); // Create a visible export file

    if (file.is_open()) {
        for (const auto& password : passwords) {
            file << password << std::endl; // Write each password to the file
        }
        file.close();

        // Create a new dialog for successful export notification
        GtkWidget *messageDialog = gtk_message_dialog_new(GTK_WINDOW(parentWindow),
                                                          GTK_DIALOG_MODAL,
                                                          GTK_MESSAGE_INFO,
                                                          GTK_BUTTONS_OK,
                                                          "Passwords successfully exported to passwords_export.txt");

        g_signal_connect(messageDialog, "response", G_CALLBACK(gtk_widget_destroy), NULL); // Destroy dialog on OK press
        gtk_dialog_run(GTK_DIALOG(messageDialog)); // Show dialog
    } else {
        GtkWidget *errorDialog = gtk_message_dialog_new(GTK_WINDOW(parentWindow),
                                                         GTK_DIALOG_MODAL,
                                                         GTK_MESSAGE_ERROR,
                                                         GTK_BUTTONS_OK,
                                                         "Failed to open file for export.");

        g_signal_connect(errorDialog, "response", G_CALLBACK(gtk_widget_destroy), NULL); // Destroy dialog on OK press
        gtk_dialog_run(GTK_DIALOG(errorDialog)); // Show dialog
    }
}

void PasswordManager::setMaxPasswordLength(int length) {
   if(length > 0 && length <= 32){
       maxPasswordLength = length; // Update the max password length variable
   }
}
