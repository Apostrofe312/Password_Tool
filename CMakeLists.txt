cmake_minimum_required(VERSION 3.30)
project(untitled2)

set(CMAKE_CXX_STANDARD 20)

# Находим PkgConfig
find_package(PkgConfig REQUIRED)

# Проверяем наличие GTK+
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Создаем исполняемый файл
add_executable(untitled2 main.cpp password_manager.cpp password_manager.h
        settings.cpp
        settings.h)

# Указываем директории с заголовочными файлами для GTK+
target_include_directories(untitled2 PRIVATE ${GTK3_INCLUDE_DIRS})

# Связываем библиотеку GTK+ с вашим проектом
target_link_libraries(untitled2 PRIVATE ${GTK3_LIBRARIES})

# Выводим сообщение о статусе
if (GTK3_FOUND)
    message(STATUS "GTK+ 3 found: ${GTK3_VERSION}")
else()
    message(FATAL_ERROR "GTK+ 3 not found!")
endif()
