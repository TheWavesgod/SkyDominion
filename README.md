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

### Combat 
The game's fighter jets are equipped with three main components: radar, missiles, and a cannon.
> #### Radar
The implementation of the radar is inspired by real radar principles, with fictional elements added. The radar gameplay includes three modes: 
* RWS mode
which is essentially a wide-area scanning mode. Enemy aircraft scanned by RWS are briefly marked on the screen for a few seconds without specific information about the enemy; you need to use this mode to locate the general position of the enemy.
* VT mode
which is vertical scanning mode, allows you to perform a vertical scan in the direction of the enemy's estimated position once identified. Enemies within the vertical scan range are detailedly marked on the screen, including the model and location, considered as designated targets. Detailedly marked enemies can be locked on; pressing the lock button initiates a two-second locking process, pressing the lock button again can switch the locked target. After locking is completed, the mode switches to STT.
* STT mode
which is single-target tracking mode, continuously locks onto the target, with detailed information about the target displayed on the screen, and no other enemy targets are shown. The tracking range of STT is conical; if the target escapes this range, you lose the target and revert to VT or RWS mode. In STT mode, you can pass the locked target information to the missile, guiding semi-active radar missiles to track the targeted enemy.

> #### Warning System
The radar warning system is also inspired by real radar principles, with fictional elements added. Depending on the different modes of enemy radar, i.e., different radar signal volumes, you will receive different warnings, including being scanned by RWS, vertical scanning, STT tracking, missile tracking, and missile approach, each with different alerts. Logic related to low altitude and landing gear deployment/retraction also has corresponding prompts and warnings.

> #### Missiles
Fighter jets are equipped with one or two types of air-to-air missiles and support switching between them, with a limited number of missiles that reset upon respawn. <br>
The game mainly implements two types of missiles: the infrared-tracking missile AIM9 and R73, which is an automatic tracking missile based on infrared principles that can automatically track the hottest target based on infrared characteristics. I implemented a relatively simple calculation of infrared characteristics; fighter jets, missiles, and decoy flares all possess infrared characteristics. When tracked by this missile, your best option is to perform evasive maneuvers, release decoy flares, and most importantly, turn off the afterburner to reduce engine temperature.<br>
Another type of missile is the semi-active radar missile AIM7 and R27, which rely on the fighter jet's own radar for guidance. Compared to infrared-tracking missiles, they have a longer range and can be launched from a great distance, but you need to continuously track the target using STT mode; otherwise, the missile will also lose the target.
