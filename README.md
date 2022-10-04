## Laboratory 2 - OOP

### Description

This simulation is supposed to represent some simple natural environment with generalized elements  
The simulation will mostly rely on interactions between the different entities

#### Entities:

* Tile - The most essential entity, most other entities reside on it. Has 3 main attributes:  
	- Water - The quanitity of water in the area. Sustains life.
	- Temperature - Influences water evaporation
	- Altitude - Influences water flow and precipitation
Tiles also store the information regarding what kind of entity is currently on it
* Cloud - An entity that helps redistribute water across Tiles, behaves differently to the Tile's water flow  
* Plant - The most common lifeform, grows on tiles with enough water and proper heat
* Tree - A "grown up" plant, generates food for other animals and a small amount of clouds via transpiration
* Fungi - Similar to Plants, require more humidity and can feed on the dead
* Corpses - The remains of a deceased animal. Can be used as a food source for other entities
* Animals:
	- Detritivores - Animals that feed from the dead
	- Herbivores - Animals that feed from plants
	- Carnivores - Animals that feed from other animals, tend to be more aggressive
	- Omnivores - Animals that feed from any of the above, less aggressive than carnivores but can still attack other animals
	
Inheritance has been added in the following way:  
![Class Diagram](/images/UML_Diagram.png)  

### Progress:

Fungi now spawn and consume water, appear visible on the Entity map  
Trees are now made to interact with the medium

No animals & corpses are spawned, as they need some bugfixing and modification of their methods
Running the program will print out the existing landscape inside the terminal in multiple sections:  
![Program execution screenie](/images/Terminal.png)  

The intensity of the colors above directly correspond to an increase in quantity of something  
**Caution!** Make sure the program output fits inside the terminal window, or you will have a nasty trace left on-screen  
For now, the program can only end execution with Ctrl+C.  
The colors have been added via ANSI escape codes.
