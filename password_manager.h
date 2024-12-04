#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include <gtk/gtk.h>
#include <string>
#include <vector>

class PasswordManager {
public:
    PasswordManager(GtkWidget* parent);
    void loadPasswords();
    void savePasswords();
    void addPassword(const std::string& link, const std::string& username, const std::string& password);
    void showPasswords();
    void updatePasswordList();
    void removePassword(const std::string& passwordToRemove);
    void exportPasswords();
    void setMaxPasswordLength(int length); // Setter for max password length
    std::vector<std::string> getPasswords() const { return passwords; }

private:
    GtkWidget* parentWindow;
    GtkListBox* passwordListBox;
    std::vector<std::string> passwords;
    int maxPasswordLength; // Variable to store max password length
};

#endif // PASSWORD_MANAGER_H
