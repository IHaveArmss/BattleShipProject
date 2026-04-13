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
  
