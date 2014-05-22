# BEP NedTrain 2014
Onze Jenkins server is te vinden op: [77.163.248.47](http://77.163.248.47)

## Mappen
 - instances - de aangeleverd instances die geladen kunnen worden in de tool
 - gui - alle code voor de gui
 - tms - alle code voor solvers
 - verslag - het verslag
 - logboek - [Anton](https://bitbucket.org/abouter/bep_nedtrain_2014/src/master/logboek/anton.md) | [Chris](https://bitbucket.org/abouter/bep_nedtrain_2014/src/master/logboek/chris.md) | [Martijn](https://bitbucket.org/abouter/bep_nedtrain_2014/src/master/logboek/martijn.md)

## Features TODO
 - Chaining zodat bij het verslepen van een taak er geen resource conflicts ontstaan.
 - Undo functie, vooral handig is als je per ongeluk bij het slepen van een task de duration aanpast.
 - Sluit knopje op de tab zelf i.p.v. erboven. 
 - Nieuwe view voor resources met chains
 - De output op stdout en de debug op stderr.
 - Vergelijken van instanties met verschillende solvers (bijv. chaining met verschillende heuristieken), waarvan de flexibiliteit berekend wordt. Vervolgens kan de meest flexibele oplossing gekozen worden.
 - Default solver na opstarten van de tool (chaining)
 
## Bugs
 - Precedence constraints, na het verwijderen van een constraint kan een task niet vrij bewegen. Mogelijke oplossing: bij de edit functie de earliest start time laten aanpassen. 
 - In een aantal invoer velden waar alleen getallen toegestaan zijn, kunnen wel punten (".") worden ingevoerd, maar deze worden later ineens verwijderd omdat het niet mag. 
 - Knoppen ineens weg geen idee waarom, herstarten applicatie helpt niet.
 - Bij het toevoegen van constraints aan taken, kan het zijn dat de tool crashed met op de console: "The program has unexpectedly finished." Daarvoor komt nog in de GUI een melding dat de constraint niet toegevoegd kan worden. 

## Vragen
 - Waar staat "tms" voor?
 - Waar staat "STJN" voor?