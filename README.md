# BEP NedTrain 2014
## Mappen
 - instances - de aangeleverd instances die geladen kunnen worden in de tool
 - gui - alle code voor de gui
 - tms - alle code voor solvers
 - verslag - het verslag
## Ideeen
 - Undo functie, vooral handig is als je per ongeluk bij het slepen van een task de 
duration aanpast.
 - Compare functie duidelijker maken, selecteer 2 solvers op de zelfde instantie en vergelijk o.a. de flexibilteit.
 - Windows
 - Bug precedence constraints, na het verwijderen van een constraint kan een task niet vrij bewegen. Mogelijke oplossing: bij de edit functie de earliest start time laten aanpassen. 
 - Pijltjes buiten groene vlakken blijven soms niet heel bij het verslepen van een task. 
 - Zoom functie ook met ctrl+scroll en horizontaal scrollen shift+scroll
 - in een aantal invoer velden waar alleen getallen toegestaan zijn, kunnen wel punten (".") worden ingevoerd, maar deze worden later ineens verwijderd omdat het niet mag. 
 - Sluit knopje op de tab zelf i.p.v. erboven. 
 - Knoppen ineeens weg geen idee waarom, herstarten applicatie helpt niet.
 - Nieuwe view voor resource chains
 - Chaining zodat bij het verslepen van een taak er geen resource conflicts ontstaan. 
 - Bij het toevoegen van constraints aan taken, kan het zijn dat de tool crashed met op de console: "The program has unexpectedly finished." Daarvoor komt nog in de GUI een melding dat de constraint niet toegevoegd kan worden. 
 - De output op stdout en de debug op stderr.
## Vragen
 - Wat doet de optie "Clear generated constraints before solving"?
