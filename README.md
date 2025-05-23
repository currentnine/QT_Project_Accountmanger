# 💰 MyBankApp - 계좌 관리 데스크탑 앱

Qt C++ 기반으로 구현한 계좌 관리 애플리케이션입니다.  
입출금, 송금, 거래 내역 관리, 즐겨찾기, 다크모드 등 다양한 기능을 제공합니다.

## 📌 프로젝트 주요 기능

- **회원가입 및 로그인**
  - 계좌번호/비밀번호 입력
  - 로그인 후 사용자 정보와 거래 메뉴 표시

- **계좌 정보 UI 분리**
  - 로그인 후 ‘계좌 정보’ 버튼을 누르면 거래 기능이 있는 패널이 등장
  - 메인 UI와 거래 UI 분리 설계

- **입금/출금/송금**
  - 금액 입력 후 버튼 클릭으로 처리
  - 송금은 직접 입력 또는 즐겨찾기에서 선택 가능

- **거래내역**
  - 거래 시 로그가 파일(`transactions.txt`)로 저장
  - 사용자 이름 기준으로 거래내역 필터링 출력
  - 키워드 검색 가능

- **즐겨찾기 기능**
  - 송금 내역에서 상대 계좌를 즐겨찾기에 추가
  - 즐겨찾기 리스트에서 바로 송금 가능
  - 즐겨찾기 항목 삭제 지원

- **다크모드 지원**
  - `ToggleSwitch` 위젯으로 실시간 테마 전환
  - 로고 색상 반전, 전체 스타일 자동 적용

---

## 🖥️ 데모 화면

> (스크린샷 추가 예정 – 앱 실행 화면, 다크모드, 즐겨찾기 등)

---

## 🔧 기술 스택

| 항목           | 내용                          |
|----------------|-------------------------------|
| 언어           | C++ (Qt Widgets 기반)         |
| 프레임워크     | Qt 6.x                        |
| 빌드 도구      | CMake                         |
| UI 디자이너    | Qt Designer (`.ui` 파일 기반) |
| 데이터 저장    | 파일 기반 (`accounts.txt`, `transactions.txt`, `favorites.txt`) |
| 기타           | QPalette, QFile, QInputDialog 등 사용 |
