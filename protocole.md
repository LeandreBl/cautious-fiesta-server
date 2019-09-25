>Connection :
```
<- [header]: { LOGIN }
   [payload]: {
     uint64_t nicknameLength;
     char nickname[nicknameLength];
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
>Create a GameSession :
```
<- [header]: { CREATE_GAMESESSION }
   [payload]: {
     uint64_t nameLen;
     char sessionName[nameLen];
   }
-> [header]: { CREATE_GAMESESSION }
   [payload]: {
     uint8_t boolIsOk;
   }
```
>Delete a GameSession :
```
<- [header]: { DELETE_GAMESESSION }
   [payload]: {
     uint64_t nameLen;
     char sessionName[nameLen];
   }
-> [header]: { DELETE_GAMESESSION }
   [payload]: {
     uint8_t boolIsOk;
   }
```
>Get a list of availables GameSessions :
```
<- [header]: { GET_GAMESESSIONS_LIST }
-> [header]: { GET_GAMESESSIONS_LIST }
   [payload]: {
     uint64_t size;
     {
       uint8_t playersConnected; /* out of 4 */
       uint64_t sessionNameLength;
       char sessionName[sessionNameLength];
     } [size]
   }
```
>Join a GameSession :
```
<- [header]: { JOIN_GAMESESSION }
   [payload]: {
     uint64_t sessionNameLength;
     char sessionName[sessionNameLength];
   }
-> [header]: { JOIN_GAMESESSION }
   [payload]: {
     uint8_t boolRequestOk;
   }
```
>Leave current GameSession :
```
<- [header]: { LEAVE_GAMESESSION }
-> [header]: { LEAVE_GAMESESSION }
   [payload]: {
     uint8_t boolRequestOk;
   }
```
>Get the player list of a GameSession :
```
<- [header]: { GET_GAMESESSION_PLAYERS_LIST }
   [payload]: {
     uint64_t sessionNameLength;
     char gameSession[sessionNameLength];
   }
-> [header]: { GET_GAMESESSION_PLAYERS_LIST }
   [payload]: {
     uint8_t boolRequestIsOk;
     uint64_t nb_of_elements;
     {
       uint8_t boolIsReadyState;
       uint64_t nicknameLength;
       char nickname[nicknameLength];
     } [nb_of_elements]
   }
```
>Send a message in your GameSession chat :
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
>Receive a message from current GameSession :
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
       uint32_t checksum;
     } [size]
   }
<- [header]: { ASSETS_REQUIREMENT }
   [payload]: {
     uint8_t boolIsOk; //only if required assets are loaded and verified
   }
```
>Asset loader
```
-> [header]: { ASSETS_LOADER }
   [payload]: {
     uint64_t nbOfAssets;
     {
       uint64_t assetNameLenght;
       char assetName[assetNameLength];
     } [nbOfAssets]
   }
<- [header]: { ASSETS_LOADER }
   [payload]: {
     uint64_t nbOfAssets
     {
       uint64_t fileSize;
       uint64_t filenameLength;
       char filename[filenameLength];
       uint32_t checksum;
       int8_t file[filesize];
     }
   }
```
>Acknowledge :
```
<- [header]: { ACK }
-> [header]: { ACK }
```