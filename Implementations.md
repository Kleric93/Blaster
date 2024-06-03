Course Implementations:

Multiplayer plugin using steam servers with basic functionality
Online subsystem and configuring online sessions
Main UI menu for joining sessions
Weapon class
Pickup widgets
Variable replications
Equipping weapons
Remote procedural calls
Animation poses
Aiming down sights
Blendspaces
Movement mechanics (running, strafing, crouching, jumping, aiming down sights)
Fabrik IK
Turning in place and rotating the root bone
Footsteps and jump sounds (self implemented to be different depending on the material surface)
Firing weapons (projectile and hitscan weapons)
Firing for multiplayer
Hit target events in multiplayer
Main HUD creation (Health, shields, ammo amount and type available and carried ammo, kills and deaths)
Crosshairs (with spread while jumping or firing and color change while aiming to an enemy)
Zoom while aimed
Automatic fire (for certain wespons)
Creation of the basic game mode (deathmatch)
Death animation (Disintegration by flying drone overhead)
Reloading effects and auto reload
Game timer (with client and server sync)
Match states
Introduction of different weapon types (RPG, Grenade launcher, SMG, assault rifle, sniper rifle, shotgun, hand grenades)
Sniper scope when aiming
Different reload for shotgun by loading single shells (can be interrupted)
Buff pickups creation (Medkit, ShieldKit, jump buff, run buff)
Secondary weapon (character can hold 2 weapons)
Lag compensation (Server Side Rewind and frame package) with limitation on ping
Cheating prevention and validation
Leaving the game and returning to the mainmenu
Spawning a crown on #1 player
Elimination announcements in the HUD
Headshot damage
Team deathmatch (with friendly-fire prevention) and team scores.
Basic Capture the flag

Self Implementations:

Bullet holes on surfaces
Random rotation and velocity on shells ejected by weapons
Custom weapon added (M4AZ designed by me, also with custom reload animation, tracer fire and sounds)
Custom reload animation where the magazine gets ejected and stays on the ground for a set amount of time
Weapon and pickup spawn points (fixed or randomized) which light up a button underneath when something is available, along with a niagara particles animation to make the player aware that the spawnpoint just spawned something there
Random spawn timer for the pickup and weapon spawn points
Realtime 2D minimap with sprites indicating other players and PoIs depending on proximity
Advanced Capture the flag (similar implementation to Unreal Tournament)
Implemented Score to win or Kills to win, which will zero out the timer and end the match in case those criteria are met
Map voting system with different maps depending on the game mode (updated in real time so that others can see what maps are the most voted before they vote themselves)
Introduced a team choice menu which updates in real time, showing the names of other players and team choice when joining a team-based match.
introduction of TAB score overview, where players can see in real time match statistics, along with how other players are doing during said match. In case of team matches the overview looks different with players divided in their respective teams with different colors
Match Text Chat
Custom elimination messages which also highlight what weapon was used to kill another player
Aim Assist when players use controller Input (auto deactivates when switching to Mouse and Keyboard)
Capture the flag UI with real time flag state updates (home, stolen, dropped) and scores.
Training area with stationary bots spawner to test all kinds of weapons, eliminations, etc...
Overhead widget showing the player name when aimed at and not obstructed by other objects (auto disappears after 0.5 seconds)
Instakill 1 shot weapon for a specific game mode (similar to Unreal Tournament's Instagib)
Advanced movement (slide, prone, diving)
Advanced UI with a full main menu with graphics, sound and input customization, weapon indexing with description, match creation with map and mode choice, timer setup, score to win setup, kills to win setup and server name to be able to easily find the server in case there are multiple sessions in progress.
C# launcher with autopatcher depending on the version of the game on the server.
Camera shake while firing weapons
Special powerup which allows the player to shoot the weapons twice as fast
UI icons for powerups which light up while a specific powerup is active
Advanced Pickup Widget for weapons with custom icons showing what type of weapon it is and what kind of ammo it uses.
Killstreak (Phantom Stride) which after 3 kills without deaths allows the player to unsheathe a sword, which is struck by a lightning bolt. This allows the player to instantly teleport to any other player and kill them with 1 slash.
Direction-based hit arrow, to show the player which direction the shots were coming from.
