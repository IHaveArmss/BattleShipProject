battleship with sockets in C
how to run :


  ```sudo apt install build-essential git libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev```

  ```git clone https://github.com/raysan5/raylib.git raylib```
  
  ```cd raylib/src/```
  
  ```make PLATFORM=PLATFORM_DESKTOP```
  
  ```sudo make install```
  
  ```make```
  
  ```./battleship```


  ```make server — builds only the server (uses gcc by default)```
  ```make server CC_SERVER=clang — builds only the server with clang (for Termux)```
  ```make — builds only the client```
  ```make all```

  Setup : android phone with termux and the files then do make server CC_SERVER=clang
  then ./server then run the client for each of the two players and select the coresponding player thing
  
info pt coduri de la socketuri
HELLO <1 sau 2> -> Client catre Server: Vreau sa fiu Player 1 sau 2. 
OK <1 sau 2> -> Server catre Client: Esti Player 1 sau 2.
REJECT -> Server catre Client: Jocul e plin, te resping. 
BOARD <100_caractere_0_si_1> -> Client catre Server: Asta e harta mea (0 apa, 1 barca). 
READY -> Server catre Clienti: Ambele harti primite, incepe meciul! 
TURN -> Server catre Client: E randul tau, alege unde ataci. 
WAIT -> Server catre Client: Asteapta confirmarea adversarului. 
ATTACK -> Client catre Server: Atac la coordonatele astea. 
HIT -> Server catre Atacator: Ai lovit o barca. Ai rand din nou. 
YOUHIT -> Server catre Atacat: Ai pierdut o bucata de barca. 
MISS -> Server catre Atacator: Ai tras in apa. Schimbam randul. 
YOUMISS -> Server catre Atacat: Adversarul a tras in apa. 
WIN -> Server catre Client: Ai scufundat tot, ai castigat! 
LOSE -> Server catre Client: Ai ramas fara barci, ai pierdut!