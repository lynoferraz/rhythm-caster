# Rhythm Spell Caster  - Riv Cartridge

A dungeon crawler rhythm game

## Description

Can you keep up the beat and cast spells in sync with the music?

In Rhythm Spell Caster you are a sorcerer that can only move on the music beat, and also can only cast spells by doing the chant in sync with the beat.

Game controls:

- Press ARROWS to move, and cast spells (while X/A2 is pressed). It should always be on rhythm.
- Hold X/A2 to cast spells.
- Press Z/A1 to release the casted spell.

Notes:

Assets from [Forgotten Runes](https://www.forgottenrunes.com/), and cover image generated with [Magic Studio](https://magicstudio.com/).

## Instructions

Download the sqfs if the relase and upload to [Rives Emulator](https://emulator.rives.io/) to play. 

You can also download rivemu locally to play. Instructions: [Rives Docs](https://docs.rives.io/docs/riv/getting-started)

## Game Parameters/Arguments

You can load [SeqToy](https://github.com/edubart/seqtoy) outcards as incards to use it as the background music.



| Argument | Type | Default Value | Description |
| - | - | - | - |
| -fps | int | 30 | Frames per second |
| -sync-factor | float | 0.3 | Sync window to hit key |
| -starting-life | int | 10 | Starting life points |
| -time-bonus | float | 1 | Score bonus per second |
| -kill-bonus | float | 1 | Score bonus per kill |
| -item-bonus | float | 1 | Score bonus per items |
| -spawn-decrease-interval | uint16 | 1 | Number of notes to decrease untill next monster spawn |
| -display-grid-lines | bool | 0 | Display/Hide grid lines |
| -display-initial-help | bool | 1 | Display/Hide initial help texts |
| -display-score | bool | 1 | Display/Hide score |
| -monster-notes-to-spawn | uint16 array | 40,40,40,40 | Initial number of notes to spawn monsters (for each monster type) |
| -monster-notes-to-move | uint16 array | 3,3,3,3 | Number of notes to move monsters (for each monster type) |
| -monster-damage | uint8 array | 1,1,1,1 | Damage of each monster type |
| -monster-initial-life-points | uint8 array | 1,1,1,1 | Monster initial life points (for each monster type) |
| -monster-spawn-object | uint16 array | 27,14,3,5 | Object where each monster type will spawn |
| -monster-tracks | uint8 array | 0,1,2,3 | Seqt track which each monster type will use |
| -item-unlock-spell | uint16 array | 0,1,2,3 | Item that will unlock the spell (for each spell type) |
| -unlockable-objects | uint64 array | 1,0,13,12,1,1,80,14,4,1,1,200,7,9,1 | When and where an object that unlocks spell will appear (active,beat_frame,x,y,obj for each object) |
| -item-interval | uint16 | 40 | After unlockables objects appeared, beats interval when bonus pont items appear |
| -item-positions | int16 array | 12,13,10,5,3,6,4,13 | After unlockables objects appeared, positions where bonus points items randomly appear |
| -spell-damage | int array | 1,1,1,2,1,1,0 | Damage of each spell type |
| -spell-duration | float array | 2.5,2.5,2.5,1.0,0.75,4.0,16.0 | Spell duration in beats |

## Spellbook

| Spell | Enchant | Alternative Enchants |
| -- | -- | -- | 
| Frostbite | &rarr; &rarr;  | &larr; &larr; / &darr; &darr; / &uarr; &uarr; |
| Frostbite 2 | &rarr; &rarr; &rarr; | &larr; &larr; &larr; / &darr; &darr; &darr; / &uarr; &uarr; &uarr; |
| Frostbite 3 | &rarr; &rarr; &rarr; &rarr; | &larr; &larr; &larr; &larr; / &darr; &darr; &darr; &darr; / &uarr; &uarr; &uarr; &uarr; |
| Shield of Light | &rarr; &uarr;  | &uarr; &rarr; / &larr; &darr; / &darr; &larr; |
| Windslash | &larr; &rarr; &rarr; | &rarr; &larr; &larr; / &darr; &uarr; &uarr; / &uarr; &darr; &darr; |
| Fireball | &larr; &larr; &rarr; &rarr; | &larr; &larr; &rarr; &rarr; / &uarr; &uarr; &darr; &darr; / &darr; &darr; &uarr; &uarr; |
| Nova | &larr; &darr; &rarr; &uarr; | &rarr; &darr; &larr; &uarr; |

Note: you don't start with all spells unlocked