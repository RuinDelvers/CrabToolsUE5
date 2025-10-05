# CrabToolsUE5
This is a plugin library implementing various subsystems that are helpful for promoting
efficient and optimized design. The following systems are currently available:

- State Machine Blueprints: A specialized blueprint for handling hierarchical state machines that can be extended, edited on instances, and composed.

- RPG Subsystem: A generalized RPG system as an alternative to UE's gameplay attribute system. Supports int and float attributes, both with recursively dependent values (such as min and max health, and maxmax and minmin health), attribute operators that can be used to non-destuctively modify attributes with priority, and status effects which are at their core simple effects, but are made so any RPG effect can be implemented.

- Ability System: An ability system that is designed to work with our state machines and have some native implementations that cover common use cases.


- Interaction System: Simple base system for creating and interacting with objects in the environment. Useful for both players and AI.

## State Machine Usage

![Sample State Machine](https://raw.githubusercontent.com/RuinDelvers/CrabToolsUE5/refs/heads/main/SampleImages/StateMachineSample.PNG)

Our State Machines are a generalized blueprint that can be used for a variety of tasks: From AI control, to player state 
management, to dialogue. Each state machine is event based; Meaning that state changes will only occur upon an event being
sent from an external source or emitted from the execution nodes in each state. This allows for a very performative method of
controlling entities. Events, are effectively just Names, that are used to identify what happened; For example, the AI_LOST event 
is emitted by some of our movement nodes to indicate that it is lost (for any reason). While the AI_ARRIVE event is used when 
arriving at their destination.

Events also act as a sort of interface for state machines; You can create a State Machine Interfaces that contain events and 
other data that defines how your state machine can be interacted with. These are also used with some custom nodes to safely send events to state machines without having the save random Name variables everywhere.


## RPG System Usage

Our RPG System handles the complex relationships between RPG attributes and resources. Attributes, such
as strength or intelligence, are objects which can be an integer or float and are characterized by
a base value, and operators on them. These operators can be simple additions to the attribute (e.g. from 
statuses), or can be programmatically complex. Resources are objects that can be either an int or float,
and are characterized by a maximal and minimal value. These maximal & minimal values can be other
attributes.

The valuable part of this system is the ease of simply placing variables into a blueprint, and the system
automatically finds them to initialize them. This also enables blueprint/c++ type checking for accessing
things, which removes error-prone key lookups. This system also enables powerful event handling for
RPG elements. Since each RPGProperty (attributes, resources, or custom) are objects, they have
easy to bind to events for their updates. This makes binding to them for UI or other necessities extremely
simple.

To use this system, simply extend the RPGComponent (either blueprint or c++), create a variable
and set the variable to the object you want. In BP they are instanced, but in c++ you use CreateDefaultSubobject
to handle this, and with some useful tools for resources to easily setup the relationships between
Min/Max attributes.

## Dialogue System

The Dialogue System is designed to work with our state machines, but does have support for custom handlers. To make a dialogue system, you would utilize the "Dialogue" Nodes (Nodes that contain dialogue in their name). Along with the DialogueComponent to handle handshaking and syncing dialogue states between characters. UI elements can hook into the dialogue component to receive dialogue & monologue data to receive. This way control (component), the model implementation (the State Machine or custom), and the view (the UI or other visible effects) are all separate & rely on a simple communication protocol.

The dialogue nodes can also be extended to handle more complex features, such as spawning in Sequencer events or other necessities when dialogue is chosen.
