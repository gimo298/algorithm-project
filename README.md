# 유전체 서열 재구성 알고리즘 성능 비교 프로젝트

## 1. 프로젝트 개요

본 프로젝트는 유전체 해독의 핵심 기술인 **재서열 분석(Re-sequencing)** 을 컴퓨터 상에서 모사하여 구현하고, 다양한 문자열 탐색 알고리즘의 서열 매핑 정확도와 처리 속도를 비교 분석하는 프로젝트입니다.

### 개발 목적

* 대용량 게놈 서열 탐색 환경을 구현
* 다양한 문자열 탐색 알고리즘의 성능 비교
* Mapping 결과가 DNA 복원 정확도에 미치는 영향 분석
* 유전체 재구성(Genome Reconstruction) 과정 구현

### 개발 환경

* Language: C++
* Compiler: GCC (g++)
* IDE: Visual Studio Code
* OS: Windows

---

## 2. 프로젝트 결과물

실행 시 생성되는 주요 파일은 다음과 같습니다.

| 파일명                             | 설명                           |
| ------------------------------- | ---------------------------- |
| `0_DNA.txt`                     | 무작위 생성 원본 DNA 서열 (100,000bp) |
| `1_ShortReads.txt`              | 원본 DNA에서 추출한 Short Read 데이터  |
| `2_TrivialMapping_result.txt`   | Trivial Mapping 결과           |
| `3_BWT_result.txt`              | BWT Mapping 결과               |
| `4_RabinKarpMapping_result.txt` | Rabin-Karp Mapping 결과        |
| `5_IndexMapping_result.txt`     | Index Mapping 결과             |
| `6_KMPMapping_result.txt`       | KMP Mapping 결과               |
| `7_Reconstruct_알고리즘명.txt`       | Position 기반 재구성 결과           |
| `8_Compare_알고리즘명.txt`           | 원본 DNA와 복원 DNA 비교 결과         |
| `9_ExecutionTime.txt`           | 알고리즘별 실행 시간 측정 결과            |

---

## 3. 프로젝트 구조

```text
algorithm-project
│
├── DNA
│   └── GenerateDNA.cpp
│
├── Reads
│   └── GenerateShortReads.cpp
│
├── Mapping
│   ├── TrivialMapping.cpp
│   ├── BWT.cpp
│   ├── RabinKarp.cpp
│   ├── IndexMapping.cpp
│   └── KMP.cpp
│
├── Assembly
│   └── Reconstruct.cpp
│
├── Compare
│   └── CompareResult.cpp
│
├── Time
│   └── executeTime.cpp
│
├── Utils
│   └── IO.cpp
│
├── Config.h
└── main.cpp
```

---

## 4. DNA 재구성(Reconstruction) 방식

본 프로젝트는 De Novo Assembly 방식이 아닌 **Position 기반 Consensus Reconstruction** 방식을 사용합니다.

### 동작 과정

#### 1. Mapping 수행

각 알고리즘을 사용하여 Short Read를 원본 게놈 상의 최적 위치에 정렬합니다.

```text
Short Read
↓
Mapping
↓
Position + Mismatch 획득
```

---

#### 2. Position 기반 배치

각 Read를 Mapping된 위치에 배치합니다.

예시

```text
Read1 : ACGTA (position = 100)
Read2 : CGTAC (position = 101)
```

```text
100 101 102 103 104 105
 A   C   G   T   A
     C   G   T   A   C
```

---

#### 3. Weighted Voting

Mismatch가 적을수록 높은 가중치를 부여합니다.

```cpp
weight = allowedMismatch - mismatch + 1
```

예시

| mismatch | weight |
| -------- | ------ |
| 0        | 2      |
| 1        | 1      |

---

#### 4. Consensus 결정

각 위치마다 A/C/G/T 점수를 누적합니다.

예시

```text
Position 105

A : 5점
C : 2점
G : 1점
T : 0점
```

↓

```text
최종 복원 염기 = A
```

---

#### 5. 예외 처리

| 상황           | 처리 |
| ------------ | -- |
| Read가 하나도 없음 | N  |
| 동점 발생        | N  |

---

## 5. 주요 모듈 설명

### GenerateDNA.cpp

원본 DNA 염기서열 생성

```text
A, C, G, T
```

를 랜덤하게 생성하여 기준 게놈을 구성합니다.

---

### GenerateShortReads.cpp

원본 DNA에서 Short Read를 생성합니다.

기능

* 랜덤 위치 추출
* 설정 길이만큼 절단
* ErrorRate에 따라 변이 생성

---

### TrivialMapping.cpp

Brute Force 방식으로 모든 위치를 탐색합니다.

특징

* 구현 단순
* 속도 느림
* 기준(Baseline) 알고리즘 역할

---

### BWT.cpp

FM-Index 기반 탐색 알고리즘입니다.

특징

* 가장 빠른 속도
* 대용량 게놈 탐색에 적합

---

### RabinKarp.cpp

Rolling Hash 기반 탐색 알고리즘입니다.

특징

* 해시 기반 탐색
* 충돌 검증 필요

---

### IndexMapping.cpp

사전 구축된 Index(Hash Table)를 이용한 탐색입니다.

특징

* 완전 일치 영역 검색에 매우 빠름

---

### KMP.cpp

LPS(Longest Prefix Suffix) 테이블을 이용한 탐색입니다.

특징

* 불필요한 비교 최소화
* 선형 시간 탐색 가능

---

### Reconstruct.cpp

Position 기반 Consensus Reconstruction 수행

기능

* Mapping 결과 활용
* Voting 수행
* 최종 DNA 복원

---

### CompareResult.cpp

복원 결과를 원본 DNA와 비교합니다.

계산 항목

* Match Count
* Mismatch Count
* Unknown Count (N)
* Accuracy

---

### executeTime.cpp

각 Mapping 알고리즘의 실행 시간을 측정합니다.

측정 단위

```text
ms (millisecond)
```

---

## 6. 실험 환경

| 항목               | 값          |
| ---------------- | ---------- |
| Genome Length    | 100,000 bp |
| Reads Count      | 10,000     |
| Read Length      | 15 bp      |
| Error Rate       | 1%         |
| Allowed Mismatch | 1          |

---

## 7. Mapping 속도 비교

| Mapping Algorithm  | Time (ms) | Time (sec) | Rank |
| ------------------ | --------: | ---------: | ---: |
| BWT Mapping        |    462.37 |       0.46 |    1 |
| Index Mapping      |   1280.59 |       1.28 |    2 |
| KMP Mapping        |   5960.44 |       5.96 |    3 |
| Rabin-Karp Mapping |   7799.46 |       7.80 |    4 |
| Trivial Mapping    |  15905.08 |      15.91 |    5 |

### 결과 분석

* BWT(FM-Index)가 가장 우수한 성능을 보임
* Index Mapping이 두 번째로 빠름
* KMP와 Rabin-Karp는 중간 수준 성능
* Trivial Mapping은 모든 위치를 탐색하므로 가장 느림

---

## 8. DNA 복원 정확도 비교

| Algorithm     | Match | Mismatch | Unknown(N) | Accuracy |
| ------------- | ----: | -------: | ---------: | -------: |
| BWT           | 77330 |      330 |      22340 |   77.33% |
| Index Mapping | 77330 |      330 |      22340 |   77.33% |
| KMP           | 77330 |      330 |      22340 |   77.33% |
| Rabin-Karp    | 77330 |      330 |      22340 |   77.33% |
| Trivial       | 77333 |      335 |      22332 |   77.33% |

### 결과 분석

* 대부분의 알고리즘이 동일한 Mapping 결과를 생성
* 복원 정확도는 거의 동일
* 차이는 주로 실행 시간에서 발생
* N 영역은 Read가 매핑되지 않은 영역

---

## 9. 결론

본 프로젝트를 통해 문자열 탐색 알고리즘을 유전체 재서열 분석 문제에 적용하여 성능을 비교하였다.

실험 결과,

* BWT(FM-Index)가 가장 빠른 성능을 보였으며
* Position 기반 Consensus Reconstruction을 통해 DNA 복원이 가능함을 확인하였다.
* Mapping 정확도가 동일한 경우 복원 정확도 역시 거의 동일하게 나타났다.

따라서 대규모 유전체 분석 환경에서는 Mapping 알고리즘의 탐색 효율이 전체 성능을 결정하는 핵심 요소임을 확인할 수 있었다.
