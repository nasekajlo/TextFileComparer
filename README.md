# Porównywarka i Merger Tekstu

Projekt dostarcza narzędzie graficzne do porównywania i scalania dwóch plików tekstowych, przeznaczone dla użytkowników nietechnicznych. Aplikacja pokazuje różnice między dwoma dokumentami i umożliwia zatwierdzanie lub odrzucanie zmian na poziomie akapitów i pojedynczych słów, a następnie zapisanie scalonego tekstu.

## Funkcjonalności

- **Porównywanie plików tekstowych:** Program porównuje dwa pliki tekstowe, wyświetlając je obok siebie i wyróżniając różnice.
- **Wykrywanie przeniesionych akapitów:** Fragmenty tekstu (akapit) obecne w obu plikach, ale w innej kolejności, są rozpoznawane jako przeniesione, a nie usunięte/dodane od nowa. W interfejsie oznaczono je kolorem niebieskim i etykietą "(przeniesiony)".
- **Ignorowanie różnic wyłącznie w złamaniach linii:** Jeśli jedyną różnicą między plikami jest inne rozmieszczenie tekstu w liniach (np. w jednym pliku zdanie jest podzielone na dwie linie, a w drugim jest w jednej linii), narzędzie nie potraktuje tego jako istotnej różnicy. Linie wewnątrz akapitu są łączone podczas porównania.
- **Granularna akceptacja zmian:** Użytkownik może zatwierdzać lub pomijać zmiany na poziomie pojedynczych akapitów, a nawet pojedynczych słów wewnątrz akapitu. Np. można dodać brakujący akapit z prawego pliku, usunąć zbędny fragment z lewego lub wybrać, którą wersję zmienionego zdania przyjąć.
- **Scalanie i zapis:** Po przejrzeniu różnic i zatwierdzeniu wybranych zmian, użytkownik może zapisać wynik scalania do nowego pliku tekstowego.

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

## Architektura i implementacja

**Parser plików:** Wykorzystuje bibliotekę ICU do poprawnego dzielenia tekstu na akapity i słowa, uwzględniając polskie znaki i Unicode. Każdy akapit jest identyfikowany na podstawie pustych linii (podwójne przejście do nowej linii). Pojedyncze przejścia do nowej linii nie tworzą nowego akapitu – treść jest łączona z poprzednią linią, co zapobiega oznaczaniu innego formatowania tekstu jako zmiany. Słowa (oraz znaki interpunkcyjne) są wyodrębniane za pomocą ICU BreakIterator i przechowywane w strukturze `Document`.

**Moduł porównujący (Diff):** Wykorzystuje algorytm najdłuższego wspólnego podciągu (Longest Common Subsequence) do znalezienia ciągów identycznych akapitów między dwoma dokumentami. Na tej podstawie określa, które akapity zostały dodane, usunięte lub zmodyfikowane. Usunięcie akapitu z jednego pliku i dodanie innego akapitu w tym samym miejscu drugiego pliku traktowane jest jako modyfikacja (zmiana akapitu) – łączy się to w jedno zgłoszenie różnicy, co ułatwia czytanie. Dla zmodyfikowanych akapitów dodatkowo znajduje się różnice na poziomie słów (ponownie używając LCS), aby wyróżnić dokładnie, które fragmenty zdania się różnią.

**Wykrywanie przeniesionych fragmentów:** Po wstępnym porównaniu narzędzie sprawdza, czy akapit oznaczony jako "usunięty" w jednym pliku pojawia się identyczny jako "dodany" w drugim (w innej pozycji). Jeśli tak, obie operacje są oznaczane jako "przeniesione" (przez co interfejs koloruje je na niebiesko i wiąże ze sobą). Dzięki temu użytkownik widzi, że np. cały akapit został przestawiony w kolejności, a nie usunięty i dodany niezależnie.

**Moduł scalania (Merger):** Odpowiada za zastosowanie decyzji użytkownika i wygenerowanie wynikowego dokumentu. Bazując na liście różnic wygenerowanej przez moduł Diff, Merger przechowuje dla każdej zmiany status "zaakceptowana/niezaakceptowana". Na tej podstawie podczas zapisu końcowego:
- Do wyniku trafiają wszystkie akapity oryginalnego lewego dokumentu, **z wyjątkiem** tych zmian, które użytkownik zaakceptował jako usunięte.
- Wszystkie akapity obecne tylko w prawym dokumencie zostają dodane **tylko jeśli** użytkownik je zaakceptował.
- Wszystkie zmienione akapity zostają w wyniku zastąpione wersją z prawego pliku **tylko jeśli** użytkownik zaakceptował zmianę; w przeciwnym razie pozostaje wersja z lewego pliku.
- Fragmenty przeniesione są usuwane ze starego miejsca i dodawane w nowe miejsce wyłącznie jeśli użytkownik zaakceptował przeniesienie (oba powiązane fragmenty muszą zostać zaakceptowane – program robi to automatycznie przy akceptacji jednego z nich). W przeciwnym razie pozostają na swoim pierwotnym miejscu.

Po zaznaczeniu decyzji wystarczy zapisać wynik – Merger generuje połączony dokument zgodnie z powyższymi zasadami i zapisuje go w kodowaniu UTF-8.

**Interfejs użytkownika:** Został wykonany w Qt5. Wszystkie elementy interfejsu (przyciski, etykiety, komunikaty) są w języku polskim. Różnice w tekście prezentowane są w tabeli, co umożliwia czytelne porównanie wiersz po wierszu (tutaj wiersze odpowiadają akapitom). Kolorowe wyróżnienia i oznaczenia (podkreślenie, przekreślenie) pozwalają intuicyjnie zrozumieć charakter zmiany:
- **Kolor zielony** – nowy tekst (dodatki z prawego pliku).
- **Kolor czerwony** – usunięty tekst (obecny tylko w lewym pliku).
- **Kolor niebieski** – tekst przeniesiony w inne miejsce.
- **Przekreślenie** – tekst, który został usunięty.
- **Podkreślenie** – tekst, który został dodany.
- **Szary kolor** – tekst niezmieniony (kontekst).

Przyciski "Akceptuj" przy każdej zmianie dają użytkownikowi pełną kontrolę nad scalaniem. W każdej chwili można zrezygnować z porównania lub wybrać inne pliki – program nie modyfikuje oryginalnych plików, działa tylko na kopiach w pamięci i zapisuje wynik do nowego pliku.

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
