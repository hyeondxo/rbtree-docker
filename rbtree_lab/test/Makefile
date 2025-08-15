.PHONY: test # test 타겟 선언

# 헤더 검색 경로에 ../src를 추가
# 테스트 코드가 rbtree.h를 include하므로, 컴파일러가 ../src/rbtree.h를 찾을 수 있게 해줌
# 주석을 풀어 -DESENTINEL을 활성화하면 테스트 코드가 센티넬 모드 기준으로 동작을 검증하게 됨
CFLAGS=-I ../src -Wall -g #-DSENTINEL

# test 타겟은 test-rbtree 실행 파일에 의존(없으면 먼저 빌드)
# 두 명령어를 순차 실행 - 첫 번째 명령에서 실패하면 그 즉시 멈추고 두 번째는 수행되지 않음
# ./test-rbtree : 일반 실행
# valgrind ./test-rbtree : 메모리 누수/잘못된 접근 검사
test: test-rbtree
	./test-rbtree
	valgrind ./test-rbtree

# test-rbtree를 만들기 위한 링크 타겟
# test-rbtree.o + ../src/rbtree.o(트리 라이브러리 객체)
test-rbtree: test-rbtree.o ../src/rbtree.o

# ../src/rbtree.o가 필요하면 src 폴더의 Makefile을 호출해 그곳에서 rbtree.o를 빌드
# 테스트 빌드가 소스 빌드를 끌어다 쓰는 구조
../src/rbtree.o:
	$(MAKE) -C ../src rbtree.o

# 테스트 폴더의 산출물(test-rbtree.*.o)을 삭제
clean:
	rm -f test-rbtree *.o
