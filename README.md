# Red-Black Tree 구현

Balanced search tree로 많이 쓰이는 Red-black tree (이하 RB tree)를 C 언어로 구현하는 과제입니다.
구현하는 추상 자료형 (ADT: abstract data type)은 ordered set, multiset 입니다.

## 구현 범위
다음 기능들을 수행할 수 있도록 RB tree를 구현합니다.

## 구현 규칙
- `src/rbtree.c` 이외에는 수정하지 않고 test를 통과해야 합니다.
- `make test`를 수행하여 `Passed All tests!`라는 메시지가 나오면 모든 test를 통과한 것입니다.
- Sentinel node를 사용하여 구현했다면 `test/Makefile`에서 `CFLAGS` 변수에 `-DSENTINEL`이 추가되도록 comment를 제거해 줍니다.

## 구현 기능 목록 정리
- `new_tree()`: RB tree 구조체 생성
  - 여러 개의 tree를 생성할 수 있어야 하며 각각 다른 내용들을 저장할 수 있어야 합니다.
- ***-> 트리와 공용 nil(BLACK) 노드를 동적 할당/초기화하고 root = nil로 설정하여 구현함***

- `delete_tree(tree)`: RB tree 구조체가 차지했던 메모리 반환
  - 해당 tree가 사용했던 메모리를 전부 반환해야 합니다. (valgrind로 나타나지 않아야 함)
- ***-> 전체 트리 노드 해제 후 t->nil과 트리 구조체까지 모두 free하여 구현함***
- `tree_insert(tree, key)`: key 추가
  - 구현하는 ADT가 multiset이므로 이미 같은 key의 값이 존재해도 하나 더 추가 합니다.
- ***-> 새 노드를 RED로 삽입(BST 연결)한 뒤 rbtree_fixup으로 RB 불변식을 회복하고 삽입 노드 포인터를 반환하여 구현함***
- `tree_find(tree, key)`
  - RB tree내에 해당 key가 있는지 탐색하여 있으면 해당 node pointer 반환
  - 해당하는 node가 없으면 NULL 반환
- ***-> 루트부터 key와의 비교로 내려가며 일치 노드를 찾고, 없으면 NULL을 반환하여 구현함***
- `tree_erase(tree, ptr)`: RB tree 내부의 ptr로 지정된 node를 삭제하고 메모리 반환
- ***-> z를 삭제(자식이 0or1이라면 이식, 2라면 successor 교체)하고, 제거된 색이 BLACK이면 delete_fixup을 통해 RB 불변식을 회복하여 구현함***
- `tree_min(tree)`: RB tree 중 최소 값을 가진 node pointer 반환
- ***-> 루트에서 왼쪽 끝으로 내려가 최솟값 노드를 반환하여 구현함***
- `tree_max(tree)`: 최대값을 가진 node pointer 반환
- ***-> 루트에서 오른쪽 끝으로 내려가 최댓값 노드를 반환하여 구현함***

- `tree_to_array(tree, array, n)`
  - RB tree의 내용을 *key 순서대로* 주어진 array로 변환
  - array의 크기는 n으로 주어지며 tree의 크기가 n 보다 큰 경우에는 순서대로 n개 까지만 변환
  - array의 메모리 공간은 이 함수를 부르는 쪽에서 준비하고 그 크기를 n으로 알려줍니다.
- ***-> 트리를 중위 순회로 arr에 저장하고 성공 시 0을 반환하여 구현함***


## 과제의 의도 (Motivation)
- 복잡한 자료구조(data structure)를 구현해 봄으로써 자신감 상승
- C 언어, 특히 포인터(pointer)와 malloc, free 등의 system call에 익숙해짐.
- 동적 메모리 할당(dynamic memory allocation)을 직접 사용해 봄으로써 동적 메모리 할당의 필요성 체감 및 data segment에 대한 이해도 상승
- 고급 언어에서 기본으로 제공되는 자료구조가 세부적으로는 어떻게 구현되어 있는지 경험함으로써 고급 언어 사용시에도 효율성 고려

## 느낀 점
처음에는 삽입, 삭제, 탐색 등의 기본 연산이 단순히 BST나 AVL 트리와 비슷할 것이라고 생각하였지만
실제 구현 과정에서 균형 유지를 위한 회전과 색 변경 로직이 상당히 까다로웠다.
특히 삽입에 비해 삭제에서의 doubly black이나 red-and-black에 대한 처리 과정이 가장 까다로웠던 것 같다.

<br>

처음 test_find_erase에서 assert 실패가 났을 때는 어디부터 건드려야 할지 몰라 굉장히 답답했지만
- 삽입, 삭제 보정 로직
- transplant에서의 부모/자식 연결
- delete_fixup의 Case 별 처리
등을 꼼꼼히 확인하고 수정하여 문제를 해결하고 테스트를 통과할 수 있었다.

<br>

또한 valgrind를 통해 메모리 누수가 없는지 확인할 수 있었기에 자료구조 구현 과정에서 메모리 관리까지 신경써야했고, 누수가 난 결과를 마주하게 되면
디버깅을 통해 어느 부분에서 메모리 해제 누락이 있었는지 확인하고, 고쳐나가야 했기에 이러한 익숙치 않은 경험이 굉장히 흥미롭고 재밌었다.

<br>

마지막에 **Passed all tests!** 메시지와 모든 메모리 해제 결과를 확인했을 때는
지금까지의 디버깅과 반복적인 수정이 헛되지 않았음을 실감하며, 레드-블랙 트리의 구조와 핵심 원리를 설명할 수 있다는 자신감 또한 얻을 수 있었다.


## 참고문헌
- [위키백과: 레드-블랙 트리](https://ko.wikipedia.org/wiki/%EB%A0%88%EB%93%9C-%EB%B8%94%EB%9E%99_%ED%8A%B8%EB%A6%AC)
([영어](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree))
- [CLRS book (Introduction to Algorithms) 13장 레드 블랙 트리 - Sentinel node를 사용한 구현](https://product.kyobobook.co.kr/detail/S000213683944)
- [Wikipedia:균형 이진 트리의 구현 방법들](https://en.wikipedia.org/wiki/Self-balancing_binary_search_tree#Implementations)
