# bootmarker_python3.py / bootmarker_python2.py
bootmarker와 bootkpi value를 출력해 이를 시간 순으로 정렬하여 하나의 csv 파일에 출력하는 프로그램입니다.
python3와 python2 버전 코드가 있습니다.

## 실행 방법
1) cmd 창에 명령어 입력

    $ python bootmarker_python3.py

2) bootmarker에서 TIME_SYNC option 추가 선택 입력(y/n)

3) 작성할 결과 csv 파일명 입력


# values_fromDQA_python3.py / values_fromDQA_python2.py
bootmarker와 bootkpi value가 각각 들어있는 두 text file을 합쳐 시간순으로 정렬하여 하나의 csv 파일에 출려하는 프로그램입니다. 
python3와 python2 버전 코드가 있습니다.

## 실행방법
1) cmd 창에 명령어 입력

     $ python values_fromDQA_python3.py

2) kpi value 파일명 입력
    ex) kpi_values.txt

3) bootmarker value 파일명 입력
    ex) values.txt

4) 생성할 결과 파일명 입력 (.csv)
    ex) result.csv


# combine_tc_python3.py / combine_tc_python2.py
여러 개의 bootmarker와 bootkpi를 합친 csv 파일을 test 항목별로 정리해 평균 실행시간과 구간 별 실행 시간차를 하나의 csv 파일에 출력하는 프로그램입니다.
python3와 python2 버전 코드가 있습니다.

## 실행 방법
1) cmd 창에 명령어 입력

    $ python combine_tc_python3.py

2) 합칠 csv 파일의 개수 정수로 입력

3) 합칠 파일의 이름 하나씩 총 n번 입력 

4) 결과 파일 combined_bootmarker.csv 확인


# version_chart.py
combine_tc_pyhon3.py 프로그램을 통해 모델 버전 별로 만들어진 두 csv 파일의 항목별 평균 실행 시간과 구간별 실행 시간 차를 비교하여 그래프로 출력하는 프로그램입니다.

해당 프로그램은 python 라이브러리를 사용하므로 python3에서만 사용 가능하며 추가적인 라이브러리 설치가 필요합니다.

    $ pip install matplotlib
    $ pip install pandas

## 실행 방법
1) cmd 창에 명령어 입력

    $ python version_chart.py

2) 비교할 두 파일명 차례대로 입력
    ex) v1.csv / v2.csv


# CPU_info_tool_chart.py
일정 시간 이후 CPU 정보를 출력하는 명령어를 실행하여 CPU idle, GMS loading, total PSS, free memory 측정값을 Threshold와 비교한 결과와 그래프를 출력하는 프로그램입니다.

해당 프로그램은 python 라이브러리를 사용하므로 python3에서만 사용 가능하며 추가적인 라이브러리 설치가 필요합니다.

    $ pip install matplotlib
    $ pip install pandas
    $ pip install numpy

# 실행 방법
1) cmd 창에 명령어 입력

    $ python CPU_info_tool_chart.py
