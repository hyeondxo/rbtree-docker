// 헤더 가드(Header Guard)
#ifndef _RBTREE_H_ // _RBTREE_H_라는 매크로가 아직 정의되지 않았다면 아래를 처리해라 -> 전처리기 조건문
#define _RBTREE_H_ // 위 조건이 참일 경우, _RBTREE_H_를 정의함. 그 뒤로 이 파일 내용이 한 번만 포함되도록 표식을 남김

/**
 * C의 #include는 텍스트를 그대로 붙여넣는 것임
 * 같은 헤더가 중복 포함되면 타입/함수 선언이 두 번 이상 나타나 빌드 오류가 발생
 * 헤더 가드는 이 헤더파일이 한 번만 들어가도록 하는 안전장치
 */

#include <stddef.h>

/**
 * 헤더파일은 Java의 interface와 유사한가?
 * -> 부분적으로 유사하지만, 완전히 같지는 않음
 *
 * 유사점
 * - 외부에 공개되는 계약(함수 원형, 자료형)을 정의
 * rbtree.c는 이 계약을 만족하도록 함수 구현을 제공
 * 그리고 사용자(driver.c)는 #include "rbtree.h"만 보고 타입/함수 시그니처를 알 수 있게 됨
 *
 * 중요한 차이
 * - Java의 interface는 implements로 구현이 강제되지만 C의 헤더는 단지 선언만을 모아둔 텍스트임
 * 구현이 없으면 링커 단계에서 undefined reference 오류가 날 뿐 언어가 강제하지는 않음
 */

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

typedef struct node_t {
    color_t color;
    key_t key;
    struct node_t *parent, *left, *right;
} node_t;

typedef struct {
    node_t *root;
    node_t *nil; // for sentinel
} rbtree;

/**
 * 센티넬(sentinel)이란? 왜 쓰는지?
 * 센티넬은 특수한 더미 노드임
 * rbtree에서는 모든 빈 리프 노드를 가리키는 공용 BLACK 노드를 하나 만들어 t->nil과 같이 사용
 * 즉, NULL 대신 t->nil이 없음을 의미
 *
 * 장점?
 * 어떤 노드의 NULL체크 대신 nil(not in list)이라는 동일한 포인터와 비교.
 * 예외 분기가 줄어들어 코드가 간결하고 안전해짐
 * 모든 NIL은 BLACK이어야 하는 규칙을 센티넬 하나로 충족
 */

rbtree *new_rbtree(void);
void delete_rbtree(rbtree *);

node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);

/**
 * insert/find에서 const key_t를 받는 이유?
 * key_t는 현재 int이므로 기능적으로는 const가 없어도 동작이 같음
 * -> 왜? int는 값으로 전달되고, 구현체에서 변경해도 원본에는 영향 x
 *
 * 하지만 이 함수는 전달받은 키 값을 변경할 생각이 없다는 의미를 코드 레벨에서 드러낼 수 있음
 * 구현부에서 key++와 같은 코드를 쓰는 실수를 하더라도 컴파일러가 잡아줌 (컴파일 오류)
 *
 * 하지만 나중에 key_t를 struct나 포인터를 통한 큰 키 타입으로 바꾸고, 그 포인터를 인자로 줄 경우 원본 데이터의 변경이
 * 가능해짐 이 때 const의 의미가 생기는 것 -> const를 포함한 타입은 함수가 그 데이터를 수정하지 못하도록 막아줄 수 있음
 * 즉, 지금은 int라 큰 차이가 없더라도 API 일관성을 미리 잡아두는 설계인 것
 *
 * const rbtree * 또한 이 트리 구조를 변경하지 않겠다는 뜻
 * 구현체에서 const rbtree *t와 같이 받는다면 t가 가리키는 대상 (rbtree 구조체)의 필드를 수정할 수 없게 됨
 * 즉 함수 안에서 t->root는 안되지만, t = other_t 와 같이 포인터 값 자체를 바꾸는 것은 가능
 * 만약 포인터도 못 바꾸게 하고 싶다면 const rbtree * const t;와 같이 사용
 * -> t 구조체의 필드와 t 포인터 자체 둘 다 변경 불가
 */

node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
int rbtree_erase(rbtree *, node_t *);

int rbtree_to_array(const rbtree *, key_t *, const size_t);

// ifndef로 연 블록을 닫는 지점
#endif // _RBTREE_H_
