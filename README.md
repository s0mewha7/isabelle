# Isabelle - UCI Chess Engine

<div style="text-align:center;">
    <img src="image.png" width="150" height="150">
</div>

*Isabelle* is a Open Source UCI (Universal Chess Interface) compatible chess engine written in C++. It is designed to provide strong chess-playing capabilities while being lightweight and easy to integrate into various chess-related applications. She currently has an estimated playing strength of 1600+ ELO.

## Features

- **UCI Compatibility:** Isabelle follows the UCI protocol, allowing it to seamlessly integrate with popular chess GUIs (Graphical User Interfaces) such as Arena, ChessBase, and SCID.
- **Strong Chess Engine:** Isabelle employs advanced chess algorithms and heuristics to play strong chess, making it suitable for players of various skill levels.
- **Efficient Implementation:** Isabelle is implemented in C++ for optimal performance and efficiency, allowing it to handle complex calculations and deep search depths efficiently.
- **Customizable:** Isabelle's parameters and search algorithms are customizable, allowing users to fine-tune its behavior to their preferences or specific use cases.
- **Portable:** Isabelle is designed to be platform-independent, making it easy to compile and run on various operating systems.

## Build

### Also requirements:
```
    - Any UNIX derived OS
    - C++ 23 compiler or other with support for the pthread library
    - Any UCI compliant chess front end (Ex: CuteChess)
```
### To compile and run, navigate to the root project directory and invoke the following two commands:

```
    - g++ src/*.cpp -lpthread -o isabelle
    - ./isabelle cli
```

## About

The core of her decision making process is built with an algorithm called Principal Variation Search (PVS); a special flavor of the classic, tried-and-true alpha beta search. Additional heuristics are used for more aggressive pruning of potentially irrelevant subtrees. These methods include razoring (WIP), futility-pruning, and delta-pruning.

High-quality evaluations of tactically quiet leaf nodes is done using a hand crafted function.


## License

Isabelle is distributed under the [MIT License](LICENSE), allowing for both personal and commercial use with attribution. See the LICENSE file for more details.
