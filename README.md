# Sky Dominion
## Introduction
**Sky Dominion** is a multiplayer online aerial combat game developed in Unreal Engine 5.3, supporting up to four players in combat. With just a Steam account, you can join forces with your friends, select a fighter jet, and engage in 1v1 or 2v2 team battles over a shared terrain.<br>
<br>
The primary motivation behind creating this game was for job-seeking purposes, to serve as part of my personal portfolio. The majority of the game's functionalities are implemented in C++, with the remainder completed using Blueprints. I chose this theme due to my interest in military action genres and my previous experience working at a military simulation company. This game was crafted based on my basic understanding of fighter jets.

## Features
### Core Gameplay
The game currently features only a multiplayer online mode where the Red and Blue teams compete against each other. By linking a Steam account, players can join a match with their companions and use weapons to destroy the opposing team. A match lasts 20 minutes, and the team with the most kills when time runs out wins. Future plans include adding a single-player mode, featuring tutorial levels and AI combat scenarios.

### Physics
The maneuvering of the fighter jets is based on a simplified aerodynamics model simulated physically, drawing from the principles and code of "Realistic Aircraft Physics for Games" produced by Jump Trajectory from Youtube. The implementation calculates the lift and drag of independent wing surfaces, aiming to present players with the realistic aircraft maneuvers as possible as I can. The data for the fighter jets closely reference real-world materials, incorporating implementations of different aerodynamic characteristics of fighter jets, such as static stability and static instability designs. I have also attempted to implement the physics of the landing gear, achieving the physical interaction of the suspension and wheels. However, there are still noticeable issues regarding the calculation of tire friction, which I plan to fix in the future.
