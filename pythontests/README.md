# Testovací skript na projekt z IFJ

## Úvod

Základnou myšliekou tohto repozitára je vytvoriť čo najvačšiu databázu testov (automatický tester je iba bonus),
preto poprosím všetkých, ktorí si ho stiahnu aby pridali aspoň jeden test a tým pomohli s jeho rozšírením. 

Repozitár obsahuje:
 - ifj_test.py   -> testovací skript 
 - test_list.txt -> zoznam testov
 - .gitignore    -> vzor pre súbory ktoré sa nemajú pridávať
 - .signals      -> pomocný súbor, zoznam signálov
 - README.md     -> readme pre github
 - test/         -> priečinok s testom

## Súbory

### ifj_test.py
Na otestovanie je porovnávaný návratový kód programu a stdout.

Prebieha i kontrola pomocov valgrindu (valgrind --tool=memcheck --leak-check=full). Ukončenie signálom je tak isto zaznamenané a vypíše sa ako samostatný riadok.

Spúšťanie skriptu:
```
./ifj_test.py [BIN_PATH]

# [BIN_PATH] cesta k vašemu binárnemu súboru k projektu IFJ
```
Príklad výstupu:
```
---IFJ test script---

Test: "Lexikalny test 1"
  Return code: True
    Your return code: 1
    Real return code: 1
  Stdout: True
    stdout saved: ./logs/lex_test/lex_test_1.stdout
    stderr saved: ./logs/lex_test/lex_test_1.stderr
  Valgrind: ==11007== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
    valgrind saved: ./logs/lex_test/lex_test_1.valgrind
  Test OK
```

### test_list.txt
V súbore je zoznam testov. Prázdne riadky a riadky začínajúce "#" sú brané ako komentáre. Pred nový test (zoznam testov) je vhodné napísať krátky komentár.

Format suboru je :
```
 [NAME]\t[FILES_NAME]\t[RETURN_CODE]
     *[NAME]        -> Meno testu
     *[FILE_NAME]  -> Zaklad nazvov suborov (ak sa moje subory volaju
                                              test1.code a tes1.out,
                                              FILES_NAME je test1)
     *[RETURN_CODE] -> Navratova hodnota programu
```

## Testy
Pre každý test je potrebné vytvoriť 3 súbory .code, .in, .out:
 - .code -> kód v jazyku ifj16
 - .in -> stdin pre daný test (ak nechcete nič na vstup dajte iba prázdny súbor)
 - .out -> aký výstup očakávate

Do každého .code súboru je vhodné dopísať hlavičku s popisom programu a autorom.

Testi umiestnite do priečinka ./test/ . Vhodné je urobiť si vlastný priečinok (viď. nižšie).

Pokiaľ chcete umiestniť testy do priečinka je potrebné to zohľadniť i v test_list.txt a rovnaký priečinok vytvoriť aj v ./logs/ priečinku.

## Príklad vytvorenia a spustenia testu: 

Chcem si vytvoriť test1. 

Vytvorím pre neho súbory test1.code.

test1.code:
``` java
/*
* Môj testovací program
* autor: ja
*/
class Main
{
	static String ahoj;
	
	static void run()
	{
		ahoj =  = "pete
";
	}
}
```
kedže nepoužijem ani stdin ani stdout vytvorím prázdne súbory test1.in a test1.out.

Následne do súboru test_list.txt dopíšem riadok:
```
# Test syntax
Moj test test1 1
```
Spustím si skript:
```
./ifj_test.py ../ifj16
```
A dostanem výstup:
```
---IFJ test script---

Test: "Môj test"
  Return code: True
    Your return code: 1
    Real return code: 1
  Stdout: True
    stdout saved: ./logs/lex_test/lex_test_1.stdout
    stderr saved: ./logs/lex_test/lex_test_1.stderr
  Valgrind: ==13929== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
    valgrind saved: ./logs/lex_test/lex_test_1.valgrind
  Test OK

DONE, ok: 1 failed: 0
```
