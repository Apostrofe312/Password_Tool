#include <gtk/gtk.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "password_manager.h"
#include "settings.h" // Include the settings header

// Function declarations
std::string generatePassword(int length, const std::vector<char>& specialChars);
void on_generate_button_clicked(GtkWidget *widget, gpointer data);
void on_save_password_button_clicked(GtkWidget *widget, gpointer data);
void on_show_passwords_button_clicked(GtkWidget *widget, gpointer data);

// Function to generate a random password
std::string generatePassword(int length, const std::vector<char>& specialChars) {
    const std::string lowercase = "abcdefghijklmnopqrstuvwxyz";
    const std::string uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string digits = "0123456789";
    std::string characters = lowercase + uppercase + digits;

    if (!specialChars.empty()) {
        characters += std::string(specialChars.begin(), specialChars.end());
    }

    std::string password;
    for (int i = 0; i < length; ++i) {
        int index = rand() % characters.size();
        password += characters[index];
    }
    return password;
}

// Callback for the generate button
void on_generate_button_clicked(GtkWidget *widget, gpointer data) {
    GtkScale *lengthScale = GTK_SCALE(g_object_get_data(G_OBJECT(widget), "length_scale"));
    GtkEntry *resultEntry = GTK_ENTRY(g_object_get_data(G_OBJECT(widget), "result_entry"));
    GtkToggleButton *specialCharsToggle = GTK_TOGGLE_BUTTON(g_object_get_data(G_OBJECT(widget), "special_chars_toggle"));

    int length = gtk_range_get_value(GTK_RANGE(lengthScale));
    if (length <= 0 || length > 32) {
        gtk_entry_set_text(resultEntry, "Invalid input!");
        return;
    }

    std::vector<char> selectedSpecialChars;
    if (gtk_toggle_button_get_active(specialCharsToggle)) {
        selectedSpecialChars.push_back('!');
        selectedSpecialChars.push_back('@');
        selectedSpecialChars.push_back('#');
    }

    std::string password = generatePassword(length, selectedSpecialChars);
    gtk_entry_set_text(resultEntry, password.c_str());
}

// Callback for the save password button
void on_save_password_button_clicked(GtkWidget *widget, gpointer data) {
    PasswordManager* manager = static_cast<PasswordManager*>(data);
    GtkEntry* nameEntry = GTK_ENTRY(g_object_get_data(G_OBJECT(widget), "name_entry"));
    GtkEntry* manualEntry = GTK_ENTRY(g_object_get_data(G_OBJECT(widget), "manual_entry"));
    GtkEntry* purposeEntry = GTK_ENTRY(g_object_get_data(G_OBJECT(widget), "purpose_entry"));

    const char* name = gtk_entry_get_text(nameEntry);
    const char* password = gtk_entry_get_text(manualEntry);
    const char* purpose = gtk_entry_get_text(purposeEntry);

    if (name && name[0] != '\0' && password && password[0] != '\0' && purpose && purpose[0] != '\0') {
        manager->addPassword(purpose, name, password); // Call addPassword with three arguments

        // Clear input fields
        gtk_entry_set_text(manualEntry, "");
        gtk_entry_set_text(nameEntry, "");
        gtk_entry_set_text(purposeEntry, "");
    }
}

// Callback for the show passwords button
void on_show_passwords_button_clicked(GtkWidget *widget, gpointer data) {
    PasswordManager* manager = static_cast<PasswordManager*>(data);
    manager->showPasswords();
}

// Main function
int main(int argc, char *argv[]) {
    srand(static_cast<unsigned int>(time(0))); // Seed random number generator

    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(window), "Password Tool");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    GtkWidget *notebook = gtk_notebook_new();

	// Create a box to center the notebook
	GtkWidget *centerBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_box_pack_start(GTK_BOX(centerBox), notebook, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(window), centerBox);

	PasswordManager manager(window);

	// Password generator tab
	GtkWidget *genTab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

	GtkWidget *lengthLabel = gtk_label_new("Select password length:");

	GtkWidget *lengthScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 1, 32, 1);
	g_object_set_data(G_OBJECT(lengthScale), "max_length", GINT_TO_POINTER(32)); // Set max length for reference

	GtkWidget *specialCharsToggle = gtk_check_button_new_with_label("Use special characters");

	GtkWidget *resultLabel = gtk_label_new("Generated password:");

	GtkWidget *resultEntry = gtk_entry_new();
	gtk_editable_set_editable(GTK_EDITABLE(resultEntry), FALSE);

	// Button to generate password
	GtkWidget *generateButton = gtk_button_new_with_label("Generate");
	g_object_set_data(G_OBJECT(generateButton), "length_scale", lengthScale);
	g_object_set_data(G_OBJECT(generateButton), "result_entry", resultEntry);
	g_object_set_data(G_OBJECT(generateButton), "special_chars_toggle", specialCharsToggle);
	g_signal_connect(generateButton, "clicked", G_CALLBACK(on_generate_button_clicked), NULL);

	// Add all elements to the password generator tab
	gtk_box_pack_start(GTK_BOX(genTab), lengthLabel, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(genTab), lengthScale, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(genTab), specialCharsToggle, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(genTab), resultLabel, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(genTab), resultEntry, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(genTab), generateButton, FALSE, FALSE, 0);

	// Password manager tab
	GtkWidget *managerTab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

	GtkWidget *purposeEntry = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(purposeEntry), "Enter purpose (website)");

	GtkWidget *nameEntry = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(nameEntry), "Enter name (login)");

	GtkWidget *manualEntry = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(manualEntry), "Enter password to save");

	GtkWidget *savePasswordButton = gtk_button_new_with_label("Save Password");
	g_object_set_data(G_OBJECT(savePasswordButton), "name_entry", nameEntry);
	g_object_set_data(G_OBJECT(savePasswordButton), "manual_entry", manualEntry);
	g_object_set_data(G_OBJECT(savePasswordButton), "purpose_entry", purposeEntry);

	g_signal_connect(savePasswordButton, "clicked", G_CALLBACK(on_save_password_button_clicked), &manager);

	// Button to show saved passwords
	GtkWidget *showPasswordsButton = gtk_button_new_with_label("Show Saved Passwords");
	g_signal_connect(showPasswordsButton, "clicked", G_CALLBACK(on_show_passwords_button_clicked), &manager);

	// Add elements to the password manager tab
	gtk_box_pack_start(GTK_BOX(managerTab), purposeEntry, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(managerTab), nameEntry, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(managerTab), manualEntry, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(managerTab), savePasswordButton, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(managerTab), showPasswordsButton, TRUE, TRUE, 0);

	// Add the password manager tab to the Notebook
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), genTab,
		gtk_label_new("Password Generator"));
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
		managerTab,
		gtk_label_new("Password Manager"));

	// Settings tab
	GtkWidget *settingsTab = create_settings_tab(&manager); // Pass the manager instance
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
		settingsTab,
		gtk_label_new("Settings"));

	g_signal_connect(window,
		"destroy",
		G_CALLBACK(gtk_main_quit),
		NULL);

	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}
