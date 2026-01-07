<div align="center">

# 🌌 THE BLACK HOLE LOCKER

### *Ultimate Starfield Screen Locker*

![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![X11](https://img.shields.io/badge/X11-F28D1A?style=for-the-badge&logo=x.org&logoColor=white)

**Linux • X11 • Terminal • Low-Level Input Control**

<br/>

*Transformer une simple animation Starfield en un écran de verrouillage quasi infranchissable*

[Installation](#-installation) •
[Utilisation](#-utilisation) •
[Documentation X11](X11.md)

</div>

---

## 📖 Introduction

**The Black Hole Locker** (alias *Ultimate Starfield Locker*) est un projet expérimental écrit en **C**, né d'un défi précis :

> 🎯 Créer un écran de verrouillage capable de neutraliser les vecteurs d'évasion classiques  
> `Ctrl+C` · `Alt+F4` · `Alt+Tab` · *focus loss*  
> **sans droits administrateur** (no sudo)

Le programme combine :

| Composant | Description |
|:---------:|-------------|
| 🎨 **Rendu visuel** | Animation immersive en terminal (ANSI / Unicode) |
| ⌨️ **Capture clavier** | Interception bas niveau via X11 |
| 🔐 **Déverrouillage** | Logique par séquence secrète |

> [!WARNING]
> Ce projet n'est **pas** un vrai lockscreen sécurisé système, mais un **proof-of-concept avancé** montrant jusqu'où on peut aller sans privilèges root.

---

## 🛠️ L'Évolution — Problèmes & Solutions

### 1️⃣ Le faux plein écran : l'illusion contrôlée

<table>
<tr>
<td width="50%">

**❌ Problème**

En C standard, il est impossible de forcer une application terminal en plein écran natif.

</td>
<td width="50%">

**✅ Solution**

- Buffer alternatif ANSI : `\033[?1049h`
- Masquage du curseur
- xterm brut sans décorations

</td>
</tr>
</table>

```bash
xterm -xrm 'xterm.overrideRedirect: true'
```

> 💡 **Ruse** : Le Window Manager n'ajoute ni bordure, ni barre de titre, ni gestion de focus.  
> ➡️ **Résultat** : une interface *stealth*, au-dessus de tout.

---

### 2️⃣ Le combat contre le Window Manager

<table>
<tr>
<td width="50%">

**❌ Problème**

GNOME / KDE interceptent les raccourcis système (`Alt+F4`, `Alt+Tab`) avant qu'ils n'atteignent le programme.

</td>
<td width="50%">

**✅ Solution ultime**

```c
XGrabKeyboard(dpy, root, False, 
              GrabModeAsync, GrabModeAsync, 
              CurrentTime);
```

</td>
</tr>
</table>

> ➡️ Le serveur X11 redirige **tout** le flux clavier matériel vers ton programme.  
> ➡️ Le Window Manager devient **sourd**.

---

### 3️⃣ Le bug du "Clavier Muet"

<table>
<tr>
<td width="50%">

**❌ Problème**

Une fois le clavier capturé par X11 :
- `read()`, `stdin`, `termios` → inutilisables
- La séquence secrète ne fonctionne plus

</td>
<td width="50%">

**✅ Solution**

```c
while (XPending(dpy)) {
    XNextEvent(dpy, &ev);
    if (ev.type == KeyPress) {
        KeySym keysym = XLookupKeysym(&ev.xkey, 0);
    }
}
```

</td>
</tr>
</table>

> ➡️ On lit directement les signaux matériels du clavier, sans passer par le terminal.

---

## 🧬 Anatomie du Code — Les 4 Piliers

<details>
<summary><b>🛡️ A. La couche X11 — La Sécurité</b></summary>

<br/>

```c
Display *dpy = XOpenDisplay(NULL);
XGrabKeyboard(dpy, root, False, GrabModeAsync, GrabModeAsync, CurrentTime);
```

- ✅ Capture exclusive du clavier
- ✅ Neutralisation des touches système (`Alt`, `Super`, `F4`…)
- ✅ Lecture directe des événements `KeyPress`

</details>

<details>
<summary><b>🌠 B. Le moteur physique — L'Animation</b></summary>

<br/>

Le Starfield repose sur une **projection 3D → 2D** réelle :

```c
x_screen = (x_3D / z_3D) * focal + center_x
y_screen = (y_3D / z_3D) * focal + center_y
```

| Effet | Technique |
|-------|-----------|
| Profondeur | `z` diminue → l'étoile "s'approche" |
| Perspective | Division naturelle |
| Luminosité | Étoiles lointaines → sombres / proches → brillantes |

**Caractères Unicode** : `█ ▓ ▒ ░ ·`

</details>

<details>
<summary><b>🔐 C. La boucle d'écoute — Le Secret</b></summary>

<br/>

```c
if (c == 'm') last_key = 'm';
else if (last_key == 'm' && c == 'p') locked = 0;
else last_key = 0;
```

- 🔹 Détection **séquentielle**, pas instantanée
- 🔹 Toute erreur reset la progression
- 🔹 Le programme reste bloqué tant que la combinaison exacte n'est pas entrée

</details>

<details>
<summary><b>🧹 D. Le nettoyage — La Restauration</b></summary>

<br/>

```c
XUngrabKeyboard(dpy, CurrentTime);
printf("\033[?1049l\033[?25h");
```

> [!CAUTION]
> **Partie critique**
> - Rend le clavier au système
> - Restaure le terminal normal
> - Évite de "casser" la session utilisateur

</details>

---

## 🚀 Installation

### Prérequis

```bash
sudo apt install libx11-dev xterm x11-utils
```

### Compilation

```bash
gcc -O3 ultimate_locker_x.c -o locker -lX11
```

> [!IMPORTANT]
> `-lX11` doit être **à la fin** (ordre du linker).

---

## 🎮 Utilisation

### Lancement

```bash
xterm -xrm 'xterm.overrideRedirect: true' -fullscreen -e ./locker
```

Ou utilisez le script fourni :

```bash
./ultimate_locker_x.sh
```

### 🔓 Déverrouillage

Tapez la séquence secrète : <kbd>M</kbd> puis <kbd>P</kbd>

### 🆘 Sortie d'urgence

| Méthode | Commande |
|:-------:|----------|
| 🖥️ TTY     | <kbd>Ctrl</kbd> + <kbd>Alt</kbd> + <kbd>F3</kbd> |
| ☠️ Kill    | `pkill locker` |
| 🪴 reboot  | Ctrl + alt + backspace -> restart sessions |
| 🌱 key_boot| [ Alt + printscreen ] + 'e'r'i'u'b' |

---

## ⚠️ Leçons apprises

| 🔑 Concept | Description |
|:----------:|-------------|
| **Terminal = surcouche** | Pour bloquer vraiment, il faut parler au serveur graphique |
| **X11 > stdin** | Pour la capture clavier bas niveau |
| **Porte de secours** | Toujours prévoir une échappatoire |

---

## 🧭 Limitations

| Status | Description |
|:------:|-------------|
| ❌ | Pas un lockscreen système sécurisé |
| ❌ | Dépend fortement de X11 (Wayland = comportement variable) |
| ✅ | Excellent exercice bas niveau, architecture, événements, input handling |

---

## 🧪 Pistes d'amélioration

- [ ] Séquence configurable (`argv`)
- [ ] Hash + mot de passe
- [ ] Timeout / idle detection
- [ ] Gestion propre de tous les signaux
- [ ] Mode "panic exit" caché
- [ ] Motion blur / traînées d'étoiles

---

## 🏁 Conclusion

<div align="center">

Ce projet démontre qu'avec du **C pur**, un **terminal** et une compréhension fine de **X11**,  
on peut détourner des mécanismes bas niveau pour créer une illusion de contrôle total.

---

*Le système pense toujours être maître.*  
*Mais parfois… on peut lui faire croire le contraire.* 🌌

<br/>

📚 **Pour en savoir plus sur X11** → **[Documentation X11](X11.md)**

</div>
