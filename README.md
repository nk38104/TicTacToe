# TicTacToe

This is a simple Tic Tac Toe game created in C++. I wanted to take a beginner project and play a little bit more with C++ and learn a bit more about Forms. Just to get a hint about how they work.

Game screen:

![main](https://user-images.githubusercontent.com/55879009/97782708-59764e00-1b93-11eb-8dd9-322365238615.PNG)


# Layout

As you can see on top-left and top-right side of the screen there is a player "profile" (player name, his playing icon\symbol).
Between them in the middle there is a game counter which counts how many games were played so far and bellow that there is a current score. 
Game window is also responsive so you can change the size of the window, although I've set minimal height and width to be as starting height and width so you can't go less than that.
On the bottom I've put a text which says whose turn it is so if players forget whose turn it is there is a simple way to find out.


# Gameplay

Gameplay is same as standard Tic Tac Toe so for more info you can read --> https://en.wikipedia.org/wiki/Tic-tac-toe

Two message boxes will show up at the end of each round. First one will show round winner status, if player 1 wins it will say "Player 1 is the winner!", etc.
Second message box will ask do you want to start a new game, in case of clicking "Yes" the game will continue but in case of clicking "No" you will exit the game and the game will terminate.
