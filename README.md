# Sky Dominion
## Introduction
**Sky Dominion** is a multiplayer online aerial combat game developed in Unreal Engine 5.3, supporting up to four players in combat. With just a Steam account, you can join forces with your friends, select a fighter jet, and engage in 1v1 or 2v2 team battles over a shared terrain.<br>
<br>
![Main Menu](https://github.com/TheWavesgod/SkyDominion/blob/develop/Asset/0240409114748.png)

The primary motivation behind creating this game was for job-seeking purposes, to serve as part of my personal portfolio. The majority of the game's functionalities are implemented in C++, with the remainder completed using Blueprints. I chose this theme due to my interest in military action genres and my previous experience working at a military simulation company. This game was crafted based on my basic understanding of fighter jets.<br> 

![MultiLobby](https://github.com/TheWavesgod/SkyDominion/blob/develop/Asset/e3a488bbe53443f397620db5b2a55d0.png)

## Features
### Core Gameplay
The game currently features only a multiplayer online mode where the Red and Blue teams compete against each other. By linking a Steam account, players can join a match with their companions and use weapons to destroy the opposing team. A match lasts 20 minutes, and the team with the most kills when time runs out wins. Future plans include adding a single-player mode, featuring tutorial levels and AI combat scenarios.<br> 

![RoundStateList](https://github.com/TheWavesgod/SkyDominion/blob/develop/Asset/58a9283aa5d04a2d3c3943af37be807.png)

### Physics
The maneuvering of the fighter jets is based on a simplified aerodynamics model simulated physically, drawing from the principles and code of "Realistic Aircraft Physics for Games" produced by [Jump Trajectory](https://www.youtube.com/watch?v=p3jDJ9FtTyM&t=238s) from Youtube. The implementation calculates the lift and drag of independent wing surfaces, aiming to present players with the realistic aircraft maneuvers as possible as I can. The data for the fighter jets closely reference real-world materials, incorporating implementations of different aerodynamic characteristics of fighter jets, such as static stability and static instability designs. I have also attempted to implement the physics of the landing gear, achieving the physical interaction of the suspension and wheels. However, there are still noticeable issues regarding the calculation of tire friction, which I plan to fix in the future. 

![TakeOff](https://github.com/TheWavesgod/SkyDominion/blob/develop/Asset/dc791d3f9dbbe3c95761e3ba4e16e61.png)

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
Another type of missile is the semi-active radar missile AIM7 and R27, which rely on the fighter jet's own radar for guidance. Compared to infrared-tracking missiles, they have a longer range and can be launched from a great distance, but you need to continuously track the target using STT mode, otherwise, the missile will also lose the target.

> #### Counter Measures
To counter infrared-tracking missiles, you can also launch decoy flares to interfere with the locking of infrared-tracking missiles. The temperature of decoy flares is very high, but note that it is generally not higher than the temperature of your aircraft when the afterburner is activated.

> #### Cannon
All fighter jets are equipped with a cannon, which has limited ammunition that resets upon respawn. The rate of fire and bullet speed are set according to real data, and tracer rounds are implemented to correct the trajectory.

> #### Damage
Damage is based on hit points; hits from bullets and missiles or collisions cause varying degrees of damage.

> #### Scoring
Attacks from cannons or missiles that result in enemy aircraft crashing can add one point to your team; death neither adds nor subtracts points.

### HUD
The HUD in this game provides players with detailed information necessary for piloting a fighter jet. This includes a pitch indicator, direction compass, ground speed and altitude display, throttle position indicator, the angle of attack and G-force values in the bottom left corner, Mach number, the landing gear status in the top right corner, the integrity of the aircraft, which represents the health points, and in the bottom right corner, the number of bullets, types of missiles, their quantity, and range indicators, radar mode, and the number of decoy flares. In addition to the previously mentioned enemy and missile markers, various warning alerts, it also includes match information such as kills, deaths, team scores, and match time.

### Networking and UI
The network utilizes a listen server model, with one player playing on the server, and the majority of game logic being calculated on the server side and then synchronized to each client player. The Online Subsystem Steam from UE was used to implement the Matchmaking feature. A simple game menu facilitates the functionality of a multiplayer game lobby, where players can select the fighter jet they wish to pilot. Teams are divided into red and blue based on the order of joining the room, and the game is started by the server-side player.

### Controls
The game supports both controller and keyboard inputs, but playing with a controller is recommended for a better gaming experience.

### Graphics and Sound
This project did not focus on graphics, utilizing mostly UE's default settings and simple level design, with plans to develop more interesting levels in the future. The fighter jet models are free assets from Unreal Marketplace's [Vigilante](https://www.unrealengine.com/marketplace/en-US/profile/Vigilante), with visual effects from the model pack’s Niagara particle system customized for this project's requirements. Sound effects are partly from the Unreal Marketplace and partly from free resources online. A list of credits is provided at the end. 
<br>

## How to Play
### Controls

| Key | Function |
| :--- | :--- |
| Gamepad Right Thumbstick X-Aixs or Mouse X-Aixs | Control left and right rotation of the camera |
| Gamepad Right Thumbstick Y-Aixs or Mouse Y-Aixs | Control up and down rotation of the camera |
| Gamepad Left Thumbstick X-Aixs or Keyboard A and D | Control the fighter jet's control surfaces for roll maneuvers |
| Gamepad Left Thumbstick Y-Aixs or Keyboard W and S | Control the fighter jet's control surfaces for Pitch maneuvers |
| Gamepad Left and Right Trigger Aixs or Keyboard E and Q | Control the fighter jet's control surfaces for Yaw maneuvers and the nose wheel steering on the ground. Pressing both button simultaneously controls the wheel brakes on the ground and the airbrakes in the air.|
| Gamepad Left and Right Shoulder or Keyboard LeftShift and LeftCtrl | Control the decrease and increase of the fighter jet's thrust. |
| Gamepad Face Button Bottom or Left Mouse Button | Hold to fire the cannon |
| Gamepad Face Button Left or Keyboard 1 | Press to change the type of Missile |
| Gamepad D-pad Right or Keyboard R | Press to change the mode of Radar |
| Gamepad Face Button Top or Keyboard F | Press to start to lock target or change the lock target |
| Gamepad Face Button Right or Keyboard Space Bar | Press to fire the current selected missile |
| Gamepad Right Thumbstick Button or Right Mouse Button | Hold to fire the countermeasures |
| Gamepad D-pad Down or Keyboard G | Press to retract or extend the landing gear |
| Gamepad D-pad Up or Keyboard C | Press to toggle the state of automatic flap |
| Gamepad D-pad Left or Keyboard X | Press to toggle the state of fly control system |
| Gamepad Special Right or Keyboard Esc | Press to show the pause menu |
| Gamepad Special Left or Keyboard Tab | Hold to display the current performance detail table |

## Getting Started
Find the windows build in [release](https://github.com/TheWavesgod/SkyDominion/releases/tag/v1.0.0).

Loading...
