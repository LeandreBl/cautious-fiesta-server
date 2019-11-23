>Connection :
```
<- [header]: { LOGIN }
   [payload]: {
     uint64_t nicknameLength;
     char nickname[nicknameLength];
	 float life;
	 float speed;
	 float attack;
	 float attackSpeed;
	 float armor;
	 uint8_t red;
	 uint8_t green;
	 uint8_t blue;
   }
-> [header]: { LOGIN }
   [payload]: {
     uint8_t boolRequestOk;
   }
```
>Disconnection :
```
<- [header]: { LOGOUT }
-> [header]: { LOGOUT }
   [payload]: {
     uint8_t boolRequestOk;
   }
```
>Create a GameRoom :
```
<- [header]: { CREATE_GAMEROOM }
   [payload]: {
     uint64_t nameLen;
     char sessionName[nameLen];
   }
-> [header]: { CREATE_GAMEROOM }
   [payload]: {
     uint8_t boolIsOk;
   }
```
>Delete a GameRoom :
```
<- [header]: { DELETE_GAMEROOM }
-> [header]: { DELETE_GAMEROOM }
   [payload]: {
     uint8_t boolIsOk;
   }
```
>Get a list of availables GameRooms :
```
<- [header]: { GET_GAMEROOMS_LIST }
-> [header]: { GET_GAMEROOMS_LIST }
   [payload]: {
     uint64_t size;
     {
       uint64_t playersConnected;
       uint64_t sessionNameLength;
       char sessionName[sessionNameLength];
     } [size]
   }
```
>Join a GameRoom :
```
<- [header]: { JOIN_GAMEROOM }
   [payload]: {
     uint64_t sessionNameLength;
     char sessionName[sessionNameLength];
   }
-> [header]: { JOIN_GAMEROOM }
   [payload]: {
     uint8_t boolRequestOk;
   }
```
>Leave current GameRoom :
```
<- [header]: { LEAVE_GAMEROOM }
-> [header]: { LEAVE_GAMEROOM }
   [payload]: {
     uint8_t boolRequestOk;
   }
```
>Get the player list of a GameRoom :
```
<- [header]: { GET_GAMEROOM_PLAYERS_LIST }
   [payload]: {
     uint64_t sessionNameLength;
     char GameRoom[sessionNameLength];
   }
-> [header]: { GET_GAMEROOM_PLAYERS_LIST }
   [payload]: {
     uint8_t boolRequestIsOk;
	 uint64_t gameRoomNameLength;
	 char GameRoom[gameRoomNameLength];
     uint64_t nb_of_elements;
     {
       uint8_t boolIsReadyState;
       uint64_t nicknameLength;
       char nickname[nicknameLength];
     } [nb_of_elements]
   }
```
>Send a message in your GameRoom chat :
```
<- [header]: { SEND_MESSAGE }
   [payload]: {
     uint64_t msgLenght;
     char msg[msgLenght];
   }
-> [header]: { SEND_MESSAGE }
   [payload]: {
     uin8_t boolRequestOK;
   }
```
>Receive a message from current GameRoom :
```
-> [header]: { RECEIVE_MESSAGE }
   [payload]: {
     uint64_t senderNicknameLength;
     char senderNickname[senderNicknameLength];
     uint64_t msgLenght;
     char msg[msgLenght];
   }
```
>Toggle Ready state to start a game :
```
<- [header]: { TOGGLE_READY }
-> [header]: { TOGGLE_READY }
   [payload]: {
     uin8_t boolActualReadyState;
   }
```
>Send to all clients that the game they are in just started :
```
-> [header]: { GAME_STARTED }
   [payload]: {
     uint16_t udpPort;
   }
```
>Assets manager
```
-> [header]: { ASSETS_REQUIREMENT }
   [payload]: {
     uint64_t size;
     {
       uint64_t assetNameLenght;
       char assetName[assetNameLength];
       uint64_t filesize;
       uint64_t checksum;
     } [size]
   }
<- [header]: { ASSETS_REQUIREMENT }
   [payload]: {
     uint8_t boolIsOk; //only if required assets are loaded and verified
   }
```
>Asset loader
```
<- [header]: { ASSETS_LOADER }
   [payload]: {
    uint64_t assetNameLenght;
    char assetName[assetNameLength];
-> [header]: { ASSETS_LOADER }
   [payload]: {
    uint16_t awaitingTcpServerPort;
    uint64_t fileSize;
    uint64_t filenameLength;
    char filename[filenameLength];
    uint64_t checksum;
   }
```
>Game start
```
-> [header]: { GAME_STARTED }
   [payload]: {
    uint16_t udpPort;
   }
#optionnal <- [header]: { GAME_STARTED }
              [payload]: {
               uint16_t remoteUdpPort;
			  }
```
>Acknowledge :
```
<- [header]: { ACK }
-> [header]: { ACK }
```