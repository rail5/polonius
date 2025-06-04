# Polonius

A text editor for very large files

> Therefore, since brevity is the soul of wit
> 
> And tediousness the limbs and outward flourishes,
> 
> I will be brief.
> 
  &mdash; Hamlet, Act II, Scene II

## About
Polonius can be used to edit files of any size (up to just over **8 million terabytes**) on systems with as little as only **a few kilobytes** of available memory.

In order to achieve this, it never loads any more data into RAM than is currently being used. All that we have to keep in memory is the part of the file that's *currently* being displayed, plus a list of the *changes* the user wants to make.

Most text editors function by:
  - **(1)** *loading the entire contents of a file into RAM*,
  - **(2)** *making changes to that portion of RAM*,
  - and then **(3)** *writing that portion of RAM back to the disk*.

There's nothing wrong with this method -- but it does limit you to how much you can load into RAM at any one time. Try editing a 100GB file in a normal text editor!

Polonius is made up of separate **binary modules**:
  - The *"reader"*, which outputs a *selected portion* of the contents of a file
  - The *"editor"*, which interprets editing instructions (*replace*, *insert*, and *remove*) and makes the requested changes to the file
  - And the interactive *UI*, which ties together the functionality of the other modules.

**[See the wiki](../../../../../../rail5/polonius/wiki/) for full documentation**. Manual pages are also available in the Debian packages.

## Development Progress
Development is done on **Debian GNU/Linux**. Releases will be provided for the **amd64** and **arm64** architectures.

**polonius-reader**:

  ![99%](https://geps.dev/progress/99)
  

**polonius-editor**:

  ![99%](https://geps.dev/progress/99)
  

**Terminal UI (polonius)**:

  ![15%](https://geps.dev/progress/15)
  

**GUI (polonius-gui)**:

  ![0%](https://geps.dev/progress/0)
