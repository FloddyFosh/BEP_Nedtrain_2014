# BEP NedTrain 2014
Onze Jenkins server is te vinden op: [77.163.248.47](http://77.163.248.47)

## Mappen
 - instances - de aangeleverd instances die geladen kunnen worden in de tool
 - gui - alle code voor de gui
 - tms - alle code voor solvers
 - verslag - het verslag
 - logboek - [Anton](https://bitbucket.org/abouter/bep_nedtrain_2014/src/d5bc7935059c40d55cf8c549a31db8eff571e023/logboek/anton.md?at=master) - [Chris](https://bitbucket.org/abouter/bep_nedtrain_2014/src/d5bc7935059c40d55cf8c549a31db8eff571e023/logboek/chris.md?at=master) - [Martijn](https://bitbucket.org/abouter/bep_nedtrain_2014/src/6746af8b0530f56a795049dca60899f2c83bce00/logboek/martijn.md?at=master)

## Features TODO
 - Chaining zodat bij het verslepen van een taak er geen resource conflicts ontstaan.
 - Porten naar Windows
 - Undo functie, vooral handig is als je per ongeluk bij het slepen van een task de duration aanpast.
 - Zoom functie ook met ctrl+scroll en horizontaal scrollen shift+scroll
 - Sluit knopje op de tab zelf i.p.v. erboven. 
 - Nieuwe view voor resources met chains
 - De output op stdout en de debug op stderr.
 - Vergelijken van instanties met verschillende solvers (bijv. chaining met verschillende heuristieken), waarvan de flexibiliteit berekend wordt. Vervolgens kan de meest flexibele oplossing gekozen worden.

## Bugs
 - Precedence constraints, na het verwijderen van een constraint kan een task niet vrij bewegen. Mogelijke oplossing: bij de edit functie de earliest start time laten aanpassen. 
 - Pijltjes buiten groene vlakken blijven soms niet heel bij het verslepen van een task.
 - In een aantal invoer velden waar alleen getallen toegestaan zijn, kunnen wel punten (".") worden ingevoerd, maar deze worden later ineens verwijderd omdat het niet mag. 
 - Knoppen ineeens weg geen idee waarom, herstarten applicatie helpt niet.
 - Bij het toevoegen van constraints aan taken, kan het zijn dat de tool crashed met op de console: "The program has unexpectedly finished." Daarvoor komt nog in de GUI een melding dat de constraint niet toegevoegd kan worden. 

## Vragen
 - Wat doet de optie "Clear generated constraints before solving"?
