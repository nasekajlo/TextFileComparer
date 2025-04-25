ZPR PROJECT

# TextFileComparer – Instrukcja instalacji i konfiguracji

## 📦 Wymagania wstępne

1. **Instalacja Qt 6.9.0**

   Należy pobrać i zainstalować Qt 6.9.0 z [oficjalnej strony Qt](https://www.qt.io/qt-educational-license#application).

2. **Instalacja Visual Studio Build Tools**

   Należy pobrać i zainstalować [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/). Podczas instalacji należy wybrać komponenty:

   - **Desktop development with C++**
   - **MSVC v142 - VS 2019 C++ x64/x86 build tools**
   - **Windows 10 SDK**

   Po instalacji należy uruchomić odpowiedni **Developer Command Prompt for VS**, aby ustawić zmienne środowiskowe dla kompilatora MSVC.

3. **Instalacja vcpkg**

   Należy pobrać i zainstalować [vcpkg](https://github.com/microsoft/vcpkg):

   ```powershell
   git clone https://github.com/microsoft/vcpkg
   .\vcpkg\bootstrap-vcpkg.bat
   ```

4. **Instalacja zależności**

   Należy zainstalować wymagane biblioteki:

   ```powershell
   .\vcpkg\vcpkg install boost
   .\vcpkg\vcpkg install icu
   ```

   Po instalacji vcpkg należy zintegrować go z systemem:

   ```powershell
   .\vcpkg\vcpkg integrate install
   ```

   Dzięki temu vcpkg będzie automatycznie wykrywał zainstalowane biblioteki podczas konfiguracji projektu.

## 🛠️ Konfiguracja projektu

W pliku `CMakeLists.txt` należy ustawić odpowiednie ścieżki do zainstalowanych bibliotek:

```cmake
cmake_minimum_required(VERSION 3.10)

project(TextFileComparer)

# Qt6
set(Qt6_DIR "C:/Qt/6.9.0/mingw_64/lib/cmake/Qt6")
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Boost
set(Boost_ROOT "C:/vcpkg/installed/x64-windows")
find_package(Boost REQUIRED)

# ICU
set(ICU_ROOT "C:/vcpkg/installed/x64-windows")
find_package(ICU REQUIRED)

add_executable(TextFileComparer
    src/file_parser.cpp
    src/comparer.cpp
    src/matcher.cpp
    src/gui.cpp
    src/merger.cpp
)

target_link_libraries(TextFileComparer
    Qt6::Widgets
    Boost::Boost
    ICU::ICU
)

enable_testing()
add_subdirectory(tests)

```

**Uwaga:** Jeśli używasz MinGW lub innego kompilatora, należy dostosować ścieżki i ustawienia w pliku `CMakeLists.txt` odpowiednio do środowiska.

## 🧱 Budowa projektu

1. Należy utworzyć katalog `build` i przejść do niego:

   ```powershell
   mkdir build
   cd build
   ```

2. Należy skonfigurować projekt za pomocą CMake:

   ```powershell
   cmake ..
   ```

3. Należy zbudować projekt:

   ```powershell
   cmake --build .
   ```
