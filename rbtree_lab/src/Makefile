# Makefile 변수 선언
# 1. 특수 선언(.PHONY)
# 2. 전역 설정(CFLAGS, LDFLAGS 등)
# 3. 빌드 규칙
# 순으로 작성
 
# 이 타겟은 실제 파일이 아니라 실행해야하는 명령을 의미
# 예를 들어, 프로젝트에 clean이라는 파일이 존재하면 make clean 시 아무 동작도 안할 수 있음. 이를 방지하고 실행 파일, 오브젝트 파일 정리를 수행
.PHONY: clean

# CFLAGS : 컴파일 옵션을 지정
# -Wall : 모든 주요 경고 메세지를 활성화
# -g : 디버깅 심볼 포함(디버거에서 코드 라인 추적 가능)
# Make는 .c 파일을 .o파일로 변환할 때 자동으로 $(CFLAGS)를 사용
# 즉, 이것은 빌드 과정에서 GCC/Clang에 전달될 옵션을 설정하는 것
CFLAGS=-Wall -g

# 타겟(driver) : 최종적으로 만들고 싶은 결과물 -> 실행 파일 driver
# 의존성(driver.o, rbtree.o) : driver를 만들기 위해 반드시 있어야 하는 파일 -> 컴파일된 오브젝트 파일(.o)
# 1. driver가 없거나, driver.o나 rbtree.o가 더 새로워졌다면 driver를 다시 생성
# 2. driver를 만들 때는 driver.o와 rbtree.o를 링크해서 실행 파일로 만들 것
driver: driver.o rbtree.o

clean:
	rm -f driver *.o

# 주요 명령어
# make : 기본 빌드
# make clean : clean 타겟 실행(빌드 산출물 삭제)
# make driver : 특정 타겟 지정해서 실행