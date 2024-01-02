<div class="center">

# !!ATTENTION!!

# Avant de commencer quoi que ce soit, ajoutez vos noms et matricules dans `names.txt`! Vérifiez avec `name_validator.py` que le format est ok!

# NE MODIFIEZ RIEN DANS LE DOSSIER `test`! SI NOUS DÉTECTONS UNE MODIFICATION, VOUS AUREZ LA NOTE 0

# TP1: Shell

</div>  

# Préambule

Le shell est une composante essentielle de tout système Unix. Il est le point de contact
entre le programmeur et le système d'exploitation.

Sans le savoir, à chaque fois que vous utilisez un programme d'exploration de
fichiers (comme "File Explorer" sur Windows), des commandes shell tout à fait
accessibles à l'utilisateur sont appelées en arrière-plan.

# Introduction

Ce TP vise à vous familiariser avec la programmation système dans un
système d’exploitation de style POSIX.

Vous devrez implémenter un terminal/shell typique Linux. Comme tous les
autres shell, il devra pouvoir appeler des commandes linux typiques
(comme `ls` ou `cat`). Il devra aussi implémenter un sous-ensemble de
commandes bash pour agencer le lancement de plusieurs programmes en même
temps.

Ce TP sera moins explicite sur le *comment* implémenter les menues
fonctions qui permettront de compléter le travail. En effet, c’est ce
*comment* qui est la difficulté du travail, pas les petites fonctions.

Les fonctions proposées et les squelettes donnés dans `main.c` sont totalement optionnels.
Si votre shell fait ce qui est demandé, vous aurez tous les points, que vous ayez utilisé le code donné
ou non.

**Attention!** Comme c’est une fonctionnalité si standard d’un OS, il
existe des fonctions déjà implémentées en C qui feraient essentiellement
tout ce TP pour vous. Dès que nous trouverons une telle fonction dans
votre remise, vous aurez la note de zéro.

# Où coder votre shell?

Notez qu'il n'y a pas de balise de regex dans ce TP (surnommé "Jeff" par vos 
magnifiques TPistes). Ceci implique que votre fonction `main` __***doit***__ avoir
le comportement attendu, et appeler votre shell correctement. Pour la remise, vous ne pouvez y placer
des tests ou d'autres artéfacts.

# 0. Exit

Avant de faire quoi que ce soit, prenez note de la fonctionnalité "exit" de votre shell.
Si l'utilisateur entre `exit` dans votre shell, votre shell DOIT sortir, avec le code de sortie `0`.

# **SI VOTRE SHELL NE SUPPORTE PAS LA COMMANDE `exit`, VOUS AUREZ AUTOMATIQUEMENT 0.**

La commande `exit` n'est jamais combinée à d'autres commandes. Elle est toujours appelée seule, sans 
opérateurs comme rN, `&&`, `||`, `&`.
https://github.com/IFhttps://classroom.github.com/a/Yp7AcqEnT2245/tpx-Velythyl
Vous pouvez directement utiliser ce code pour vous aider:

    char *line;
    readline(&line);
    if(strcmp(line, "exit") == 0) {
        free(line);
        exit(0);
    }

Nous avons inclus une implémentation très basique de `readline` dans votre
code, ainsi que le bloc de code ci-dessus. Autrement dit, `exit` est déjà
implémenté dans votre shell: vous devez seulement vous assurer que la fonctionnalité
ne brise pas avec le code que vous allez ajouter.

# 1. Exec

Commençons par l’essentiel. Lorsqu’on lance votre shell, on veut pouvoir
exécuter un seul programme.

Donc, lorsqu’on appelle votre shell, on doit pouvoir ensuite appeler
`ls` ou `cat` ou `UneCommandeQuiNexistePas`. Lorsque la commande est valide, elle doit
imprimer la sortie attendue. Lorsque la commande n’est pas valide, elle
doit imprimer `<la commande>: command not found` (<la commande>
étant la commande qui n'as pas été trouvée).

Ceci implique bien sûr d’implémenter une ligne de commande qui lit des
caractères jusqu’à ce qu’on tape ENTER.

## readline

Implémentez la fonction readline, qui lit une ligne entière de la ligne
de commande.

Ensuite, vous devrez utiliser la fonction `execvp` pour exécuter la
commande lue par `readline`. Remarquez que `execvp` est une fonction
assez violente: elle écrase le processus en cours. Indice: utilisez un
`fork` judicieusement...

Notez que l’exécution séquentielle de plusieurs lignes devrait être
supportée. Si l’usager entre `sleep 10` et `echo a` un après l’autre, le
shell devrait correctement attendre 10 secondes, puis imprimer "a".

# 2. ET et OU

Ensuite, on aimerait que le shell soit capable d’exécuter plusieurs
commandes, ou même d’exécuter des commandes conditionnellement.

C’est le but des opérateurs `&&` et `||`.

`&&`: Lorsqu’on demande `echo Salut && echo Allo`, le shell devrait
imprimer Salut et Allo, sur deux lignes différentes. C’est le cas
puisque echo a bien réussi son exécution, donc `&&` passe à la prochaine
commande. Cependant, si on demande
`CeciNEstPasUneCommandeMyDude && echo allo`, on ne recevrait que l’usuel
`CeciNEstPasUneCommandeMyDude: command not found` en sortie.

`||`: Lorsqu’on demande `echo Salut || echo Allo`, le shell devrait
imprimer Salut, et c’est tout. C’est le cas puisque `||` a vu que la
dernière commande a réussi, donc il arrête l’exécution. Si la première
commande avait échoué, il aurait passé à la deuxième.

Ici, on ne peut vous donner de sous-question. Bien sûr, vous devrez
probablement briser la ligne lue en plus petits blocs. Vous devrez
ensuite extraire les symboles propres au shell que vous construisez (jusqu'à cette question-ci, c'est seulement "&&" et "||"). Et ensuite écrire une fonction qui
chaîne ou arrête l’appel des commandes selon ces symboles. Mais le
*comment* est une grande partie de la difficulté de ce TP: vous devrez
trouver vous-même.

## Particularités:

Il est fortement recommandé de jouer dans un vrai terminal bash pour comprendre
le fonctionnement de ces deux commandes. Notez, par exemple, 
que `echo a && aaaah || echo c` imprime
```
a
aaaah: command not found
c
```
# 3. Arrière-plan

Lorsqu’on met `&` à la fin d’une ligne, votre shell doit placer la ligne
entière en arrière-plan. Donc, si on appelle `sleep 10 && echo a &` et
tout de suite après, `echo b`, b devrait imprimer avant a.

Il n’y a pas de limite sur le nombre de lignes mises en arrière-plan.
Mais chaque ligne s’exécute toujours séquentiellement (du point de vue
de cette ligne-là).

Vous pouvez prendre pour acquis que tous les "&" seront en fin de ligne.

Vous ne **devez pas** écrire le “[1] PID” qui est normalement
imprimé dans un vrai shell.

Indice: vous pouvez utiliser `fork` avant l’exécution des lignes
appropriées...

# 4. Commandes fictive: rN 

`rN`: Ceci change la syntaxe du shell comme suit:
`rN(<commande X, possiblement composée de plusieurs mots>) ->` exécute la commande N fois.

Donc: 
- `r3(echo a)` appelle trois fois `echo a`.
- `r3(echo a) && agkeag || echo c` appelle trois fois `echo a`, puis imprime le message d'erreur pour `agkeag`, puis appelle `echo c`
- `r3(echo a) && echo b || echo c` appelle trois fois `echo a`, puis appelle `echo b`, puis arrête.
- `echo a && r400(agkeag) || echo c` appelle `echo a`, 
  puis imprime 400 fois le message d'erreur pour `agkeag`, puis appelle `echo c`

Le code de retour de rN est le code de retour de la dernière fonction que rN
a exécuté (autrement dit, si lors de `r40(<command>)`, seulement le 40e appel échoue, r40 échoue quand même pour les fins de `&&` et `||`).

Le N dans rN peut être n'importe quel entier positif (entrant dans un int).

## Barème et remise

-   Votre note sera divisé équitablement entre chaque question, sauf la question 0. Cette dernière
vaut 0 points, mais ne pas la faire entraîne une note de 0. Il faut la faire avant toute chose.
- Les barèmes standards du [TPX](https://github.com/IFT2245/TPX) s'appliquent (fuites mémoires, accès illégaux, etc).
