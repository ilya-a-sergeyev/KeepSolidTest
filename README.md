# KeepSolidTest

used:

openssl
curl
jsoncpp
protobuf

ToDoCheckList
    ToDoAuthentificator (abstract)
        GetSessionID
    ToDoTransport (abstract)
        GetData(sessionID)

PostToDoAuthentificator: ToDoAuthentificator
    
SslToDoTransport: ToDoTransport    
    
    