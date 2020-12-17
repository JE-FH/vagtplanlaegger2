# Vagtplanlægger til hospitaler
Denne readme vil forklare hvordan du bruger programmet og hvordan input data skal formateres.
Vi går ud fra at du har installeret gcc så du kan kompilere c programmer.

Lavet af: Gruppe A312a

## Compile programmet
### Windows
```cmd
gcc -O3 -ansi -Wall -pedantic main.c -o vagtplanlaegger.exe
```
### Linux og lignende
```bash
gcc -O3 -ansi -Wall -pedantic main.c -o vagtplanlaegger -lm
```

## Brug af programmet
Programmet har tre forskellige funktioner som er forklaret hver for sig


### Skab vagtplan
Skab vagtplan bruger vores genetiske algoritme til at skabe en vagtplan udfra en medarbejder liste. Den bruger altid filen `medarbejdere.csv` som input.

Derefter spørger programmet efter hvor mange medarbejdere der skal være på hver vagt.

Denne funktion bruger ikke nogle programparametre, derfor skal man bare køre exe filen i kommandoprompten for denne funktionalitet.

Efter vagtplanen er skabt, så bliver den gemt som `lavet-vagtplan.csv`


### Test vagtplan
Denne del af programmet tester en enkelt vagtplan ved hjælp af fitness funktionen. 

Ligesom skab vagtplan, så bruger den filen `medarbejdere.csv` som medarbejder listen

Udover dette, så tager den to program parametre, et kald ville se sådan her ud
```cmd
vagtplanlaegger.exe test <filnavn på vagtplan>
```
Hvor <filnavn på vagtplan> er navnet på den vagtplan du vil teste


### Print vagtplan
Denne funktion af programmet laver en pæn version af vagtplanen til hver medarbejder.

De pæne vagtplaner bliver gemt inde under `output/`, derfor så skal man lave den mappe før man kører programmet.

Denne funktion læser medarbejder listen fra filen `medarbejdere.csv`

Denne del tager også to programparametre, et kald ville se sådan her ud
```cmd
vagtplanlaegger.exe print <filnavn på vagtplan>
```
Hvor <filnavn på vagtplan> er navnet på den vagtplan du vil teste


## Format af input filerne
Alle filer er gemt som CSV, vores program kan indlæse både komma og semikolon som separator, dog så gemmer programmet alle filer med komma som separator.

### Medarbejder filen
Denne fil skal være en CSV fil som beskriver hver medarbejder, den skal indeholde disse kolonner i samme rækkefølge

```
Navn, Ønsket fridag, Ønsket vagt, UUID
```

Et eksempel på en medarbejder liste er inkluderet i filen `medarbejdere.csv`. Den indeholder 10 medarbejdere. Hver medarbejder skal have et unikt UUID for at programmet kan fungere korrekt

Navne må desuden ikke være længere end 50 bogstaver.

### Lavet vagtplan format
En lavet vagtplan består af 21 blokke, derfor så er en vagtplan også gemt som 21 rækker som hver repræsenterer en blok. Hver linje i vagtplanen bliver formateret på følgende måde
```
Dag, Vagt, navn1.uuid1, navn2.uuid2, navn3.uuid3, ..., $
```
Dag og Vagt bliver ignoreret af programmet, defor så skal blokkene komme i kronologisk rækkefølge

Et eksempel på sådan en fil kan findes i filen `vagtplan-eksempel.csv`. Den vagtplan virker kun med medarbejder listen `medarbejdere.csv` som der er gemt i forvejen.
