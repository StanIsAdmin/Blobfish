# Blobfish
An encryption/decryption protocol inspired from enigma

## But et principes de base

Le but de mon implémentation est d'encoder tout type de fichier. Il ne suppose donc pas un format défini. Pour ce faire, il considère tout fichier à encoder comme une suite d'octets, ou encore de nombres allant de 0 à 255. Une fois le fichier chargé en mémoire dans sa totalité, chaque octet est encodé suivant l'algorithme qui sera décrit ci-après. Le processus est répété un certain nombre de fois en modifiant le mot de passe, puis le résultat est écrit sur le disque.

L'algorithme de cryptage utilise un mot de passe relativement compliqué, constitué d'une chaîne de 32 entiers allant de 0 à 31. Tous les entiers de l'intervalle doivent y être représentés une et une seule fois. Dans le cas contraire, l'algorithme ne sera pas réversible. Pour faciliter la mémorisation de ce mot de passe, on pourra dans le futur envisager de le construire à partir d'une chaîne de caractères simple. La première phrase d'un roman ou quelques vers pourraient alors être utilisés pour l'encodage.

Entre les différentes passes d'encodage, le mot de passe utilisé est modifié selon un algorithme simple connu depuis l'Antiquité. On commence par prendre le premier entier (s'il vaut 0, on prend le second) et on l'ajoute à chacun des autres. Ensuite, pour s'assurer de conserver des nombres inférieurs à 32, on utilise sur chaque nombre l'opérateur modulo.

## Intermède mathématique

Pour bien comprendre ce qui suit, il est nécessaire de maîtriser quelques notions d'algèbre concernant les permutations. Ceux ici à qui la notion mathématique de groupe dit quelque chose maîtrisent le sujet et peuvent sans souci passer cette partie.

Soit un ensemble E d'éléments distincts, et soit f : E → E une fonction agissant sur E. On peut utiliser f pour crypter (et par après retrouver l'original) si et seulement si elle est inversible. On dit d'une telle fonction qu'elle est une permutation sur E.
On peut représenter une permutation de différentes manières. La plus intuitive est d'utiliser un tableau reprenant sur une ligne les valeurs de départ, sur l'autre les valeurs d'arrivée :

| A | B | C | D |
|---|---|---|---|
| D | B | A | C |

Ici A est envoyé sur D, B est conservé, C est envoyé sur A et D sur C. On voit immédiatement l'inconvénient d'une telle représentation : elle prend de la place sur notre feuille. C'est pourquoi on a imaginé la décomposition en cycles, une autre manière d'écrire une permutation, plus compacte : (ADC) (B). Pour la lire, il suffit de considérer que chaque élément est envoyé sur celui qui le suit immédiatement. Quand on atteint la fin d'une parenthèse, on revient au début, ce qui fait que C est bien envoyé sur A, comme précédemment. On omet enfin souvent les éléments conservés, de sorte qu'il suffit en fait d'écrire (ADC).

On peut effectuer diverses opérations sur les permutations. La plus courante est la composition, qui permet de synthétiser l'application de plusieurs permutations en une seule. Elle est symbolisée par l'opérateur ∘ (rond). Toute composition de permutations est une permutation (c'est évident, mais ce sera d'une importance capitale par la suite, donc je le mentionne).

### En mémoire

Les permutations utilisées par l'algorithme portent sur des nombres de 0 à 31. Chacune est représentée par un tableau de 32 entiers, le premier élément étant l'image de 0, le second de 1 et ainsi de suite. Comme les tableaux permettent un accès peu coûteux à un élément arbitraire, la résolution de l'image d'un nombre est extrêmement rapide.

### Paire-mutations

Sur base du mot de passe, on va créer un ensemble de paires - appelons ainsi les permutations échangeant deux éléments et laissant les autres inchangés. Leur génération est très simple : pour former la énième paire, on associe le énième nombre du mot de passe avec le 32 - énième. Les paires ensuite composées pour former une unique permutation réutilisable plus tard - appelons-la P.
D'autres permutations entrent dans la composition de l'algorithme : elles associent à chaque nombre x le nombre x + n modulo 32 (n allant de 0 à 31). Appelons celles-ci les permutations Cn (C pour César, de qui l'histoire dit qu'il fut le premier à utiliser ce mécanisme de cryptage).

Le mécanisme de cryptage à proprement parler n'utilise pas directement P et les Cn. Il utilise plutôt les composées Qn = P ∘ Cn ∘ P. Celles-ci sont donc calculées à chaque changement de mot de passe et stockées dans un tableau, Q0 à la position 0, Q1 à la position 1 et ainsi de suite.

### Rotors

Il me reste un dernier objet à présenter avant de pouvoir crypter : le rotor. Un rotor est constitué de trois nombres, sa "valeur" étant la somme des trois. Il possède également un maximum, que ni les trois nombres ni la valeur ne peuvent égaler ou dépasser.
Les rotors supportent une seule opération : l'incrémentation. Elle est construite ainsi : tout d'abord, on ajoute un au premier nombre (et à la valeur). S'il atteint le maximum, on le remet à zéro et on ajoute un au second nombre. Si celui-ci atteint également le maximum, on procède de même sur le troisième nombre. Quand ce dernier est maximum, il est simplement remis à zéro.
Deux rotors sont utilisés par le mécanisme d'encodage : un rotor à types, qui a pour maximum huit, et un rotor à valeurs, avec pour maximum 32. Le rotor à types est initialisé en fonction de l'écart entre les nombres 0 et 31, 1 et 30 et 2 et 29 dans le mot de passe ; le rotor a valeurs commence sa vie à (0, 0, 0).

## Encodage

Chaque passe d'encodage commence par le calcul des Qn et l'initialisation des rotors pour le nouveau mot de passe. Ensuite, l'encodage se fait séquentiellement, en commençant au début du fichier et en allant vers la fin.
Pour chaque octet x, on procède ainsi :
1) On pose r = x modulo 32, t = x / 32 (division entière) ;
2) On demande au rotor à valeurs sa valeur v, et on prend n, le v-ième nombre dans le mot de passe ;
3) On pose s = Qn(r) ;
4) On ajoute 32 × (rotor à type + t) à r ;
5) On vérifie que r est toujours plus petit que 256 ;
6) On incrémente les deux rotors.

## Développements futurs
### Décryptage
Ça serait pas mal de vérifier que l'algorithme fonctionne en décryptant. :-)

### Exécution parallèle

L'exécution en parallèle du traitement permettrait de multiplier les performances par le nombre de cœurs, ce qui n'est pas négligeable sur les ordinateurs actuels. La programmation parallèle pourrait également conduire à la possibilité d'utiliser OpenCL pour exécution sur la carte graphique - si toutefois c'est possible.

Je propose pour ce faire de découper le fichier en blocs logiques de quelques dizaines d'octets (la taille exacte devrait être déterminée par essais et erreurs). Chaque bloc ne pourrait être utilisé que par un thread à la fois (utilisation de mutexes).

La fonction principale déclencherait l'exécution des threads les uns après les autres (avec un nombre maximum configurable), chacun étant responsable d'une passe (et donc initialisé avec le mot de passe correct). Ils se livreraient une course effrénée vers la fin du fichier (sans pouvoir se dépasser afin de préserver l'ordre des passes).

Une fois qu'un thread aurait terminé de travailler, il le ferait savoir à la fonction principale (comment ?) qui lui attribuerait une nouvelle passe et le relancerait, réutilisant les threads pour éviter d'appeler trop souvent les fonctions système qui les créent (elles sont peut-être lentes).

### Génération du mot de passe
Le mot de passe dans son état actuel est complètement nul. Retenir dans l'ordre une suite de 32 nombres n'est pas du tout simple, et résulterait en des mots de passe peu nombreux comme 0 1 2 3..., ce qui est dangereux pour l'utilisateur. Voici un essai pour trouver un algorithme qui transforme toute chaîne de caractères suffisamment longue en mot de passe viable. Il travaille sur des caractères Unicode afin de permettre à tous d'utiliser un texte dans leur langue.
1) Déterminer tous les caractères utilisés ;
2) Vérifier si l'on a au moins 32 caractères différents. Si c'est le cas, passer à l'étape 5 ;
3) Remplacer le premier caractère répété par un autre, plus grand que le plus grand actuellement utilisé ;
4) Revenir à l'étape 1 ;
5) Supprimer les deuxième, troisième, ... occurrences de chaque caractère ;
6) Tronquer la chaîne pour n'avoir plus que 32 caractères ;
7) Remplacer le caractère ayant la plus petite valeur Unicode par 0, le suivant par 1, etc dans l'ordre du standard.

