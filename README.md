## Window Named Pipes Example
Client and Server that can send some data and command to each other. 
Common code moved to a library. 

To send data between client and server I create simple protocol. 
Each packet start with byte which set what packet contain and what receiver should do with it. 
Commands and data types described in `SBAPPLib\SBAPPLib.h` in enums `proto_command_id` and `proto_value_type`

Class `sbapp_pipe` have methods to prepare packet in char buffer and get command from such buffer. 

Protocol supports sending objects of class someTestClass. 
Server have list of received objects and can return object to Client or call a method of object. 

Client simple open pipe, send some data and commands to server, check answers and quit. 
You can run many clients simultaneously. I add few second sleep between sending packet so it will be easier to run them together. 

Server can work in two modes. Overlapped and Multythreaded as described in msdn articles about named pipes usage. 
In Overlapped mode client send all test packets and after that get answers from pipe and process them.  
In Multythreaded mode client check servers answer before sending next packet. 

## Build 
Create some folder
```
mkdir Build
cd Build
```
Prepare visual studio compiler environment 
```
call "R:\Program Files\Visual Studio 2015 CE\vc\bin\x86_amd64\vcvarsx86_amd64.bat" 
```
Use cmake to generate visual studio project 
```
cmake -G "Visual Studio 15 2017"  -DPRINT_POCKET_INFO=ON -DOVERLAPPED_MODE=ON ../   
```
Option `PRINT_POCKET_INFO` enables more info about sent packet in logs 
Option 'VERLAPPED_MODE' enables FILE_FLAG_OVERLAPPED in servers named pipe
Build programs
```
cmake --build . --target ALL_BUILD --config Debug -- /p:CharacterSet=Unicode
``` 

## Run 
You can find programs in `Build\Release` or `Build\Debug` 
Just run SBAPPServer and then run few copies of SBAPPClient. 
Output will be something like that 
``` 
Pipe client connected
41: Recieved int :"29600"
41: Recieved str :"SomeString"
41: Recieved : someClass[ cost:12345, count:333, points:116, Name:"TestName", City:"CityNameName" ]
41: Recieved : someClass[ cost:543210, count:111, points:237, Name:"AnotherName", City:"MoreCityName" ]
41: Recieved request for object id:"1"
41: Recieved function call for object id:"1", function generatePoints
41: Recieved function call for object id:"1", function resetCost
41: Recieved request for object id:"1"
Pipe client disconnected
``` 

log line started with random id of a connection from which data was received. In case you run few clients. 
Then some info about incoming packet. 
