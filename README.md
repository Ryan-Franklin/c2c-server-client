project2
Ryan Franklin
Professor. Mina Guirguis
Computer Networks: Dec 4--11:59 PM
Goal: Command and Control (C2C) server:

Objective:

This project is implemented with the Socket API in the languages c and c++ and utilizes arrays as buffers to pass messages between a server and a client. When established, the server will create the initial socket in an indefinite loop listening for a client. Once the server is contacted by an agent the server will receive a buffer which contains an action that the server will then execute and log the correct action. I wanted to make an array of the agents and agent time sheets similar to a list() in python, so it was easy to iterate through them.  I would introduce a linked list or some other form of dynamic data structure for better memory usage and be expanded to greater capacity.

Running/Compiling:

Server:
- Compile: g++ -o c2c c2cServer.cpp
- Running: ./c2c <port number>
Agent:
- Compile: gcc agent.c -o agent
- Running: ./agent <ip address> <port number> <action>

Note: worked with Lucas Anesti in planning and prep for the project.

