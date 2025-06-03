## Użycie

1. Uruchom program `TextFileComparer`. Otworzy się główne okno aplikacji.
2. Wybierz dwa pliki tekstowe do porównania, korzystając z przycisków **"Wybierz..."** obok pól "Plik 1" i "Plik 2". Ścieżki wybranych plików pojawią się w polach tekstowych.
3. Kliknij przycisk **"Porównaj"**. Program wczyta pliki i wyświetli ich porównanie w tabeli poniżej.
4. Przejrzyj wyświetlone różnice:
   - Tekst obecny tylko w lewym pliku jest pokazany po lewej stronie na czerwono, przekreślony.
   - Tekst obecny tylko w prawym pliku jest pokazany po prawej stronie na zielono, podkreślony.
   - Tekst zmieniony (w obu plikach, ale różniący się) jest pokazany po obu stronach: usunięte fragmenty na czerwono (lewa kolumna), dodane fragmenty na zielono (prawa kolumna).
   - Tekst przeniesiony jest oznaczony kolorem niebieskim i opisem "(przeniesiony)" – pojawia się jako usunięty w starej lokalizacji i dodany w nowej.
   - Tekst niezmieniony jest wyświetlany na szaro (dla kontekstu) w obu kolumnach.
5. Przy każdej wykrytej zmianie znajduje się przycisk **"Akceptuj"** w trzeciej kolumnie. Kliknięcie go spowoduje:
   - W przypadku fragmentu dodanego (zielonego) – dodanie go do wyniku (przycisk zmieni się na "Zaakceptowano").
   - W przypadku fragmentu usuniętego (czerwonego) – usunięcie go w wyniku.
   - W przypadku fragmentu zmodyfikowanego – zastąpienie wersji lewej wersją prawą.
   - W przypadku fragmentu przeniesionego – usunięcie go ze starego miejsca **i** wstawienie w nowe (wystarczy zaakceptować jeden z dwóch powiązanych fragmentów, a drugi zostanie automatycznie zaakceptowany).
   Po zaakceptowaniu zmiany przycisk staje się nieaktywny, co oznacza, że decyzja została podjęta.
6. Jeśli nie chcesz uwzględniać jakiejś zmiany, pozostaw odpowiedni przycisk niekliknięty (niezaakceptowany). Domyślnie wszystkie zmiany są pominięte, co oznacza zachowanie oryginalnej treści z lewego pliku.
7. Po przejściu przez wszystkie różnice kliknij **"Zapisz wynik"**. Wybierz lokalizację i nazwę pliku, do którego ma zostać zapisany scalony tekst.
8. Jeśli zapis się powiedzie, zobaczysz komunikat potwierdzający (**"Plik wynikowy został zapisany pomyślnie."**). Możesz teraz otworzyć ten plik, aby zobaczyć połączony dokument.

## Wymagania i kompilacja

Do kompilacji projektu wymagane są:
- Kompilator zgodny z C++17.
- Biblioteka Qt5 (moduł Widgets do GUI).
- Biblioteka ICU (International Components for Unicode) do obsługi Unicode.
- Biblioteka Boost (wykorzystane nagłówkowo do wyjątków i smart pointerów).
- Google Test (opcjonalnie, jeśli chce się uruchomić testy jednostkowe).

Na systemie Ubuntu można zainstalować potrzebne pakiety poleceniem:

# TextFileComparer – Instrukcja instalacji i konfiguracji

## 📦 Wymagania wstępne

1. **Qt 6.9.0**  
   Zainstaluj Qt 6.9.0 z [oficjalnej strony Qt](https://www.qt.io/qt-educational-license#application).

2. **Biblioteki systemowe (Linux/Ubuntu)**  
   Zainstaluj wymagane biblioteki:
   ```bash
   sudo apt-get update
   sudo apt-get install libboost-all-dev libicu-dev
   ```

3. **Zmienne środowiskowe**  
   Aby umożliwić CMake i kompilatorowi wykrycie bibliotek Qt oraz ICU, dodaj do pliku `~/.bashrc` (lub `~/.zshrc`):
   ```bash
   # Qt
   export CMAKE_PREFIX_PATH="$HOME/Qt/6.9.0/gcc_64"

   # ICU
   export ICU_ROOT=/usr
   export ICU_LIBRARIES="/usr/lib/x86_64-linux-gnu/libicuuc.so;/usr/lib/x86_64-linux-gnu/libicui18n.so;/usr/lib/x86_64-linux-gnu/libicudata.so"
   ```

   Następnie załaduj zmiany:
   ```bash
   source ~/.bashrc
   ```

## ⚙️ Konfiguracja VS Code

Aby VS Code wiedział o używanym kompilatorze, bibliotekach i Qt:

1. W katalogu projektu utwórz katalog do generowania pliku `compile_commands.json`:
   ```bash
   mkdir cmake_compile_commands
   cd cmake_compile_commands
   cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
   ```

2. W pliku `.vscode/settings.json` dodaj:
   ```json
   {
       "C_Cpp.default.compileCommands": "${workspaceFolder}/cmake_compile_commands/compile_commands.json"
   }
   ```

## 🛠️ Budowa projektu

1. Utwórz katalog `build` i przejdź do niego:
   ```bash
   mkdir build
   cd build
   ```

2. Skonfiguruj projekt:
   ```bash
   cmake ..
   ```

3. Zbuduj projekt:
   ```bash
   cmake --build .
   ```

4. Uruchom testy (opcjonalnie):
   ```bash
   ctest
   ```

## 📄 Uruchomienie aplikacji

Po zbudowaniu, w katalogu `build/src/` znajduje się wykonywalny plik `TextFileComparer`. Uruchom go:
```bash
./TextFileComparer
```

Miłego używania!
