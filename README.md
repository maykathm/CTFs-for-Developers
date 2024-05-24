# CTFs-for-Developers: A repository for playing developer-oriented capture-the-flag games

## Why CTFs?

CTFs excel at showcasing the unintended consequences to the choices we developers make. We spend so much time gently coaxing good sense into lines of code that we can forget the code's true purpose in its final morphed form as a series of instructions the machine executes. Though immensely satisfying and its own kind of art, our objective isn't *actually* to write that clever lambda expression that makes the code so easy to understand and honestly kind of beautiful. We write code to make the machine solve problems for us.

A CTF can show us situations in which our code design choices can lead to unintended behavior. While contrived and at times silly, they can show us what's behind our code and how see our binary world from a quirky sideways vantage point.

## How to play

Gradually as I find time to write out the different puzzles I have in mind, I will add a new folder for each puzzle in this repository. They will make use of Docker for creating the puzzle environment, and each will have its own dockerfile. As suggested in their name, the CTFs will consist in retrieving a flag from the environment, and the objective will be outlined in its README.md file. Each README.md will also contain hints and a walkthrough, though those sections will be hidden so as to not spoil those who wish to not be spoiled.

My general intent is to provide various scenarios in which one must put on a developer hat and exploit some perhaps lesser-known quirks of  programming languages and/or environments to access restricted data. That said, obviously, you control all Docker containers you build, so you can easily get the flags by just logging in as root. If that gives you joy, go for it. Otherwise, I would advise working each problem as the intended user.