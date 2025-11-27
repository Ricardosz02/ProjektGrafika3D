# Retro-Shoot 3D

Retro-Shoot 3D to oldschoolowa strzelanka inspirowana klasykami lat 90., wykorzystująca raycasting 3D, system kluczy oraz walkę z hordami potworów i finałowym bossem.

## 🎮 Fabuła
Jesteś żołnierzem infiltrującym tajną bazę. Musisz przejść przez **5 poziomów**, eliminując potwory, zbierając klucze i odblokowując drogę naprzód.  
Na końcu czeka ostateczne starcie z bossem: **Mutagenicznym Kolosem**.

## 🧩 Mechaniki gry
- **Raycasting 3D** – tekstury ścian, podłoga/sufit z efektem mgły  
- **System kluczy** – 4 kolory kluczy → otwierają odpowiednie drzwi  
- **Arsenał broni**: pistolet, shotgun, rakietnica  
- **Przeciwnicy**: zombie, mutant (billboardowane modele 3D, AI: patrol / gonitwa)  
- **Boss końcowy** – 3 fazy, słabe punkty  
- **Strzelanie** – hitscan, efekty krwi i eksplozji  

## ⌨️ Sterowanie
| Klawisz | Akcja |
|--------|--------|
| ↑ | Przód |
| ↓ | Tył |
| ← | Obrót w lewo |
| → | Obrót w prawo |
| Spacja | Strzał |
| E | Zbierz / Otwórz |
| Shift | Sprint |
| ESC | Pauza |

## 🛠️ Technologia
- C++, OpenGL, GLFW  
- Obsługa wielu map z plików  
- Modele `.obj`  
- System cząsteczek  
- Autosejw  

## 🗺️ Plan poziomów

### 1. Wejście
- 3 zombie  
- Żółty klucz → drzwi → winda  
- Sekret: apteczka

### 2. Laboratoria
- 5 zombie + miniboss  
- Czerwony klucz → drzwi → winda  
- Pickup: shotgun

### 3. Korytarze
- Horda 8 zombie + pułapka  
- Niebieski klucz → drzwi → winda  
- Pickup: rakietnica

### 4. Reaktor
- Horda 12 zombie + miniboss  
- Zielony klucz → drzwi → winda  
- Pickup: amunicja

### 5. Arena Bossa – Mutageniczny Kolos
**Faza 1:** 5 zombie → 1 przycisk  
**Faza 2:** 8 zombie + spadające bloki → 2 przyciski  
**Faza 3:** 12 zombie + spadające bloki → 3 przyciski  

**Zwycięstwo:** koniec gry.
