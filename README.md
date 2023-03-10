THIS REPO IS CURRENTLY A WORK-IN-PROGRESS. Polonius is not yet functional

# polonius
A memory-efficient and modular text editor


> Therefore, since brevity is the soul of wit
> 
> And tediousness the limbs and outward flourishes,
> 
> I will be brief.
> 
  – Hamlet, Act II, Scene II

## About
Polonius is a text editor with a focus on memory efficiency.

Although Polonius can be used as a general-purpose text editor, it is primarily designed for editing **very large files** on systems with **very little RAM**.

In order to achieve this, it never loads any more data into RAM than is currently being used. All that we have to keep in memory is the part of the file that's *currently* being displayed, plus a list of the *changes* the user wants to make.

Polonius is made up of separate **binary modules**:
  - The *"reader"*, which outputs a *selected portion* of the contents of a file
  - The *"editor"*, which interprets editing instructions (*replace*, *insert*, and *remove*) and makes the requested changes to the file
  - And the interactive *UI*, which the user interfaces with and which ties together the functionality of the other modules.
