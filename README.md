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

---

## 10. 실험 가이드

이 프로젝트는 `main.cpp`를 실행하면 실험 입력 데이터와 결과 JSON을 만들고, `plot_experiment_metrics.py`를 실행하면 그 JSON을 읽어서 그래프를 그리는 구조입니다.

핵심은 다음 두 파일입니다.

* `main.cpp`: 어떤 조건으로 실험 데이터를 만들고 비교할지 결정
* `plot_experiment_metrics.py`: 어느 버전의 결과를 읽고 어떤 범위로 그래프를 그릴지 결정

### 10-1. 먼저 알아야 할 결과 폴더 구조

실험 결과는 `Result/DNA_SHORTREAD_ver{n}` 형식의 폴더에 저장됩니다.

예시

```text
Result/
└── DNA_SHORTREAD_ver3/
    ├── DNA.txt
    ├── Shortreads_15/
    │   ├── len15_1.txt
    │   ├── len15_2.txt
    │   ├── len15_3.txt
    │   ├── result_len15_1.json
    │   ├── result_len15_2.json
    │   └── result_len15_3.json
    ├── Shortreads_20/
    └── ...
```

각 파일의 의미는 다음과 같습니다.

* `DNA.txt`: 해당 버전 실험에서 사용할 원본 DNA
* `Shortreads_{길이}`: 같은 read 길이에 대한 입력 묶음 폴더
* `len{길이}_{실행번호}.txt`: 실제 short read 입력 파일
* `result_len{길이}_{실행번호}.json`: 각 mapping 알고리즘의 실행 시간과 정확도 결과

중요한 점은 `version`을 기준으로 DNA와 Short Read가 묶인다는 것입니다.

* 같은 `version`을 다시 실행하면 기존 `DNA.txt`, `len*.txt`, `result_len*.json`을 재사용하거나 이미 존재하면 생략합니다.
* 완전히 새로운 실험 세트를 만들고 싶으면 `version`을 새 값으로 바꾸는 것이 가장 안전합니다.
* 같은 `version`을 유지한 채 `cfg.length`, `CntOfReads`, `readLengths`를 바꾸면 기존 파일과 충돌하여 예외가 날 수 있습니다.

### 10-2. 실험 조건은 `main.cpp`에서 바꾼다

다른 사람이 실험할 때 가장 먼저 봐야 하는 곳은 `main.cpp`입니다.

```cpp
int version = 3;
int runCount = 3;

Config cfg;
cfg.length = 10000;
cfg.CntOfReads = 1000;
cfg.ErrorRate = 0.01;
cfg.allowedMismatch = 3;

vector<int> readLengths = {
    15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100
};
```

실험 변수의 의미는 아래와 같습니다.

| 변수 | 위치 | 의미 |
| --- | --- | --- |
| `version` | `main.cpp` | 결과를 저장할 실험 버전 번호. 결과 폴더 이름 `DNA_SHORTREAD_ver{version}`에 직접 반영됨 |
| `runCount` | `main.cpp` | 같은 read 길이에 대해 몇 번 반복 실행할지 결정 |
| `cfg.length` | `main.cpp` | 원본 DNA 길이 |
| `cfg.CntOfReads` | `main.cpp` | 한 번의 실험에서 생성할 short read 개수 |
| `cfg.ErrorRate` | `main.cpp` | read 생성 시 삽입할 오류 비율 |
| `cfg.allowedMismatch` | `main.cpp` | mapping 시 허용할 mismatch 개수 |
| `readLengths` | `main.cpp` | 실험할 short read 길이 목록 |
| `cfg.LenOfReads` | `RunMappingExperiment.cpp` 내부 | 각 반복에서 현재 `readLength` 값으로 자동 설정됨 |

실제로는 `readLengths`를 하나씩 순회하면서, 각 길이에 대해 `runCount`만큼 반복 실험합니다.

즉 총 실행 횟수는 다음과 같습니다.

```text
실험 수 = readLengths 개수 × runCount
```

예를 들어 `readLengths = [15, 20, 25]`, `runCount = 3`이면 총 9개의 결과 JSON이 생성됩니다.

### 10-3. 변수별로 언제 바꾸면 되는가

실험 목적에 따라 주로 아래 변수들을 바꾸면 됩니다.

* DNA 크기를 늘려 성능 차이를 크게 보고 싶다: `cfg.length` 증가
* read 수가 많아질 때 실행 시간이 어떻게 변하는지 보고 싶다: `cfg.CntOfReads` 증가
* read 길이에 따른 성능 변화를 보고 싶다: `readLengths` 수정
* 오류가 많은 입력에서 정확도가 어떻게 달라지는지 보고 싶다: `cfg.ErrorRate` 증가
* mismatch 허용 범위를 비교하고 싶다: `cfg.allowedMismatch` 변경
* 독립된 새 실험 세트를 만들고 싶다: `version` 변경
* 같은 조건으로 여러 번 반복해 평균적인 경향을 보고 싶다: `runCount` 증가

권장 사용 방식은 다음과 같습니다.

* 입력 조건을 바꿨다면 `version`도 함께 바꾸기
* 그래프 범위를 바꿀 때는 `main.cpp`가 아니라 `plot_experiment_metrics.py`만 수정하기
* 기존 결과를 덮어쓰기보다 새 `version`으로 남겨서 비교 가능하게 관리하기

### 10-4. 실험 실행 순서

1. `main.cpp`에서 실험 변수 수정
2. C++ 프로그램 빌드 후 실행
3. `Result/DNA_SHORTREAD_ver{version}` 아래에 결과 JSON 생성 확인
4. `plot_experiment_metrics.py`에서 같은 `version`을 읽도록 설정
5. Python 스크립트 실행 후 그래프 확인

VS Code 기준으로는 C++ 빌드 후 `app.exe`를 실행하면 실험이 진행됩니다.

### 10-5. 그래프 설정은 `plot_experiment_metrics.py`에서 바꾼다

그래프는 `plot_experiment_metrics.py`의 `SETTINGS` 딕셔너리로 제어합니다.

```python
SETTINGS = {
    "version": 3,
    "min_length": 15,
    "max_length": 115,
    "length_step": 5,
    "time_y_min": 0.0,
    "time_y_max": None,
    "time_focus_algorithm": "Trivial",
    "time_zoom_padding_ratio": 0.08,
    "accuracy_y_min": 75.0,
    "accuracy_y_max": 100.0,
    "result_root": Path("Result"),
}
```

각 그래프 변수의 의미는 다음과 같습니다.

| 변수 | 의미 |
| --- | --- |
| `version` | 읽어올 결과 폴더 번호. `Result/DNA_SHORTREAD_ver{version}`을 찾음 |
| `min_length` | 그래프에 포함할 최소 read 길이 |
| `max_length` | 그래프에 포함할 최대 read 길이 |
| `length_step` | read 길이 간격 |
| `time_y_min`, `time_y_max` | 시간 그래프 y축 범위 |
| `time_focus_algorithm` | 별도 분리해서 보여줄 알고리즘 이름 |
| `time_zoom_padding_ratio` | 나머지 알고리즘 시간 그래프를 얼마나 여유 있게 확대할지 결정 |
| `accuracy_y_min`, `accuracy_y_max` | 정확도 그래프 y축 범위 |
| `result_root` | 결과 폴더 루트 경로 |

현재 그래프 스크립트는 다음 방식으로 동작합니다.

* 시간 그래프: `Trivial`은 별도 서브플롯으로 분리, 나머지 알고리즘은 확대해서 비교
* 정확도 그래프: `Trivial`은 별도 서브플롯으로 분리, 나머지 알고리즘은 한 그래프에서 비교
* 결과 파일 이름이 `result_len{길이}_{실행번호}.json` 형식을 따를 때만 읽음

### 10-6. 다른 사람이 가장 자주 헷갈리는 부분

#### 1. 왜 결과 폴더 이름이 `DNA_SHORTREAD_ver{n}`인가?

이 프로젝트는 DNA, short read, 결과 JSON을 하나의 실험 세트로 묶기 위해 `version` 번호를 사용합니다.

즉 `ver3`이라면,

* `Result/DNA_SHORTREAD_ver3/DNA.txt`
* `Result/DNA_SHORTREAD_ver3/Shortreads_15/len15_1.txt`
* `Result/DNA_SHORTREAD_ver3/Shortreads_15/result_len15_1.json`

이 모두가 같은 실험 묶음입니다.

#### 2. 왜 어떤 파일은 다시 실행해도 새로 안 만들어지는가?

입력 파일과 결과 파일이 이미 있으면 재생성하지 않도록 되어 있습니다.

따라서 설정을 바꿨는데도 예전 결과가 남아 있으면,

* `version`을 새로 바꾸거나
* 기존 해당 버전 폴더를 직접 정리한 뒤 다시 실행해야 합니다.

#### 3. `Config.h`의 `LenOfReads`는 직접 수정해야 하는가?

보통 직접 수정할 필요 없습니다.

`main.cpp`에서 `readLengths`를 순회할 때 각 길이가 `RunMappingExperiment.cpp` 내부에서 `cfg.LenOfReads = readLength`로 자동 반영됩니다.

즉 read 길이 실험 범위는 `Config.h`보다 `main.cpp`의 `readLengths`가 더 중요합니다.

### 10-7. 빠르게 실험을 시작하는 최소 수정 포인트

다른 사람이 처음 실험할 때는 아래 변수만 바꾸면 충분합니다.

* `main.cpp`
* `version`
* `runCount`
* `cfg.length`
* `cfg.CntOfReads`
* `cfg.ErrorRate`
* `cfg.allowedMismatch`
* `readLengths`

그래프를 그릴 때는 아래 변수만 맞추면 됩니다.

* `plot_experiment_metrics.py`
* `SETTINGS["version"]`
* `SETTINGS["min_length"]`
* `SETTINGS["max_length"]`
* `SETTINGS["length_step"]`

이 네 가지를 현재 생성된 결과와 맞추면 그래프는 정상적으로 생성됩니다.
