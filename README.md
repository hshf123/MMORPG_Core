# ServerCore 라이브러리

C++ IOCP서버와 C# 더미 클라이언트를 사용하여 제작한 서버 코어 라이브러리 입니다.

## 실행

이 프로젝트는 Visual Studio 2022환경에서 제작 되었습니다.<br><br>
코드를 내려받고 실행을 하기 위해선 Main.cpp의 tdbBalancer를 사용중인 곳을 모두 주석처리해야 실행 가능합니다.(DB연결 및 쿼리 실행과 관련된 부분)<br><br>
서버 동작과 C# 네트워크 코드 테스트를 위한 프로젝트 이기 때문에 에코 서버 이상의 기능은 없습니다.

## 프로젝트 설명

<h3>ServerCore 프로젝트</h3>
DB, 멀티스레드, IOCP 네트워크, 로깅, 메모리 풀, 시간 및 문자열 유틸을 지원하도록 제작되어있습니다.<br><br>
DB는 Poco라이브러리의 일부 기능을 가져와 제작되었고, 로그의 경우 spdlog의 기능을 가져와 제작되었습니다.<br><br>

<h3>GameServer 프로젝트</h3>
ServerCore 라이브러리를 사용하여 실제 서버가 될 프로젝트 입니다.<br><br>
멀티스레드 환경에서 유저가 보내는 패킷에 대한 DB처리는 순서보장이 가능하도록 제작되었습니다.<br><br>
클라이언트와의 송수신을 위한 패킷은 Google Protobuf를 사용합니다.<br><br>

<h3>DummyClient 프로젝트</h3>
C# 네트워크 코드 제작과 서버 기능 테스트를 위한 더미 클라이언트입니다.<br><br>
.net 4.5에서 추가된 async/await을 사용한 비동기 네트워크 입출력 기능을 사용하여 제작되었고, 비동기 환경에서 사용가능한 Lock을 구현하여 멀티스레드로 동작할때도 문제없이 처리 되도록 구현되었습니다.<br><br>

<h3>추가 개선 사항</h3>
기존 IOCP에 Registered I/O 기능을 추가하여 네트워크 효율성 증가를 위한 테스트를 진행 했습니다. CorePch.h의 USE_RIO 디파인 On/Off를 통해 기존 IOCP와 RIO의 차이를 볼 수 있게 되었습니다.<br><br>

메모리 풀 방식을 기존 락을 잡고 풀링하던 방식에서 락프리 스택을 이용한 풀링 방식을 제작했습니다. 성능적으로 크게 눈에 띄는 차이는 없었지만 컨텐츠가 추가되면 될 수록 성능적으로 개선되었을거라 기대하고 있습니다.<br><br>
