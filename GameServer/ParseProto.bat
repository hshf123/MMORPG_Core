pushd %~dp0
protoc.exe -I=./ --cpp_out=./ --csharp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ --csharp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ --csharp_out=./ ./Protocol.proto

protoc.exe -I=./ --cpp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ ./Protocol.proto

XCOPY /q /s /y .\Enum.cs ..\DummyClient\Protobuf\
XCOPY /q /s /y .\Struct.cs ..\DummyClient\Protobuf\
XCOPY /q /s /y .\Protocol.cs ..\DummyClient\Protobuf\

DEL .\Enum.cs
DEL .\Struct.cs
DEL .\Protocol.cs

PAUSE